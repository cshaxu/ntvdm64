$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/system-native-role-verification.json --base eb218c706 --prefix system-native-role- --expected-count 8
if($LASTEXITCODE){exit $LASTEXITCODE}
$evidence=Join-Path $PSScriptRoot 'system-runs'
foreach($record in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath $record.path
    if($file.Length -ne $record.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $record.sha256){throw 'Captured system witness drift'}
}
$run=Join-Path $evidence 'system-integration-r5'
foreach($case in @('system-baseline','system-baseline-negative','system-candidate','system-candidate-negative','dummy-baseline','dummy-baseline-negative','dummy-candidate','dummy-candidate-negative','bios-control','bios-control-negative')){
    $completion=Get-Content "$run/$case.completion.json" -Raw|ConvertFrom-Json
    if($completion.exit -ne 0){throw "Runner nonpass: $case"}
    $negative=$case.EndsWith('-negative')
    $report=[IO.File]::ReadAllText("$run/$case/observer.txt")
    $screen=[IO.File]::ReadAllText("$run/$case/observer.txt.console.txt")
    if([string]::IsNullOrWhiteSpace($screen)){throw 'Missing guest output'}
    $guestExit=if($negative){'exit=0x00000001'}else{'exit=0x00000000'}
    if($report -notmatch 'result=exited' -or $report -notmatch $guestExit){throw 'Unexpected guest outcome'}
    if($case.StartsWith('system-')){
        if($negative){
            if($screen -notmatch 'S24_CMOS_FAIL' -or $screen -match 'S24_SYSTEM_OK'){throw 'Wrong CMOS oracle not rejected'}
        }else{
            foreach($marker in @('S24_TIMER_OK','S24_CMOS_OK','S24_RTC_PERIODIC_OK','S24_PIC_OK','S24_DMA_PORT_OK','S24_ROM_READ_OK','S24_SYSTEM_OK')){
                if($screen -notmatch $marker){throw "Missing system assertion: $marker"}
            }
        }
    }elseif($case.StartsWith('dummy-')){
        if($negative){
            if($screen -notmatch 'DUMMY INT FAIL phase=D' -or $screen -match 'DUMMY INT PASS'){throw 'Wrong dummy oracle not rejected'}
        }elseif($screen -notmatch 'DUMMY INT PASS cases=16'){throw 'Dummy dispatch incomplete'}
    }elseif($negative){
        if($screen -notmatch 'BIOS DISPATCH FAIL phase=C' -or $screen -match 'BIOS DISPATCH PASS'){throw 'Wrong BIOS oracle not rejected'}
    }elseif($screen -notmatch 'BIOS DISPATCH PASS cases=836' -or $screen -notmatch 'CONTROL768 ILLEGAL64 EMSBAD4'){throw 'BIOS regression incomplete'}
    foreach($record in (Import-Csv "$run/$case/inputs.csv")){
        if((Get-FileHash -LiteralPath $record.Path).Hash -ne $record.Hash){throw 'Guest probe/tool input changed'}
    }
    $package=if($case.Contains('candidate')){'build/t473-s10/candidate-validation/package'}else{'build/package-uuid/package-c'}
    foreach($record in (Import-Csv "$run/$case/runtime-inputs.csv")){
        $name=Split-Path $record.Path -Leaf
        if((Get-FileHash (Join-Path "$package/system32" $name)).Hash -ne $record.Hash){throw 'Staged runtime differs from package'}
    }
}
foreach($name in @('completion','pic.completion','build.completion','incremental-after.completion')){
    $done=Get-Content "$run/$name.json" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw "Incomplete owned run: $name"}
}
$pic=[IO.File]::ReadAllText("$run/pic.stdout.txt")+[IO.File]::ReadAllText("$run/pic.stderr.txt")
if($pic -notmatch 'PIC_RESET_EOI assertions=24 failures=0'){throw 'Missing native assertions'}
if([IO.File]::ReadAllText("$run/incremental-after.stdout.txt") -notmatch 'no work to do'){throw 'Incremental not clean'}
foreach($csv in @('native-inputs.csv','preserved-production-inputs.csv')){
    foreach($record in (Import-Csv "$run/$csv")){
        if((Get-FileHash -LiteralPath $record.Path).Hash -ne $record.Hash){throw "Current input drift: $csv"}
    }
}
$old='build/t473-s10/bios-dispatch/bios-baseline/EMSPROBE.COM'
$new='build/t473-s10/system-integration-r5/bios-control/EMSPROBE.COM'
if((Get-FileHash $old).Hash -ne (Get-FileHash $new).Hash){throw 'Default BIOS probe changed'}
$candidate=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($candidate).Count -ne 10){throw 'Candidate incomplete'}
foreach($row in $candidate){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate changed'}
}
$baseline=Get-Content build/package-uuid/package-c/product-system32-manifest.json -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Baseline incomplete'}
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
'PASS eight system roles, 77 image-bound functions, ten real guest/oracle contracts, 24 native assertions, incremental reuse and unchanged published package; wider device and candidate qualification remain open'
