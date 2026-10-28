$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
$observed=Get-Content "$PSScriptRoot/ccpu-remaining-header-observations.json" -Raw|ConvertFrom-Json
if($observed.base -ne '771ddfb0c' -or $observed.headers.Count -ne 12){throw 'Wrong review cohort'}
$deps=[IO.File]::ReadAllText("$PSScriptRoot/ccpu-remaining-header-source/deps.stdout.txt")
$blocks=[regex]::Split($deps,'(?m)(?=^obj/)')
$ledger=Import-Csv "$PSScriptRoot/combined-initial-status.csv"
foreach($h in $observed.headers){
    if((Get-FileHash -LiteralPath (Join-Path 'src/mvdm' $h.relative_path)).Hash -ne $h.source_sha256){throw 'Reviewed source changed'}
    $row=@($ledger|Where-Object relative_path -eq $h.relative_path)
    if($row.Count -ne 1 -or $row[0].status -ne 'pending'){throw 'Unproved classification change'}
    $needle='src/mvdm/'+$h.relative_path
    $actual=@($blocks|Where-Object {$_.Replace('\','/').IndexOf($needle,[StringComparison]::OrdinalIgnoreCase) -ge 0}|ForEach-Object {($_ -split "`n")[0].TrimEnd()})
    if(($actual -join "`n") -cne (@($h.valid_dependency_consumers) -join "`n") -or ($actual|Where-Object {$_ -notmatch '\(VALID\)'})){throw 'Dependency observation changed or stale'}
    if($h.adjacent_cvid_variant_sha256){
        $variant=Join-Path 'src/mvdm/softpc.new/base/cvidc' (Split-Path $h.relative_path -Leaf)
        if((Get-FileHash -LiteralPath $variant).Hash -ne $h.adjacent_cvid_variant_sha256 -or $h.source_sha256 -eq $h.adjacent_cvid_variant_sha256){throw 'Distinct schema input changed'}
    }
}
foreach($record in $observed.inputs){
    if((Get-FileHash -LiteralPath $record.Path).Hash -ne $record.Hash){throw 'Review input drift'}
}
foreach($record in (Import-Csv "$PSScriptRoot/ccpu-remaining-header-source/inputs.csv")){
    if((Get-FileHash -LiteralPath $record.Path).Hash -ne $record.Hash){throw 'Ninja observation changed its product inputs'}
}
& "$PSScriptRoot/ccpu-header-macro-delivery-verification.ps1"
if($LASTEXITCODE){exit $LASTEXITCODE}
'PASS twelve exact pending source roles and whole-worker dependency observations; no runtime acceptance'
