param(
    [Parameter(Mandatory)][string]$MeasuredWorker,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$ReportRoot,
    [string]$Baseline='build/M0-T428/S6/r002/runtime',
    [ValidateRange(1,10)][int]$Iterations=3
)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($ReportRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Reports must remain below build/'}
if(Test-Path $root){throw 'Fresh reports required'}
if(Test-Path Z:\){throw 'Z: occupied'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker'}
$worker=(Resolve-Path $MeasuredWorker).Path;$observerPath=(Resolve-Path $Observer).Path
$baselinePath=(Resolve-Path $Baseline).Path
$runtime=Join-Path $root 'runtime'
$null=New-Item -ItemType Directory -Path $runtime
Copy-Item (Join-Path $baselinePath 'system32') $runtime -Recurse
Copy-Item (Join-Path $baselinePath 'system.ini') $runtime
Copy-Item $worker (Join-Path $runtime 'system32/ntvdm.exe') -Force
$names=@('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_MOUSE_HOOK','MVDM_OBSERVER_MOUSE_BURST',
 'MVDM_OBSERVER_MOUSE_RETIRE','MVDM_TEST_WORKER_PERFORMANCE','MVDM_TEST_CONSOLE_WIRE_PATH')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
$scope=$null;$results=@()
try {
    & subst.exe Z: $runtime
    if($LASTEXITCODE){throw 'SUBST failed'}
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_MOUSE_HOOK='O:\winnt\tests\S7MOUSE.dll'
    $env:MVDM_OBSERVER_MOUSE_BURST='200'
    Remove-Item Env:MVDM_OBSERVER_MOUSE_RETIRE,Env:MVDM_TEST_CONSOLE_WIRE_PATH -ErrorAction SilentlyContinue
    for($iteration=0;$iteration -le $Iterations;++$iteration){
        $scope=New-IsolatedPackageScope $runtime 'Z:\'
        $report=Join-Path $root "mouse-$iteration.txt"
        $prefix=Join-Path $root "worker-$iteration"
        if($iteration){$env:MVDM_TEST_WORKER_PERFORMANCE=$prefix}
        else{Remove-Item Env:MVDM_TEST_WORKER_PERFORMANCE -ErrorAction SilentlyContinue}
        $timer=[Diagnostics.Stopwatch]::StartNew()
        & $observerPath 'Z:\system32\run16.exe' 'Z:\system32\' $report --observation-timeout-ms 60000 'O:\winnt\tests\WMS7.COM'
        $observerExit=$LASTEXITCODE;$timer.Stop()
        $record=Get-Content $report -Raw
        $screen=Get-Content ($report+'.console.txt') -Raw
        $hook=Get-Content ($report+'.mouse-window.txt') -Raw
        if($observerExit -or $record -notmatch '(?m)^result=exited\r?$' -or
            $record -notmatch '(?m)^exit=0x00000000\r?$' -or $screen -notmatch 'WINDOW-MOUSE-PASS' -or
            $hook -notmatch '(?m)^burst-records=200\r?$' -or $hook -notmatch '(?m)^input-sink-acknowledged=yes\r?$'){
            throw "Real 200-input guest probe failed: $iteration"
        }
        $metrics=@(Get-ChildItem -LiteralPath $root -Filter "worker-$iteration-*.txt")
        if($iteration){
            if($metrics.Count -ne 1){throw 'Missing/ambiguous measured worker report'}
            $text=Get-Content $metrics[0].FullName -Raw
            if($text -notmatch 'overflow=0 ' -or $text -notmatch 'phase=mouse-irq-consumption ' -or
                $text -notmatch 'phase=video-transfer ' -or $text -match 'phase=mouse-unmatched-consumption ' -or
                $text -match 'phase=mouse-rejected '){throw 'Incomplete or rejected measurement'}
            $enqueued=[regex]::Matches($text,'(?m)^phase=mouse-enqueued ').Count
            $merged=[regex]::Matches($text,'(?m)^phase=mouse-merged ').Count
            $consumed=[regex]::Matches($text,'(?m)^phase=mouse-irq-consumption ').Count
            if($enqueued+$merged -lt 203 -or $consumed -ne $enqueued -or
                $text -notmatch 'phase=handoff-barrier ' -or $text -notmatch 'phase=io-close-ack ' -or
                $text -match '(?m)^phase=.* error=[1-9][0-9]*\r?$'){
                throw 'Input conservation or final handoff acknowledgement failed'
            }
        }elseif($metrics.Count){throw 'Disabled instrumentation wrote a report'}
        $results+=[ordered]@{Iteration=$iteration;Enabled=($iteration -ne 0);ElapsedMs=$timer.ElapsedMilliseconds;Report=$report;Metrics=@($metrics | ForEach-Object FullName)}
        Stop-IsolatedPackageScope $scope;$scope=$null
        Write-Output "PASS real 200-input guest probe iteration=$iteration elapsed-ms=$($timer.ElapsedMilliseconds)"
    }
    [ordered]@{Profile='x86 /MT CCPU40; private desktop; direct Window input sink, not raw-input/RDP';
        Worker=(Get-FileHash $worker).Hash;Observer=(Get-FileHash $observerPath).Hash;
        Hook=(Get-FileHash 'O:\winnt\tests\S7MOUSE.dll').Hash;Guest=(Get-FileHash 'O:\winnt\tests\WMS7.COM').Hash;
        Cases=$results}|ConvertTo-Json -Depth 6|Set-Content (Join-Path $root 'measurements.json') -Encoding UTF8
}finally{
    try {if($scope){Stop-IsolatedPackageScope $scope}}finally{
        & subst.exe Z: /d
        foreach($name in $names){
            if($null -eq $saved[$name]){Remove-Item -LiteralPath ('Env:'+$name) -ErrorAction SilentlyContinue}
            else{Set-Item -LiteralPath ('Env:'+$name) -Value $saved[$name]}
        }
    }
}
