$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$sourcePath = Join-Path $repositoryRoot 'src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c'
$headerPath = Join-Path $repositoryRoot 'src/bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.h'
$source = Get-Content -LiteralPath $sourcePath -Raw
$header = Get-Content -LiteralPath $headerPath -Raw

foreach ($pattern in @('bx_ntvdm_bop_ingress_v1_dispatch',
        'bx_ntvdm_bop_provider_registry_v1_select',
        'bx_ntvdm_mantle_generic_ud_bridge_v1')) {
    if ($source -notmatch $pattern) { throw "Missing generic bridge invariant: $pattern" }
}
foreach ($pattern in @('adapter_runtime', 'host_session', 'GetEnvironment',
        'guest_read', 'guest_write', 'multi_write', 'bulk_result',
        'OpenNT', 'SIM->', 'bx_gui', 'callback')) {
    if ($source -match $pattern -or $header -match $pattern) {
        throw "Forbidden generic ingress bridge dependency: $pattern"
    }
}
if ($source -notmatch 'return bx_ntvdm_vdm_generic_ud_classify_v1\(event, &ingress, &selection\) \?\s*0 : 0;') {
    throw 'The S6 bridge must classify and decline until a provider slice is admitted.'
}
Write-Output 'bx-vdm-generic-ud-bridge-boundary: global classification-only bridge verified'
