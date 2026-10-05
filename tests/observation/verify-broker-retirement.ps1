[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [switch]$FullDeadlines
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$physical=(Resolve-Path $ProcessPackageRoot).Path
if(!$physical.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){
    throw 'Require an isolated build package'
}
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Existing broker must not be controlled'}
$paths=@()
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$physicalBinary=Get-PackageBinaryRoot $physical
$launchBinary=Get-PackageBinaryRoot $PackageRoot
$nativeNames=@(Get-PackageNativeWorkerNames $physical)
$nativeFilter=($nativeNames | ForEach-Object {"Name='$_'"}) -join ' OR '
foreach($name in @('run16.exe','ntcon.exe','ntsrv.exe','ntvdm.exe')+$nativeNames){
    $actual=Join-Path $physicalBinary $name;$launch=Join-Path $launchBinary $name
    if((Get-FileHash $actual).Hash -ne (Get-FileHash $launch).Hash){throw 'Candidate mismatch'}
    $paths+=@($actual,$launch)
}
$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($kind in @('ntvdm','ntvwm')){
        # Routine gate checks real frontend-loss shutdown/receipt wiring.
        # Workerless and empty 10s rules are covered by the production policy
        # fixture with explicit time. FullDeadlines retains the old slow
        # end-to-end timer diagnostic, but is not a routine per-P gate.
        $faults=if($FullDeadlines){@('worker','frontend')}else{@('frontend')}
        foreach($fault in $faults){
            $report=Join-Path $LogRoot "$LogPrefix-$kind-$fault.txt"
            if(Test-Path $report){throw 'Use fresh evidence'}
            $observerProcess=$null;$worker=$null;$frontend=$null;$broker=$null;$nativeTarget=$null
            try {
                $target=if($kind -eq 'ntvdm'){@('command')}else{@('cmd.exe','/d','/k')}
                $observerProcess=Start-Process -FilePath (Resolve-Path $Observer).Path -WindowStyle Hidden -PassThru -ArgumentList (
                    @((Join-Path $launchBinary 'run16.exe'),$PackageRoot,$report)+$target+@('--observation-timeout-ms','30000'))
                $until=[DateTime]::UtcNow.AddSeconds(15)
                do {
                    $workerFilter=if($kind -eq 'ntvwm'){$nativeFilter}else{"Name='ntvdm.exe'"}
                    $workerRows=@(Get-CimInstance Win32_Process -Filter $workerFilter | Where-Object {$_.ExecutablePath -in $paths})
                    $frontRows=@(Get-CimInstance Win32_Process -Filter "Name='ntcon.exe'" | Where-Object {$_.ExecutablePath -in $paths})
                    $brokerRows=@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'" | Where-Object {$_.ExecutablePath -in $paths})
                    if($workerRows.Count -eq 1 -and $frontRows.Count -eq 1 -and $brokerRows.Count -eq 1){
                        $worker=Get-Process -Id $workerRows[0].ProcessId;$null=$worker.Handle
                        $frontend=Get-Process -Id $frontRows[0].ProcessId;$null=$frontend.Handle
                        $broker=Get-Process -Id $brokerRows[0].ProcessId;$null=$broker.Handle
                        break
                    }
                    Start-Sleep -Milliseconds 50 # Test observation, not production control.
                }while([DateTime]::UtcNow -lt $until)
                if(!$worker -or !$frontend -or !$broker){throw 'Expected worker/root/broker were not registered'}
                if($kind -eq 'ntvwm'){
                    # Registration precedes CreateProcess: wait for the actual target,
                    # not merely the worker's registry presence, before injecting death.
                    do {
                        $children=@(Get-CimInstance Win32_Process -Filter "Name='cmd.exe'" | Where-Object {$_.ParentProcessId -eq $worker.Id})
                        if($children.Count -eq 1){break}
                        if($worker.HasExited -or $observerProcess.HasExited){break}
                        Start-Sleep -Milliseconds 50 # Test observation only.
                    }while([DateTime]::UtcNow -lt $until)
                    if($children.Count -ne 1){throw 'Expected the pinned native target before fault injection'}
                    $nativeTarget=Get-Process -Id $children[0].ProcessId;$null=$nativeTarget.Handle
                }
                if($fault -eq 'worker'){$worker.Kill()}else{$frontend.Kill()}
                # The owner-approved workerless-root grace is ten seconds;
                # frontend death still closes its worker without that grace.
                if(!$frontend.WaitForExit(15000)){throw 'Broker did not retire workerless frontend after ten-second grace'}
                if(!$worker.WaitForExit(8000)){throw 'Worker did not obey broker frontend-loss instruction'}
                if(!$observerProcess.WaitForExit(10000)){throw 'Direct launcher failed to return'}
                $text=Get-Content $report -Raw
                if($text -notmatch '(?m)^result=exited\r?$'){throw 'Timeout is not task completion'}
                $exit=[regex]::Match($text,'(?m)^exit=0x([0-9a-f]+)\r?$')
                if(!$exit.Success -or [Convert]::ToUInt32($exit.Groups[1].Value,16) -ne 1067){throw 'Worker/frontend failure did not return the broker task failure 1067'}
                # Nothing remains registered: now, and only now, empty grace applies.
                if($FullDeadlines){
                    if(!$broker.WaitForExit(15000)){throw 'Empty broker failed to retire after its grace'}
                    Write-Output "PASS $kind $fault loss: broker-ordered peer retirement, failed direct receipt, empty service retirement"
                }else{
                    # Fixture owns this exact broker; cleanup is not proof of
                    # idle timer expiry, which the deterministic unit asserts.
                    Write-Output "PASS $kind frontend loss: real worker shutdown, frontend exit and direct receipt 1067; explicit fixture cleanup"
                }
            }finally{
                foreach($process in @($observerProcess,$worker,$frontend,$broker,$nativeTarget)){
                    if($process){if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)};$process.Dispose()}
                }
                foreach($process in @(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})){
                    Stop-Process -Id $process.ProcessId -Force -ErrorAction SilentlyContinue
                }
            }
        }
    }
}finally{$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$old}
