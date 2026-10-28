$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/video-native-role-verification.json --base 1b506b719 --prefix video-native-role- --expected-count 6
if($LASTEXITCODE){exit $LASTEXITCODE}
$evidence=Join-Path $PSScriptRoot 'video-runs'
foreach($row in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath $row.path
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw 'Captured video evidence changed'}
}
foreach($group in @('video-invalid-guest','video-dispatch-control')){
    $cases=if($group -eq 'video-invalid-guest'){@('baseline','baseline-negative','candidate','candidate-negative')}else{@('baseline','baseline-negative')}
    foreach($case in $cases){
        $root=Join-Path $evidence "$group/$case"
        $done=Get-Content "$evidence/$group/$case.completion.json" -Raw|ConvertFrom-Json
        $context=Get-Content "$root/runner-context.json" -Raw|ConvertFrom-Json
        if($done.exit -ne 0 -or $context.integrity_sid -ne 'S-1-16-8192' -or !$context.private_desktop){throw 'Incomplete/incorrect guest context'}
        $report=[IO.File]::ReadAllText("$root/observer.txt")
        $screen=[IO.File]::ReadAllText("$root/observer.txt.console.txt")
        $negative=$case.EndsWith('-negative')
        $exit=if($negative){'exit=0x00000001'}else{'exit=0x00000000'}
        if($report -notmatch 'result=exited' -or $report -notmatch $exit -or [string]::IsNullOrWhiteSpace($screen)){throw 'Guest outcome incomplete'}
        if($group -eq 'video-invalid-guest'){
            if($negative){
                if($screen -notmatch 'VIDEO INVALID FAIL phase=V' -or $screen -match 'VIDEO INVALID PASS'){throw 'Wrong video oracle accepted'}
            }elseif($screen -notmatch 'VIDEO INVALID PASS cases=1812' -or $screen -notmatch 'GENERIC908 EGA904'){throw 'Incomplete video cases'}
        }elseif($negative){
            if($screen -notmatch 'BIOS DISPATCH FAIL phase=C' -or $screen -match 'BIOS DISPATCH PASS'){throw 'Wrong default oracle accepted'}
        }elseif($screen -notmatch 'BIOS DISPATCH PASS cases=836'){throw 'Default BIOS control incomplete'}
        foreach($row in (Import-Csv "$root/inputs.csv")){
            if((Get-FileHash -LiteralPath $row.Path).Hash -ne $row.Hash){throw 'Probe/tool input drift'}
        }
        $package=if($case.StartsWith('candidate')){'build/t473-s10/candidate-validation/package'}else{'build/package-uuid/package-c'}
        foreach($row in (Import-Csv "$root/runtime-inputs.csv")){
            if((Get-FileHash (Join-Path "$package/system32" (Split-Path $row.Path -Leaf))).Hash -ne $row.Hash){throw 'Runtime identity drift'}
        }
    }
}
foreach($group in @('video-image','video-role-final','video-invalid-guest','video-dispatch-control')){
    $done=Get-Content "$evidence/$group/completion.json" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw 'Owned run nonpass'}
}
$candidate=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($candidate).Count -ne 10){throw 'Candidate incomplete'}
foreach($row in $candidate){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Baseline incomplete'}
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
'PASS six video file-role changes, two-audit/edition identities, 21 code bodies, 67 initial pointers, both 1812-case guest witnesses/oracle negatives and unchanged 836-case control; no full video or release qualification'
