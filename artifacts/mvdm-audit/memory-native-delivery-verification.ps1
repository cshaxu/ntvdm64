$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/memory-native-role-verification.json --base 4d0f2d2c6 --prefix memory-native-role- --expected-count 3
if($LASTEXITCODE){exit $LASTEXITCODE}
foreach($group in @('memory-image','memory-role-r2','umb-supervisor-r3','memory-ios-supervisor')){
    $done=Get-Content "build/t473-s10/$group/completion.json" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw 'Memory source/native supervisor failed'}
}
foreach($row in (Get-Content "$PSScriptRoot/memory-role-runs/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath (Join-Path "$PSScriptRoot/memory-role-runs" $row.path)
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw 'Memory role receipt drift'}
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
'PASS one partial UMB owner/two physical alternatives, original CPU40 and VDD callers, 4240/70 normal assertions and ABI negatives; six invalid-helper inputs retained as non-pass, not established product failures; packages unchanged'
