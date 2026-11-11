param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=(Resolve-Path $BuildRoot).Path
if((Get-Content "$root/exit.txt" -Raw).Trim() -ne '0'){throw 'Latency group did not complete'}
if(Test-Path "$root/summary.json"){throw 'Keep sealed summary'}
$results=@()
foreach($case in @('normal','normal-sampled','private')){
 $rows=@(Import-Csv "$root/$case/latency.csv")
 $ordered=@($rows|ForEach-Object {[long]$_.microseconds}|Sort-Object)
 if($ordered.Count -ne 12){throw 'Missing measurements'}
 $results+=[pscustomobject]@{case=$case;operations=12;minUs=$ordered[0];medianUs=($ordered[5]+$ordered[6])/2;p95Us=$ordered[11];meanUs=($ordered|Measure-Object -Average).Average;maxUs=$ordered[11]}
}
$symbols=@{}
foreach($name in @('ntvdm.exe','WOW32.DLL')){
 $lines=Get-Content "$root/$name.map"
 $preferred=[Convert]::ToInt64(([regex]::Match(($lines -join "`n"),'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
 $symbols[$name]=@($lines|ForEach-Object {if($_ -match '^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+(\S+)\s+([0-9A-Fa-f]{8})\s'){[pscustomobject]@{name=$Matches[1];rva=([Convert]::ToInt64($Matches[2],16)-$preferred)}}}|Sort-Object rva)
}
$log=Get-Content "$root/normal-sampled-sample.stdout.txt"
$modules=@($log|ForEach-Object {if($_ -match '^module=(\S+) base=([0-9A-F]+) size=([0-9A-F]+)'){[pscustomobject]@{name=$Matches[1];base=[Convert]::ToInt64($Matches[2],16);size=[Convert]::ToInt64($Matches[3],16)}}})
$threads=@($log|ForEach-Object {if($_ -match '^cpu tid=(\d+) microseconds=(\d+)'){[pscustomobject]@{id=[int]$Matches[1];cpuUs=[long]$Matches[2]}}}|Sort-Object cpuUs -Descending)
$timings=Import-Csv "$root/normal-sampled/latency.csv"
$first=[long]$timings[0].start;$last=[long]$timings[11].stop
$samples=@()
foreach($line in $log){
 if($line -notmatch ('^sample=\d+ tid='+$threads[0].id+' pc=([0-9A-F]+) .* qpc=(\d+)$')){continue}
 $address=[Convert]::ToInt64($Matches[1],16);$at=[long]$Matches[2]
 if($at -lt $first -or $at -gt $last){continue}
 $module=$modules|Where-Object {$address -ge $_.base -and $address -lt $_.base+$_.size}|Select-Object -First 1
 $name=if($module){$module.name}else{'unresolved'};$offset=$null
 if($module -and $symbols.ContainsKey($module.name)){
  $rva=$address-$module.base;$entries=$symbols[$module.name];$lo=0;$hi=$entries.Count-1
  while($lo -le $hi){$mid=($lo+$hi) -shr 1;if($entries[$mid].rva -le $rva){$lo=$mid+1}else{$hi=$mid-1}}
  if($hi -ge 0){$name+='!'+$entries[$hi].name;$offset=$rva-$entries[$hi].rva}
 }
 $samples+=[pscustomobject]@{qpc=$at;pc=('{0:X8}' -f $address);module=$module.name;nearestMapSymbol=$name;offset=$offset}
}
if(!$samples.Count){throw 'No active-input samples'}
$samples|Export-Csv -NoTypeInformation "$root/active-input-samples.csv"
@{latency=$results;sampledThread=$threads[0];activeWallUs=($last-$first)*1000000/[long]$timings[0].frequency;activeSamples=$samples.Count;histogram=@($samples|Group-Object nearestMapSymbol|Sort-Object Count -Descending|Select-Object Name,Count);limits='Posted-input-to-pixel observation, not physical input; sampling perturbation separately measured; nearest MAP symbols are statistical attribution, not full call stacks or CPU-weighted percentages.'}|ConvertTo-Json -Depth 6|Set-Content "$root/summary.json"
Get-Content "$root/summary.json"
