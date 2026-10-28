$ErrorActionPreference='Stop'
Set-Location (Join-Path $PSScriptRoot '../..')
& python -B -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/retired-host-role-verification.json --base de734bc9c --prefix retired-host-role- --expected-count 4
if($LASTEXITCODE){exit $LASTEXITCODE}
foreach($kind in @('normal','role')){
    $done=Get-Content "build/t473-s10/retired-host-image-final/$kind.completion.json" -Raw|ConvertFrom-Json
    if($done.exit -ne 0){throw 'Retired host source/role supervisor failed'}
}
foreach($row in (Get-Content "$PSScriptRoot/retired-host-runs/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath (Join-Path "$PSScriptRoot/retired-host-runs" $row.path)
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw 'Retired host receipt drift'}
}
$recovery=Get-Content "$PSScriptRoot/retired-host-install-recovery/result.json" -Raw|ConvertFrom-Json
if($recovery.result -ne 'pass' -or !$recovery.restored_existing_accepted_package -or $recovery.new_candidate_qualified -or $recovery.guest_changed){throw 'Installed recovery receipt incomplete'}
foreach($row in (Get-Content "$PSScriptRoot/retired-host-install-recovery/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath (Join-Path "$PSScriptRoot/retired-host-install-recovery" $row.path)
    if($file.Length -ne $row.bytes -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne $row.sha256){throw 'Installed recovery receipt drift'}
}
$candidate=Get-Content "$PSScriptRoot/sol-creation-debugger/candidate-package-inputs.json" -Raw|ConvertFrom-Json
if(@($candidate).Count -ne 10){throw 'Candidate package incomplete'}
foreach($row in $candidate){
    if((Get-FileHash (Join-Path 'build/t473-s10/candidate-validation/package' $row.RelativePath)).Hash -ne $row.Sha256){throw 'Candidate changed'}
}
$baseline=Get-Content "$PSScriptRoot/latest-source-release/ten-component-manifest.json" -Raw|ConvertFrom-Json
if(@($baseline).Count -ne 10){throw 'Current source-built package incomplete'}
foreach($row in $baseline){foreach($directory in @('assets/release','O:/winnt/system32')){
    if((Get-FileHash (Join-Path $directory $row.Name)).Hash -ne $row.Sha256){throw 'Current source-built publication differs'}
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
'PASS four physical NT predecessors and retained controls; selected capabilities remain partial; installed/release images match the owner-selected latest source-built package'
