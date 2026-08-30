[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path

function Invoke-FormalGraph([string]$scriptName, [string]$buildPath,
    [string]$programPath = '') {
    & (Join-Path $root ('tools/build/' + $scriptName)) -Architecture $Architecture -RepositoryRoot $root
    & ninja -C (Join-Path $root $buildPath)
    if ($LASTEXITCODE -ne 0) { throw "Ninja failed: $buildPath" }
    if (![string]::IsNullOrWhiteSpace($programPath)) {
        & (Join-Path $root $programPath)
        if ($LASTEXITCODE -ne 0) { throw "Fixture failed: $programPath" }
    }
}

# One package-level local matrix. Every executable remains source-local and
# selector-disabled; complementary source-proven unavailable outcomes are in
# m0-t289-s6-dpmi-family-disposition-matrix.tsv.
Invoke-FormalGraph 'New-T289S3DpmiFoundationNinja.ps1' ("build/M0-T289/S3/{0}" -f $Architecture) ("build/M0-T289/S3/{0}/bin/t289-s3-protected-machine-fixture.exe" -f $Architecture)
Invoke-FormalGraph 'New-T289S5RealModeFrameNinja.ps1' ("build/M0-T289/S5/realmode-frame-{0}" -f $Architecture) ("build/M0-T289/S5/realmode-frame-{0}/bin/t289-s5-realmode-frame-fixture.exe" -f $Architecture)
Invoke-FormalGraph 'New-T289S5DpmiRegistrationNinja.ps1' ("build/M0-T289/S5/registration-{0}" -f $Architecture) ("build/M0-T289/S5/registration-{0}/bin/dpmi-registration-fixture.exe" -f $Architecture)
Invoke-FormalGraph 'New-T289S5RealModeStackNinja.ps1' ("build/M0-T289/S5/realmode-stack-{0}" -f $Architecture) ("build/M0-T289/S5/realmode-stack-{0}/bin/t289-s5-realmode-stack-fixture.exe" -f $Architecture)
Invoke-FormalGraph 'New-T289S5DpmiInterruptAuditNinja.ps1' ("build/M0-T289/S5/audit-{0}" -f $Architecture)
Invoke-FormalGraph 'New-T289S6DpmiVddMiscNinja.ps1' ("build/M0-T289/S6/vdd-misc-{0}" -f $Architecture) ("build/M0-T289/S6/vdd-misc-{0}/bin/dpmi-vdd-misc-fixture.exe" -f $Architecture)

Write-Host "T289 S6 DPMI family matrix passed: $Architecture"
