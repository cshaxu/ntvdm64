[CmdletBinding()]
param([string]$CaseRoot,[Parameter(Mandatory)][string]$BuildRoot,[switch]$SelfTest)
# Postprocess existing native-PC and callback-QPC records; never sample a live
# process. Counts locate retained observations, not CPU shares or time budgets.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned analysis required'}
$null=New-Item -ItemType Directory $root
function Read-CodeMap($file){
 $lines=Get-Content $file
 $preferred=[regex]::Match(($lines -join "`n"),'Preferred load address is ([0-9A-Fa-f]+)')
 if(!$preferred.Success){throw 'Missing exact image MAP preferred base'}
 $symbols=@($lines|ForEach-Object {if($_ -match '^\s+0001:[0-9A-Fa-f]{8}\s+(\S+)\s+([0-9A-Fa-f]{8})\s'){
  [pscustomobject]@{name=$Matches[1];va=[Convert]::ToUInt64($Matches[2],16)}
 }}|Sort-Object va)
 if(!$symbols.Count){throw 'No code-segment symbols'}
 return @{preferred=[Convert]::ToUInt64($preferred.Groups[1].Value,16);symbols=$symbols}
}
function Locate($pc,$modules,$tables){
 $module=@($modules|Where-Object {$pc -ge $_.base -and $pc -lt ($_.base+$_.size)})
 if(!$module.Count){return [pscustomobject]@{module='<outside-known-module>';nearest='<unresolved>';offset=0}}
 if($module.Count -ne 1){throw 'Ambiguous native module ranges'}
 $name=$module[0].name;$map=$tables[$name.ToLowerInvariant()]
 if(!$map){return [pscustomobject]@{module=$name;nearest='<no-MAP>';offset=$pc-$module[0].base}}
 $va=$pc-$module[0].base+$map.preferred;$items=$map.symbols;$lo=0;$hi=$items.Count-1;$best=-1
 while($lo -le $hi){$mid=[int][Math]::Floor(($lo+$hi)/2);if($items[$mid].va -le $va){$best=$mid;$lo=$mid+1}else{$hi=$mid-1}}
 if($best -lt 0){return [pscustomobject]@{module=$name;nearest='<before-first-code-symbol>';offset=$pc-$module[0].base}}
 return [pscustomobject]@{module=$name;nearest=$items[$best].name;offset=$va-$items[$best].va}
}
try{
 if($SelfTest){
  $modules=@([pscustomobject]@{name='ntvdm.exe';base=0x1000;size=0x100})
  $tables=@{'ntvdm.exe'=@{preferred=0x400000;symbols=@([pscustomobject]@{name='A';va=0x400010},[pscustomobject]@{name='B';va=0x400020})}}
  $a=Locate 0x1010 $modules $tables;$b=Locate 0x1025 $modules $tables;$outside=Locate 0x1100 $modules $tables;$before=Locate 0x1005 $modules $tables
  if($a.nearest -ne 'A' -or $a.offset -ne 0 -or $b.nearest -ne 'B' -or $b.offset -ne 5 -or $outside.module -ne '<outside-known-module>' -or $before.nearest -ne '<before-first-code-symbol>'){throw 'Module/symbol boundary controls failed'}
  'CALLBACK_PC_PARSER positive=2 range_boundaries=2'|Set-Content "$root/result.txt"
  0|Set-Content "$root/exit.txt";return
 }
 $case=(Resolve-Path -LiteralPath $CaseRoot).Path
 if(!$case.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Retained owned case required'}
 foreach($path in @("$case/exit.txt","$case/measurement/exit.txt")){if((Get-Content $path -Raw).Trim() -ne '0'){throw 'Complete callback measurement required'}}
 $sampleFile="$case/measurement/normal-sampled-sample.stdout.txt";$callbackFile="$case/measurement/normal-sampled/callbacks.csv"
 $inputs=@($PSCommandPath,$sampleFile,$callbackFile,"$case/measurement/ntvdm.exe.map","$case/measurement/WOW32.DLL.map","$case/diagnostic-inputs.csv")
 foreach($row in Import-Csv "$case/diagnostic-inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Retained diagnostic image/source changed'}}
 $inputs|Get-FileHash|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $lines=Get-Content $sampleFile
 $modules=@($lines|ForEach-Object {if($_ -match '^module=(\S+) base=([0-9A-Fa-f]{8}) size=([0-9A-Fa-f]{8})$'){
  [pscustomobject]@{name=$Matches[1];base=[Convert]::ToUInt64($Matches[2],16);size=[Convert]::ToUInt64($Matches[3],16)}
 }})
 $tables=@{'ntvdm.exe'=(Read-CodeMap "$case/measurement/ntvdm.exe.map");'wow32.dll'=(Read-CodeMap "$case/measurement/WOW32.DLL.map")}
 $callbacks=@(Import-Csv $callbackFile|ForEach-Object {
  [pscustomobject]@{thread=[long]$_.thread;entered=[long]$_.entered;returned=[long]$_.returned;phaseEntered=[long]$_.phase_entered;phaseReturned=[long]$_.phase_returned}
 })
 if($callbacks.Count -ne 30 -or @($callbacks.thread|Sort-Object -Unique).Count -ne 1){throw 'Expected thirty callbacks in one verified window thread'}
 $callbackThread=$callbacks[0].thread;$ambiguous=0;$total=0
 $selected=@($lines|ForEach-Object {
  if($_ -match '^sample=(\d+) tid=(\d+) pc=([0-9A-Fa-f]{8}) .* qpc=(\d+)$'){
   ++$total;$round=[int]$Matches[1];$nativeTid=[long]$Matches[2];$pc=[Convert]::ToUInt64($Matches[3],16);$when=[long]$Matches[4]
   $active=@($callbacks|Where-Object {$when -ge $_.entered -and $when -le $_.returned})
   if($active.Count -gt 1){++$ambiguous}
   elseif($active.Count -eq 1){
    $location=Locate $pc $modules $tables
    $phase=if($when -lt $active[0].phaseEntered){'pre'}elseif($when -le $active[0].phaseReturned){'dispatch'}else{'post'}
    [pscustomobject]@{round=$round;nativeThread=$nativeTid;callbackThread=$callbackThread;role=$(if($nativeTid -eq $callbackThread){'callback-thread'}else{'other-thread'});phase=$phase;pc=('{0:X8}' -f $pc);qpc=$when;module=$location.module;nearestCodeSymbol=$location.nearest;offset=$location.offset}
   }
  }
 })
 if(!$total){throw 'No recorded native samples'}
 $selected|Export-Csv -NoTypeInformation "$root/selected.csv"
 $counts=@($selected|Group-Object role,phase,module,nearestCodeSymbol|ForEach-Object {
  $r=$_.Group[0];[pscustomobject]@{role=$r.role;phase=$r.phase;module=$r.module;nearestCodeSymbol=$r.nearestCodeSymbol;samples=$_.Count}
 }|Sort-Object samples -Descending)
 $counts|ConvertTo-Json|Set-Content "$root/counts.json"
 @{recordedSamples=$total;insideCallbackIntervals=$selected.Count;callbackThreadSamples=@($selected|Where-Object role -eq 'callback-thread').Count;ambiguousIntervalSamples=$ambiguous;liveProcessRead=$false;limits='Unweighted sparse locations, not CPU/wall-time shares; nearest MAP code symbol is not a verified frame or exact function extent; raw stack words are not unwound; sampler pauses affect workload; other-thread samples are contextual, not causal proof.'}|ConvertTo-Json|Set-Content "$root/result.json"
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Frozen analysis input changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
