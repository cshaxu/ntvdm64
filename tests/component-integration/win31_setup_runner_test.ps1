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
# Setup PIF fields have the historical 63-byte path ceiling.  Exercise the
# supplied short physical package in place; only the fake PATH launcher and
# its observation log live under build.  SETUP.PIF/NT profiles are explicitly
# package-owned runtime artifacts and are intentionally created there.
$package = $source
$shim = Join-Path $root 'shim'; New-Item -ItemType Directory -Path $shim | Out-Null
@"
@echo off
echo %* > "%NTVDM_WIN31_RUNNER_LOG%"
exit /b 37
"@ | Set-Content -LiteralPath (Join-Path $shim 'run16.cmd') -Encoding ASCII
$log = Join-Path $root 'run16-args.txt'
$savedPath = $env:PATH
try {
    $env:PATH = "$shim;$savedPath"; $env:NTVDM_WIN31_RUNNER_LOG = $log
    $p = Start-Process -FilePath powershell.exe -ArgumentList @('-NoProfile','-ExecutionPolicy','Bypass','-File',"$package\PATCH\run-setup.ps1",'-InstallRoot','X:\unused') -Wait -PassThru
    if($p.ExitCode -ne 37) {throw "run-setup did not preserve run16 exit code: $($p.ExitCode)"}
} finally {$env:PATH = $savedPath; Remove-Item Env:NTVDM_WIN31_RUNNER_LOG -ErrorAction SilentlyContinue}
if(!(Test-Path -LiteralPath $log)) {throw 'run-setup did not invoke PATH run16'}
if((Get-Content -LiteralPath $log -Raw).Trim() -ne "$package\PATCH\SETUP.PIF") {throw 'run-setup did not launch the package-local Setup PIF'}
foreach($name in @('SETUP.PIF','CONFIG.NT','AUTOEXEC.NT')) {if(!(Test-Path -LiteralPath "$package\PATCH\$name")) {throw "run-setup did not create package-local setup artifact: $name"}}
if(Test-Path -LiteralPath "$package\PATCH\WORK") {throw 'run-setup created WORK'}
'PASS run-setup invokes PATH run16 on package-local Setup PIF, preserves its result and creates no WORK'
