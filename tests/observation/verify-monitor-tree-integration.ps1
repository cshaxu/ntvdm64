param([Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$MonitorRpc,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$ProcessPackageRoot,
      [Parameter(Mandatory)][string]$LogRoot)
$ErrorActionPreference='Stop'
$physical=(Resolve-Path $ProcessPackageRoot).Path
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
$logs=[IO.Path]::GetFullPath($LogRoot)
if(!$logs.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or !(Test-Path $logs)){
    throw 'Require a predeclared build evidence directory'
}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$scope=New-IsolatedPackageScope $physical $PackageRoot
$saved=@{}
foreach($name in @('MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INPUT_GATE','MVDM_OBSERVER_SHORT_HISTORY','MVDM_OBSERVER_MILESTONE_INPUT')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
$children=[Collections.Generic.List[Diagnostics.Process]]::new()
$pins=[Collections.Generic.List[Diagnostics.Process]]::new()
$gate=$null;$ready=$null;$release=$null;$sequence=0
function Observe([string]$Name,[string[]]$Arguments,[string]$ScriptedText='') {
    $report=Join-Path $logs ($Name+'.txt')
    if(Test-Path $report){throw 'Require fresh evidence'}
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
    $start.UseShellExecute=$false;$start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.WorkingDirectory=$PackageRoot
    $argsList=@((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report)+$Arguments+@('--observation-timeout-ms','45000')
    if($ScriptedText){$argsList+=@('--observe-console-input-text',$ScriptedText)}
    foreach($argument in $argsList){$start.ArgumentList.Add($argument)}
    $process=[Diagnostics.Process]::Start($start);$children.Add($process)
    return [pscustomobject]@{Process=$process;Report=$report}
}
function Snapshot {
    $script:sequence++
    $out=Join-Path $logs ("snapshot-$sequence.json")
    $err=Join-Path $logs ("snapshot-$sequence.err")
    $query=Start-Process -FilePath (Resolve-Path $MonitorRpc).Path -ArgumentList '--tree-json' -WindowStyle Hidden -PassThru -RedirectStandardOutput $out -RedirectStandardError $err
    try {
        if(!$query.WaitForExit(8000) -or $query.ExitCode){throw "Snapshot RPC failed: $err"}
        $rows=Get-Content $out -Raw|ConvertFrom-Json
        return $rows
    }finally{if(!$query.HasExited){$query.Kill();$query.WaitForExit(5000)|Out-Null};$query.Dispose()}
}
function Pin([uint32]$PidValue,[string]$Name) {
    $process=Get-Process -Id $PidValue;$null=$process.Handle
    if($process.HasExited -or $process.Path -notin @((Join-Path $PackageRoot $Name),(Join-Path $physical $Name))){
        $process.Dispose();throw 'Snapshot process identity changed'
    }
    $pins.Add($process);return $process
}
function Close-Node([int]$Category,[uint32]$PidValue,[string]$Name) {
    $close=Start-Process -FilePath (Resolve-Path $MonitorRpc).Path -ArgumentList @('--close-node',$Category,$PidValue) -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $logs "$Name.close.txt") -RedirectStandardError (Join-Path $logs "$Name.close.err")
    try {if(!$close.WaitForExit(18000) -or $close.ExitCode){throw "Actual node close failed: $Name"}}
    finally{if(!$close.HasExited){$close.Kill();$close.WaitForExit(5000)|Out-Null};$close.Dispose()}
}
function Wait-Tree([scriptblock]$Condition) {
    $deadline=[DateTime]::UtcNow.AddSeconds(12)
    do {
        $rows=@(Snapshot)
        if(& $Condition $rows){return $rows}
        Start-Sleep -Milliseconds 100
    }while([DateTime]::UtcNow -lt $deadline)
    throw 'Expected current tree state did not arrive'
}
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_SHORT_HISTORY='1'
    $env:MVDM_OBSERVER_MILESTONE_INPUT='1'
    Remove-Item Env:MVDM_OBSERVER_INPUT_GATE -ErrorAction SilentlyContinue
    $dos=Observe 'dos' @('command.com')
    $rows=Wait-Tree {param($r) @($r|Where-Object {$_.category -eq 2 -and $_.kind -eq 0 -and $_.depth -eq 1}).Count -eq 1}
    $dosWorker=$rows|Where-Object {$_.category -eq 2 -and $_.kind -eq 0}
    $dosRoot=$rows|Where-Object {$_.key -eq $dosWorker.parent}
    if(!$dosRoot -or $dosRoot.category -ne 1){throw 'DOS lacks authoritative frontend parent'}
    $dosProcess=Pin $dosWorker.pid 'ntvdm.exe';$dosFrontend=Pin $dosRoot.pid 'ntcon.exe'

    $gateName='Local\monitor-tree-'+[guid]::NewGuid().ToString('N')
    $gate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,$gateName)
    $env:MVDM_OBSERVER_INPUT_GATE=$gateName
    $native=Observe 'native' @('cmd','/d','/k') "echo TREE-INDEPENDENT-OK`rexit /b 23`r"
    Remove-Item Env:MVDM_OBSERVER_INPUT_GATE -ErrorAction SilentlyContinue
    $rows=Wait-Tree {param($r) @($r|Where-Object {$_.category -eq 2 -and $_.kind -eq 2 -and $_.depth -eq 1}).Count -eq 1}
    $nativeWorker=$rows|Where-Object {$_.category -eq 2 -and $_.kind -eq 2}
    $nativeRoot=$rows|Where-Object {$_.key -eq $nativeWorker.parent}
    if(!$nativeRoot -or $nativeRoot.category -ne 1 -or $nativeRoot.key -eq $dosRoot.key){throw 'Independent Consoles share a root'}
    $nativeProcess=Pin $nativeWorker.pid 'ntvwm.exe';$nativeFrontend=Pin $nativeRoot.pid 'ntcon.exe'

    $wow=Observe 'wow' @('winmine.exe')
    $rows=Wait-Tree {param($r) @($r|Where-Object {$_.category -eq 2 -and $_.kind -eq 1 -and $_.depth -eq 0}).Count -eq 1}
    $wowWorker=$rows|Where-Object {$_.category -eq 2 -and $_.kind -eq 1}
    $wowProcess=Pin $wowWorker.pid 'ntvdm.exe'
    $rows=Wait-Tree {param($r) @($r|Where-Object {$_.category -eq 3 -and $_.parent -eq $wowWorker.key}).Count -ge 1}
    foreach($task in @($rows|Where-Object {$_.category -eq 3})){
        if($task.pid -ne 0 -or $task.depth -ne 1 -or $task.actions -ne 0 -or !$task.image){throw 'WOW task identity/readonly projection is invalid'}
    }
    $token='monitor-gui-'+[guid]::NewGuid().ToString('N')
    $ready=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,"Local\$token-ready")
    $release=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,"Local\$token-release")
    $gui=Observe 'gui' @((Join-Path $PackageRoot 'native-gui-startup-probe.exe'),$token)
    if(!$ready.WaitOne(12000) -or !$gui.Process.WaitForExit(10000)){throw 'GUI startup did not return while target lives'}
    $guiReport=Get-Content $gui.Report -Raw
    if($guiReport -notmatch '(?m)^result=exited\r?$' -or $guiReport -notmatch '(?m)^exit=0x00000000\r?$'){throw 'GUI direct startup result failed'}
    $rows=Wait-Tree {param($r) @($r|Where-Object {$_.category -eq 4}).Count -eq 1}
    $guiTarget=$rows|Where-Object {$_.category -eq 4}
    if($guiTarget.depth -ne 0 -or $guiTarget.parent -notmatch '^0+:0:0:0+$'){throw 'Detached GUI incorrectly retains a carrier parent'}
    $guiProcess=Pin $guiTarget.pid 'native-gui-startup-probe.exe'
    $rows|ConvertTo-Json -Depth 4|Set-Content (Join-Path $logs 'mixed-tree.json')

    Close-Node 1 $dosRoot.pid 'dos-root'
    if(!$dosFrontend.WaitForExit(5000) -or !$dosProcess.WaitForExit(5000) -or !$dos.Process.WaitForExit(10000)){throw 'DOS root close was acknowledged before actual shutdown'}
    $dosRecord=Get-Content $dos.Report -Raw
    if($dosRecord -notmatch '(?m)^result=exited\r?$' -or $dosRecord -notmatch '(?m)^exit=0x0000042b\r?$'){
        throw 'Frontend loss must complete the unfinished DOS request with 1067'
    }
    if($nativeProcess.HasExited -or $nativeFrontend.HasExited -or $wowProcess.HasExited -or $guiProcess.HasExited){throw 'Root close ended an unrelated identity'}
    $rows=Wait-Tree {param($r) @($r|Where-Object {$_.key -eq $dosRoot.key -or $_.key -eq $dosWorker.key}).Count -eq 0}
    if(@($rows|Where-Object {$_.key -eq $wowWorker.key}).Count -ne 1){throw 'WOW worker lost after unrelated root close'}
    'PASS actual DOS frontend close and final prune; independent native/WOW/GUI survive'

    Close-Node 4 $guiTarget.pid 'gui-target'
    if(!$guiProcess.WaitForExit(5000) -or $nativeProcess.HasExited -or $wowProcess.HasExited){throw 'GUI close completion or isolation failed'}
    $gate.Set()|Out-Null
    if(!$native.Process.WaitForExit(15000)){throw 'Independent native session no longer responds'}
    $record=Get-Content $native.Report -Raw;$screen=Get-Content ($native.Report+'.console.txt') -Raw
    if($record -notmatch '(?m)^result=exited\r?$' -or $record -notmatch '(?m)^exit=0x00000017\r?$' -or $screen -notmatch '(?m)^\[\d+\] TREE-INDEPENDENT-OK\s*$'){
        throw 'Independent native session did not consume this input and return 23'
    }
    if($wowProcess.HasExited){throw 'WOW follows unrelated text target completion'}
    'PASS actual GUI close; unrelated native consumes new input/returns 23; WOW remains independent'
}finally{
    foreach($entry in $saved.GetEnumerator()){[Environment]::SetEnvironmentVariable($entry.Key,$entry.Value)}
    # Exact pinned objects, not process names or an inferred descendant tree.
    foreach($process in $children){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    foreach($process in $pins){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
    Stop-IsolatedPackageScope $scope
    foreach($event in @($gate,$ready,$release)){if($event){$event.Dispose()}}
}
