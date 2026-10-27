$ErrorActionPreference='Stop'
Set-Location 'O:\repos.hobby\ntvdm64'
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/ccpu-control-role-verification.json --base 400c2e392 --prefix ccpu-control-role- --expected-count 10
if($LASTEXITCODE){exit $LASTEXITCODE}
$image=Get-Content artifacts/mvdm-audit/ccpu-control/image/result.json -Raw|ConvertFrom-Json
if((Get-FileHash build/t473-s10/candidate-validation/package/system32/ntvdm.exe).Hash -ne $image.image_sha256 -or (Get-FileHash build/.ninja/msvc-x86-release/worker/ntvdm.exe).Hash -ne $image.cached_worker_sha256){throw 'Code witness input drift'}
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
'PASS control guest/negative/code witnesses, exact ten-row reconciliation, image preservation and documentation gates'
