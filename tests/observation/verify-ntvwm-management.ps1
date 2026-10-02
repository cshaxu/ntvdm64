[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$MonitorRpc,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\Logs2',
    [switch]$TwoSessions,
    [switch]$FrontendLoss,
    [switch]$WorkerLoss
)
$ErrorActionPreference='Stop'
if($FrontendLoss -and $WorkerLoss){throw 'Select one fault injection'}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$physical=(Resolve-Path $ProcessPackageRoot).Path
if(!$physical.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Require isolated build candidate'}
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'An existing broker must not be controlled by this test'}
$paths=@()
foreach($name in @('run16.exe','ntsrv.exe','ntkvm.exe','ntvwm.exe')){
    $actual=Join-Path $physical $name;$launch=Join-Path $PackageRoot $name
    if((Get-FileHash $actual).Hash -ne (Get-FileHash $launch).Hash){throw 'Candidate mismatch'}
    $paths+=@($actual,$launch)
}
$report=Join-Path $LogRoot "$LogPrefix.txt"
if(Test-Path $report){throw 'Use fresh evidence'}
$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$observerProcess=$null;$worker=$null;$target=$null;$monitor=$null
$frontend=$null
$otherObserver=$null;$otherWorker=$null;$otherTarget=$null;$gate=$null
$oldGate=$env:MVDM_OBSERVER_INPUT_GATE
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $observerProcess=Start-Process -FilePath (Resolve-Path $Observer).Path -ArgumentList @(
        (Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report,
        'cmd.exe','/d','/k','--observation-timeout-ms','25000') -WindowStyle Hidden -PassThru
    $deadline=[DateTime]::UtcNow.AddSeconds(15)
    do {
        $native=@(Get-CimInstance Win32_Process -Filter "Name='ntvwm.exe'" | Where-Object {$_.ExecutablePath -in $paths})
        if($native.Count -eq 1){
            $children=@(Get-CimInstance Win32_Process -Filter "Name='cmd.exe'" | Where-Object {$_.ParentProcessId -eq $native[0].ProcessId})
            if($children.Count -eq 1){
                $worker=Get-Process -Id $native[0].ProcessId;$null=$worker.Handle
                $target=Get-Process -Id $children[0].ProcessId;$null=$target.Handle
                break
            }
        }
        Start-Sleep -Milliseconds 100
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$worker -or !$target){throw 'Real NTVWM/CMD pair did not start'}
    $frontend=$null
    if($FrontendLoss){
        $owners=@(Get-CimInstance Win32_Process -Filter "Name='ntkvm.exe'" | Where-Object {$_.ExecutablePath -in $paths})
        if($owners.Count -ne 1){throw 'Expected exactly one authenticated test frontend before isolation launch'}
        $frontend=Get-Process -Id $owners[0].ProcessId;$null=$frontend.Handle
    }
    if($TwoSessions){
        $gateName='Local\ntvwm-isolation-'+[guid]::NewGuid().ToString('N')
        $gate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,$gateName)
        $env:MVDM_OBSERVER_INPUT_GATE=$gateName
        $otherReport=Join-Path $LogRoot "$LogPrefix-other.txt"
        if(Test-Path $otherReport){throw 'Use fresh second-session evidence'}
        $otherObserver=Start-Process -FilePath (Resolve-Path $Observer).Path -ArgumentList @(
            (Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$otherReport,
            'cmd.exe','/d','/k','--observation-timeout-ms','25000',
            '--observe-console-input-text',('"'+"echo ISOLATED-SESSION-OK`rexit /b 23`r"+'"')) -WindowStyle Hidden -PassThru
        $env:MVDM_OBSERVER_INPUT_GATE=$oldGate
        $deadline=[DateTime]::UtcNow.AddSeconds(10)
        do {
            $native=@(Get-CimInstance Win32_Process -Filter "Name='ntvwm.exe'" | Where-Object {
                $_.ExecutablePath -in $paths -and $_.ProcessId -ne $worker.Id
            })
            if($native.Count -eq 1){
                $children=@(Get-CimInstance Win32_Process -Filter "Name='cmd.exe'" | Where-Object {$_.ParentProcessId -eq $native[0].ProcessId})
                if($children.Count -eq 1){
                    $otherWorker=Get-Process -Id $native[0].ProcessId;$null=$otherWorker.Handle
                    $otherTarget=Get-Process -Id $children[0].ProcessId;$null=$otherTarget.Handle
                    break
                }
            }
            Start-Sleep -Milliseconds 100
        }while([DateTime]::UtcNow -lt $deadline)
        if(!$otherWorker -or !$otherTarget){throw 'Independent second NTVWM/CMD pair did not start'}
    }
    if($WorkerLoss){
        if($worker.Path -notin $paths){throw 'Pinned worker identity changed'}
        $worker.Kill()
        if(!$worker.WaitForExit(5000)){throw 'Fault injection did not end NTVWM'}
        if($target.HasExited){throw 'Worker-loss case requires an attached client that remains alive'}
    }elseif($FrontendLoss){
        if($frontend.Path -notin $paths){throw 'Pinned frontend identity changed'}
        $frontend.Kill()
        if(!$frontend.WaitForExit(5000)){throw 'Fault injection did not end frontend'}
    }else{
        $monitor=Start-Process -FilePath (Resolve-Path $MonitorRpc).Path -ArgumentList @('--terminate',$worker.Id) -WindowStyle Hidden -PassThru `
            -RedirectStandardOutput "$report.management.log" -RedirectStandardError "$report.management.err"
        if(!$monitor.WaitForExit(18000) -or $monitor.ExitCode -ne 0){throw 'Management RPC rejected or timed out'}
    }
    if(!$WorkerLoss -and (!$worker.WaitForExit(5000) -or !$target.WaitForExit(5000))){throw 'NTVWM or attached CMD survived acknowledged close'}
    if(!$observerProcess.WaitForExit(10000)){throw 'Direct launcher did not return after management close'}
    $record=Get-Content $report -Raw
    if($record -notmatch '(?m)^result=exited\r?$'){throw 'Launcher timeout is not completion'}
    $result=[regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)\r?$')
    if(!$result.Success -or [Convert]::ToUInt32($result.Groups[1].Value,16) -eq 0){
        throw 'Explicit worker loss must not be reported as successful target completion'
    }
    if($WorkerLoss){
        if([Convert]::ToUInt32($result.Groups[1].Value,16) -ne 1067 -or $target.HasExited){
            throw 'Worker failure must return 1067 without actively terminating its native target'
        }
        Write-Output 'PASS unexpected NTVWM death returns 1067 while its native target survives'
    }else{
        Write-Output "PASS real close (frontend-loss=$FrontendLoss): worker=$($worker.Id) target=$($target.Id) target-exit=$($target.ExitCode)"
    }
    if($TwoSessions){
        if($otherWorker.HasExited -or $otherTarget.HasExited){throw 'Closing one worker ended the independent session'}
        $gate.Set()|Out-Null
        if(!$otherObserver.WaitForExit(15000)){throw 'Independent session stopped responding'}
        $otherRecord=Get-Content $otherReport -Raw
        $otherScreen=Get-Content ($otherReport+'.console.txt') -Raw
        if($otherRecord -notmatch '(?m)^result=exited\r?$' -or
            $otherRecord -notmatch '(?m)^exit=0x00000017\r?$' -or
            $otherRecord -notmatch '(?m)^scripted-console-input=delivered\r?$' -or
            $otherScreen -notmatch '(?m)^\[\d+\] ISOLATED-SESSION-OK\s*$'){
            throw 'Independent session did not echo after management close and return 23'
        }
        Write-Output 'PASS independent Console survives selected worker close, accepts new input and returns 23'
    }
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$old
    $env:MVDM_OBSERVER_INPUT_GATE=$oldGate
    # Test started only after proving no broker existed. Restrict cleanup to
    # exact isolated package paths and pinned test process objects.
    foreach($process in @($target,$worker,$monitor,$observerProcess,$otherTarget,$otherWorker,$otherObserver,$frontend)){
        if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    }
    if($gate){$gate.Dispose()}
    foreach($process in @(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})){
        Stop-Process -Id $process.ProcessId -Force -ErrorAction SilentlyContinue
    }
}
