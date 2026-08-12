$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$source = Get-Content (Join-Path $repository 'src\bochs\cpu\ctrl_xfer16.cc') -Raw
$start = $source.IndexOf('BX_CPU_C::IRET16')
$end = $source.IndexOf('BX_NEXT_TRACE(i);', $start)
$end = $source.IndexOf('}', $end) + 1
if ($start -lt 0 -or $end -le $start) { throw 'Unable to isolate IRET16.' }
$block = $source.Substring($start, $end - $start)
foreach ($required in @('BX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC',
        'ntdos64 iret tf', 'Bit16u ip     = pop_16();',
        'Bit16u cs_raw = pop_16();', 'Bit16u flags  = pop_16();',
        'flags & 0x0100', 'old-cs=', 'old-ip=', 'old-ss=', 'old-sp=',
        'target-cs=', 'target-ip=', 'flags=')) {
    if (-not $block.Contains($required)) { throw "Missing IRET TF diagnostic element: $required" }
}
$diagnosticStart = $block.LastIndexOf('#if BX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC')
$diagnosticEnd = $block.IndexOf('#endif', $diagnosticStart)
if ($diagnosticStart -lt 0 -or $diagnosticEnd -le $diagnosticStart) {
    throw 'Unable to isolate post-pop IRET TF diagnostic.'
}
$diagnostic = $block.Substring($diagnosticStart, $diagnosticEnd - $diagnosticStart + 6)
if ($diagnosticStart -lt $block.IndexOf('Bit16u flags  = pop_16();')) {
    throw 'IRET TF diagnostic must occur after the original IRET frame pops.'
}
foreach ($forbidden in @('bx_ntvdm_', 'adapter', 'BOP', 'OpenNT', 'DOS', 'SVC_',
        'pop_16(', 'push_16(', 'read_virtual', 'write_virtual', 'set_reg', 'set_CF',
        'EIP =', 'load_seg_reg', 'write_flags', 'exception(')) {
    if ($diagnostic.Contains($forbidden)) { throw "Forbidden term in IRET TF diagnostic: $forbidden" }
}
$generator = Get-Content (Join-Path $repository 'tools\New-T95S7RuntimeTraceBuildRoot.ps1') -Raw
foreach ($required in @("'cpu\ctrl_xfer16.cc'", "'cpu\ctrl_xfer16_trace.obj'",
        'BX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC=1',
        'cpu\ctrl_xfer16_trace.obj: cpu\ctrl_xfer16.cc',
        'cpu\exception.o cpu\data_xfer16_trace.obj cpu\ctrl_xfer16_trace.obj',
        'cpu\ctrl_xfer32_trace.obj iodev/libiodev.a')) {
    if (-not $generator.Contains($required)) { throw "Missing IRET TF build-closure element: $required" }
}
foreach ($forbidden in @('config.h /D', 'cpu_loop', 'BX_DEBUGGER=1',
        'BX_INSTRUMENTATION=1', 'adapter_iret')) {
    if ($generator.Contains($forbidden)) { throw "Forbidden IRET TF closure expansion: $forbidden" }
}
Write-Output 'Bochs IRET TF diagnostic boundary checks passed.'
