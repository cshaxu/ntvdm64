$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$node=(Get-Command node -CommandType Application).Source
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
Get-ChildItem Env:|Where-Object{$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH='C:\Windows\System32;C:\Windows'
$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$env:OPENNT_BROKER_PRODUCT_BUILD=Join-Path $repo 'build/.ninja/pkg-uuid-x64/service'
$runtime=Join-Path $repo 'build/t473-s10/candidate-validation/package'
$sys=Join-Path $runtime 'system32'
& "$repo/tools/audit/Invoke-ProductVerification.ps1" -RuntimeRoot $runtime -BuildCache "$repo/build/t473-s10/candidate-validation/control-cache" `
 -Observer "$repo/build/.ninja/console-observers-x64/console-startup-observer.exe" `
 -WindowObserver "$repo/build/.ninja/console-observers-x64/worker-window-snapshot.exe" `
 -TerminalObserver "$repo/build/.ninja/console-observers-x64/terminal-observer.exe" `
 -LogRoot "$repo/build/t473-s10/candidate-full-r1" -Suite Full -Node $node `
 -GuestFixture "$repo/build/M0-T451/S5/revalidate-fixture/G7.COM" `
 -WowBaselineRoots "$repo/build/t473-s10/package-product-review-r1" `
 -NativeHook64 "$sys/nthook64.dll" -NativeWorker "$sys/ntvwm.exe" `
 -NativeFrontend "$sys/ntcon.exe" -NativeMonitor "$sys/ntmon.exe" `
 -NativeLauncher "$sys/run16.exe" -NativeService "$sys/ntsrv.exe"
