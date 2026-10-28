$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/planar-native-role-verification.json --base bea69f8b4 --prefix planar-native-role- --expected-count 3
if($LASTEXITCODE){exit $LASTEXITCODE}
foreach($group in @('planar-image','planar-role')){
    $done=Get-Content "build/t473-s10/$group/completion.json" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw 'Planar source/role supervisor failed'}
}
foreach($row in (Get-Content "$PSScriptRoot/planar-role-runs/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath (Join-Path "$PSScriptRoot/planar-role-runs" $row.path)
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw 'Planar role receipt drift'}
}
$candidate=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($candidate).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $candidate){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Accepted package incomplete'}
foreach($row in $baseline){foreach($directory in @('assets/release','O:/winnt/system32')){
    if((Get-FileHash (Join-Path $directory $row.Name)).Hash -ne $row.Sha256){throw 'Published package changed'}
}}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --cached --check
if($LASTEXITCODE){exit $LASTEXITCODE}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT exceeds 16KiB'}
'PASS three planar partial owners, two catalogs/editions, 40 functions, 33 dispatch slots, six munge targets and 26114 actual-object cases; packages unchanged, guest rendering/lifetimes remain open'
