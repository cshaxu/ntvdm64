$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/disk-native-role-verification.json --base 2d9a08911 --prefix disk-native-role- --expected-count 7
if($LASTEXITCODE){exit $LASTEXITCODE}
$evidence=Join-Path $PSScriptRoot 'disk-runs'
foreach($record in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath $record.path
    if($file.Length -ne $record.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $record.sha256){throw 'Captured disk witness drift'}
}
foreach($case in @('baseline','baseline-negative','candidate','candidate-negative')){
    $root=Join-Path $evidence "disk-guest/$case"
    $report=[IO.File]::ReadAllText("$root/observer.txt")
    $screen=[IO.File]::ReadAllText("$root/observer.txt.console.txt")
    $negative=$case.EndsWith('-negative')
    $exit=if($negative){'exit=0x00000001'}else{'exit=0x00000000'}
    if($report -notmatch 'result=exited' -or $report -notmatch $exit -or [string]::IsNullOrWhiteSpace($screen)){throw 'Captured guest outcome incomplete'}
    if($negative){
        if($screen -notmatch 'S28_INT13_RESET_FAIL' -or $screen -match 'S28_DISKS_OK'){throw 'Wrong carry oracle not rejected'}
    }elseif($screen -notmatch 'S28_INT13_RESET_OK S28_DISKS_OK'){throw 'Disk reset marker absent'}
    foreach($row in (Import-Csv "$root/inputs.csv")){
        if((Get-FileHash -LiteralPath $row.Path).Hash -ne $row.Hash){throw 'Probe/tool input changed'}
    }
    $package=if($case.StartsWith('candidate')){'build/t473-s10/candidate-validation/package'}else{'build/package-uuid/package-c'}
    foreach($row in (Import-Csv "$root/runtime-inputs.csv")){
        if((Get-FileHash (Join-Path "$package/system32" (Split-Path $row.Path -Leaf))).Hash -ne $row.Hash){throw 'Guest runtime differs from pinned package'}
    }
}
foreach($directory in @('gfi-object-contract','disk-owner-review','disk-guest','disk-role-final')){
    $name=if($directory -eq 'gfi-object-contract'){'supervisor.completion.json'}else{'completion.json'}
    $done=Get-Content "$evidence/$directory/$name" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw "Owned run incomplete: $directory"}
}
$candidate=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($candidate).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $candidate){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Held candidate changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Accepted package incomplete'}
foreach($row in $baseline){
    foreach($directory in @('assets/release','O:/winnt/system32')){
        if((Get-FileHash (Join-Path $directory $row.Name)).Hash -ne $row.Sha256){throw 'Published baseline changed'}
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
'PASS seven partial original disk roles, two-audit/edition join, 102 image-bound functions, 47 initializer pointers, 433/70/37 object assertions, four guest/oracle outcomes and unchanged packages; real-media and profile obligations remain open'
