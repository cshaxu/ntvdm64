$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
& python.exe -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/selected-ccpu-header-role-verification.json --base 9ca2db194 --prefix selected-ccpu-header-role- --expected-count 71
if($LASTEXITCODE){exit $LASTEXITCODE}
foreach($name in @('ccpu-preprocessing','sol-creation-debugger')){
    $root='artifacts/mvdm-audit/'+$name
    $observation=Get-Content "$root/observations.json" -Raw|ConvertFrom-Json
    foreach($record in $observation.records){
        if((Get-FileHash (Join-Path $root $record.file)).Hash -ne $record.sha256){throw 'Recorded diagnostic identity mismatch'}
    }
    $sourceField=if($name -eq 'ccpu-preprocessing'){'product_source_changed'}else{'production_source_changed'}
    if(!$observation.PSObject.Properties[$sourceField] -or !$observation.PSObject.Properties['guest_changed'] -or
            $observation.$sourceField -ne $false -or $observation.guest_changed -ne $false){throw 'Unexpected or missing source change claim'}
}
$root='artifacts/mvdm-audit/sol-creation-debugger'
$observation=Get-Content "$root/observations.json" -Raw|ConvertFrom-Json
if($observation.publishable -or @($observation.runs|Where-Object {$_.exit -ne 1 -or $_.result -ne 'non-pass'}).Count){throw 'Diagnostic overclaim'}
if(!$observation.cleanup_claim_correction){throw 'Missing correction to pre-cleanup process claim'}
$correction=Get-Content "$root/cleanup-correction.json" -Raw|ConvertFrom-Json
if(!$correction.scan_completed -or !$correction.targets_absent -or @($correction.matching_active_images).Count){throw 'Cleanup correction/observation incomplete'}
$cleanup=@(Get-Content "$root/cleanup-result.json" -Raw|ConvertFrom-Json)
if($cleanup.Count -ne 4 -or @($cleanup|Where-Object {!$_.Applied}).Count -or
        ($cleanup|Measure-Object Files -Sum).Sum -ne 33 -or ($cleanup|Measure-Object Bytes -Sum).Sum -ne 1106736){throw 'Cleanup results differ'}
if((Get-Content "$root/entry-first-result.txt" -Raw) -notmatch 'CAPTURED_5023'){throw 'First observation witness missing'}
if((Get-Content "$root/entry-interval-result.txt" -Raw) -notmatch 'visits=25887 writes_5023=1270 unique_callers=5'){throw 'Interval counters differ'}
$report=Get-Content "$root/claim-failure-result.txt" -Raw
if($report -notmatch 'visits=0 no-claim-failure' -or $report -notmatch 'PASS SOL Options command, modal dialog, close and parent recovery' -or $report -notmatch 'launcher did not finish'){throw 'Failure-only diagnostic outcome differs'}
if($report -match 'clear.*failed|resume-context failed'){throw 'Observation cleanup failed'}
$package=Get-Content "$root/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($package).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $package){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate input changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
foreach($row in $baseline){
    foreach($directory in @('assets/release','O:/winnt/system32')){
        if((Get-FileHash (Join-Path $directory $row.Name)).Hash -ne $row.Sha256){throw 'Accepted published baseline changed'}
    }
}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
exit $LASTEXITCODE
