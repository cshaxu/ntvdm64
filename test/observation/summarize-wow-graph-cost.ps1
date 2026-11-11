[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop';$root=[IO.Path]::GetFullPath($BuildRoot)
if(Test-Path "$root/graph-cost-summary.json"){throw 'Preserve sealed graph interpretation'}
$names=@('capture','normalize','project','refresh','commit','unused');$summary=@();$omitted=@()
Get-FileHash $PSCommandPath,"$root/diagnostic.json","$root/inputs.csv","$root/diagnostic-inputs.csv"|Export-Csv -NoTypeInformation "$root/graph-summary-inputs.csv"
foreach($case in @('normal','normal-sampled','private')){
 $exit="$root/measurement/$case-game.exit.txt"
 if(!(Test-Path $exit) -or (Get-Content $exit -Raw).Trim() -ne '0'){
  $omitted+=[pscustomobject]@{case=$case;reason='Actual diagnostic probe did not pass; no timing inference'};continue
 }
 $path="$root/measurement/$case-game.stdout.txt";$rows=@(Get-Content $path|ForEach-Object {
  if($_ -match '^cost phase=(before|after) operation=(\d+) kind=(\d+) calls=(\d+) ticks=(\d+) items=(\d+) largest=(\d+) longest=(\d+) frequency=(\d+) qpc=(\d+) sequence=(\d+)$'){
   [pscustomobject]@{phase=$Matches[1];operation=[int]$Matches[2];kind=[int]$Matches[3];calls=[long]$Matches[4];ticks=[long]$Matches[5];items=[long]$Matches[6];frequency=[long]$Matches[9];qpc=[long]$Matches[10];sequence=[long]$Matches[11]}
  }
 })
 if($rows.Count -ne 144){throw 'All twelve six-kind before/after pairs required'}
 $frequency=@($rows.frequency|Sort-Object -Unique)
 if($frequency.Count -ne 1 -or $frequency[0] -le 0 -or @($rows|Where-Object {$_.sequence % 2}).Count){throw 'Invalid counter snapshot'}
 $latency=@(Import-Csv "$root/measurement/$case/latency.csv")
 if($latency.Count -ne 12){throw 'Actual input intervals required'}
 for($kind=0;$kind -lt 6;$kind++){
  $calls=0L;$ticks=0L;$items=0L;$brackets=0L
  for($op=0;$op -lt 12;$op++){
   $before=@($rows|Where-Object {$_.operation -eq $op -and $_.kind -eq $kind -and $_.phase -eq 'before'})
   $after=@($rows|Where-Object {$_.operation -eq $op -and $_.kind -eq $kind -and $_.phase -eq 'after'})
   $clock=@($latency|Where-Object {[int]$_.operation -eq $op})
   if($before.Count -ne 1 -or $after.Count -ne 1 -or $clock.Count -ne 1){throw 'Duplicated/missing observation'}
   if($after.calls -lt $before.calls -or $after.ticks -lt $before.ticks -or $after.items -lt $before.items -or $before.qpc -gt [long]$clock[0].start -or $after.qpc -lt [long]$clock[0].stop){throw 'Invalid counter/input bracket'}
   $calls+=$after.calls-$before.calls;$ticks+=$after.ticks-$before.ticks;$items+=$after.items-$before.items;$brackets+=$after.qpc-$before.qpc
  }
  if($kind -eq 5 -and ($calls -or $ticks -or $items)){throw 'Unused counter contaminated'}
  $summary+=[pscustomobject]@{case=$case;kind=$names[$kind];operations=12;calls=$calls;items=$items;inclusiveMilliseconds=1000.0*$ticks/$frequency[0];bracketMilliseconds=1000.0*$brackets/$frequency[0];inputMilliseconds=($latency|Measure-Object microseconds -Sum).Sum/1000.0}
 }
}
if(!$summary.Count){throw 'No valid actual timing group'}
$summary|ConvertTo-Json|Set-Content "$root/graph-cost-summary.json"
@{wholeRunExit=(Get-Content "$root/exit.txt" -Raw).Trim();omitted=$omitted;limits='Inclusive nested counter durations, not disjoint CPU percentages; observer quiescence lies inside counter brackets; partial results never turn failed whole group green';performanceFixed=$false;published=$false}|ConvertTo-Json -Depth 5|Set-Content "$root/graph-cost-limits.json"
