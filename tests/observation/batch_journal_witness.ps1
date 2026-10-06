function Assert-BatchJournal {
 param([Parameter(Mandatory)][AllowEmptyCollection()][string[]]$Actual,
       [Parameter(Mandatory)][string[]]$Expected)
 if(!$Expected.Count -or ($Actual -join "`n") -cne ($Expected -join "`n")){
  throw 'Missing, duplicate, stale, wrong-width or reordered batch execution journal'
 }
}
