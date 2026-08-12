$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$source = Get-Content (Join-Path $repository 'src\bochs\cpu\data_xfer16.cc') -Raw
foreach ($required in @(
        'BX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC',
        'ntdos64 stack transfer mov-sp', 'ntdos64 stack transfer mov-ss',
        'real_mode()', 'BX_16BIT_REG_SP', 'BX_SEG_REG_SS', 'PREV_RIP')) {
    if (-not $source.Contains($required)) { throw "Missing stack-transfer diagnostic element: $required" }
}
$movSpStart = $source.IndexOf('BX_CPU_C::MOV_GwEwM')
$movSsStart = $source.IndexOf('BX_CPU_C::MOV_SwEw')
if ($movSpStart -lt 0 -or $movSsStart -le $movSpStart) {
    throw 'Unable to isolate registered MOV transfer handlers.'
}
$movSp = $source.Substring($movSpStart, $movSsStart - $movSpStart)
$movSs = $source.Substring($movSsStart)
foreach ($block in @($movSp, $movSs)) {
    foreach ($forbidden in @('bx_ntvdm_', 'adapter', 'BOP', 'OpenNT', 'DOS',
            'SVC_', 'copy_from_ordinary_ram', 'copy_to_ordinary_ram',
            'set_reg', 'set_CF', 'RIP =', 'EIP =')) {
        if ($block.Contains($forbidden)) {
            throw "Forbidden semantic or state-changing term in stack diagnostic: $forbidden"
        }
    }
}
if ($movSp.IndexOf('BX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC') -gt
        $movSp.IndexOf('BX_WRITE_16BIT_REG(i->dst(), val16)')) {
    throw 'MOV SP diagnostic must be guarded before the write is emitted.'
}
Write-Output 'Bochs stack-transfer diagnostic boundary checks passed.'
