[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot,
    [string]$RuntimeTestRoot = 'O:\winnt\tests',
    [switch]$ReserveInt15,
    [string]$NasmExecutable = 'nasm.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
$buildBase = [IO.Path]::GetFullPath((Join-Path $repository 'build'))
if (!$build.StartsWith($buildBase + [IO.Path]::DirectorySeparatorChar,
        [StringComparison]::OrdinalIgnoreCase)) {
    throw "BuildRoot must remain below ${buildBase}: $build"
}
if (Test-Path -LiteralPath $build) { throw "Refusing to overwrite existing build root: $build" }
$nasm = Get-Command -Name $NasmExecutable -ErrorAction Stop
New-Item -ItemType Directory -Path $build | Out-Null
$repeatText = [IO.File]::ReadAllText((Join-Path $repository 'tests\observation\xms_repeat.bat'))
[IO.File]::WriteAllText((Join-Path $build 'R35.BAT'), ($repeatText -replace '\r?\n', "`r`n"), [Text.Encoding]::ASCII)
$output = Join-Path $build 'X35.COM'
& $nasm.Source -f bin -o $output (Join-Path $repository 'tests\observation\xms_capability.asm')
if ($LASTEXITCODE -ne 0) { throw "NASM S35 XMS fixture failed: $LASTEXITCODE" }
$profileDefines = @('-DUMB_PROFILE')
if (!$ReserveInt15) { $profileDefines += '-DDEFAULT_INT15' }
& $nasm.Source -f bin @profileDefines -o (Join-Path $build 'U35.COM') (Join-Path $repository 'tests\observation\xms_capability.asm')
if ($LASTEXITCODE -ne 0) { throw "NASM S35 profile fixture failed: $LASTEXITCODE" }
& $nasm.Source -f bin -o (Join-Path $build 'X35UMB.SYS') (Join-Path $repository 'tests\observation\xms_umb_driver.asm')
if ($LASTEXITCODE -ne 0) { throw "NASM S35 UMB boot fixture failed: $LASTEXITCODE" }
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$compile = "call `"$vs`" -arch=x86 -host_arch=x86 >nul && cl /nologo /MT /W4 /I `"$repository\src\opennt-abi\source\public\internal\windows\inc`" `"$repository\tests\observation\xms_profile_builder.c`" /Fo`"$build\profile.obj`" /Fe`"$build\profile.exe`""
cmd.exe /d /s /c $compile
if ($LASTEXITCODE -ne 0) { throw "S35 profile builder failed: $LASTEXITCODE" }
$profileArguments = @($build, $RuntimeTestRoot)
if (!$ReserveInt15) { $profileArguments += '--default-int15' }
& (Join-Path $build 'profile.exe') @profileArguments
if ($LASTEXITCODE -ne 0) { throw "S35 PIF generation failed: $LASTEXITCODE" }
[ordered]@{
    schema = 'm0.t420.s35.xms-guest.v1'
    source = 'tests/observation/xms_capability.asm'
    int15ReservedKb = $(if ($ReserveInt15) {128} else {0})
    outputs = @('X35.COM','U35.COM','U35.PIF','U35.NT','U35AUTO.NT','X35UMB.SYS','R35.BAT' | ForEach-Object {
        [ordered]@{ path = $_; sha256 = (Get-FileHash -LiteralPath (Join-Path $build $_) -Algorithm SHA256).Hash.ToLowerInvariant() }
    })
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8
Write-Host "S35 XMS guest fixture completed: $output"
