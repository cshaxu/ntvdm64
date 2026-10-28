$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$out=Join-Path $repo 'build/t473-s10/latest-source-release'
$node=(Get-Command node -CommandType Application).Source
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$args='/d /s /c ""'+$worker+'\run-ninja-parallel.cmd" frontend-request-client-test.exe broker-frontend-bootstrap-test.exe monitor-rpc-test.exe native-gui-startup-probe.exe"'
$p=Start-Process $env:ComSpec -ArgumentList $args -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$out/fixtures.stdout.txt" -RedirectStandardError "$out/fixtures.stderr.txt"
@{pid=$p.Id;started=$p.StartTime.ToUniversalTime().ToString('o');image=$env:ComSpec;arguments=$args}|ConvertTo-Json|Set-Content "$out/fixtures.process.json"
$p.WaitForExit();$p.WaitForExit();$code=$p.ExitCode;@{exit=$code}|ConvertTo-Json|Set-Content "$out/fixtures.completion.json";$p.Dispose();if($code){throw 'Current control fixture build failed'}
$runtime=Join-Path $out 'package';$sys=Join-Path $runtime 'system32';$cache=Join-Path $out 'control-cache'
New-Item -ItemType Directory -Path $cache|Out-Null
foreach($row in (Get-Content "$runtime/product-system32-manifest.json" -Raw|ConvertFrom-Json)){Copy-Item -LiteralPath (Join-Path $sys $row.Name) -Destination (Join-Path $cache $row.Name)}
foreach($name in @('frontend-request-client-test.exe','broker-frontend-bootstrap-test.exe','monitor-rpc-test.exe','native-gui-startup-probe.exe')){Copy-Item -LiteralPath (Join-Path $worker $name) -Destination (Join-Path $cache $name)}
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH='C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$env:OPENNT_BROKER_PRODUCT_BUILD=Join-Path $repo 'build/.ninja/msvc-x64-release/service'
& "$repo/tools/audit/Invoke-ProductVerification.ps1" -RuntimeRoot $runtime -BuildCache $cache -Observer "$repo/build/.ninja/console-observers-x64/console-startup-observer.exe" -WindowObserver "$repo/build/.ninja/console-observers-x64/worker-window-snapshot.exe" -TerminalObserver "$repo/build/.ninja/console-observers-x64/terminal-observer.exe" -LogRoot "$out/full" -Suite Full -Node $node -GuestFixture "$repo/build/M0-T451/S5/revalidate-fixture/G7.COM" -WowBaselineRoots "$repo/build/t473-s10/package-product-review-r1" -NativeHook64 "$sys/nthook64.dll" -NativeWorker "$sys/ntvwm.exe" -NativeFrontend "$sys/ntcon.exe" -NativeMonitor "$sys/ntmon.exe" -NativeLauncher "$sys/run16.exe" -NativeService "$sys/ntsrv.exe"
if($LASTEXITCODE){exit $LASTEXITCODE}
