$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/input-native-role-verification.json --base 010952cdd --prefix input-native-role- --expected-count 5
if($LASTEXITCODE){exit $LASTEXITCODE}
$evidence=Join-Path $PSScriptRoot 'input-runs'
foreach($row in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath $row.path
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw 'Captured input evidence drift'}
}
$run=Join-Path $evidence 'input-object-contracts'
foreach($name in @('keycode','nls','ios')){
    $receipt=Get-Content "$run/$name/result.json" -Raw|ConvertFrom-Json
    $text=[IO.File]::ReadAllText("$run/$name/run.stdout.txt")
    $marker=$name.ToUpperInvariant()+'_OBJECT_CONTRACT checks='+$receipt.checks+' failures=0 callbacks='+$receipt.callbacks
    if($receipt.result -ne 'pass' -or !$text.Contains($marker)){throw 'Original-object normal witness incomplete'}
    foreach($row in $receipt.inputs){
        if((Get-FileHash -LiteralPath $row.path).Hash -ne $row.sha256){throw 'Native contract input changed'}
    }
    if((Get-FileHash -LiteralPath $receipt.fixture_path).Hash -ne $receipt.fixture_sha256){throw 'Native fixture changed'}
    if([IO.File]::ReadAllText("$run/$name/negative-abi.stdout.txt") -notmatch 'C2118'){throw 'ABI negative missing'}
}
$boundary=Get-Content "$run/boundary.completion.json" -Raw|ConvertFrom-Json
$text=[IO.File]::ReadAllText("$run/boundary.stdout.txt")
if($boundary.exit -ne 1 -or $boundary.expected_exit -ne 1 -or $text -notmatch 'expected_rejected=2 actual_rejected=0'){
    throw 'Known exact-bound contract failure not preserved'
}
if(([regex]::Matches($text,'sentinel=123 observed=123 restored=0')).Count -ne 2){throw 'One-past read/restoration evidence incomplete'}
foreach($group in @('input-image','input-role-final','input-object-contracts')){
    $done=Get-Content "$evidence/$group/completion.json" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw 'Owned source/fixture supervisor failed'}
}
$candidate=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($candidate).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $candidate){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Accepted package incomplete'}
foreach($row in $baseline){
    foreach($directory in @('assets/release','O:/winnt/system32')){
        if((Get-FileHash (Join-Path $directory $row.Name)).Hash -ne $row.Sha256){throw 'Published package changed'}
    }
}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --cached --check
if($LASTEXITCODE){exit $LASTEXITCODE}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT exceeds 16KiB'}
'PASS five input/NLS file-role changes, two-audit identities, six bound functions/callback slots, 217 scan bytes, 268322/3645/70 bounded normal assertions and ABI negatives; exact-bound strict failure retained, not fixed or waived; packages unchanged'
