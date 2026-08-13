$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$header = Get-Content (Join-Path $repository 'src\bx-vdm\bx_ntvdm_startup_transaction_bridge.h') -Raw
$source = Get-Content (Join-Path $repository 'src\bx-vdm\bx_ntvdm_startup_transaction_bridge.c') -Raw
$combined = $header + "`n" + $source
foreach ($required in @(
        'prepare_from_environment',
        'bx_ntvdm_host_service_catalog_v1_lookup',
        'bx_ntvdm_host_service_plane_v1_prepare',
        'bx_ntvdm_exception_result_v1_resume',
        'take_pending_write',
        'event->fault_rip + 4u')) {
    if (-not $combined.Contains($required)) { throw "Missing startup bridge control: $required" }
}
foreach ($forbidden in @(
        'adapter_runtime', 'host_session', 'guest_read', 'gather',
        'observation', 'multi_write', 'dem_debug', 'drive', 'dpb',
        'hard_error', 'cmd_set', 'BX_MEM', 'bochs')) {
    if ($combined.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Forbidden broad-runtime or Bochs term in startup bridge: $forbidden"
    }
}
Write-Output 'bx-ntvdm-startup-transaction-bridge boundary: PASS'
