$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
& python.exe -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/native-source-next-role-verification.json --base 536087158 --prefix native-source-next-role- --expected-count 48
if($LASTEXITCODE){exit $LASTEXITCODE}
$root='artifacts/mvdm-audit/creation-negative-only'
$observation=Get-Content "$root/ordinary-control-r3-observations.json" -Raw|ConvertFrom-Json
foreach($record in $observation.records){
    if((Get-FileHash (Join-Path $root $record.file)).Hash -ne $record.sha256){throw 'Ordinary-control evidence identity mismatch'}
}
if((Get-Content "$root/ordinary-control-r3-exit.json" -Raw|ConvertFrom-Json).exit -ne 0){throw 'Control did not pass'}
if((Get-Content "$root/ordinary-control-r3-result.txt" -Raw) -notmatch 'PASS SOL Options command, modal dialog, close and parent recovery'){throw 'Missing actual control witness'}
if($observation.candidate_qualified -or !$observation.earlier_two_failures_retained){throw 'Diagnostic overclaim'}
if((Get-FileHash build/t473-s10/candidate-validation/package/product-system32-manifest.json).Hash -ne $observation.package_manifest_sha256){throw 'Candidate package changed'}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
exit $LASTEXITCODE
