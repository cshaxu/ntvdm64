[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$RepositoryRoot,
    [Parameter(Mandatory = $true)][string]$BuildRoot
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
$vs = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs)) { throw "Missing x86 build environment: $vs" }
New-Item -ItemType Directory -Path $build | Out-Null
$source = Join-Path $repository 'tests\observation\command_native_child_fixture.c'
$output = Join-Path $build 'S34N.EXE'
$command = 'call "{0}" -arch=x86 -host_arch=x86 >nul && cl.exe /nologo /TC /W4 /WX /O2 "{1}" /link /out:"{2}" kernel32.lib' -f $vs, $source, $output
& cmd.exe /d /s /c $command
if ($LASTEXITCODE -ne 0) { throw "S34 native-child fixture failed: $LASTEXITCODE" }
[ordered]@{
    schema = 'm0.t420.s34.command-native-child.v1'
    source = 'tests/observation/command_native_child_fixture.c'
    outputs = @([ordered]@{ path = 'S34N.EXE'; sha256 = (Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash.ToLowerInvariant() })
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $build 'manifest.json') -Encoding utf8
Write-Host "S34 COMMAND native-child fixture completed: $output"
