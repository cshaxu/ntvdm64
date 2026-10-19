[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,
 [string]$WorkerBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/worker'),
 [string]$ProviderBuildRoot=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32'),
 [Parameter(Mandatory)][string]$Observer,
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'))
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned diagnostic required'}
$null=New-Item -ItemType Directory $root
$shell=(Get-Process -Id $PID).Path
function Start-Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;deadlineSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 return $p
}
function Finish-Owned($p,$name,$seconds){
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned diagnostic deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$code|Set-Content "$root/$name.exit.txt";if($code){throw "$name failed $code"};$p.Dispose()
}
function Owned($name,$exe,$line,$seconds){$p=Start-Owned $name $exe $line $seconds;Finish-Owned $p $name $seconds}
function Read-Samples($file,$version,$environment){
 $rows=@(Get-Content $file|ForEach-Object {
  if($_ -match '^observer-cost operation=(\d+) total-qpc=(\d+) post-qpc=(\d+) blit-qpc=(\d+) flush-qpc=(\d+) sleep-qpc=(\d+) blit-calls=(\d+) flush-calls=(\d+) sleep-calls=(\d+) frequency=(\d+)$'){
   [pscustomobject]@{version=$version;environment=$environment;operation=[int]$Matches[1];total=[long]$Matches[2];post=[long]$Matches[3];blit=[long]$Matches[4];flush=[long]$Matches[5];sleep=[long]$Matches[6];blitCalls=[int]$Matches[7];flushCalls=[int]$Matches[8];sleepCalls=[int]$Matches[9];frequency=[long]$Matches[10]}
  }
 })
 if($rows.Count -ne 12){throw "Incomplete observer samples $file"}
 $latency=@(Get-Content $file|ForEach-Object {if($_ -match '^latency operation=(\d+) microseconds=(\d+) .*start-qpc=(\d+) stop-qpc=(\d+) frequency=(\d+)$'){[pscustomobject]@{operation=[int]$Matches[1];us=[long]$Matches[2];start=[long]$Matches[3];stop=[long]$Matches[4];frequency=[long]$Matches[5]}}})
 if($latency.Count -ne 12){throw 'Missing original timing witness'}
 for($i=0;$i -lt 12;++$i){
  $r=$rows[$i]
  if($r.operation -ne $i -or !$r.frequency -or !$r.blitCalls -or $r.flushCalls -ne $r.blitCalls -or $r.sleepCalls -ne ($r.blitCalls - 1) -or $r.total -lt ($r.post+$r.blit+$r.flush+$r.sleep)){throw 'Inconsistent/nonexclusive observer leaf timing'}
  $l=$latency[$i]
  if($l.operation -ne $i -or $l.frequency -ne $r.frequency -or $l.stop-$l.start -ne $r.total -or [Math]::Floor([double]$r.total*1000000/$r.frequency) -ne $l.us){throw 'Observer leaves do not match original operation stopwatch'}
 }
 return $rows
}
$native=$null;$probe=$null;$gate=$null
try{
 $sources=@($PSCommandPath,"$PSScriptRoot/wow_observer_cost.h","$PSScriptRoot/wow_observer_cost_fixture.c","$PSScriptRoot/wow_mine_native_cost_control.c","$PSScriptRoot/wow_mine_latency_probe.c","$PSScriptRoot/wow_mine_gameplay_probe.c","$PSScriptRoot/verify-wow-mine-latency.ps1","$PSScriptRoot/isolated_package_cleanup.ps1",$Observer,$CompilerDriver,"$repo/assets/winnt.zip","$repo/assets/release/release-manifest.json","$WorkerBuildRoot/ntvdm.exe","$WorkerBuildRoot/ntvdm.exe.map","$ProviderBuildRoot/wow32.dll","$ProviderBuildRoot/wow32.dll.map")
 $images=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
 if($images.Count -ne 13){throw 'Coherent13 required'}
 foreach($image in $images){$path="$repo/assets/release/$($image.name)";if((Get-FileHash $path).Hash -ne $image.sha256){throw 'Release manifest mismatch'};$sources+=$path}
 $sources|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $null=New-Item -ItemType Directory "$root/sealed-images"
 foreach($image in $images){Copy-Item "$repo/assets/release/$($image.name)" "$root/sealed-images/$($image.name)"}
 foreach($pair in @(@('ntvdm.exe',$WorkerBuildRoot),@('WOW32.DLL',$ProviderBuildRoot))){
  if((Get-FileHash "$($pair[1])/$($pair[0])").Hash -ne (Get-FileHash "$root/sealed-images/$($pair[0])").Hash){throw 'MAP/build image does not match delivered runtime'}
  Copy-Item "$($pair[1])/$($pair[0]).map" "$root/sealed-images/$($pair[0]).map"
 }
 @{runtimeHead=(& git -C $repo rev-parse HEAD);source='Exact delivered thirteen images; diagnostic instrumentation is observer-only';productionChanged=$false;guestChanged=$false;pollingMilliseconds=20;limits='Posted input, native capture and actual sleep wall time; native reference is not a matching VM workload or physical mouse latency. Leaf times may wait on painter/compositor, are not pure overhead and cannot be subtracted into guest CPU time.'}|ConvertTo-Json|Set-Content "$root/contract.json"
 foreach($spec in @(@('unit','wow_observer_cost_fixture.c','kernel32.lib'),@('native','wow_mine_native_cost_control.c','user32.lib gdi32.lib'))){
  Owned ('compile-'+$spec[0]) $env:ComSpec ('/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /MT /O2 /W4 /D_CRT_SECURE_NO_WARNINGS /Fo"'+$root+'/'+$spec[0]+'.obj" /Fe"'+$root+'/'+$spec[0]+'.exe" "'+$PSScriptRoot+'/'+$spec[1]+'" /link '+$spec[2]+'"') 120
 }
 Owned unit "$root/unit.exe" '--control' 15
 if((Get-Content "$root/unit.stdout.txt" -Raw) -notmatch 'OBSERVER_COST_UNIT errors=0 native_calls=5 exact_args=1 success_failure=1 exact_errors=1 qpc=1 reset=1'){throw 'Incomplete transparent leaf/API/error controls'}
 Expand-Archive "$repo/assets/winnt.zip" "$root/media";$package="$root/media/WINNT"
 foreach($image in $images){Copy-Item "$root/sealed-images/$($image.name)" "$package/system32/$($image.name)" -Force}
 Owned measurement $shell ('-NoProfile -File "'+$PSScriptRoot+'/verify-wow-mine-latency.ps1" -PackageRoot "'+$package+'" -Observer "'+$Observer+'" -CompilerDriver "'+$CompilerDriver+'" -BuildRoot "'+$root+'/measurement" -WorkerMap "'+$root+'/sealed-images/ntvdm.exe.map" -WowMap "'+$root+'/sealed-images/WOW32.DLL.map" -ObserverCost') 360
 $samples=@()
 foreach($environment in @('normal','normal-sampled','private')){$samples+=Read-Samples "$root/measurement/$environment-game.stdout.txt" project $environment}
 foreach($environment in @('normal','private')){
  $name='native-'+$environment;$out="$root/$name";$null=New-Item -ItemType Directory $out
  $desktop=if($environment -eq 'normal'){'-'}else{'NTVDMObserverCost-'+$PID}
  $native=Start-Owned ($name+'-host') "$root/native.exe" $desktop 45
  $gateName='Local\NTVDMObserverCost-'+$PID+'-'+$environment
  $gate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,$gateName)
  $probe=Start-Owned ($name+'-probe') "$root/measurement/latency.exe" ($native.Id.ToString()+' "'+$desktop+'" "'+$out+'/mine" "'+$gateName+'"') 45
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  while(!(Test-Path "$out/mine.ready") -and !$probe.HasExited -and !$native.HasExited -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 50}
  if(!(Test-Path "$out/mine.ready")){
   @{probeExited=$probe.HasExited;probeExit=$(if($probe.HasExited){$probe.ExitCode}else{$null});nativeExited=$native.HasExited;nativeExit=$(if($native.HasExited){$native.ExitCode}else{$null})}|ConvertTo-Json|Set-Content "$out/readiness-failure.json"
   throw 'Native board reference not ready'
  }
  $null=$gate.Set();Finish-Owned $probe ($name+'-probe') 45;$probe=$null
  Finish-Owned $native ($name+'-host') 10;$native=$null;$gate.Dispose();$gate=$null
  if((Get-Content "$root/$name-host.stdout.txt" -Raw) -notmatch 'NATIVE_OBSERVER_CONTROL errors=0 down=12 up=12 paints=\d+ flag=0'){throw 'Native repaint/pair/normal-exit reference failed'}
  $samples+=Read-Samples "$root/$name-probe.stdout.txt" native $environment
 }
 if($samples.Count -ne 60){throw 'Incomplete fixed diagnostic matrix'}
 $samples|Export-Csv -NoTypeInformation "$root/samples.csv"
 $summary=foreach($group in $samples|Group-Object version,environment){
  $result=[ordered]@{version=$group.Group[0].version;environment=$group.Group[0].environment;operations=12}
  foreach($metric in @('total','post','blit','flush','sleep','residual')){
   $values=@($group.Group|ForEach-Object {$ticks=if($metric -eq 'residual'){$_.total - $_.post - $_.blit - $_.flush - $_.sleep}else{$_.$metric};[double]$ticks*1000000/$_.frequency}|Sort-Object)
   $result[$metric+'MedianUs']=($values[5]+$values[6])/2
  }
  [pscustomobject]$result
 }
 $summary|ConvertTo-Json|Set-Content "$root/summary.json"
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Frozen source/runtime input changed'}}
 foreach($image in $images){foreach($dir in @("$repo/assets/release",'O:\winnt\system32',"$root/sealed-images")){if((Get-FileHash "$dir/$($image.name)").Hash -ne $image.sha256){throw 'Delivered/sealed runtime changed during observer-only diagnostic'}}}
 @{operations=60;unit=$true;actualGuest=$true;nativeReference=$true;productionChanged=$false;interpretationPending=$true}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 foreach($p in @($probe,$native)){if($p){$null=$p.Handle;if(!$p.HasExited){$p.Kill();$null=$p.WaitForExit(5000)};$p.Dispose()}}
 if($gate){$gate.Dispose()}
}
