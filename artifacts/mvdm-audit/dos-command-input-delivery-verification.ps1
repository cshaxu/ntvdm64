$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/dos-command-input-role-verification.json --base bba17e229 --prefix dos-command-input-role- --expected-count 85
if($LASTEXITCODE){exit $LASTEXITCODE}
foreach($record in (Get-Content "$PSScriptRoot/dos-command-input-runs/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath $record.path
    if($file.Length -ne $record.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $record.sha256){throw 'Captured source-audit record drift'}
}
$package=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
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
'PASS exact 85-role source reconciliation, original identities, preserved images and documentation; no new runtime qualification'
