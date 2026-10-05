[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [Parameter(Mandatory)][string]$BuildCache,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateSet('Product','Control','Full')][string]$Suite='Full',
    [string]$GuestFixture,
    [string]$WindowObserver,
    [string]$TerminalObserver,
    [string]$Node='node',
    [string[]]$WowBaselineRoots,
    [ValidateSet('Observed','Paced')][string]$InputPolicy='Observed',
    [switch]$FullDeadlines
)
$ErrorActionPreference='Stop'
$total=[Diagnostics.Stopwatch]::StartNew()
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$runtime=(Resolve-Path $RuntimeRoot).Path
$cache=(Resolve-Path $BuildCache).Path
$observerPath=(Resolve-Path $Observer).Path
$log=[IO.Path]::GetFullPath($LogRoot)
$build=(Join-Path $repo 'build')+'\'
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require a fresh repository-build evidence directory'
}
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtimeScope=New-IsolatedPackageScope $runtime
$cacheScope=New-IsolatedPackageScope $cache
$runtimeBinary=Get-PackageBinaryRoot $runtime
foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe')){
    if((Get-FileHash (Join-Path $cache $name)).Hash -ne (Get-FileHash (Join-Path $runtimeBinary $name)).Hash){
        throw "Build cache/runtime mismatch: $name; build affected targets first"
    }
}
$packageNames=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')
if(Test-Path -LiteralPath (Join-Path $cache 'nthook32.dll')){
    $hook=Join-Path $runtimeBinary 'nthook32.dll'
    if(!(Test-Path -LiteralPath $hook) -or
       (Get-FileHash (Join-Path $cache 'nthook32.dll')).Hash -ne (Get-FileHash $hook).Hash){
        throw 'Build cache/runtime mismatch: nthook32.dll'
    }
    $packageNames+='nthook32.dll'
}
$manifest=foreach($name in $packageNames){
    $path=Join-Path $runtimeBinary $name
    $bytes=[IO.File]::ReadAllBytes($path);$pe=[BitConverter]::ToInt32($bytes,60)
    if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'Non-x86 runtime'}
    [pscustomobject]@{Name=$name;Sha256=(Get-FileHash $path).Hash}
}
if($Suite -ne 'Control' -and (!$GuestFixture -or !$WindowObserver -or !$WowBaselineRoots)){
    throw 'Product gate requires guest fixture, Window observer and retained WOW baselines'
}
$null=New-Item -ItemType Directory -Path $log
$manifest|ConvertTo-Json|Set-Content "$log/runtime-manifest.json"
$timings=[Collections.Generic.List[object]]::new()
$preparationMs=$total.ElapsedMilliseconds
$variables=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_WINDOW_INPUT','MVDM_OBSERVER_SHORT_HISTORY','TEST_RUNTIME_ROOT',
    'MVDM_TEST_DIRECT_CMD','MVDM_TEST_S34_REPEAT_DIR','OPENNT_BROKER_PRODUCT_BUILD',
    'OPENNT_VERSION_TEST_BUILD','OPENNT_VERSION_TEST_LOGS','OPENNT_VERSION_TEST_RUNTIME',
    'MVDM_OBSERVER_MILESTONE_INPUT')
