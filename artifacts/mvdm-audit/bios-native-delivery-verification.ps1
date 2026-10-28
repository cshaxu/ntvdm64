$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/bios-native-role-verification.json --base e0b7e8a42 --prefix bios-native-role- --expected-count 7
if($LASTEXITCODE){exit $LASTEXITCODE}
$evidence=Join-Path $PSScriptRoot 'bios-runs'
foreach($record in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath $record.path
    if($file.Length -ne $record.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $record.sha256){throw 'Captured BIOS witness drift'}
}
foreach($case in @('bios-baseline','bios-baseline-negative','bios-candidate','bios-candidate-negative','control','control-negative','sign-flags','sign-flags-negative')){
    $completion=Get-Content "$evidence/bios-dispatch/$case.completion.json" -Raw|ConvertFrom-Json
    if($completion.exit -ne 0){throw "Runner nonpass: $case"}
    $negative=$case.EndsWith('-negative')
    $report=Get-Content "$evidence/bios-dispatch/$case/observer.txt" -Raw
    $screen=Get-Content "$evidence/bios-dispatch/$case/observer.txt.console.txt" -Raw
    $guestExit=if($negative){'exit=0x00000001'}else{'exit=0x00000000'}
    if($report -notmatch 'result=exited' -or $report -notmatch $guestExit){throw 'Unexpected guest outcome'}
    if($case.StartsWith('bios-')){
        if($negative){
            if($screen -notmatch 'BIOS DISPATCH FAIL phase=C' -or $screen -match 'BIOS DISPATCH PASS'){throw 'Wrong BIOS oracle not rejected'}
        }elseif($screen -notmatch 'BIOS DISPATCH PASS cases=836' -or $screen -notmatch 'CONTROL768 ILLEGAL64 EMSBAD4'){
            throw 'Incomplete BIOS guest witness'
        }
    }elseif($negative){
        if($screen -notmatch 'FAIL phase=A'){throw 'Existing wrong oracle not rejected'}
    }elseif($screen -notmatch 'PASS'){
        throw 'Existing positive guest regression failed'
    }
    foreach($record in (Import-Csv "$evidence/bios-dispatch/$case/inputs.csv")){
        if((Get-FileHash -LiteralPath $record.Path).Hash -ne $record.Hash){throw 'Guest probe/tool input changed'}
    }
    $package=if($case.StartsWith('bios-candidate')){'build/t473-s10/candidate-validation/package'}else{'build/package-uuid/package-c'}
    foreach($record in (Import-Csv "$evidence/bios-dispatch/$case/runtime-inputs.csv")){
        $name=Split-Path $record.Path -Leaf
        if((Get-FileHash (Join-Path "$package/system32" $name)).Hash -ne $record.Hash){throw 'Observed staged image differs from pinned package'}
    }
}
$packageInputs=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($packageInputs).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $packageInputs){
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
'PASS seven partial original BIOS roles, 52 bound functions, 286 vector pointers, eight real guest/oracle runs, rejected wrong dispatch, preserved packages and documentation; no full runtime qualification'
