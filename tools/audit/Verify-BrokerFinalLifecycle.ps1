[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [string]$PackageRoot='O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$BarrierBroker,
    [switch]$BrokerLoss,
    [switch]$WorkerLoss,
    [switch]$LauncherLoss,
    [switch]$TwoWorkers
)
$ErrorActionPreference='Stop'
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$trace=Join-Path $PackageRoot "logs\$LogPrefix.trace"
if(Test-Path -LiteralPath $trace){throw 'Use a fresh log prefix'}
$paths=@('run16.exe','basesrv.exe','ntvdm.exe') | ForEach-Object {Join-Path $PackageRoot $_}
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='basesrv.exe' OR Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -in $paths})
}
if((PackageProcesses).Count){throw 'Package already in use'}
$owned=[Collections.Generic.HashSet[int]]::new()
$observers=[Collections.Generic.List[object]]::new()
$results=[Collections.Generic.List[string]]::new()
$previous=[Environment]::GetEnvironmentVariable('MVDM_BASESRV_TRACE_PATH')
$previousGate=[Environment]::GetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE')
$inputGate=$null
function TraceText {if(Test-Path -LiteralPath $trace){Get-Content -LiteralPath $trace -Raw}else{''}}
function CollectOwned {
    foreach($o in $observers){
        if(Test-Path -LiteralPath $o.Report){
            $m=[regex]::Match((Get-Content -LiteralPath $o.Report -Raw),'(?m)^pid=(\d+)')
            if($m.Success){[void]$owned.Add([int]$m.Groups[1].Value)}
        }
    }
    for($depth=0;$depth -lt 8;++$depth){
        $new=@(PackageProcesses | Where-Object {$owned.Contains([int]$_.ParentProcessId) -and !$owned.Contains([int]$_.ProcessId)})
        foreach($p in $new){[void]$owned.Add([int]$p.ProcessId)}
        if(!$new.Count){break}
    }
}
function StartObserved([string]$name,[string]$target,[string]$inputText=''){
    $report=Join-Path $PackageRoot "logs\$LogPrefix-$name.txt"
    $args=@((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report,$target,'--observation-timeout-ms','20000')
    if($inputText){$args+=@('--observe-console-input-text',('"'+$inputText+'"'))}
    $start=[Diagnostics.ProcessStartInfo]::new($Observer,($args -join ' '))
    $start.UseShellExecute=$false
    $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.WorkingDirectory=$PackageRoot
    foreach($key in @('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INPUT_GATE','MVDM_BASESRV_TRACE_PATH')){
        $value=[Environment]::GetEnvironmentVariable($key)
        if($value){$start.EnvironmentVariables[$key]=$value}
    }
    $p=[Diagnostics.Process]::Start($start)
    [void]$owned.Add($p.Id)
    $o=[pscustomobject]@{Process=$p;Report=$report;Name=$name}
    $observers.Add($o)
    return $o
}
function FinishObserved($o,[int]$expected){
    if(!$o.Process.WaitForExit(55000)){throw "Observer timeout: $($o.Name)"}
    CollectOwned
    $record=Get-Content -LiteralPath $o.Report -Raw
    $match=[regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)')
    if($record -notmatch '(?m)^result=exited' -or !$match.Success){throw "No exit: $($o.Name)"}
    $actual=[Convert]::ToUInt32($match.Groups[1].Value,16)
    if($actual -ne $expected){throw "$($o.Name): $actual expected $expected"}
    $results.Add("PASS $($o.Name): $actual")
}
function StopOwnedWorkers {
    CollectOwned
    foreach($p in (PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe' -and $owned.Contains([int]$_.ProcessId)})){
        Stop-Process -Id $p.ProcessId
    }
}
try {
    [Environment]::SetEnvironmentVariable('MVDM_BASESRV_TRACE_PATH',$trace)
    if($BrokerLoss -or $WorkerLoss -or $LauncherLoss){
        $inputGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmS10BrokerLost')
        [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE','Local\MvdmS10BrokerLost')
        $loss=StartObserved 'broker-loss' 'COMMAND.COM' "exit`r"
        $other=if($TwoWorkers){StartObserved 'unrelated-worker' 'COMMAND.COM' "exit`r"}else{$null}
        $requiredTasks=if($TwoWorkers){2}else{1}
        $deadline=[DateTime]::UtcNow.AddSeconds(15)
        # Production trace hooks have been retired. The observer captures the
        # real COMMAND prompt before blocking input; require that evidence.
        $ready=@($loss); if($other){$ready+=@($other)}
        do {
            $readyCount=@($ready | Where-Object {
                $screen=$_.Report+'.pre-input-console.txt.console.txt'
                (Test-Path -LiteralPath $screen) -and
                    ((Get-Content -LiteralPath $screen -Raw) -match '(?i)[a-z]:\\[^\r\n]*>')
            }).Count
            if($readyCount -eq $requiredTasks){break}
            Start-Sleep -Milliseconds 50
        } while([DateTime]::UtcNow -lt $deadline)
        if($readyCount -ne $requiredTasks){throw 'No real COMMAND prompt before loss test'}
        CollectOwned
        $servers=@(PackageProcesses | Where-Object {$_.Name -eq 'basesrv.exe' -and $owned.Contains([int]$_.ProcessId)})
        if($servers.Count -ne 1){throw 'Cannot identify owned broker'}
        $workers=@(PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe' -and $owned.Contains([int]$_.ProcessId)})
        $launchers=@(PackageProcesses | Where-Object {$_.Name -eq 'run16.exe' -and $owned.Contains([int]$_.ProcessId)})
        # The private-desktop observer may have a relay parent. Its report
        # identifies the actual launched product process in either mode.
        $launchMatch=[regex]::Match((Get-Content -LiteralPath $loss.Report -Raw),'(?m)^pid=(\d+)')
        if(!$launchMatch.Success){throw 'Observer has not recorded its launcher'}
        $launcher=@($launchers | Where-Object {$_.ProcessId -eq [int]$launchMatch.Groups[1].Value})
        if($launcher.Count -ne 1){throw 'Cannot identify observer-owned launcher'}
        $launcherId=[int]$launcher[0].ProcessId
        $worker=@($workers | Where-Object {$_.ParentProcessId -eq $launcherId})
        if($workers.Count -ne $requiredTasks -or $worker.Count -ne 1){throw 'Cannot identify owned pairs'}
        $victim=if($WorkerLoss){$worker[0]}elseif($LauncherLoss){$launchers | Where-Object {$_.ProcessId -eq $launcherId}}else{$servers[0]}
        Stop-Process -Id $victim.ProcessId
        if($TwoWorkers -and !$BrokerLoss){
            Start-Sleep -Milliseconds 500
            $otherWorker=$workers | Where-Object {$_.ProcessId -ne $worker[0].ProcessId}
            if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $otherWorker.ProcessId})){throw 'Unrelated worker was terminated'}
        }
        if($LauncherLoss -and !(PackageProcesses | Where-Object {$_.ProcessId -eq $worker[0].ProcessId})){throw 'Claimed worker killed with launcher'}
        [void]$inputGate.Set()
        if(!$loss.Process.WaitForExit(55000)){throw 'Broker-loss observer timeout'}
        $record=Get-Content -LiteralPath $loss.Report -Raw
        if($record -notmatch '(?m)^result=exited' -or $record -match '(?m)^exit=0x00000000'){
            throw 'Process loss did not produce a bounded explicit failure'
        }
        $lossExit=[regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)')
        if(!$lossExit.Success){throw 'Missing explicit failure code'}
        $actualLoss=[Convert]::ToUInt32($lossExit.Groups[1].Value,16)
        if($BrokerLoss -and $actualLoss -ne 1722){throw 'Broker loss must return 1722'}
        if($WorkerLoss -and $actualLoss -ne 1067){throw 'Worker loss must return 1067'}
        if($other){FinishObserved $other $(if($BrokerLoss){1722}else{0})}
        if($BrokerLoss){
            $deadline=[DateTime]::UtcNow.AddSeconds(5)
            while((PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe'}) -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 50}
            if(PackageProcesses | Where-Object {$_.Name -eq 'ntvdm.exe'}){throw 'Worker survived broker loss'}
            $results.Add('PASS broker loss: launcher and worker exit, no replay')
        } else {
            if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $servers[0].ProcessId})){throw 'Unrelated broker terminated'}
            $results.Add("PASS $($victim.Name) loss: bounded parent outcome, broker survives")
            StopOwnedWorkers
        }
        Start-Sleep -Seconds 2
        $after=StartObserved 'after-loss' 'MEM.EXE'; FinishObserved $after 0
        $memText=(Get-Content -LiteralPath ($after.Report+'.console.txt') -Raw) -replace '\s',''
        if($memText -notmatch 'bytesavailableXMSmemory'){throw 'MEM did not print its real memory report after loss'}
    } elseif($BarrierBroker){
        $BarrierBroker=(Resolve-Path -LiteralPath $BarrierBroker).Path
        if($BarrierBroker -notmatch '\\build\\M0-T412\\S10\\'){throw 'Barrier binary must remain in S10 build'}
        $ready=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmS10TimerReady')
        $release=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmS10TimerRelease')
        $barrier=Start-Process -FilePath $BarrierBroker -WindowStyle Hidden -PassThru
        try {
            if(!$ready.WaitOne(75000)){throw 'Timer callback barrier not reached'}
            $arrival=StartObserved 'callback-arrival' 'MEM.EXE'
            Start-Sleep -Milliseconds 750
            [void]$release.Set()
            FinishObserved $arrival 0
            if(!$barrier.WaitForExit(10000)){throw 'Old empty broker did not exit'}
            if((TraceText) -notmatch 'phase=empty-stop'){throw 'Missing stopped epoch'}
            $results.Add('PASS callback-in-progress: original callback wins; launcher retries against fresh broker')
        } finally {
            [void]$release.Set()
            if(!$barrier.HasExited){Stop-Process -Id $barrier.Id}
            $ready.Dispose();$release.Dispose()
        }
    } else {
        $first=StartObserved 'concurrent-a' 'MEM.EXE'
        $second=StartObserved 'concurrent-b' 'MEM.EXE'
        FinishObserved $first 0; FinishObserved $second 0
        $servers=@(PackageProcesses | Where-Object {$_.Name -eq 'basesrv.exe'})
        if($servers.Count -ne 1 -or !$owned.Contains([int]$servers[0].ProcessId)){throw 'Not exactly one owned broker'}
        $serverId=[int]$servers[0].ProcessId
        $text=TraceText
        if([regex]::Matches($text,'phase=prepare ').Count -ne 2 -or [regex]::Matches($text,'phase=first-yes ').Count -ne 1){throw 'Concurrent command/first identity mismatch'}
        $duplicate=Start-Process -FilePath (Join-Path $PackageRoot 'basesrv.exe') -WindowStyle Hidden -PassThru
        [void]$owned.Add($duplicate.Id)
        if(!$duplicate.WaitForExit(5000) -or $duplicate.ExitCode -ne 1740){throw 'Duplicate broker did not reject endpoint'}
        $results.Add('PASS concurrent startup and singleton: two commands, one broker, one first-VDM')
        StopOwnedWorkers
        Start-Sleep -Seconds 2
        if((TraceText) -notmatch 'phase=worker-process-cleanup'){throw 'No original abrupt-worker cleanup'}
        $again=StartObserved 'after-worker-exit' 'MEM.EXE'; FinishObserved $again 0
        if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $serverId})){throw 'Broker lost on worker cleanup'}
        StopOwnedWorkers
        Start-Sleep -Seconds 2
        $before=TraceText
        if($before -notmatch 'phase=empty-grace'){throw 'No empty grace'}
        # Arrival during the real production 60-second grace cancels this epoch.
        Start-Sleep -Seconds 35
        $arrival=StartObserved 'grace-arrival' 'MEM.EXE'; FinishObserved $arrival 0
        if(!(PackageProcesses | Where-Object {$_.ProcessId -eq $serverId})){throw 'Arrival failed to retain broker'}
        StopOwnedWorkers
        $deadline=[DateTime]::UtcNow.AddSeconds(75)
        while((PackageProcesses | Where-Object {$_.ProcessId -eq $serverId}) -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Seconds 1}
        if(PackageProcesses | Where-Object {$_.ProcessId -eq $serverId}){throw 'Empty broker did not stop'}
        if((TraceText) -notmatch 'phase=empty-stop'){throw 'No empty-stop witness'}
        $restart=StartObserved 'restart' 'MEM.EXE'; FinishObserved $restart 0
        if([regex]::Matches((TraceText),'phase=first-yes ').Count -ne 2){throw 'Restart did not reset first-VDM exactly once'}
        $results.Add('PASS abrupt cleanup, grace arrival cancellation, empty stop and fresh first-VDM')
        $wow=StartObserved 'wow-boundary' 'system32\WRITE.EXE'
        if(!$wow.Process.WaitForExit(55000)){throw 'WOW observer did not finish'}
        CollectOwned
        $wowRecord=Get-Content -LiteralPath $wow.Report -Raw
        if((TraceText) -notmatch 'phase=get-dispatched state=00000102 status=00000000'){throw 'No successful shared WOW PIF request'}
        $results.Add('PASS real worker shared-WOW PIF acquisition; completion is separately tested through original service fixture')
        if($wowRecord -match '(?m)^result=timeout'){
            $results.Add('LIMIT: WRITE timed out at 20 seconds after successful broker acquisition; not workload acceptance')
        } elseif($wowRecord -match '(?m)^result=exited'){
            $results.Add('OBSERVATION: WRITE returned; see exact exit code in report, not workload acceptance')
        } else {throw 'Unclassified WOW observation'}
    }
} finally {
    CollectOwned
    foreach($p in (PackageProcesses | Where-Object {$owned.Contains([int]$_.ProcessId)})){Stop-Process -Id $p.ProcessId -ErrorAction SilentlyContinue}
    foreach($o in $observers){if(!$o.Process.HasExited){Stop-Process -Id $o.Process.Id -ErrorAction SilentlyContinue}}
    [Environment]::SetEnvironmentVariable('MVDM_BASESRV_TRACE_PATH',$previous)
    [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE',$previousGate)
    if($inputGate){[void]$inputGate.Set();$inputGate.Dispose()}
    $results | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-results.txt") -Encoding utf8
    $results | Write-Output
}