$old=@{};foreach($name in $variables){$old[$name]=[Environment]::GetEnvironmentVariable($name)}
function Invoke-Gate([string]$Name,[scriptblock]$Body) {
    $watch=[Diagnostics.Stopwatch]::StartNew();$passed=$false
    try { & $Body;$passed=$true } finally {
        $watch.Stop()
        $bodyMs=$watch.ElapsedMilliseconds
        $cleanup=[Diagnostics.Stopwatch]::StartNew()
        $cleaned=$false
        try {Stop-IsolatedPackageScope $runtimeScope;Stop-IsolatedPackageScope $cacheScope;$cleaned=$true}
        finally {
            $timings.Add([pscustomobject]@{Gate=$Name;ElapsedMs=$bodyMs;
                CleanupMs=$cleanup.ElapsedMilliseconds;Passed=($passed -and $cleaned)})
        }
    }
}
function Invoke-ShortRuntime([scriptblock]$Body) {
    if(Test-Path Z:\){throw 'Z: in use'}
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    # Bind the alias to this exact package while it exists; cleanup before unmap.
    $paths=$runtimeScope.Paths
    $savedObserver=$observerPath
    if($observerPath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){
        $observerPath=Join-Path 'Z:\' $observerPath.Substring($runtime.Length+1)
    }
    $runtimeScope.Paths=@($paths)+@($paths|ForEach-Object {Join-Path 'Z:\' $_.Substring($runtime.Length+1)})
    try { & $Body } finally {
        try {Stop-IsolatedPackageScope $runtimeScope} finally {
            & subst.exe Z: /d
            $runtimeScope.Paths=$paths
            $observerPath=$savedObserver
        }
    }
}
try {
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
                & "$repo/tests/observation/observe-wow-frontiers.ps1" -Observer $observerPath -WindowObserver $WindowObserver -Prefix wow-restored -PackageRoot Z:\ -ProcessPackageRoot $runtime -LogRoot $log -WaitTarget
                foreach($guest in @('winmine','sol','write')){
                    $current=Get-Content "$log/wow-restored-$guest-windows.txt" -Raw
                    $last=($current -split 'sample-ms=')[-1]
                    $pattern=switch($guest){
                        winmine {'visible=1 class=00C900A800C000D7 text=00C900A800C000D7'}
                        sol {'visible=1 class=005300740061007400690063 text=00C400DA00B400E600B200BB00B900BB'}
                        write {'visible=1 class=005300740061007400690063 text=004E006F0074[0-9A-F]+'}
                    }
                    foreach($baselineRoot in $WowBaselineRoots){
                        $prior=Join-Path $baselineRoot "wow-restored-$guest-windows.txt"
                        if(!(Test-Path $prior)){$prior=Join-Path $baselineRoot "closure-wow-r001-$guest-windows.txt"}
                        $baseline=Get-Content $prior -Raw
                        $signature=[regex]::Match($baseline,$pattern).Value
                        if(!$signature -or !$last.Contains($signature) -or !$last.Contains('alive=1')){throw "WOW frontier changed: $guest"}
                        $oldReport=Get-Content ($prior.Replace('-windows.txt','.txt')) -Raw
                        $report=Get-Content "$log/wow-restored-$guest.txt" -Raw
                        foreach($marker in @('result=timeout','exit=0x53504354')){
                            if(!$oldReport.Contains($marker) -or !$report.Contains($marker)){throw "WOW observation changed: $guest"}
                        }
                    }
                    "PASS independent retained WOW frontier $guest; not gameplay acceptance"
                }
            }
            foreach($mode in @('Console','Window')){
                if($mode -eq 'Window'){$env:MVDM_OBSERVER_WINDOW_INPUT='1'}else{Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue}
                Invoke-Gate "$mode-17" {
                    try {
                        if($InputPolicy -eq 'Observed'){$env:MVDM_OBSERVER_MILESTONE_INPUT='1'}
                        & "$repo/tools/audit/Verify-CommandExitStatus.ps1" -Observer $observerPath -PackageRoot Z:\ -ProcessPackageRoot $runtime -LogRoot $log -LogPrefix "$mode-17" -GuestFixturePath $GuestFixture -OrdinaryFrontend
                    }finally{Remove-Item Env:MVDM_OBSERVER_MILESTONE_INPUT -ErrorAction SilentlyContinue}
                }
            }
        }
    }
    if($Suite -ne 'Product'){
        Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
        Invoke-Gate 'rpc' {
            # native-command/native-registry are in-process archive cases,
            # now run concurrently in verify-service-fixtures, not duplicated here.
            $cases=@('client','bootstrap','startup-rejections','startup-timeout','native-worker-failure','native-completed-worker-loss','monitor-rpc')
            if($FullDeadlines){$cases+=@('workerless-grace','workerless-cancel')}
            & "$repo/tests/observation/verify-s7-rpc-fixtures.ps1" -Observer $observerPath -PackageRoot $cache -ReportPrefix "$log/rpc" -Cases $cases
        }
        Invoke-Gate 'native-gui' {
            & "$repo/tests/observation/verify-native-gui-routing.ps1" -Observer $observerPath -PackageRoot $cache -ReportPrefix "$log/gui" -FullDeadlines:$FullDeadlines
        }
        Invoke-ShortRuntime {
            $env:MVDM_OBSERVER_SHORT_HISTORY='1'
            Invoke-Gate 'version-negatives' {
                $env:OPENNT_BROKER_PRODUCT_BUILD=$cache
                $env:OPENNT_VERSION_TEST_BUILD="$log/version-negative"
                $env:OPENNT_VERSION_TEST_LOGS="$log/version-negative/logs"
                $env:OPENNT_VERSION_TEST_RUNTIME='Z:\'
                & $Node "$repo/tools/audit/Verify-ProductVersions.mjs"
                if($LASTEXITCODE){throw 'Version negative gate failed'}
            }
            if($TerminalObserver){
                Invoke-Gate 'strict-dir' {
                    $env:TEST_RUNTIME_ROOT='Z:\'
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
            Invoke-Gate 'modern-edit-return' {& "$repo/tests/observation/verify-command-native-edit-return.ps1" -Observer $observerPath -PackageRoot Z:\ -ReportPath "$log/modern-edit.txt"}
            Invoke-Gate 'cooked-return' {& "$repo/tests/observation/verify-frontend-relaunch.ps1" -Observer $observerPath -PackageRoot Z:\ -ReportPath "$log/cooked-return.txt"}
            Invoke-Gate 'rapid-relaunch' {& "$repo/tests/observation/verify-frontend-rapid-relaunch.ps1" -Observer $observerPath -PackageRoot Z:\ -ReportPath "$log/rapid.txt"}
            Invoke-Gate 'interactive-relaunch' {& "$repo/tests/observation/verify-frontend-rapid-interactive.ps1" -Observer $observerPath -PackageRoot Z:\ -ReportPath "$log/interactive.txt"}
            Invoke-Gate 'session-isolation' {& "$repo/tests/observation/verify-ntvwm-management.ps1" -Observer $observerPath -MonitorRpc "$cache/monitor-rpc-test.exe" -PackageRoot Z:\ -ProcessPackageRoot $runtime -LogPrefix isolation -LogRoot $log -TwoSessions}
            Invoke-Gate 'retirement-wiring' {& "$repo/tests/observation/verify-broker-retirement.ps1" -Observer $observerPath -PackageRoot Z:\ -ProcessPackageRoot $runtime -LogPrefix retirement -LogRoot $log -FullDeadlines:$FullDeadlines}
        }
        Invoke-Gate 'nested-window-handoff' {& "$repo/tests/observation/verify-broker-io-handoff.ps1" -RuntimeRoot $runtime -Observer $observerPath -ReportPrefix "$log/handoff" -Case nested-window}
    }
    foreach($row in $manifest){if((Get-FileHash (Join-Path $runtimeBinary $row.Name)).Hash -ne $row.Sha256){throw 'Runtime identity changed during verification'}}
    "PASS $Suite selected gates; identical $($packageNames.Count)-file package"
}finally{
    try {Stop-IsolatedPackageScope $runtimeScope;Stop-IsolatedPackageScope $cacheScope} finally {
        foreach($name in $variables){
            if($null -eq $old[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
            else{[Environment]::SetEnvironmentVariable($name,$old[$name])}
        }
        $total.Stop()
        [pscustomobject]@{Suite=$Suite;ElapsedMs=$total.ElapsedMilliseconds;
            InputPolicy=$InputPolicy;PreparationMs=$preparationMs;Gates=$timings.ToArray()}|
            ConvertTo-Json -Depth 4|Set-Content "$log/timings.json"
    }
}
