[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$root=[IO.Path]::GetFullPath($BuildRoot)
if((Get-Content "$root/exit.txt" -Raw).Trim() -ne '0'){throw 'Complete diagnostic/control group must pass first'}
if(Test-Path "$root/cost-summary.json"){throw 'Keep sealed diagnostic summary'}
Get-FileHash $PSCommandPath,"$root/diagnostic.json","$root/diagnostic-inputs.csv","$root/inputs.csv"|Export-Csv -NoTypeInformation "$root/cost-summary-inputs.csv"
$names=@('class-collection','retired-windows','view-copy','graph-project','class-sort','view-sort')
$summary=@();$operations=@()
foreach($case in @('normal','normal-sampled','private')){
 $path="$root/measurement/$case-game.stdout.txt"
 $rows=@(Get-Content $path|ForEach-Object {
  if($_ -match '^cost phase=(before|after) operation=(\d+) kind=(\d+) calls=(\d+) ticks=(\d+) items=(\d+) largest=(\d+) longest=(\d+) frequency=(\d+) qpc=(\d+) sequence=(\d+)$'){
   [pscustomobject]@{phase=$Matches[1];operation=[int]$Matches[2];kind=[int]$Matches[3];calls=[long]$Matches[4];ticks=[long]$Matches[5];items=[long]$Matches[6];largest=[long]$Matches[7];longest=[long]$Matches[8];frequency=[long]$Matches[9];qpc=[long]$Matches[10];sequence=[long]$Matches[11]}
  }
 })
 if($rows.Count -ne 144){throw "Missing stable snapshots for $case"}
 if(@(Get-Content "$root/baseline/$case-game.stdout.txt"|Where-Object {$_ -like 'cost *'}).Count){throw 'Cost instrumentation leaked into the normal probe control'}
 $frequency=@($rows.frequency|Sort-Object -Unique)
 if($frequency.Count -ne 1 -or $frequency[0] -le 0 -or @($rows|Where-Object {$_.sequence % 2}).Count){throw 'Inconsistent diagnostic frequency or sequence'}
 $latency=@(Import-Csv "$root/measurement/$case/latency.csv")
 $baseline=@(Import-Csv "$root/baseline/$case/latency.csv")
 if($latency.Count -ne 12 -or $baseline.Count -ne 12){throw 'Twelve input measurements required for each side'}
 for($operation=0;$operation -lt 12;$operation++){
  $clock=@($latency|Where-Object {[int]$_.operation -eq $operation})
  if($clock.Count -ne 1){throw 'Missing unique posted-input interval'}
  for($kind=0;$kind -lt 6;$kind++){
   $before=@($rows|Where-Object {$_.operation -eq $operation -and $_.kind -eq $kind -and $_.phase -eq 'before'})
   $after=@($rows|Where-Object {$_.operation -eq $operation -and $_.kind -eq $kind -and $_.phase -eq 'after'})
   if($before.Count -ne 1 -or $after.Count -ne 1){throw 'Duplicated or missing counter boundary'}
   if($after.calls -lt $before.calls -or $after.ticks -lt $before.ticks -or $after.items -lt $before.items -or $before.qpc -gt [long]$clock[0].start -or $after.qpc -lt [long]$clock[0].stop){throw 'Nonmonotonic or mis-bracketed diagnostic interval'}
   $operations+=[pscustomobject]@{case=$case;operation=$operation;kind=$names[$kind];calls=$after.calls-$before.calls;milliseconds=1000.0*($after.ticks-$before.ticks)/$frequency[0];items=$after.items-$before.items;bracketMilliseconds=1000.0*($after.qpc-$before.qpc)/$frequency[0];inputMilliseconds=[double]$clock[0].microseconds/1000.0;lifetimeLargestItems=$after.largest;lifetimeLongestMilliseconds=1000.0*$after.longest/$frequency[0]}
  }
 }
 foreach($kind in $names){
  $group=@($operations|Where-Object {$_.case -eq $case -and $_.kind -eq $kind})
  $summary+=[pscustomobject]@{case=$case;kind=$kind;operations=12;calls=($group|Measure-Object calls -Sum).Sum;inclusiveMilliseconds=($group|Measure-Object milliseconds -Sum).Sum;items=($group|Measure-Object items -Sum).Sum;inputMilliseconds=($group|Measure-Object inputMilliseconds -Sum).Sum;bracketMilliseconds=($group|Measure-Object bracketMilliseconds -Sum).Sum;baselineInputMilliseconds=($baseline|Measure-Object microseconds -Sum).Sum/1000.0}
 }
}
$operations|Export-Csv -NoTypeInformation "$root/cost-operations.csv"
$summary|ConvertTo-Json -Depth 4|Set-Content "$root/cost-summary.json"
@{source='Isolated diagnostic worker, stable even-sequence native snapshots';durations='Inclusive per kind; never sum nested kinds';latency='Posted input plus drawing and20ms polling; not physical mouse latency';observer='Counter reads and quiescence wait lie outside the input stopwatch but inside the counter bracket; compare baseline observer effects';originalGuestModified=$false;productionModified=$false}|ConvertTo-Json|Set-Content "$root/cost-limits.json"
'PASS exact twelve-operation stable counter brackets in three environments and uninstrumented probe controls'|Set-Content "$root/cost-summary.txt"
