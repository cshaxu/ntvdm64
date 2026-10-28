$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';Set-Location $repo
$env:PYTHONDONTWRITEBYTECODE='1'
& python.exe -B artifacts/mvdm-audit/guest-source-role-checkpoint.py build/t473-s10/vdd-final-checkpoint.json --base 6f7ca8722 --prefix vdd-native-role- --expected-count 2
if($LASTEXITCODE){throw 'VDD checkpoint failed'}
& artifacts/mvdm-audit/latest-source-delivery-verification.ps1
& tools/governance/Verify-DocumentationGovernance.ps1
& tools/governance/Test-DocumentationRelativeLinks.ps1
& git diff --check
if($LASTEXITCODE){throw 'Diff check failed'}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT too large'}
'PASS source/ledger/object/probe evidence, unchanged current publication and documentation gates'
