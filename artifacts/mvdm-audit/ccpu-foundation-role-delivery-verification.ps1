$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/ccpu-foundation-role-verification.json --base 094c0cd96 --prefix ccpu-foundation-role- --expected-count 9
if($LASTEXITCODE){exit $LASTEXITCODE}
$root='artifacts/mvdm-audit/ccpu-neg64'
$manifest=Get-Content "$root/observations.json" -Raw|ConvertFrom-Json
foreach($r in $manifest.records){if((Get-FileHash "$root/$($r.file)").Hash -ne $r.sha256){throw 'NEG64 durable record drift'}}
$native=Get-Content "$root/result.json" -Raw|ConvertFrom-Json
if($native.result -ne 'pass' -or $native.observed_cases -ne 631410 -or $native.cpu_state_stubs -ne $false){throw 'Missing native witness'}
$image=Get-Content "$root/image-result.json" -Raw|ConvertFrom-Json
foreach($r in $image.images){if((Get-FileHash $r.path).Hash -ne $r.image_sha256){throw 'Native/worker image drift'}}
$package=Get-Content artifacts/mvdm-audit/sol-creation-debugger/candidate-package-inputs.json -Raw|ConvertFrom-Json
if(@($package).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $package){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate input changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Accepted package incomplete'}
foreach($row in $baseline){
    foreach($directory in @('assets/release','O:/winnt/system32')){
        if((Get-FileHash (Join-Path $directory $row.Name)).Hash -ne $row.Sha256){throw 'Accepted baseline changed'}
    }
}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
if($LASTEXITCODE){exit $LASTEXITCODE}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT exceeds 16KiB'}
'PASS nine foundation partial integrations; 52 full unchanged; candidate and accepted images unchanged'
