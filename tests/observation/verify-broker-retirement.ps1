[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogRoot,
    [Parameter(Mandatory)][string]$LogPrefix
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
foreach($name in @('run16.exe','ntkvm.exe','ntsrv.exe','ntvdm.exe','ntw32.exe')){
    $actual=Join-Path $physical $name;$launch=Join-Path $PackageRoot $name
    if((Get-FileHash $actual).Hash -ne (Get-FileHash $launch).Hash){throw 'Candidate mismatch'}
    $paths+=@($actual,$launch)
}
$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($kind in @('ntvdm','ntw32')){
        foreach($fault in @('worker','frontend')){
            $report=Join-Path $LogRoot "$LogPrefix-$kind-$fault.txt"
            if(Test-Path $report){throw 'Use fresh evidence'}
            $observerProcess=$null;$worker=$null;$frontend=$null;$broker=$null;$nativeTarget=$null
            try {
                $target=if($kind -eq 'ntvdm'){@('command')}else{@('cmd.exe','/d','/k')}
                $observerProcess=Start-Process -FilePath (Resolve-Path $Observer).Path -WindowStyle Hidden -PassThru -ArgumentList (
                    @((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report)+$target+@('--observation-timeout-ms','30000'))
                $until=[DateTime]::UtcNow.AddSeconds(15)
                do {
                    $workerRows=@(Get-CimInstance Win32_Process -Filter "Name='$kind.exe'" | Where-Object {$_.ExecutablePath -in $paths})
                    $frontRows=@(Get-CimInstance Win32_Process -Filter "Name='ntkvm.exe'" | Where-Object {$_.ExecutablePath -in $paths})
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
                if($kind -eq 'ntw32'){
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
                if(!$frontend.WaitForExit(8000)){throw 'Broker did not retire workerless frontend'}
                if(!$worker.WaitForExit(8000)){throw 'Worker did not obey broker frontend-loss instruction'}
                if(!$observerProcess.WaitForExit(10000)){throw 'Direct launcher failed to return'}
                $text=Get-Content $report -Raw
                if($text -notmatch '(?m)^result=exited\r?$'){throw 'Timeout is not task completion'}
                $exit=[regex]::Match($text,'(?m)^exit=0x([0-9a-f]+)\r?$')
                if(!$exit.Success -or [Convert]::ToUInt32($exit.Groups[1].Value,16) -eq 0){throw 'Fault was reported as success'}
                # Nothing remains registered: now, and only now, empty grace applies.
                if(!$broker.WaitForExit(15000)){throw 'Empty broker failed to retire after its grace'}
                Write-Output "PASS $kind $fault loss: broker-ordered peer retirement, failed direct receipt, empty service retirement"
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
