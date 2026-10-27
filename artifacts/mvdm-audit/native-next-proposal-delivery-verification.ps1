$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
Set-Location $repo
& python.exe -B -X utf8 artifacts/mvdm-audit/native-next-proposal-verification.py
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Verify-DocumentationGovernance.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& tools/governance/Test-DocumentationRelativeLinks.ps1
if($LASTEXITCODE){exit $LASTEXITCODE}
& git diff --check
exit $LASTEXITCODE
