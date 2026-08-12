$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent $PSScriptRoot
$source = Get-Content (Join-Path $repository 'src\bochs\cpu\exception.cc') -Raw
foreach ($required in @(
        '#define BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER 0',
        '#include "bx_ntvdm_bop_catalog_v1.h"',
        'static void bx_ntvdm_bop_catalog_listener',
        'bx_ntvdm_bop_catalog_v1_observe',
        'ntdos64 adapter bop observed', 'rip=%llx',
        'stack-valid=%u', 'frame-valid=%u')) {
    if (-not $source.Contains($required)) { throw "Missing BOP listener boundary element: $required" }
}
$listenerStart = $source.IndexOf('static void bx_ntvdm_bop_catalog_listener')
$listenerEnd = $source.IndexOf('#endif', $listenerStart)
if ($listenerStart -lt 0 -or $listenerEnd -lt $listenerStart) { throw 'Unable to isolate BOP listener block.' }
$listener = $source.Substring($listenerStart, $listenerEnd - $listenerStart)
foreach ($forbidden in @('0x50', '0x51', '0x52', '0x53', '0x54', '0x56', '0x57',
        'set_reg', 'set_CF', 'RIP =', 'copy_to_ordinary_ram',
        'bx_ntvdm_adapter_runtime', 'OpenNT', 'DOS', 'DEM')) {
    if ($listener.Contains($forbidden)) { throw "Forbidden BOP listener term: $forbidden" }
}
foreach ($required in @('BX_NTVDM_CPU_EXECUTION_REAL', 'cpu_state->ss',
        'cpu_state->esp', 'ordinary_ram_readable', 'copy_from_ordinary_ram',
        'stack_address <= 0xffffcu', 'frame_address <= 0xffff8u',
        'stack_frame', 'frame_code')) {
    if (-not $listener.Contains($required)) { throw "Missing generic stack diagnostic term: $required" }
}
$capture = $source.IndexOf('bx_ntvdm_bop_catalog_listener(BX_CPU_ID')
$startup = $source.IndexOf('#if BX_NTVDM_ENABLE_STARTUP_TRANSACTION', $capture)
if ($capture -lt 0 -or $startup -lt 0 -or $capture -ge $startup) {
    throw 'BOP listener is not ordered before the startup transaction consumer.'
}
Write-Output 'BOP catalog listener boundary checks passed.'
