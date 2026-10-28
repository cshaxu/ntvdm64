$ErrorActionPreference='Stop'
$repo='O:/repos.hobby/ntvdm64';$root="$repo/build/t473-s10/latest-source-release-r3"
Set-Location $repo
& artifacts/mvdm-audit/latest-source-delivery-verification.ps1
& tools/governance/Verify-DocumentationGovernance.ps1
& tools/governance/Test-DocumentationRelativeLinks.ps1
& git diff --check
if($LASTEXITCODE){throw 'Diff check failed'}
$files=@(& git diff --name-only -- '*.ps1')+@('tests/component-integration/szdd_roundtrip_test.ps1','tests/component-integration/win31_setup_profile_generation_test.ps1','artifacts/mvdm-audit/latest-source-delivery-verification.ps1')
foreach($file in $files){if(!(Test-Path -LiteralPath $file)){continue};$tokens=$null;$errors=$null;[void][Management.Automation.Language.Parser]::ParseFile((Join-Path $repo $file),[ref]$tokens,[ref]$errors);if($errors){throw "PowerShell parse errors: $file $errors"}}
if((Get-Item docs/states/CURRENT.md).Length -gt 16384){throw 'CURRENT too large'}
'PASS delivery, documentation, relative links, PowerShell syntax and diff checks'
