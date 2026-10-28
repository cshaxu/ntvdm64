$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
$evidence=Join-Path $PSScriptRoot 'ccpu-header-macros'
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/ccpu-header-macro-role-verification.json --base 70985c440 --prefix ccpu-header-macro-role- --expected-count 10
if($LASTEXITCODE){exit $LASTEXITCODE}
$manifest=Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json
foreach($row in $manifest){
    $file=Get-Item -LiteralPath $row.path
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw "Captured evidence changed: $($row.path)"}
}
foreach($row in (Get-Content "$evidence/preprocess/run/cache-inputs.json" -Raw|ConvertFrom-Json)){
    if((Get-FileHash -LiteralPath $row.Path).Hash -ne $row.Hash){throw 'Preprocessing changed cached production input'}
}
foreach($case in @('control','control-negative','sign-flags','sign-flags-negative','bit-baseline','bit-candidate')){
    $knownFailure=$case.StartsWith('bit-')
    $expected=if($knownFailure){1}else{0}
    $completion=Get-Content "$evidence/regression/$case.completion.json" -Raw|ConvertFrom-Json
    if($completion.exit -ne $expected){throw "Unexpected diagnostic outcome: $case"}
    $inputs=Import-Csv "$evidence/regression/$case/inputs.csv"
    foreach($row in $inputs){
        # Generated media are bound by their recorded hash, assembler command and
        # source; current checked-in test/observer inputs must still match.
        if($row.Path -like '*\tests\*' -or $row.Path -like '*\build\tools\*'){
            if((Get-FileHash -LiteralPath $row.Path).Hash -ne $row.Hash){throw "Regression source changed: $($row.Path)"}
        }
    }
    $report=Get-Content "$evidence/regression/$case/observer.txt" -Raw
    $screen=Get-Content "$evidence/regression/$case/observer.txt.console.txt" -Raw
    if($report -notmatch 'result=exited'){throw 'Guest did not exit'}
    if($knownFailure){
        if($report -notmatch 'exit=0x00000001' -or $screen -notmatch 'BITMEM POSITIVE PASS' -or $screen -notmatch 'BITMEM near=8000 far=8000 carry=0' -or $screen -notmatch 'BITMEM NEGATIVE FAIL'){throw 'Recorded signed-index defect changed'}
    }elseif($case.EndsWith('-negative')){
        if($report -notmatch 'exit=0x00000001' -or $screen -notmatch 'FAIL'){throw 'Wrong oracle was not rejected'}
    }elseif($report -notmatch 'exit=0x00000000' -or $screen -notmatch 'PASS'){throw 'Positive regression failed'}
}
$native=Get-Content "$evidence/native-bit/oracle.completion.json" -Raw|ConvertFrom-Json
$nativeScreen=Get-Content "$evidence/native-bit/oracle.stdout.txt" -Raw
if($native.exit -ne 0 -or $nativeScreen -notmatch 'BITMEM NEGATIVE near=0000 far=0000 carry=1'){throw 'Hardware oracle observation missing'}
foreach($row in (Import-Csv "$evidence/native-bit/inputs.csv")){
    if((Get-FileHash -LiteralPath $row.Path).Hash -ne $row.Hash){throw 'Hardware observation input drift'}
}
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
& git diff --cached --check
if($LASTEXITCODE){exit $LASTEXITCODE}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT exceeds 16KiB'}
'PASS ten-role reconciliation, eight preprocessing contexts, four oracle regressions, two reproduced nonpasses, preserved images and documentation; no runtime qualification'
