[CmdletBinding()]
param([string]$RepositoryRoot = '')

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
}

function Require([bool]$Condition, [string]$Message) {
    if (!$Condition) { throw $Message }
}

# These were the handwritten XMS provider and mixed XMS/DPMI route.  Their
# removal is intentional: DPMI's remaining historical classifier has a
# DPMI-only identity, while XMS is recovered through the OpenNT source mirror.
$retiredPaths = @(
    'src/bx-vdm/bop-v1/bx_ntvdm_xms_package_session_v1.c',
    'src/bx-vdm/bop-v1/bx_ntvdm_xms_dpmi_plane_v1.c',
    'src/bx-vdm/bx_ntvdm_xms_package_session_v1.h',
    'src/bx-vdm/bx_ntvdm_xms_dpmi_plane_v1.h',
    'tests/bx-vdm/bx_ntvdm_xms_package_session_v1_test.c',
    'tests/bx-vdm/bx_ntvdm_xms_dpmi_plane_v1_test.c',
    'tests/bx-machine/t199_s31_xms_native_fixture.cc',
    'tests/bx-machine/t199_s35_native_bop_composition_fixture.cc'
)
foreach ($relative in $retiredPaths) {
    Require -Condition (!(Test-Path -LiteralPath (Join-Path $RepositoryRoot $relative))) `
        -Message "Retired XMS v1 input remains: $relative"
}

$legacyProviderHits = @(
    rg -l -i 'bx_ntvdm_xms_(package_session|dpmi_plane)|xms_package_session_v1|xms_dpmi_plane_v1' `
        (Join-Path $RepositoryRoot 'src'), (Join-Path $RepositoryRoot 'tests') `
        --glob '*.[ch]' --glob '*.cc' --glob '*.ps1' --glob '*.json'
)
Require ($legacyProviderHits.Count -eq 0) 'An executable/test/tool XMS v1 provider identity remains.'

$manifestPath = Join-Path $RepositoryRoot 'tools/build/t225-s7-full-module-manifest.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$bxVdm = @($manifest.modules | Where-Object { $_.name -eq 'bx-vdm' })
Require ($bxVdm.Count -eq 1) 'Expected one formal bx-vdm module.'
$sources = @($bxVdm[0].sources)
Require (($sources -join "`n") -notmatch 'bop-v1') 'Formal bx-vdm module retains a bop-v1 source.'
foreach ($source in @(
    'src/bx-vdm/bop/opennt/xms/xms.c',
    'src/bx-vdm/bop/opennt/xms/xmsa20.c',
    'src/bx-vdm/bop/opennt/xms/xmsblock.c',
    'src/bx-vdm/bop/opennt/xms/xmsdisp.c',
    'src/bx-vdm/bop/opennt/xms/xmsmisc.c',
    'src/bx-vdm/bop/opennt/xms/xmsumb.c',
    'src/bx-vdm/bop/opennt/xms/suballoc.c',
    'src/bx-vdm/bop/shim/xms_shim.c',
    'src/bx-vdm/bop/shim/xms_native_session_shim.c',
    'src/bx-vdm/bop/xms_v2_generic_ud_bridge.c',
    'src/bx-vdm/bop/xms_v2_runtime_session.c'
)) {
    Require ($source -in $sources) "Formal XMS recovery source is missing: $source"
}

Write-Host 'T237 S3 XMS v1 retirement gate passed.'
