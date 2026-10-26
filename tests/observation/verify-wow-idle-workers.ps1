param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,[Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$Nasm,[switch]$SharedTasks,[switch]$LifecycleOnly)
# Run from an owned hidden supervisor. One service/package owns both workers.
$ErrorActionPreference='Stop'
$LifecycleOnly = $LifecycleOnly.IsPresent
if($LifecycleOnly -and !$SharedTasks){throw 'LifecycleOnly requires SharedTasks'}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_WOW_WINDOW_TRACE_PATH','MVDM_WOW_DISPATCH_TRACE_PATH','MVDM_WOW_CALLBACK_FRAME_TRACE_PATH')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 $source=if($SharedTasks){"$PSScriptRoot/wow_idle_tasks_probe.c"}else{"$PSScriptRoot/wow_idle_workers_probe.c"}
 & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX "/Fo$root/host.obj" "/Fe$root/IDLEHOST.EXE" $source *> "$root/compile.txt"
 if($LASTEXITCODE){throw 'Native witness compile failed'}
 $guestSource="$PSScriptRoot/win16_idle_probe.asm"
 $firstDefines=@(if($SharedTasks){'-DSHARED_WITNESS';'-DTICK_LIMIT=16'})
 $peerDefines=@(if($SharedTasks){'-DSHARED_WITNESS';'-DNO_REGISTER';'-DTICK_LIMIT=4'}else{'-DTICK_LIMIT=12'})
 & $Nasm -f bin @firstDefines -o "$root/IDLPROBE.EXE" -l "$root/first.lst" $guestSource *> "$root/first-assemble.txt"
 if($LASTEXITCODE){throw 'First NE assembly failed'}
 & $Nasm -f bin -DPEER @peerDefines -o "$root/IDLPEER.EXE" -l "$root/peer.lst" $guestSource *> "$root/peer-assemble.txt"
 if($LASTEXITCODE){throw 'Peer NE assembly failed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $null=New-Item -ItemType Directory "$runtime/tests" -Force
 foreach($name in @('IDLEHOST.EXE','IDLPROBE.EXE','IDLPEER.EXE')){Copy-Item "$root/$name" "$runtime/tests/$name"}
 $scope.Paths+="$runtime\tests\IDLEHOST.EXE"
 @($source,$guestSource,"$root/IDLEHOST.EXE","$root/IDLPROBE.EXE","$root/IDLPEER.EXE",
   "$runtime/system32/ntvdm.exe","$runtime/system32/WOW32.DLL","$runtime/system32/nthook32.dll",
   "$runtime/system32/run16.exe","$runtime/system32/KRNL386.EXE","$runtime/system32/USER.EXE")|
   Get-FileHash|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime\system32;"+$env:SystemRoot+'\System32'
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:MVDM_WOW_WINDOW_TRACE_PATH="$root/lifetime.txt"
 $env:MVDM_WOW_DISPATCH_TRACE_PATH="$root/dispatch.txt"
 $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH="$root/callbacks.txt"
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 # Shared tasks use explicit public launcher waits; a native Hook startup
 # receipt is deliberately not used as proof of a shared task's lifetime.
 $arguments=if($SharedTasks){@("$runtime\tests\IDLEHOST.EXE","$runtime\tests","$root\observer.txt")}
     else{@("$runtime\system32\run16.exe","$runtime\tests","$root\observer.txt",'--wait',"$runtime\tests\IDLEHOST.EXE")}
 $arguments+=@("$runtime\tests\IDLPROBE.EXE","$runtime\tests\IDLPEER.EXE","$root\native.txt")
 if($SharedTasks){
   $arguments+=@("$runtime\system32\ntvdm.exe")
 }else{$arguments+=@("$root\lifetime.txt","$runtime\system32\ntvdm.exe")}
 $arguments+=@('--observation-timeout-ms','30000')
 foreach($arg in $arguments){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');deadline=$process.StartTime.AddSeconds(40).ToUniversalTime().ToString('o');arguments=@($start.ArgumentList)}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 if(!$process.WaitForExit(40000)){throw 'Owned observer deadline'}
 $process.Refresh()
 if($process.ExitCode -or (Get-Content "$root/observer.txt" -Raw) -notmatch 'exit=0x00000000'){throw 'Native/guest launch completion failed'}
 $overlap=if($SharedTasks){'peer-exited registrar-still-running'}else{'first-exited peer-still-running'}
 if((Get-Content "$root/native.txt" -Raw) -notmatch $overlap){throw 'Missing live peer observation'}
 # One character per byte keeps native file offsets valid even if unrelated
 # trace text contains non-ASCII bytes. The matched markers are ASCII.
 $trace=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes("$root/lifetime.txt"))
 $results=@()
 $recordSize=if($SharedTasks){22}else{16}
 foreach($name in @('IDLPROBE','IDLPEER')){
    Copy-Item "$runtime/tests/$name.BIN" "$root/$name.bin"
    $b=[IO.File]::ReadAllBytes("$root/$name.bin")
    if($b.Length -lt 3*$recordSize -or $b.Length % $recordSize -or (!$SharedTasks -and $b.Length -ne 3*$recordSize)){throw "Incomplete $name report"}
    $records=$b.Length/$recordSize
    for($i=0;$i -lt $records;$i++){
      $phase=if(!$i){0}elseif($i -eq $records-1){2}else{1}
      if([Text.Encoding]::ASCII.GetString($b,$recordSize*$i,4) -ne 'IDL1' -or [BitConverter]::ToUInt16($b,$recordSize*$i+4) -ne $phase -or [BitConverter]::ToUInt16($b,$recordSize*$i+6)){throw "Invalid $name phase/error"}
    }
    $last=$b.Length-$recordSize
    $task=[BitConverter]::ToUInt16($b,$last+14);$ticks=[BitConverter]::ToUInt16($b,$last+10)
    $isPeer=$name -eq 'IDLPEER'
    $expectedTicks=if($SharedTasks){if($isPeer){4}else{16}}else{if($isPeer){12}else{2}}
    $restored=if($SharedTasks -and $isPeer){2}else{1}
    if(!$task -or [BitConverter]::ToUInt16($b,$last+12) -ne $restored -or $ticks -ne $expectedTicks){throw "Invalid $name restoration/ticks"}
    $idles=[BitConverter]::ToUInt16($b,$last+8)
    if($SharedTasks -and $isPeer){if($idles){throw 'Unregistered peer received its own vector'}}elseif(!$idles){throw 'No registered idle callback'}
    $cross=0;$crossTask=0
    if($SharedTasks){$cross=[BitConverter]::ToUInt16($b,$last+16);$crossTask=[BitConverter]::ToUInt16($b,$last+18)}
    $results += [pscustomobject]@{name=$name;task=('{0:X8}' -f $task);ticks=$ticks;idles=$idles;cross=$cross;crossTask=('{0:X8}' -f $crossTask)}
 }
 # Task handles may legitimately collide across two workers. Identify the
 # two process/task pairs from the trace, not by treating HTASK as a PID.
 $idle=[regex]::Matches($trace,'(?m)^(\d+) TaskRetirement .*task=(\w+) stage=idle-dispatch status=0\r?$')
 $pids=@($idle|ForEach-Object {$_.Groups[1].Value}|Sort-Object -Unique)
 if($SharedTasks){
   if($pids.Count -ne 1 -or $results[0].task -eq $results[1].task -or
       (!$LifecycleOnly -and (!$results[0].cross -or $results[0].crossTask -ne $results[1].task))){throw 'Missing A-vector callback executing on B task in one worker'}
   foreach($item in $results){
     $completion=[regex]::Matches($trace,'(?m)^'+$pids[0]+' TaskRetirement .*task='+$item.task+' stage=complete status=0\r?$')
     if($completion.Count -ne 1 -or @($idle|Where-Object {$_.Groups[2].Value -eq $item.task -and $_.Index -gt $completion[0].Index}).Count){throw 'Missing unique retirement or idle after retirement'}
     # The lifecycle slice proves task overlap, terminal cleanup and the
     # surviving registrar. It intentionally does not claim the later
     # cross-task idle-vector/message-identity contract; an unregistered
     # peer therefore need not have its own taskman idle-dispatch marker.
     if(!$LifecycleOnly -and !@($idle|Where-Object {$_.Groups[2].Value -eq $item.task}).Count){throw 'Missing taskman dispatch for task'}
   }
   $peerCompletion=[regex]::Match($trace,'(?m)^'+$pids[0]+' TaskRetirement .*task='+$results[1].task+' stage=complete status=0\r?$')
   if(!@($idle|Where-Object {$_.Groups[2].Value -eq $results[0].task -and $_.Index -gt $peerCompletion.Index}).Count){throw 'Registrar did not continue idle after peer retirement'}
   $results|ConvertTo-Json|Set-Content "$root/identities.json"
   $detail=if($LifecycleOnly){'two distinct tasks overlapped and retired independently'}else{'A vector ran on unregistered B task'}
   "PASS shared worker $($pids[0]), $detail, both requests exited normally"|Set-Content "$root/result.txt"
   '0'|Set-Content "$root/exit.txt"
   return
 }
 if($pids.Count -ne 2){throw 'Did not observe two actual idle worker PIDs'}
 $native=Get-Content "$root/native.txt" -Raw
 $exits=[regex]::Matches($native,'(?m)^worker-exited pid=(\d+) exit=0 (?:peer=\d+ trace_bytes=\d+|final=1)\r?$')
 if($exits.Count -ne 2){throw 'Missing actual worker process-handle exit evidence'}
 $retired=@()
 foreach($pidText in $pids){
   $tasks=@($idle|Where-Object {$_.Groups[1].Value -eq $pidText}|ForEach-Object {$_.Groups[2].Value}|Sort-Object -Unique)
   if($tasks.Count -ne 1 -or $tasks[0] -notin $results.task){throw 'Unexpected worker idle task identity'}
   if(@($exits|Where-Object {$_.Groups[1].Value -eq $pidText}).Count -ne 1 -or
      [regex]::Matches($native,'(?m)^worker-observed pid='+$pidText+' created=').Count -ne 1){throw 'Worker identity/exit mismatch'}
   $retired += [pscustomobject]@{pid=$pidText;task=$tasks[0];exit=0;kind='original-process-exit'}
 }
 # Original last-app ExitKernel bypasses the shared-worker SG_EXIT/task marker.
 # Require actual process exit plus subsequent peer guest idle, before cleanup.
 $first=[regex]::Match($native,'worker-exited pid=(\d+) exit=0 peer=(\d+) trace_bytes=(\d+)')
 if(!$first.Success -or $first.Groups[1].Value -eq $first.Groups[2].Value){throw 'Missing independent surviving peer'}
 $offset=[int]$first.Groups[3].Value
 if($offset -gt $trace.Length -or !@($idle|Where-Object {$_.Index -ge $offset -and $_.Groups[1].Value -eq $first.Groups[2].Value}).Count){throw 'No peer idle after actual first process exit'}
 if(@($idle|Where-Object {$_.Index -ge $offset -and $_.Groups[1].Value -eq $first.Groups[1].Value}).Count){throw 'Guest idle after its worker exit'}
 @($results,$retired)|ConvertTo-Json -Depth 4|Set-Content "$root/identities.json"
 'PASS two real separate WOW workers, own idle callbacks, retirement and surviving peer idle'|Set-Content "$root/result.txt"
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/error.txt";'1'|Set-Content "$root/exit.txt";throw}
finally {
 if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
 if($runtime){foreach($name in @('IDLPROBE','IDLPEER')){if(Test-Path "$runtime/tests/$name.BIN"){Copy-Item "$runtime/tests/$name.BIN" "$root/$name.bin" -Force}}}
 if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime}
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
