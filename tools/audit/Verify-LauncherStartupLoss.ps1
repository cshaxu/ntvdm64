[CmdletBinding()]
param([Parameter(Mandatory)][string]$LogPrefix)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid prefix'}
$root=(Resolve-Path build/M0-T412/S10/startup-barrier).Path
$results=[Collections.Generic.List[string]]::new()
foreach($case in @(@{Stage='before';Victim='launcher'},@{Stage='after';Victim='launcher'},@{Stage='before';Victim='broker'})){
    $ready=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmLaunchReady')
    $release=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\MvdmLaunchRelease')
    $oldTrace=$env:MVDM_BASESRV_TRACE_PATH
    $env:MVDM_BASESRV_TRACE_PATH="O:\winnt\logs\$LogPrefix-$($case.Stage)-$($case.Victim).trace"
    if(Test-Path $env:MVDM_BASESRV_TRACE_PATH){throw 'Fresh prefix required'}
    $launcher=$null;$children=@();$waiter=$null
    try {
        $existing=@(Get-CimInstance Win32_Process | Where-Object {$_.Name -in @('basesrv.exe','ntvdm.exe','run16.exe') -and ($_.ExecutablePath -like "$root\*" -or $_.ExecutablePath -like 'O:\winnt\*')})
        if($existing.Count){throw 'Package or fixture already running'}
        $launcher=Start-Process (Join-Path $root "$($case.Stage).exe") -ArgumentList 'O:\winnt\COMMAND.COM' -WorkingDirectory O:\winnt -WindowStyle Hidden -PassThru
        if(!$ready.WaitOne(15000)){throw 'Launch cut point not reached'}
        $children=@(Get-CimInstance Win32_Process | Where-Object {$_.ParentProcessId -eq $launcher.Id -and
            ($_.ExecutablePath -like "$root\*" -or (!$_.ExecutablePath -and $_.Name -eq 'ntvdm.exe'))})
        $children | Select-Object ProcessId,ParentProcessId,Name,ExecutablePath | ConvertTo-Json | Set-Content "O:\winnt\logs\$LogPrefix-$($case.Stage)-$($case.Victim)-children.json"
        $worker=@($children | Where-Object {$_.Name -eq 'ntvdm.exe'})
        $broker=@($children | Where-Object {$_.Name -eq 'basesrv.exe'})
        if($worker.Count -ne 1 -or $broker.Count -ne 1){throw 'Cannot identify owned startup children'}
        $waiter=Get-Process -Id $worker[0].ProcessId
        if($case.Victim -eq 'launcher'){Stop-Process -Id $launcher.Id}
        else {Stop-Process -Id $broker[0].ProcessId; [void]$release.Set()}
        if(!$waiter.WaitForExit(10000)){throw 'Suspended worker leaked'}
        if(!$launcher.WaitForExit(10000) -or $launcher.ExitCode -eq 0){throw 'Launcher failed to report loss'}
        $results.Add("PASS $($case.Stage) Prepare, $($case.Victim) death: suspended worker exited")
    } finally {
        [void]$release.Set()
        if($launcher -and !$launcher.HasExited){Stop-Process -Id $launcher.Id}
        foreach($child in $children){
            $live=Get-CimInstance Win32_Process -Filter "ProcessId=$($child.ProcessId)"
            if($live -and $live.CreationDate -eq $child.CreationDate -and $live.ExecutablePath -eq $child.ExecutablePath){Stop-Process -Id $child.ProcessId}
        }
        if($waiter){$waiter.Dispose()}
        if($launcher){$launcher.Dispose()}
        $ready.Dispose();$release.Dispose();$env:MVDM_BASESRV_TRACE_PATH=$oldTrace
    }
}
$results | Set-Content "O:\winnt\logs\$LogPrefix-results.txt"
$results
