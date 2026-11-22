Add-Type -Path "$PSScriptRoot/native-guest-final/logon-env.dll"
$identity=[Security.Principal.WindowsIdentity]::GetCurrent()
try {$entries=[WowMessageLogonEnvironment]::Read($identity.Token)} finally {$identity.Dispose()}
foreach($name in @([Environment]::GetEnvironmentVariables('Process').Keys)){
    [Environment]::SetEnvironmentVariable($name,$null,'Process')
}
foreach($entry in $entries){$at=$entry.IndexOf('=',1);[Environment]::SetEnvironmentVariable($entry.Substring(0,$at),$entry.Substring($at+1),'Process')}
try {
$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$env:OPENNT_BROKER_PRODUCT_BUILD="O:\repos.hobby\ntvdm64\build\.ninja\msvc-x64-release\service"
$node='C:\Program Files\WindowsApps\OpenAI.CodexPrimaryRuntime.v26-1119-907-0_26.1119.907.0_x64__3k8sg7r9htsxt\dependencies\node\bin\node.exe'
& "$repo\tools\audit\Invoke-ProductVerification.ps1" -RuntimeRoot "$repo\build\t483-s20-p85-window-control\candidate-final" -BuildCache "$repo\build\t483-s20-p85-window-control\full-cache" -Observer "$repo\build\.ninja\msvc-x64-release\message-probes\console-startup-observer.exe" -WindowObserver "$repo\build\t483-s5-p79-final\observers-x64\worker-window-snapshot.exe" -TerminalObserver "$repo\build\t483-s5-p79-final\observers-x64\terminal-observer.exe" -GuestFixture "$repo\build\M0-T420\S44\p79-fixture\G7.COM" -NativeHook64 "$repo\build\t483-s20-p85-window-control\candidate-final\system32\nthook64.dll" -NativeWorker "$repo\build\t483-s20-p85-window-control\candidate-final\system32\ntvwm.exe" -NativeFrontend "$repo\build\t483-s20-p85-window-control\candidate-final\system32\ntcon.exe" -NativeMonitor "$repo\build\t483-s20-p85-window-control\candidate-final\system32\ntmon.exe" -NativeLauncher "$repo\build\t483-s20-p85-window-control\candidate-final\system32\run16.exe" -NativeService "$repo\build\t483-s20-p85-window-control\candidate-final\system32\ntsrv.exe" -MonitorRpc "$repo\build\.ninja\msvc-x64-release\monitor\monitor-rpc-test.exe" -WowBaselineRoots "$repo\build\t483-s5-p78\full-final" -LogRoot "$repo\build\t483-s20-p85-window-control\full-userenv" -Suite Full -Node $node -Python 'C:/Program Files/Python313/python.exe'








$LASTEXITCODE | Set-Content build/t483-s20-p85-window-control/full-userenv.exit

exit $LASTEXITCODE

}catch{$_|Out-String|Write-Output;1|Set-Content O:/repos.hobby/ntvdm64/build/t483-s20-p85-window-control/full-userenv.exit;exit 1}
