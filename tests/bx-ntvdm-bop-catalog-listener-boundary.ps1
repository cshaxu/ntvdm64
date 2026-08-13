$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent $PSScriptRoot
$bochs = Get-Content (Join-Path $repository 'src\bochs\cpu\exception.cc') -Raw
$adapter = Get-Content (Join-Path $repository 'src\bx-vdm\bx_ntvdm_exception_observer_v1.c') -Raw
foreach ($forbidden in @('BOP_CATALOG_LISTENER', 'bx_ntvdm_bop_catalog',
        'bop observed', 'selector=', 'stack-valid=', 'frame-valid=')) {
    if ($bochs.Contains($forbidden)) { throw "Bochs retains BOP observation: $forbidden" }
}
foreach ($required in @('bx_ntvdm_bop_catalog_v1_observe',
        'ntdos64 adapter bop observed', 'selector=%02x', 'service-state=%u',
        'cpu_before->eax', 'cpu_before->esp')) {
    if (-not $adapter.Contains($required)) { throw "Missing adapter observation element: $required" }
}
foreach ($forbidden in @('copy_from_ordinary_ram', 'copy_to_ordinary_ram',
        'set_reg', 'set_CF', 'resume_rip', 'BX_MEM')) {
    if ($adapter.Contains($forbidden)) { throw "Observer exceeds copied-fact boundary: $forbidden" }
}
Write-Output 'Adapter-owned BOP observation boundary checks passed.'
