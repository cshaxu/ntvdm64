[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[switch]$SelfTest)
$ErrorActionPreference='Stop'
function Correlate($latency,$callbacks,$window,$thread){
 if($latency.Count -ne 15 -or $callbacks.Count -ne 30){throw 'Incomplete input/callback set'}
 if(@($callbacks|Group-Object slot).Count -ne 30){throw 'Repeated reservation'}
 $records=@($callbacks|Sort-Object slot)
 for($i=0;$i -lt 15;++$i){
  $l=$latency[$i];$down=$records[2*$i];$up=$records[2*$i+1]
  foreach($r in @($down,$up)){
   if($r.window -ne $window -or $r.thread -ne $thread -or !$r.normal -or !$r.accepted -or $r.frequency -ne $l.frequency -or $r.returned -lt $r.entered){throw 'Wrong native callback identity/state/clock'}
   if($r.phase_count -ne 1 -or $r.phase_entered -lt $r.entered -or $r.phase_returned -lt $r.phase_entered -or $r.phase_returned -gt $r.returned){throw 'Ambiguous/unordered dispatch phase'}
  }
  $expectedDown=if($i -lt 12){516}else{513};$expectedUp=$expectedDown+1
  if($l.operation -ne $i -or $down.message -ne $expectedDown -or $up.message -ne $expectedUp -or $down.entered -gt $up.entered -or $down.entered+1 -lt $l.start -or $l.stop+1 -lt $down.entered){throw 'Ambiguous/wrong input-to-callback pairing'}
  $last=[Math]::Max($down.returned,$up.returned);$factor=1000000.0/$l.frequency
  $out=[pscustomobject]@{operation=$i;kind=$(if($i -lt 12){'flag'}else{'reveal'});window=$window;thread=$thread;frequency=$l.frequency;entryDelayUs=($down.entered-$l.start)*$factor;downCallbackUs=($down.returned-$down.entered)*$factor;upCallbackUs=($up.returned-$up.entered)*$factor;callbackSpanUs=($last-$down.entered)*$factor;betweenCallbacksUs=($up.entered-$down.returned)*$factor;observationAfterReturnUs=($l.stop-$last)*$factor;totalUs=($l.stop-$l.start)*$factor;nested=($up.entered -lt $down.returned)}
  foreach($pair in @(@('down',$down),@('up',$up))){$r=$pair[1];$out|Add-Member -NotePropertyName ($pair[0]+'PreUs') -NotePropertyValue (($r.phase_entered-$r.entered)*$factor);$out|Add-Member -NotePropertyName ($pair[0]+'DispatchUs') -NotePropertyValue (($r.phase_returned-$r.phase_entered)*$factor);$out|Add-Member -NotePropertyName ($pair[0]+'PostUs') -NotePropertyValue (($r.returned-$r.phase_returned)*$factor)}
  $out
 }
}
$root=[IO.Path]::GetFullPath($BuildRoot)
try{
 if($SelfTest){
  if(Test-Path $root){throw 'Fresh parser controls required'};$null=New-Item -ItemType Directory $root
  $latency=@(0..14|ForEach-Object {[pscustomobject]@{operation=$_;start=1000+100*$_;stop=1050+100*$_;frequency=10000000}})
  $callbacks=@(0..29|ForEach-Object {$n=[int][Math]::Floor($_/2);$msg=if($n -lt 12){516}else{513};$entered=1010+100*$n+10*($_%2);$returned=1060+100*$n-20*($_%2);[pscustomobject]@{slot=$_;window='00123456';thread=123;normal=1;accepted=1;frequency=10000000;message=$msg+($_%2);entered=$entered;returned=$returned;phase_entered=$entered+2;phase_returned=$returned-2;phase_count=1}})
  $positive=@(Correlate $latency $callbacks '00123456' 123)
  if($positive.Count -ne 15 -or !$positive[0].nested -or $positive[0].observationAfterReturnUs -ge 0){throw 'Signed/nested positive control failed'}
  $negative=0
  foreach($kind in @('thread','window','message','clock','ordering','missing','duplicate','phase-order','phase-count','phase-outside')){
   $bad=@($callbacks|ForEach-Object {$_.PSObject.Copy()})
   switch($kind){thread{$bad[0].thread=124}window{$bad[0].window='00123457'}message{$bad[0].message=517}clock{$bad[0].frequency=1}ordering{$bad[0].entered=1}missing{$bad=$bad[0..28]}duplicate{$bad[1].slot=$bad[0].slot}phase-order{$bad[0].phase_returned=$bad[0].phase_entered-1}phase-count{$bad[0].phase_count=2}phase-outside{$bad[0].phase_entered=$bad[0].entered-1}}
   $rejected=$false;try{$null=@(Correlate $latency $bad '00123456' 123)}catch{$rejected=$true}
   if(!$rejected){throw "Missed parser negative $kind"};++$negative
  }
  "CALLBACK_COST_PARSER positive=15 signed_negative_lag=1 nested=1 rejected=$negative"|Set-Content "$root/result.txt"
  0|Set-Content "$root/exit.txt";return
 }
 if((Get-Content "$root/measurement/exit.txt" -Raw).Trim() -ne '0'){throw 'Incomplete actual callback measurement'}
 $all=foreach($environment in @('normal','normal-sampled','private')){
  $ready=Get-Content "$root/measurement/$environment/mine.ready" -Raw
  $w=[regex]::Match($ready,'window=([0-9A-Fa-f]+)');$t=[regex]::Match($ready,'callback-thread=(\d+)')
  if(!$w.Success -or !$t.Success){throw 'Missing actual HWND/native-thread observation'}
  $window=$w.Groups[1].Value.ToLowerInvariant().PadLeft(8,'0');$thread=[long]$t.Groups[1].Value
  $latency=@(Import-Csv "$root/measurement/$environment/latency.csv"|ForEach-Object {[pscustomobject]@{operation=[int]$_.operation;start=[long]$_.start;stop=[long]$_.stop;frequency=[long]$_.frequency}})
  $latency+=@(Get-Content "$root/measurement/$environment-game.stdout.txt"|ForEach-Object {if($_ -match '^callback-open operation=(\d+) microseconds=(\d+) start-qpc=(\d+) stop-qpc=(\d+) frequency=(\d+)$'){[pscustomobject]@{operation=12+[int]$Matches[1];start=[long]$Matches[3];stop=[long]$Matches[4];frequency=[long]$Matches[5]}}})
  $callbacks=@(Get-Content "$root/measurement/$environment-game.stdout.txt"|ForEach-Object {
   if($_ -match '^callback-cost slot=(\d+) window=([0-9a-fA-F]{8}) message=(\d+) thread=(\d+) entered=(\d+) returned=(\d+) accepted=(\d+) normal=(\d+) result=(-?\d+) error=(\d+) frequency=(\d+) phase-entered=(\d+) phase-returned=(\d+) phase-count=(\d+)$'){
    [pscustomobject]@{slot=[int]$Matches[1];window=$Matches[2].ToLowerInvariant();message=[int]$Matches[3];thread=[long]$Matches[4];entered=[long]$Matches[5];returned=[long]$Matches[6];accepted=[int]$Matches[7];normal=[int]$Matches[8];result=[long]$Matches[9];error=[long]$Matches[10];frequency=[long]$Matches[11];phase_entered=[long]$Matches[12];phase_returned=[long]$Matches[13];phase_count=[int]$Matches[14]}
   }
  })
  $callbacks|Export-Csv -NoTypeInformation "$root/measurement/$environment/callbacks.csv"
  Correlate $latency $callbacks $window $thread|ForEach-Object {$_|Add-Member -NotePropertyName environment -NotePropertyValue $environment -PassThru}
 }
 $all|Export-Csv -NoTypeInformation "$root/correlated.csv"
 $summary=foreach($group in $all|Group-Object environment,kind){
  $row=[ordered]@{environment=$group.Group[0].environment;kind=$group.Group[0].kind;operations=$group.Count;negativeObservationLags=@($group.Group|Where-Object observationAfterReturnUs -lt 0).Count;nested=@($group.Group|Where-Object nested).Count}
  foreach($metric in @('entryDelayUs','downCallbackUs','upCallbackUs','callbackSpanUs','observationAfterReturnUs','totalUs','downPreUs','downDispatchUs','downPostUs','upPreUs','upDispatchUs','upPostUs')){$values=@($group.Group.$metric|Sort-Object);$mid=[int][Math]::Floor($values.Count/2);$row[$metric+'Median']=$(if($values.Count%2){$values[$mid]}else{($values[$mid-1]+$values[$mid])/2});$row[$metric+'Max']=$values[-1]}
  [pscustomobject]$row
 }
 $summary|ConvertTo-Json|Set-Content "$root/summary.json"
 'Inclusive native callback wall times, not CPU shares; posted input, not physical latency; cross-thread ordering has one QPC tick uncertainty; signed observation lag is retained.'|Set-Content "$root/limits.txt"
}catch{if(Test-Path $root){$_|Out-String|Set-Content "$root/summary-failure.txt"};throw}
