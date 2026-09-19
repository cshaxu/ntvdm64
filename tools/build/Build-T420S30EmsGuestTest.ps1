[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [Parameter(Mandatory = $true)][string]$RuntimeTestRoot,
    [string]$NasmExecutable = 'nasm.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
if (!(Test-Path -LiteralPath $RuntimeTestRoot)) { throw "RuntimeTestRoot does not exist: $RuntimeTestRoot" }
$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Path $build | Out-Null
& $nasm.Source -f bin -o (Join-Path $build 'E30.COM') (Join-Path $repository 'tests\observation\ems_capability.asm')
if ($LASTEXITCODE -ne 0) { throw "NASM E30 fixture failed: $LASTEXITCODE" }
& $nasm.Source -f bin -o (Join-Path $build 'D30.COM') (Join-Path $repository 'tests\observation\dosx_himem_capability.asm')
if ($LASTEXITCODE -ne 0) { throw "NASM D30 fixture failed: $LASTEXITCODE" }
& $nasm.Source -f bin -o (Join-Path $build 'P30.COM') (Join-Path $repository 'tests\observation\pure_dos_capability.asm')
if ($LASTEXITCODE -ne 0) { throw "NASM P30 fixture failed: $LASTEXITCODE" }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$compile = "call `"$vs`" -arch=x86 -host_arch=x64 >nul && cl /nologo /MT /W4 /I `"$repository\src\opennt-abi\source\public\internal\windows\inc`" `"$repository\tests\observation\ems_pif_builder.c`" /Fe:`"$build\ems-pif-builder.exe`""
cmd.exe /d /s /c $compile
if ($LASTEXITCODE -ne 0) { throw "EMS PIF builder compilation failed: $LASTEXITCODE" }
& (Join-Path $build 'ems-pif-builder.exe') $RuntimeTestRoot
if ($LASTEXITCODE -ne 0) { throw "EMS PIF/config generation failed: $LASTEXITCODE" }
$pureCompile = "call `"$vs`" -arch=x86 -host_arch=x64 >nul && cl /nologo /MT /W4 /I `"$repository\src\opennt-abi\source\public\internal\windows\inc`" `"$repository\tests\observation\pure_dos_pif_builder.c`" /Fe:`"$build\pure-dos-pif-builder.exe`""
cmd.exe /d /s /c $pureCompile
if ($LASTEXITCODE -ne 0) { throw "Pure DOS PIF builder compilation failed: $LASTEXITCODE" }
& (Join-Path $build 'pure-dos-pif-builder.exe') $RuntimeTestRoot
if ($LASTEXITCODE -ne 0) { throw "Pure DOS PIF/config generation failed: $LASTEXITCODE" }
@{ schema = 'm0.t420.s30.profile-guest.v2'; outputs = @('D30.COM','P30.COM','P30.PIF','P30.NT','P30AUTO.NT','E30.COM','E30.PIF','E30.NT','E30AUTO.NT') } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8
Write-Host "S30 profile guest fixtures completed: $build"
