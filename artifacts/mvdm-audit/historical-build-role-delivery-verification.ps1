$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
& python.exe -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/historical-build-role-verification.json --base 35454cc49 --prefix historical-build-role- --expected-count 36
if($LASTEXITCODE){exit $LASTEXITCODE}
$root='artifacts/mvdm-audit/native-cbt-info'
$observation=Get-Content "$root/observations.json" -Raw|ConvertFrom-Json
foreach($record in $observation.records){
    if((Get-FileHash (Join-Path $root $record.file)).Hash -ne $record.sha256){throw 'Native observation identity mismatch'}
}
if((Get-Content "$root/exit.json" -Raw|ConvertFrom-Json).exit -ne 0){throw 'Native observation did not complete'}
if((Get-Content "$root/result.txt" -Raw) -notmatch 'rounds=500 created=3500 samples=5000 info_failures=0 invalid_windows=0 class_failures=0 thread_failures=0'){throw 'Native observation counters differ'}
if($observation.product_source_changed -or $observation.guest_changed){throw 'Unexpected source change claim'}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
exit $LASTEXITCODE
