[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$BuildCache,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateSet('Product','Control','Full')][string]$Suite='Full',
    [string]$GuestFixture,
    [string]$Dos4gwFixtureRoot,
    [string]$WindowObserver,
    [string]$TerminalObserver,
    [string]$NativeHook64,
    [string]$NativeWorker,
    [string]$NativeFrontend,
    [string]$NativeMonitor,
    [string]$NativeLauncher,
    [string]$NativeService,
    [string]$MonitorRpc,
    [string]$Node='node',
    [string]$Python='python',
    [string[]]$WowBaselineRoots,
    [ValidateSet('Observed','Paced')][string]$InputPolicy='Observed',
    [switch]$FullDeadlines,
    [switch]$WindowOwnedConsole
)
$ErrorActionPreference='Stop'
$total=[Diagnostics.Stopwatch]::StartNew()
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$sourceRuntime=(Resolve-Path $RuntimeRoot).Path
$cache=(Resolve-Path $BuildCache).Path
$observerPath=(Resolve-Path $Observer).Path
$Node=(Get-Command $Node -CommandType Application -ErrorAction Stop).Source
$Python=Get-Command $Python -CommandType Application -ErrorAction Stop |
    Select-Object -First 1 -ExpandProperty Source
$log=[IO.Path]::GetFullPath($LogRoot)
$build=(Join-Path $repo 'build')+'\'
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require a fresh repository-build evidence directory'
}
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=$null
try {
    $runtime=New-PhysicalRuntimeStage $sourceRuntime
    $runtimeScope=New-IsolatedPackageScope $runtime
    # The cache is an immutable comparison source; no gate launches from it.
    # It therefore has no process-ownership scope.  Requiring it to be a
    # build package incorrectly rejects the released package before a case.
    $cacheScope=$null
} catch {
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    throw
}
$runtimeBinary=Get-PackageBinaryRoot $runtime
if($NativeWorker){
    $NativeWorker=(Resolve-Path -LiteralPath $NativeWorker).Path
    if(!$NativeHook64 -or !$NativeWorker.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native worker requires both Hooks and build-owned input'}
}
if($NativeFrontend){
    $NativeFrontend=(Resolve-Path -LiteralPath $NativeFrontend).Path
    if(!$NativeWorker -or !$NativeFrontend.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native frontend requires the native worker and a build-owned input'}
}
if($NativeMonitor){
    $NativeMonitor=(Resolve-Path -LiteralPath $NativeMonitor).Path
    if(!$NativeFrontend -or !$NativeMonitor.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native monitor requires the native frontend and a build-owned input'}
}
if($MonitorRpc){
    $MonitorRpc=(Resolve-Path -LiteralPath $MonitorRpc).Path
    if(!$MonitorRpc.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Management fixture must be build-owned'}
}
if($NativeLauncher){
    $NativeLauncher=(Resolve-Path -LiteralPath $NativeLauncher).Path
    if(!$NativeMonitor -or !$NativeLauncher.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native launcher requires the native monitor and a build-owned input'}
}
if($NativeService){
    $NativeService=(Resolve-Path -LiteralPath $NativeService).Path
    if(!$NativeLauncher -or !$NativeService.StartsWith($build,[StringComparison]::OrdinalIgnoreCase)){throw 'Native service requires the native launcher and a build-owned input'}
}
foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe')){
    $input=if($name -eq 'ntvwm.exe' -and $NativeWorker){$NativeWorker}elseif($name -eq 'ntcon.exe' -and $NativeFrontend){$NativeFrontend}elseif($name -eq 'ntmon.exe' -and $NativeMonitor){$NativeMonitor}elseif($name -eq 'run16.exe' -and $NativeLauncher){$NativeLauncher}elseif($name -eq 'ntsrv.exe' -and $NativeService){$NativeService}else{Join-Path $cache $name}
    if((Get-IsolatedFileSha256 $input) -ne (Get-IsolatedFileSha256 (Join-Path $runtimeBinary $name))){
        throw "Build cache/runtime mismatch: $name; build affected targets first"
    }
}
$packageNames=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')
if(Test-Path -LiteralPath (Join-Path $cache 'nthook32.dll')){
    $hook=Join-Path $runtimeBinary 'nthook32.dll'
    if(!(Test-Path -LiteralPath $hook) -or
       (Get-IsolatedFileSha256 (Join-Path $cache 'nthook32.dll')) -ne (Get-IsolatedFileSha256 $hook)){
        throw 'Build cache/runtime mismatch: nthook32.dll'
    }
    $packageNames+='nthook32.dll'
}
if(Test-Path -LiteralPath (Join-Path $runtimeBinary 'nthook64.dll')){
    if(!$NativeHook64 -or $packageNames -notcontains 'nthook32.dll'){
        throw 'Hook64 runtime requires explicit matching build input and Hook32'
    }
    $hook64=(Resolve-Path -LiteralPath $NativeHook64).Path
    if((Get-IsolatedFileSha256 $hook64) -ne (Get-IsolatedFileSha256 (Join-Path $runtimeBinary 'nthook64.dll'))){
        throw 'Build/runtime mismatch: nthook64.dll'
    }
    $packageNames+='nthook64.dll'
}elseif($NativeHook64){throw 'Hook64 build input missing from runtime'}
$manifest=foreach($name in $packageNames){
    $path=Join-Path $runtimeBinary $name
    $bytes=[IO.File]::ReadAllBytes($path);$pe=[BitConverter]::ToInt32($bytes,60)
    $machine=if($name -eq 'nthook64.dll' -or ($name -eq 'ntvwm.exe' -and $NativeWorker) -or ($name -eq 'ntcon.exe' -and $NativeFrontend) -or ($name -eq 'ntmon.exe' -and $NativeMonitor) -or ($name -eq 'run16.exe' -and $NativeLauncher) -or ($name -eq 'ntsrv.exe' -and $NativeService)){0x8664}else{0x14c}
    if([BitConverter]::ToUInt16($bytes,$pe+4) -ne $machine){throw "Wrong runtime machine: $name"}
    [pscustomobject]@{Name=$name;Sha256=(Get-IsolatedFileSha256 $path);Machine=$machine}
}
if($Suite -ne 'Control' -and (!$GuestFixture -or !$WindowObserver -or !$WowBaselineRoots)){
    throw 'Product gate requires guest fixture, Window observer and retained WOW baselines'
}
$null=New-Item -ItemType Directory -Path $log
$manifest|ConvertTo-Json|Set-Content "$log/runtime-manifest.json"
$timings=[Collections.Generic.List[object]]::new()
$preparationMs=$total.ElapsedMilliseconds
$variables=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_CAF_OWNED_CONSOLE','MVDM_OBSERVER_CAF_OUTER_COMMAND','MVDM_OBSERVER_SHORT_HISTORY','TEST_RUNTIME_ROOT',
    'MVDM_TEST_DIRECT_CMD','MVDM_TEST_S34_REPEAT_DIR','OPENNT_BROKER_PRODUCT_BUILD',
    'OPENNT_VERSION_TEST_BUILD','OPENNT_VERSION_TEST_LOGS','OPENNT_VERSION_TEST_RUNTIME',
    'MVDM_OBSERVER_MILESTONE_INPUT','PATH')
$old=@{};foreach($name in $variables){$old[$name]=[Environment]::GetEnvironmentVariable($name)}
function Invoke-Gate([string]$Name,[scriptblock]$Body) {
    $watch=[Diagnostics.Stopwatch]::StartNew();$passed=$false
    try { & $Body;$passed=$true } finally {
        $watch.Stop()
        $bodyMs=$watch.ElapsedMilliseconds
        $cleanup=[Diagnostics.Stopwatch]::StartNew()
        $cleaned=$false
        try {
            Stop-IsolatedPackageScope $runtimeScope
            if($cacheScope){Stop-IsolatedPackageScope $cacheScope}
            $cleaned=$true
        }
        finally {
            $timings.Add([pscustomobject]@{Gate=$Name;ElapsedMs=$bodyMs;
                CleanupMs=$cleanup.ElapsedMilliseconds;Passed=($passed -and $cleaned)})
        }
    }
}
function Invoke-ShortRuntime([scriptblock]$Body) {
    # Test packages execute from their physical build-owned path.  Do not
    # create an OS-global drive mapping: it leaks across sessions and hides
    # path/ownership bugs from the same isolated-package checks used below.
    & $Body
}
try {
    if ($Suite -ne 'Control') {
        Invoke-Gate 'guest-media' {
            & $Python "$repo/tests/observation/verify-guest-media.py" `
                $runtime "$repo/assets/winnt-guest-manifest.json" *> "$log/guest-media.txt"
            if($LASTEXITCODE){throw 'Guest media contract failed; inspect guest-media.txt'}
        }
    }
    Invoke-Gate 'ntvdm-export-ordinals' {
        & $Python "$repo/tests/observation/verify-ntvdm-export-ordinals.py" `
            (Join-Path $runtimeBinary 'ntvdm.exe') `
            "$repo/src/ntvdm-exe/ntvdm-export-ordinals.csv" *> "$log/ntvdm-export-ordinals.txt"
        if($LASTEXITCODE){throw 'NTVDM export ordinal contract failed; inspect ntvdm-export-ordinals.txt'}
    }
    # Match the ordinary command matrix's controlled lookup environment.
    # Original COMMAND's large-environment limitation is tested separately.
    $env:PATH=$runtimeBinary+';'+(Join-Path $env:SystemRoot 'System32')
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    # Only the ordinary shell matrix opts into echo/result milestones.
    # Other probes may have non-echo input contracts; do not reinterpret them.
    Remove-Item Env:MVDM_OBSERVER_MILESTONE_INPUT -ErrorAction SilentlyContinue
    Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
    if($Suite -ne 'Control'){
        Invoke-ShortRuntime {
            # Explicit disposable Console geometry, independent of RDP defaults.
            # Never resize an inherited user's Console.
            $env:MVDM_OBSERVER_SHORT_HISTORY='1'
            Invoke-Gate 'wow-frontiers' {
                & "$repo/tests/observation/observe-wow-frontiers.ps1" -Observer $observerPath -WindowObserver $WindowObserver -Prefix wow-restored -PackageRoot $runtime -ProcessPackageRoot $runtime -LogRoot $log -WaitTarget
                foreach($guest in @('winmine','sol','write')){
                    $current=Get-Content "$log/wow-restored-$guest-windows.txt" -Raw
                    $last=($current -split 'sample-ms=')[-1]
                    $pattern=switch($guest){
                        # The selected guest may be English or the retained
                        # Chinese media. Still require the exact baseline
                        # signature below; this is not a liveness waiver.
                        winmine {'visible=1 class=(?:00C900A800C000D7 text=00C900A800C000D7|004D0069006E00650073007700650065007000650072 text=004D0069006E00650073007700650065007000650072)'}
                        # Retained packages expose either the selected SOL
                        # status/static frontier or its visible main Solitaire
                        # window.  Both are exact UTF-16 application identities;
                        # do not weaken this to an arbitrary visible window.
                        sol {'visible=1 class=(?:005300740061007400690063 text=(?:00C400DA00B400E600B200BB00B900BB|004F007500740020006F00660020006D0065006D006F00720079)|0053006F006C006900740061006900720065 text=0053006F006C006900740061006900720065)'}
                        # As with SOL, retain the historical status/static
                        # signature as well as the exact visible Write main
                        # window emitted by the current retained baseline.
                        write {'visible=1 class=(?:005300740061007400690063 text=004E006F0074[0-9A-F]+|004D005300570052004900540045005F004D0045004E0055 text=005700720069007400650020002D002000280055006E007400690074006C006500640029)'}
                    }
                    foreach($baselineRoot in $WowBaselineRoots){
                        $prior=Join-Path $baselineRoot "wow-restored-$guest-windows.txt"
                        if(!(Test-Path $prior)){$prior=Join-Path $baselineRoot "closure-wow-r001-$guest-windows.txt"}
                        if(!(Test-Path $prior)){$prior=Join-Path $baselineRoot "wow-$guest-windows.txt"}
                        $baseline=Get-Content $prior -Raw
                        $signature=[regex]::Match($baseline,$pattern).Value
                        if(!$signature){throw "WOW baseline lacks required application frontier: $guest ($prior)"}
                        if(!$last.Contains($signature) -or !$last.Contains('alive=1')){
                            throw "WOW frontier changed: $guest"
                        }
                        $oldReport=Get-Content ($prior.Replace('-windows.txt','.txt')) -Raw
                        $report=Get-Content "$log/wow-restored-$guest.txt" -Raw
                        foreach($marker in @('result=timeout','exit=0x53504354')){
                            if(!$oldReport.Contains($marker) -or !$report.Contains($marker)){throw "WOW observation changed: $guest"}
                        }
                    }
                    "PASS independent retained WOW frontier $guest; not gameplay acceptance"
                }
            }
            # SHORT_HISTORY also seeds "prior shell output" into the screen;
            # it is not just a geometry setting. Do not leak that independent
            # dirty-screen fixture into the ordinary command/echo matrix.
            # The strict DIR gate below retains its own seeded-history test.
            Remove-Item Env:MVDM_OBSERVER_SHORT_HISTORY -ErrorAction SilentlyContinue
            foreach($mode in @('Console','Window')){
                if($mode -eq 'Window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
                Invoke-Gate "$mode-17" {
                    try {
                        if($mode -eq 'Window' -and $WindowOwnedConsole){
                            Remove-Item Env:MVDM_OBSERVER_PRIVATE_DESKTOP -ErrorAction SilentlyContinue
                            $env:MVDM_OBSERVER_CAF_OWNED_CONSOLE='1'
                            # The observer uses this exact launch-image path
                            # to find only this isolated package's NTCON.
                            $env:MVDM_OBSERVER_CAF_OUTER_COMMAND='"'+(Join-Path $runtimeBinary 'run16.exe')+'"'
                        }
                        if($InputPolicy -eq 'Observed'){$env:MVDM_OBSERVER_MILESTONE_INPUT='1'}
                        & "$repo/tools/audit/Verify-CommandExitStatus.ps1" -Observer $observerPath -PackageRoot $runtime -ProcessPackageRoot $runtime -LogRoot $log -LogPrefix "$mode-17" -GuestFixturePath $GuestFixture -OrdinaryFrontend
                    }finally{
                        Remove-Item Env:MVDM_OBSERVER_MILESTONE_INPUT -ErrorAction SilentlyContinue
                        if($mode -eq 'Window' -and $WindowOwnedConsole){
                            Remove-Item Env:MVDM_OBSERVER_CAF_OWNED_CONSOLE -ErrorAction SilentlyContinue
                            Remove-Item Env:MVDM_OBSERVER_CAF_OUTER_COMMAND -ErrorAction SilentlyContinue
                            $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
                        }
                    }
                }
            }
        }
    }
    if($Suite -ne 'Product'){
        Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
        # These native fixtures create the service through their executable's
        # sibling path. Use the real System32 package layout, not the flat
        # linker cache (whose service would derive the wrong Windows root).
        $packageRows=@((Get-Content -LiteralPath (Join-Path $runtime 'product-system32-manifest.json') -Raw | ConvertFrom-Json))
        $packageIds=@($packageRows | Select-Object -ExpandProperty PackageId -Unique)
        if($packageIds.Count -ne 1){throw 'Control fixtures require one package identity'}
        foreach($fixture in @('frontend-request-client-test.exe','broker-frontend-bootstrap-test.exe',
            'monitor-rpc-test.exe','native-gui-startup-probe.exe')){
            $input=if($fixture -eq 'monitor-rpc-test.exe' -and $MonitorRpc){$MonitorRpc}else{Join-Path $cache $fixture};$output=Join-Path $runtimeBinary $fixture
            Copy-Item -LiteralPath $input -Destination $output -Force
            if((Get-IsolatedFileSha256 $input) -ne (Get-IsolatedFileSha256 $output)){throw 'Control fixture copy mismatch'}
            & "$repo/tools/build/Set-PackageIdentity.ps1" -Image $output -PackageId ([guid]$packageIds[0])
        }
        Invoke-Gate 'rpc' {
            # native-command/native-registry are in-process archive cases,
            # now run concurrently in verify-service-fixtures, not duplicated here.
            # The fixture owns this finite contract list.  Do not retain
            # historical timeout/workerless labels after their implementations
            # were intentionally removed: ValidateSet rejection would make the
            # product gate fail before it exercised any actual peer.
            $cases=@('client','bootstrap','startup-rejections',
                'native-worker-failure','native-completed-worker-loss','monitor-rpc')
            & "$repo/tests/observation/verify-s7-rpc-fixtures.ps1" -Observer $observerPath -PackageRoot $runtimeBinary -ReportPrefix "$log/rpc" -Cases $cases
        }
        Invoke-Gate 'native-gui' {
            & "$repo/tests/observation/verify-native-gui-routing.ps1" -Observer $observerPath -PackageRoot $runtime -ReportPrefix "$log/gui" -MonitorRpc $MonitorRpc -FullDeadlines:$FullDeadlines
        }
        Invoke-ShortRuntime {
            $env:MVDM_OBSERVER_SHORT_HISTORY='1'
            Invoke-Gate 'version-negatives' {
                # These configure only the negative-peer builder. Do not
                # leak its long diagnostic paths into later guest requests.
                # Restore the incoming environment even if Node throws or
                # reports a failed negative; never strip a caller's values.
                $versionNames=@('OPENNT_BROKER_PRODUCT_BUILD','OPENNT_VERSION_TEST_BUILD',
                    'OPENNT_VERSION_TEST_LOGS','OPENNT_VERSION_TEST_RUNTIME','OPENNT_VERSION_PWSH')
                $versionSaved=@{}
                foreach($name in $versionNames){$versionSaved[$name]=[Environment]::GetEnvironmentVariable($name,'Process')}
                try {
                    $env:OPENNT_BROKER_PRODUCT_BUILD=if($versionSaved.OPENNT_BROKER_PRODUCT_BUILD){$versionSaved.OPENNT_BROKER_PRODUCT_BUILD}elseif($NativeService){Split-Path $NativeService -Parent}else{$cache}
                    $env:OPENNT_VERSION_TEST_BUILD="$log/version-negative"
                    $env:OPENNT_VERSION_TEST_LOGS="$log/version-negative/logs"
                    $env:OPENNT_VERSION_TEST_RUNTIME=$runtime
                    $env:OPENNT_VERSION_PWSH=(Get-Process -Id $PID).Path
                    & $Node "$repo/tools/audit/Verify-ProductVersions.mjs"
                    if($LASTEXITCODE){throw 'Version negative gate failed'}
                }finally{
                    foreach($name in $versionNames){
                        if($null -eq $versionSaved[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
                        else{[Environment]::SetEnvironmentVariable($name,$versionSaved[$name],'Process')}
                    }
                }
            }
            if($TerminalObserver){
                Invoke-Gate 'strict-dir' {
                    $env:TEST_RUNTIME_ROOT=$runtime
                    $env:MVDM_TEST_DIRECT_CMD='1'
                    $env:MVDM_TEST_S34_REPEAT_DIR='1'
                    try {
                        & $TerminalObserver 80 30 "$log/strict-dir.txt" --s34-full-dir
                        if($LASTEXITCODE){throw 'Strict DIR gate failed'}
                    }finally{
                        foreach($name in @('TEST_RUNTIME_ROOT','MVDM_TEST_DIRECT_CMD','MVDM_TEST_S34_REPEAT_DIR')){
                            if($null -eq $old[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
                            else{[Environment]::SetEnvironmentVariable($name,$old[$name])}
                        }
                    }
                }
            }else{throw 'Control gate requires the retained Terminal observer'}
            Invoke-Gate 'modern-edit-return' {& "$repo/tests/observation/verify-command-native-edit-return.ps1" -Observer $observerPath -PackageRoot $sourceRuntime -ReportPath "$log/modern-edit.txt"}
            Invoke-Gate 'cooked-return' {& "$repo/tests/observation/verify-frontend-relaunch.ps1" -Observer $observerPath -PackageRoot $runtime -ReportPath "$log/cooked-return.txt"}
            Invoke-Gate 'rapid-relaunch' {& "$repo/tests/observation/verify-frontend-rapid-relaunch.ps1" -Observer $observerPath -PackageRoot $runtime -ReportPath "$log/rapid.txt"}
            Invoke-Gate 'interactive-relaunch' {& "$repo/tests/observation/verify-frontend-rapid-interactive.ps1" -Observer $observerPath -PackageRoot $runtime -ReportPath "$log/interactive.txt"}
            Invoke-Gate 'session-isolation' {& "$repo/tests/observation/verify-ntvwm-management.ps1" -Observer $observerPath -MonitorRpc (Join-Path $runtimeBinary 'monitor-rpc-test.exe') -PackageRoot $runtime -ProcessPackageRoot $runtime -LogPrefix isolation -LogRoot $log -TwoSessions}
            Invoke-Gate 'retirement-wiring' {& "$repo/tests/observation/verify-broker-retirement.ps1" -Observer $observerPath -PackageRoot $runtime -ProcessPackageRoot $runtime -LogPrefix retirement -LogRoot $log -FullDeadlines:$FullDeadlines}
        }
        Invoke-Gate 'nested-window-handoff' {& "$repo/tests/observation/verify-broker-io-handoff.ps1" -RuntimeRoot $sourceRuntime -Observer $observerPath -ReportPrefix "$log/handoff" -Case nested-window -WindowOwnedConsole:$WindowOwnedConsole}
        if($Dos4gwFixtureRoot) {
            Invoke-Gate 'dos4gw-nesticle' {& "$repo/tests/observation/verify-dos4gw-nesticle.ps1" -RuntimeRoot $sourceRuntime -Observer $observerPath -FixtureRoot $Dos4gwFixtureRoot -ReportRoot "$log/dos4gw-nesticle"}
        } else {
            Write-Output 'SKIP dos4gw-nesticle: supply -Dos4gwFixtureRoot with the verified external fixtures'
        }
    }
    foreach($row in $manifest){if((Get-IsolatedFileSha256 (Join-Path $runtimeBinary $row.Name)) -ne $row.Sha256){throw 'Runtime identity changed during verification'}}
    "PASS $Suite selected gates; identical $($packageNames.Count)-file package"
}finally{
    try {
        Stop-IsolatedPackageScope $runtimeScope
        if($cacheScope){Stop-IsolatedPackageScope $cacheScope}
    } finally {
        foreach($name in $variables){
            if($null -eq $old[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
            else{[Environment]::SetEnvironmentVariable($name,$old[$name])}
        }
        $total.Stop()
        [pscustomobject]@{Suite=$Suite;ElapsedMs=$total.ElapsedMilliseconds;
            InputPolicy=$InputPolicy;WindowOwnedConsole=[bool]$WindowOwnedConsole;
            PreparationMs=$preparationMs;Gates=$timings.ToArray()}|
            ConvertTo-Json -Depth 4|Set-Content "$log/timings.json"
        if($runtime){Remove-PhysicalRuntimeStage $runtime}
    }
}
