$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$source = Get-Content (Join-Path $repository 'src\bochs\cpu\ctrl_xfer32.cc') -Raw
$start = $source.IndexOf('BX_CPU_C::JMP_Ap')
$end = $source.IndexOf('BX_NEXT_TRACE(i);', $start)
$end = $source.IndexOf('}', $end) + 1
if ($start -lt 0 -or $end -le $start) { throw 'Unable to isolate JMP_Ap.' }
$block = $source.Substring($start, $end - $start)
foreach ($required in @('BX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC',
        'ntdos64 real-mode far-jump', 'real_mode()', 'old-cs=', 'old-ip=',
        'target-cs=', 'target-ip=', 'cs_raw', 'disp32')) {
    if (-not $block.Contains($required)) { throw "Missing real-mode far-jump diagnostic element: $required" }
}
$diagnosticStart = $block.LastIndexOf('#if BX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC')
$diagnosticEnd = $block.IndexOf('#endif', $diagnosticStart)
if ($diagnosticStart -lt 0 -or $diagnosticEnd -le $diagnosticStart) {
    throw 'Unable to isolate far-jump diagnostic.'
}
$diagnostic = $block.Substring($diagnosticStart, $diagnosticEnd - $diagnosticStart + 6)
if ($diagnosticStart -lt $block.IndexOf('cs_raw = i->Iw2();')) {
    throw 'Far-jump diagnostic must use already-decoded operands.'
}
if ($diagnosticStart -gt $block.IndexOf('load_seg_reg')) {
    throw 'Far-jump diagnostic must precede original CS assignment.'
}
foreach ($forbidden in @('bx_ntvdm_', 'adapter', 'BOP', 'OpenNT', 'DOS', 'SVC_',
        'read_virtual', 'write_virtual', 'system_read', 'set_reg', 'set_CF',
        'EIP =', 'load_seg_reg', 'exception(')) {
    if ($diagnostic.Contains($forbidden)) { throw "Forbidden term in far-jump diagnostic: $forbidden" }
}
$generator = Get-Content (Join-Path $repository 'tools\New-T95S7RuntimeTraceBuildRoot.ps1') -Raw
if (-not $generator.Contains('BX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC=1')) {
    throw 'Runtime trace build shim does not enable the registered far-jump diagnostic.'
}
Write-Output 'Bochs real-mode far-jump diagnostic boundary checks passed.'
