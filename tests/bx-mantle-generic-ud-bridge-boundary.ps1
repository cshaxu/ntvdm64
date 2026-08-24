$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$headerPath = Join-Path $repositoryRoot 'src/adapter-bop/bx_ntvdm_generic_ud_bridge.h'
$sourcePath = Join-Path $repositoryRoot 'src/adapter-bop/bx_ntvdm_bop_ingress_v1.c'
$corePath = Join-Path $repositoryRoot 'src/bx-core/cpu/exception.cc'
foreach ($path in @($headerPath, $sourcePath, $corePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing generic #UD bridge input: $path" }
}
$header = Get-Content -LiteralPath $headerPath -Raw
$source = Get-Content -LiteralPath $sourcePath -Raw
$core = Get-Content -LiteralPath $corePath -Raw
foreach ($pattern in @('struct bx_ntvdm_generic_ud_event_v1',
        'struct bx_ntvdm_generic_ud_outcome_v1',
        'struct bx_ntvdm_generic_ud_outcome_v2',
        'BX_NTVDM_GENERIC_UD_PASS_THROUGH', 'BX_NTVDM_GENERIC_UD_RESUME',
        'BX_NTVDM_GENERIC_UD_STOP', 'BX_NTVDM_GENERIC_UD_CONTEXT_PROTECTED',
        'bx_ntvdm_mantle_generic_ud_bridge_v1', 'bx_ntvdm_mantle_generic_ud_bridge_v2',
        'bx_ntvdm_mantle_generic_ud_fixture_stop_observed')) {
    if ($header -notmatch $pattern) { throw "Missing generic #UD bridge invariant: $pattern" }
}
foreach ($pattern in @('0x50u', '0x52u', '0x53u', '0x54u', '0x57u', '0x59u',
        '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:WOW)', 'getenv', 'SIM->', 'bx_gui')) {
    if ($header -match $pattern -or $source -match $pattern) {
        throw "Forbidden generic #UD bridge dependency: $pattern"
    }
}
if ($source -notmatch 'return 0;') { throw 'The uncomposed generic #UD bridge must decline.' }
foreach ($pattern in @('bx_ntvdm_exception_intercept', 'bx_ntvdm_adapter_runtime',
        'bx_ntvdm_adapter_interceptor', 'bx_ntvdm_try_intercept_ud',
        'getenv', 'SIM->quit_sim', '(?-i:BOP)', '(?-i:OpenNT)',
        '(?-i:DOS)', '(?-i:WOW)')) {
    if ($core -match $pattern) {
        throw "Forbidden legacy composition remains in core exception path: $pattern"
    }
}
if ($core -notmatch 'bx_ntvdm_mantle_generic_ud_bridge_v1' -or
    $core -notmatch 'bx_ntvdm_mantle_generic_ud_bridge_v2' -or
    $core -notmatch 'SetCR0\(context_cr0\)' -or
    $core -notmatch 'writeEFlags\(mantle_context_outcome\.eflags_values' -or
    $core -notmatch 'bx_pc_system\.kill_bochs_request = 1;') {
    throw 'The core must retain only the fixed mantle bridge and controlled-stop mechanics.'
}
Write-Output 'adapter-bop-generic-ud-bridge-boundary: selector-blind fixed bridge verified'
