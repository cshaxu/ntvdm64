$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path $PSScriptRoot)
$exception = Get-Content (Join-Path $repository 'src\bx-core\cpu\exception.cc') -Raw
$main = Get-Content (Join-Path $repository 'src\bochs\main.cc') -Raw
$default = '#ifndef BX_NTVDM_ENABLE_STARTUP_TRANSACTION' + "`n" +
           '#define BX_NTVDM_ENABLE_STARTUP_TRANSACTION 0' + "`n" + '#endif'
if (-not $exception.Contains($default)) { throw 'Missing BX-ABI-036 default-off macro.' }
foreach ($required in @(
        'bx_ntvdm_startup_transaction_bridge_v1_dispatch',
        'bx_ntvdm_startup_transaction_bridge_v1_take_pending_write',
        'copy_to_ordinary_ram', 'resume_rip', 'guest transaction committed',
        'request->cpu_state->cs', 'request->cpu_state->eip')) {
    if (-not ($exception + "`n" + $main).Contains($required)) { throw "Missing narrow startup mechanics token: $required" }
}
$start = $exception.IndexOf('#if BX_NTVDM_ENABLE_STARTUP_TRANSACTION', [StringComparison]::Ordinal)
$end = $exception.IndexOf('#if BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT', $start + 1, [StringComparison]::Ordinal)
if ($start -lt 0 -or $end -lt $start) { throw 'Unable to isolate narrow startup block.' }
$block = $exception.Substring($start, $end - $start)
foreach ($forbidden in @('adapter_runtime', 'guest_read', 'gather', 'observation', 'multi_write', 'BOP', 'OpenNT', 'DOS', 'WOW', 'DEM', 'COMMAND', '0xC4')) {
    if ($block.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "Forbidden startup mechanics term: $forbidden" }
}
if ($main.Contains('bx_ntvdm_startup_transaction_bridge')) {
    throw 'Narrow startup transaction bridge must remain in the #UD mechanics seam, not main.cc.'
}
Write-Output 'Bochs narrow startup-transaction boundary verification passed.'
