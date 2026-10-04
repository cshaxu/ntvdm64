[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Fixture,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateRange(1,4)][int]$Concurrency=4,
    [switch]$FailureProbe
)
$ErrorActionPreference='Stop'
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){
    throw 'Require a fresh evidence directory below repository build'
}
$observerPath=(Resolve-Path $Observer).Path
$fixturePath=(Resolve-Path $Fixture).Path
# Each fixture owns an in-process service and PID-qualified pipe names.
# None binds the global RPC endpoint. Do not add real-package cases here.
$cases=@('parent-resume-origin','io-authority','route-cancellation','prepared-native-root-loss','frontend-unclaimed-stop','frontend-unclaimed-reconnect','frontend-delegated','frontend-wait','frontend-wait-root-loss',
    'frontend-wait-request-loss','frontend-wait-worker-loss','frontend-rundown',
    'reenter-before-return','reenter-after-return','reenter-nested-return',
    'reenter-pending-command','reenter-before-increment','launcher-completed-rundown',
    'completed-worker-loss','wow-start-late-query','default','frontend-authority','shared-worker-residency',
    'native-command','native-worker','management-gui','management-frontend-close',
    'management-terminate','unfinished-worker-exit')
if($FailureProbe){$cases=@('runner-failure')}
$null=New-Item -ItemType Directory -Path $log
$running=[Collections.Generic.List[object]]::new()
$results=[Collections.Generic.List[object]]::new()
$total=[Diagnostics.Stopwatch]::StartNew()
$next=0
try {
    while($next -lt $cases.Count -or $running.Count){
        while($next -lt $cases.Count -and $running.Count -lt $Concurrency){
            $case=$cases[$next++]
            $report=Join-Path $log "$case.txt"
            $start=[Diagnostics.ProcessStartInfo]::new($observerPath)
            $start.UseShellExecute=$false
            $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
            $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
            foreach($argument in @($fixturePath,[IO.Path]::GetDirectoryName($fixturePath),$report)){
                $start.ArgumentList.Add($argument)
            }
            if($case -ne 'default'){$start.ArgumentList.Add('--'+$case)}
            $start.ArgumentList.Add('--observation-timeout-ms')
            $start.ArgumentList.Add('45000')
            $watch=[Diagnostics.Stopwatch]::StartNew()
            $process=[Diagnostics.Process]::Start($start)
            $running.Add([pscustomobject]@{Case=$case;Report=$report;Process=$process;Watch=$watch})
        }
        # Wait on the oldest exact owned process, not on a fixed pacing sleep.
        # Other running fixtures continue independently; per-case budget includes
        # time since launch, so queue ordering cannot multiply its timeout.
        $item=$running[0]
        $remaining=[Math]::Max(0,55000-$item.Watch.ElapsedMilliseconds)
        if(!$item.Process.WaitForExit([int]$remaining)){throw "Fixture timeout: $($item.Case)"}
        $item.Watch.Stop()
        $text=Get-Content $item.Report -Raw
        $screen=Get-Content ($item.Report+'.console.txt') -Raw
        if($item.Process.ExitCode -or $text -notmatch '(?m)^result=exited\r?$' -or
            $text -notmatch '(?m)^exit=0x00000000\r?$' -or !$screen.Contains('PASS')){
            throw "Fixture assertion/actual exit failed: $($item.Case)"
        }
        $results.Add([pscustomobject]@{Case=$item.Case;ElapsedMs=$item.Watch.ElapsedMilliseconds;Result='PASS'})
        $item.Process.Dispose()
        $running.RemoveAt(0)
    }
} finally {
    foreach($item in $running){
        # A CHECK failure may return before the fixture closes its suspended
        # children. Only clean this owned fixture PID's exact test-image children;
        # never enumerate or control product sessions by name alone.
        if(Test-Path $item.Report){
            $reportText=Get-Content $item.Report -Raw
            $match=[regex]::Match($reportText,'(?m)^pid=(\d+)\r?$')
            if($match.Success){
                $fixturePid=[uint32]$match.Groups[1].Value
                foreach($child in @(Get-CimInstance Win32_Process -Filter "ParentProcessId=$fixturePid")){
                    if($child.CommandLine -and $child.CommandLine.StartsWith('"'+$fixturePath+'" ',[StringComparison]::OrdinalIgnoreCase)){
                        Stop-Process -Id $child.ProcessId -Force -ErrorAction Stop
                    }
                }
            }
        }
        if(!$item.Process.HasExited){$item.Process.Kill();$null=$item.Process.WaitForExit(5000)}
        $item.Process.Dispose()
    }
    $total.Stop()
    [pscustomobject]@{Concurrency=$Concurrency;ElapsedMs=$total.ElapsedMilliseconds;
        ObserverHash=(Get-FileHash $observerPath).Hash;FixtureHash=(Get-FileHash $fixturePath).Hash;
        Cases=@($results.ToArray())}|ConvertTo-Json -Depth 4|Set-Content (Join-Path $log 'timings.json')
}
"PASS service$($cases.Count) retained assertions; concurrency=$Concurrency elapsed-ms=$($total.ElapsedMilliseconds)"
