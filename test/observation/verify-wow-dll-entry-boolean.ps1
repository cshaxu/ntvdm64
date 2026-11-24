[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$WowBuildRoot = 'build/.ninja/msvc-x86-release/wow32'
)
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if (!$root.StartsWith($repo + '\build\', [StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $root)) { throw 'Fresh build-owned evidence directory required' }
$null = New-Item -ItemType Directory -Path $root
& "$repo/tools/build-fixtures.cmd" x86 wow-entry-abi *> "$root/build.stdout.txt"
if($LASTEXITCODE){throw 'Native ABI fixture build failed'}
$cache=Join-Path $repo 'build/.ninja/fixtures-x86'
$bridge=Join-Path $repo 'tools/build/wow32/templates/wow32-dll-entry-bridge.c'
$object=Join-Path $cache 'wow-entry-abi.c'
$fixture=Join-Path $repo 'test/mvdm-host/wow_dll_entry_boolean_fixture.c'
$exe=Join-Path $cache 'wow-entry-boolean.exe'
& $exe > "$root/run.stdout.txt" 2> "$root/run.stderr.txt"
if ($LASTEXITCODE) { throw "Selected bridge BOOLEAN contract failed: $LASTEXITCODE" }
$result = [IO.File]::ReadAllText("$root/run.stdout.txt")
if ($result -notmatch 'attach=00000001 false_attach=00000000 thread_false=00000000 detach_false=00000000 cleanups=2') {
    throw 'Unexpected bridge return or cleanup witness'
}
@{ result = 'pass'; scope = 'Current bridge ABI projection with controlled BOOLEAN provider; excludes registration lifecycle'; output = $result; inputs = @(Get-FileHash -LiteralPath $bridge, $object, $fixture) } |
    ConvertTo-Json -Depth 6 | Set-Content -Encoding utf8 "$root/result.json"

$provider = Join-Path $repo 'test/mvdm-host/wow_dll_loader_provider.c'
$loader = Join-Path $repo 'test/mvdm-host/wow_dll_loader_fixture.c'
$dll=Join-Path $cache 'wow-entry-provider.dll'
$loaderExe=Join-Path $cache 'wow-entry-loader.exe'
& $loaderExe $dll > "$root/loader.stdout.txt" 2> "$root/loader.stderr.txt"
$loaderExit=$LASTEXITCODE
if ($loaderExit) { throw "Windows loader test failed: $loaderExit" }
$loaderOutput = [IO.File]::ReadAllText("$root/loader.stdout.txt")
if ($loaderOutput -notmatch 'WOW_BRIDGE_OS_LOADER attach_false=failed attach_true=loaded unload=1 bitmap=1 registry=1 rounds=8') { throw 'Missing loader attach/unload witness' }
@{ result = 'pass'; scope = 'Windows loader with current return/cleanup projection and controlled provider; excludes registration and actual WOW guest lifecycle'; output = $loaderOutput; inputs = @(Get-FileHash -LiteralPath $object, $provider, $loader, $dll, $loaderExe) } |
    ConvertTo-Json -Depth 6 | Set-Content -Encoding utf8 "$root/loader-result.json"
