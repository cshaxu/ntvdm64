param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$root = [IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build') + '\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)) {throw 'Use a fresh build-owned runner-test output'}
$source = (Resolve-Path -LiteralPath $PackageRoot).Path.TrimEnd('\')
New-Item -ItemType Directory -Path $root | Out-Null
# SETUP.CMD changes to its media root before launching DOS Setup.  Exercise a
# build-owned copy: the source package is treated as release input and remains
# untouched, while the runner test works even when its supplied media root is
# not a cmd.exe current-directory target in the test host.
$package = Join-Path $root 'package'
Copy-Item -LiteralPath $source -Destination $package -Recurse
$shim = Join-Path $root 'shim'; New-Item -ItemType Directory -Path $shim | Out-Null
@"
@echo off
echo %* > "%NTVDM_WIN31_RUNNER_LOG%"
exit /b 37
"@ | Set-Content -LiteralPath (Join-Path $shim 'run16.cmd') -Encoding ASCII
$log = Join-Path $root 'run16-args.txt'
$savedPath = $env:PATH
try {
    $env:PATH = "$shim;$savedPath"; $env:NTVDM_WIN31_RUNNER_LOG = $log; $env:NTVDM_SETUP_TEST_NO_PAUSE = '1'
    $p = Start-Process -FilePath cmd.exe -ArgumentList @('/d','/c',"$package\PATCH\SETUP.CMD") -Wait -PassThru
    if($p.ExitCode -ne 37) {throw "SETUP.CMD did not preserve run16 exit code: $($p.ExitCode)"}
} finally {$env:PATH = $savedPath; Remove-Item Env:NTVDM_WIN31_RUNNER_LOG -ErrorAction SilentlyContinue; Remove-Item Env:NTVDM_SETUP_TEST_NO_PAUSE -ErrorAction SilentlyContinue}
if(!(Test-Path -LiteralPath $log)) {throw 'SETUP.CMD did not invoke PATH run16'}
if((Get-Content -LiteralPath $log -Raw).Trim() -ne 'command /c SETUP.EXE') {throw 'SETUP.CMD did not launch the DOS COMMAND setup entry'}
$entry = Get-Content -LiteralPath "$package\PATCH\SETUP.CMD" -Raw
if($entry -match '(?i)powershell|\.ps1|SETUP\.PIF') {throw 'SETUP.CMD retains a PowerShell or Win16-installer dependency'}
foreach($name in @('SETUP.PIF','CONFIG.NT','AUTOEXEC.NT','WORK','TEMP')) {if(Test-Path -LiteralPath "$package\PATCH\$name") {throw "SETUP.CMD unexpectedly generated a setup artifact: $name"}}
'PASS SETUP.CMD invokes PATH run16 on DOS COMMAND /c SETUP.EXE, preserves its result and creates no scratch payload'
