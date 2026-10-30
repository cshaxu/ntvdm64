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
$wow = (Resolve-Path -LiteralPath (Join-Path $repo $WowBuildRoot)).Path
$bridge = Join-Path $wow 'wow32-dll-entry-bridge.c'
$object = Join-Path $wow 'obj/wow32-dll-entry-bridge.obj'
$driver = Join-Path $wow 'msvc-x86.cmd'
$fixture = Join-Path $repo 'tests/mvdm-host/wow_dll_entry_boolean_fixture.c'
if ((Get-Content -LiteralPath $bridge -Raw) -notmatch 'extern unsigned char __stdcall W32DllInitialize') {
    throw 'Generated bridge does not use the original one-byte return contract'
}
$exe = Join-Path $root 'wow-dll-entry-boolean.exe'
$line = 'call "' + $driver + '" cl.exe /nologo /MT /W4 /Fo"' + $root + '/" /Fe"' + $exe + '" "' + $fixture + '" "' + $object + '"'
$buildCmd = Join-Path $root 'build.cmd'
Set-Content -LiteralPath $buildCmd -Value "@echo off`r`n$line`r`nexit /b %errorlevel%" -Encoding ascii
Push-Location $root
try { & $env:ComSpec /d /c $buildCmd > "$root/build.stdout.txt" 2> "$root/build.stderr.txt"; $buildExit = $LASTEXITCODE }
finally { Pop-Location }
if ($buildExit) { throw "Fixture build failed: $buildExit" }
& $exe > "$root/run.stdout.txt" 2> "$root/run.stderr.txt"
if ($LASTEXITCODE) { throw "Selected bridge BOOLEAN contract failed: $LASTEXITCODE" }
$result = [IO.File]::ReadAllText("$root/run.stdout.txt")
if ($result -notmatch 'attach=00000001 false_attach=00000000 thread_false=00000000 detach_false=00000000 cleanups=2') {
    throw 'Unexpected bridge return or cleanup witness'
}
@{ result = 'pass'; scope = 'Selected bridge object with controlled BOOLEAN provider; not real DLL lifecycle acceptance'; output = $result; inputs = @(Get-FileHash -LiteralPath $bridge, $object, $fixture) } |
    ConvertTo-Json -Depth 6 | Set-Content -Encoding utf8 "$root/result.json"

$provider = Join-Path $repo 'tests/mvdm-host/wow_dll_loader_provider.c'
$loader = Join-Path $repo 'tests/mvdm-host/wow_dll_loader_fixture.c'
$dll = Join-Path $root 'wow-bridge-test.dll'
$loaderExe = Join-Path $root 'wow-bridge-loader.exe'
$commands = @"
@echo off
call "$driver"
cl.exe /nologo /MT /LD /W4 "$provider" "$object" /Fe"$dll"
if errorlevel 1 exit /b 1
cl.exe /nologo /MT /W4 "$loader" /Fe"$loaderExe"
if errorlevel 1 exit /b 1
"$loaderExe" "$dll"
exit /b %errorlevel%
"@
$cmd = Join-Path $root 'loader-test.cmd'
Set-Content -LiteralPath $cmd -Value $commands -Encoding ascii
Push-Location $root
try { & $env:ComSpec /d /c $cmd > "$root/loader.stdout.txt" 2> "$root/loader.stderr.txt"; $loaderExit = $LASTEXITCODE }
finally { Pop-Location }
if ($loaderExit) { throw "Windows loader test failed: $loaderExit" }
$loaderOutput = [IO.File]::ReadAllText("$root/loader.stdout.txt")
if ($loaderOutput -notmatch 'WOW_BRIDGE_OS_LOADER attach_false=failed attach_true=loaded unload=1 bitmap=1 registry=1 rounds=8') { throw 'Missing loader attach/unload witness' }
@{ result = 'pass'; scope = 'Windows loader with selected bridge and controlled provider; not actual WOW guest DLL lifecycle'; output = $loaderOutput; inputs = @(Get-FileHash -LiteralPath $object, $provider, $loader, $dll, $loaderExe) } |
    ConvertTo-Json -Depth 6 | Set-Content -Encoding utf8 "$root/loader-result.json"
