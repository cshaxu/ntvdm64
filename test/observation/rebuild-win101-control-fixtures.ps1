param([Parameter(Mandatory)][string]$BuildCache)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cache=(Resolve-Path (Join-Path $repo $BuildCache)).Path
if(!$cache.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build-owned cache required'
}
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
$commands=@('@echo off',
    'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
    'if errorlevel 1 exit /b %errorlevel%',('cd /d "'+$cache+'"'),
    ('"'+$ninja+'" frontend-request-client-test.exe broker-frontend-bootstrap-test.exe native-gui-startup-probe.exe'),
    'exit /b %errorlevel%')
[IO.File]::WriteAllLines("$cache/control-fixtures.cmd",$commands,[Text.Encoding]::ASCII)
& "$cache/control-fixtures.cmd" *> "$cache/control-fixtures.log"
if($LASTEXITCODE){throw 'Control fixture rebuild failed'}
'PASS control fixtures rebuilt against the current application identity'
