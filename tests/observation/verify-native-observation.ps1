[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$RuntimeRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$MonitorRpc,
 [Parameter(Mandatory)][string]$Probe,
 [Parameter(Mandatory)][string]$LogRoot,
 [ValidateSet('ChildFirst','RootFirst','ForcedChild','ShortChild','SuspendedChild','Concurrent','CmdNested','ReportDenied','ReportSlow')][string]$Case='ChildFirst'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$physical=(Resolve-Path $RuntimeRoot).Path
$scope=New-IsolatedPackageScope $physical
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Fresh build evidence required'}
if(Test-Path Z:\){throw 'Z: occupied'}
New-Item -ItemType Directory -Path $log|Out-Null
$fixture=Join-Path $physical 'tests\OBS';New-Item -ItemType Directory -Path $fixture -Force|Out-Null
Copy-Item -LiteralPath (Resolve-Path $Probe).Path -Destination (Join-Path $fixture 'P.EXE')
$probeTool=(Resolve-Path $MonitorRpc).Path;$observerPath=(Resolve-Path $Observer).Path
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$events=@();$observerProcess=$null;$childProcess=$null;$mapped=$false
function Read-Trace([int]$WorkerId,[string]$Name) {
 $text=& $probeTool --trace-json $WorkerId
 if($LASTEXITCODE){throw 'Actual trace RPC failed'}
 $text|Set-Content (Join-Path $log "$Name.json")
 return (($text -join "`n")|ConvertFrom-Json)
}
try {
 subst.exe Z: $physical;if($LASTEXITCODE){throw 'subst failed'};$mapped=$true
 $scope.Paths+=@($scope.Paths|ForEach-Object {Join-Path 'Z:\' $_.Substring($physical.Length+1)})
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $prefix='Local\native-trace-'+[guid]::NewGuid().ToString('N')
 foreach($suffix in @('ready','root','child','root-host','root-done','root-resume')) {
  $events+=,[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,"$prefix-$suffix")
 }
 $report=Join-Path $log 'observer.txt';$journal='Z:\tests\OBS\J.TXT'
 if(Test-Path (Join-Path $fixture 'J.TXT')){Remove-Item -LiteralPath (Join-Path $fixture 'J.TXT')}
 # The unhooked harness host owns the borrowed visible Console until the
 # test ends. Direct completion must not accidentally close the test Console.
 $hostMode=if($Case -eq 'ShortChild'){'host-fast'}elseif($Case -eq 'SuspendedChild'){'host-suspended'}elseif($Case -eq 'Concurrent'){'host-concurrent'}elseif($Case -eq 'CmdNested'){'host-cmd'}else{'host'}
 $arguments=@('Z:\tests\OBS\P.EXE','Z:\',$report,$hostMode,
  "$prefix-ready","$prefix-root","$prefix-child",$journal,'--observation-timeout-ms','20000')
 $observerProcess=Start-Process -FilePath $observerPath -ArgumentList $arguments -WindowStyle Hidden -PassThru
 if(!$events[0].WaitOne(10000)){throw 'Actual controlled child did not enter'}
 $deadline=[DateTime]::UtcNow.AddSeconds(3);$rootPid=0;$childPid=0
 do {
  $lines=@(Get-Content (Join-Path $fixture 'J.TXT'))
  foreach($line in $lines) {
   if($line -match '^(ROOT|CHILD|CREATED) (\d+) width=(32|64) hook=1$') {
    if($Matches[1] -eq 'ROOT'){$rootPid=[int]$Matches[2]}else{$childPid=[int]$Matches[2]}
   }
  }
  if($rootPid -and $childPid){break}
  Start-Sleep -Milliseconds 20
 }while([DateTime]::UtcNow -lt $deadline)
 if(!$rootPid -or !$childPid){throw 'This run did not prove both Hook installations/identities'}
 $treeText=& $probeTool --tree-json
 if($LASTEXITCODE){throw 'Management snapshot failed'}
 $tree=@(($treeText -join "`n")|ConvertFrom-Json)
 $workers=@($tree|Where-Object {$_.category -eq 2 -and $_.kind -in @(2,3)})
 if($workers.Count -ne 1 -or $workers[0].stack -ne 1){throw 'Observed child changed Direct stack'}
 $worker=$workers[0]
 if($Case -eq 'Concurrent') {
  $journalRows=@(Get-Content (Join-Path $fixture 'J.TXT'))
  $childIds=@($journalRows|Where-Object {$_ -match '^CHILD \d+ width=(32|64) hook=1$'}|ForEach-Object {[int](($_ -split ' ')[1])})
  if($childIds.Count -ne 16 -or @($childIds|Sort-Object -Unique).Count -ne 16){throw 'Sixteen actual hooked child witnesses not proved'}
  $deadline=[DateTime]::UtcNow.AddSeconds(3)
  do {
   $trace=Read-Trace $worker.pid 'concurrent'
   $observed=@($trace.nodes|Where-Object {$_.relation -eq 2})
   if($observed.Count -eq 16 -and !@($observed|Where-Object {$_.state -ne 2}).Count){break}
   Start-Sleep -Milliseconds 20
  }while([DateTime]::UtcNow -lt $deadline)
  $direct=@($trace.nodes|Where-Object {$_.relation -eq 1 -and $_.pid -eq $rootPid})
  if($direct.Count -ne 1 -or $observed.Count -ne 16){throw 'Concurrent CREATE lost or duplicate'}
  foreach($node in $observed) {
   if($node.pid -notin $childIds -or $node.parent -ne $direct[0].node -or
      $node.state -ne 2 -or !($node.flags -band 8) -or $node.exit -ne 23){throw 'Concurrent identity/order/source/exit differs'}
  }
  if(@($observed.node|Sort-Object -Unique).Count -ne 16 -or $events[4].WaitOne(0)){throw 'Concurrent facts changed Direct or duplicated IDs'}
  $null=$events[1].Set();if(!$events[4].WaitOne(5000)){throw 'Concurrent Direct did not complete'}
  $null=$events[3].Set();if(!$observerProcess.WaitForExit(5000)){throw 'Concurrent host did not complete'}
  if($observerProcess.ExitCode -or (Get-Content $report -Raw) -notmatch '(?m)^exit=0x00000025\r?$'){throw 'Concurrent Direct37 changed'}
  'PASS actual 16 concurrent short children: no missing/duplicate CREATE/EXIT; Direct37 unchanged'
  return
 }
 $created=Read-Trace $worker.pid 'created'
 if($Case -in @('ReportDenied','ReportSlow')) {
  $direct=@($created.nodes|Where-Object {$_.relation -eq 1 -and $_.pid -eq $rootPid})
  if($direct.Count -ne 1 -or !($created.coverage -band 1)){throw 'Observation gap not explicit'}
  if($Case -eq 'ReportDenied' -and @($created.nodes|Where-Object {$_.pid -eq $childPid}).Count){throw 'Denied report created a fake child fact'}
  $duration=@($lines|Where-Object {$_ -match '^CREATEMS (\d+) '})
  if($Case -eq 'ReportSlow' -and ($duration.Count -ne 1 -or [int](($duration[0] -split ' ')[1]) -ge 1500)){
   throw 'Observer publication blocked native creation for the injected 2000ms'
  }
  $childProcess=Get-Process -Id $childPid;$null=$childProcess.Handle
  $null=$events[2].Set();if(!$childProcess.WaitForExit(5000) -or $childProcess.ExitCode -ne 23){throw 'Report fault changed actual child23'}
  if($events[4].WaitOne(0)){throw 'Report fault completed Direct early'}
  $null=$events[1].Set();if(!$events[4].WaitOne(5000)){throw 'Report fault stranded Direct'}
  $null=$events[3].Set();if(!$observerProcess.WaitForExit(5000)){throw 'Report fault stranded host'}
  if($observerProcess.ExitCode -or (Get-Content $report -Raw) -notmatch '(?m)^exit=0x00000025\r?$'){throw 'Report fault changed Direct37'}
  "PASS actual ${Case}: native creation/child23/Direct37 survive; gap explicit"
  return
 }
 $rootRelation=if($Case -eq 'CmdNested'){2}else{1}
 $root=@($created.nodes|Where-Object {$_.pid -eq $rootPid -and $_.relation -eq $rootRelation})
 if($Case -eq 'CmdNested') {
  $cmdRoot=@($created.nodes|Where-Object {$_.relation -eq 1 -and $_.image -match '(?i)\\cmd\.exe$'})
  $cmdInner=@($created.nodes|Where-Object {$_.relation -eq 2 -and $_.image -match '(?i)\\cmd\.exe$'})
  if($cmdRoot.Count -ne 1 -or $cmdInner.Count -ne 1 -or $created.nodes.Count -ne 4 -or
     $cmdInner[0].parent -ne $cmdRoot[0].node -or $root.Count -ne 1 -or $root[0].parent -ne $cmdInner[0].node){
   throw 'Actual CMD/CMD/probe chain missing or wrong; no process-name ancestry guesses'
  }
 }
 $child=@($created.nodes|Where-Object {$_.pid -eq $childPid -and $_.relation -eq 2})
 if($root.Count -ne 1 -or $child.Count -ne 1 -or !$child[0].node -or
  $child[0].parent -ne $root[0].node -or $child[0].source -ne 2 -or
  ($Case -ne 'ShortChild' -and $child[0].state -ne 1)){throw 'Real child CREATE/parent/source missing'}
 if($Case -eq 'ShortChild') {
  if((Get-Content (Join-Path $fixture 'J.TXT') -Raw) -notmatch '(?m)^CHILDRESULT 23 '){throw 'Actual short child did not return23'}
 }else{
  $childProcess=Get-Process -Id $childPid;$null=$childProcess.Handle
  # Before the initial thread runs, MainModule may not exist. The unique
  # CreateProcess witness plus authenticated retained-object image is the
  # ownership evidence; don't make absence of user-mode LDR data a failure.
  if($child[0].image -notin @('Z:\tests\OBS\P.EXE',(Join-Path $fixture 'P.EXE'))){throw 'Child identity mismatch'}
 }
 if($Case -eq 'SuspendedChild') {
  if(!($child[0].flags -band 4) -or @($lines|Where-Object {$_ -match '^CHILD '}).Count){throw 'Caller suspended flag/entry boundary changed'}
  $null=$events[5].Set()
 }
 if($Case -eq 'RootFirst') {
  $null=$events[1].Set()
  if(!$events[4].WaitOne(5000)){throw 'Direct root did not return independently'}
  $result=Get-Content (Join-Path $fixture 'J.TXT') -Raw
  if($result -notmatch '(?m)^DIRECTRESULT 37 ' -or $childProcess.HasExited){throw 'Observed child changed direct37/lifetime'}
 }elseif($Case -eq 'ForcedChild'){$childProcess.Kill()}
 if($Case -ne 'ForcedChild'){$null=$events[2].Set()}
 if($childProcess -and !$childProcess.WaitForExit(5000)){throw 'Controlled child did not exit'}
 $expected=if($Case -eq 'ShortChild'){23}else{$childProcess.ExitCode}
 $expectedBits=[BitConverter]::ToUInt32([BitConverter]::GetBytes([int]$expected),0)
 $deadline=[DateTime]::UtcNow.AddSeconds(3)
 do {
  $finished=Read-Trace $worker.pid 'finished'
  $node=@($finished.nodes|Where-Object {$_.pid -eq $childPid})
  if($node.Count -eq 1 -and $node[0].state -eq 2){break}
  Start-Sleep -Milliseconds 20
 }while([DateTime]::UtcNow -lt $deadline)
 if($node.Count -ne 1 -or $node[0].state -ne 2 -or !($node[0].flags -band 8) -or
   [uint32]$node[0].exit -ne $expectedBits){throw 'True kernel EXIT/code not recorded'}
 if($Case -ne 'RootFirst') {
  if($events[4].WaitOne(0)){throw 'Observed exit completed Direct root'}
  $null=$events[1].Set()
  if(!$events[4].WaitOne(5000)){throw 'Direct root did not return'}
 }
 $null=$events[3].Set()
 if(!$observerProcess.WaitForExit(5000)){throw 'Harness host did not return'}
 if($observerProcess.ExitCode -or (Get-Content $report -Raw) -notmatch '(?m)^exit=0x00000025\r?$'){
  throw 'Direct37/observer result changed'
 }
 "PASS actual $Case CREATE/EXIT, real object parent, observation-only state and Direct37"
}finally {
 # Release this invocation's named gates first, including on a failed oracle;
 # don't strand an unselected fixture descendant behind a killed worker.
 foreach($index in @(1,2,3,5)){if($events.Count -gt $index){$null=$events[$index].Set()}}
 if($observerProcess -and !$observerProcess.HasExited){$null=$observerProcess.WaitForExit(3000)}
 if($childProcess){if(!$childProcess.HasExited){$childProcess.Kill();$null=$childProcess.WaitForExit(5000)};$childProcess.Dispose()}
 if($observerProcess){if(!$observerProcess.HasExited){$observerProcess.Kill();$null=$observerProcess.WaitForExit(5000)};$observerProcess.Dispose()}
 foreach($event in $events){$event.Dispose()}
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
 try {Stop-IsolatedPackageScope $scope} finally {if($mapped){subst.exe Z: /d}}
}
