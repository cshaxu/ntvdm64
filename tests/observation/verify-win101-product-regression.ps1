param([Parameter(Mandatory)][string]$RuntimeRoot,
      [Parameter(Mandatory)][string]$BuildCache,
      [Parameter(Mandatory)][string]$LogRoot,
      [string]$ReleaseRoot='',
      [ValidateSet('Full','Control')][string]$Suite='Full')
$ErrorActionPreference='Stop'
function Image($component,$name,$baseline) {
    if($ReleaseRoot){return Join-Path (Join-Path $ReleaseRoot $component) $name}
    return $baseline
}
# Match the accepted T434 ten-image native overrides and historical WOW bars.
# Product matrices share BaseSrv/Z: and remain serial inside the existing runner.
& "$PSScriptRoot/../../tools/audit/Invoke-ProductVerification.ps1" `
 -RuntimeRoot $RuntimeRoot -BuildCache $BuildCache -LogRoot $LogRoot -Suite $Suite `
 -Observer build/M0-T433/S5/r012-observer/console-startup-observer.exe `
 -WindowObserver build/M0-T433/S5/r012-observer/worker-window-snapshot.exe `
 -TerminalObserver build/M0-T432/S6/r019-regression/terminal-observer.exe `
 -GuestFixture build/M0-T432/S6/r001/G7.COM `
 -NativeHook64 (Image hook64 nthook64.dll build/M0-T434/S3/r003-hook64/nthook64.dll) `
 -NativeWorker (Image worker ntvwm.exe build/M0-T434/S3/r004-worker/ntvwm.exe) `
 -NativeFrontend (Image frontend ntcon.exe build/M0-T434/S3/r005-frontend/ntcon.exe) `
 -NativeMonitor (Image monitor ntmon.exe build/M0-T434/S5/r024-monitor/ntmon.exe) `
 -NativeLauncher (Image launcher run16.exe build/M0-T434/S3/r006-launcher/run16.exe) `
 -NativeService (Image service ntsrv.exe build/M0-T434/S3/r002-service/ntsrv.exe) `
 -MonitorRpc (Image monitor monitor-rpc-test.exe build/M0-T434/S5/r024-monitor/monitor-rpc-test.exe) `
 -Node 'C:\Users\neko\AppData\Local\Logi\LogiPluginService\PluginHosts\node22\node\node.exe' `
 -WowBaselineRoots @('build/M0-T434/S4/r032-full','build/M0-T434/S3/r052-full','build/M0-T434/S2/r019-full','build/M0-T433/S5/r026-full')
