$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$generator = Get-Content (Join-Path $repository 'tools\New-T95S7RuntimeTraceBuildRoot.ps1') -Raw
foreach ($required in @(
        "'cpu\data_xfer16.cc'", "'cpu\data_xfer16_trace.obj'",
        'BX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC=1',
        'cpu\data_xfer16_trace.obj: cpu\data_xfer16.cc',
        'cpu\exception.o cpu\data_xfer16_trace.obj cpu\ctrl_xfer16_trace.obj cpu\ctrl_xfer32_trace.obj',
        'cpu\ctrl_xfer32_trace.obj iodev/libiodev.a', 'cpu/libcpu.a cpu/cpudb/libcpudb.a')) {
    if (-not $generator.Contains($required)) { throw "Missing stack-transfer closure element: $required" }
}
foreach ($forbidden in @('config.h /D', 'cpu_loop', 'BX_DEBUGGER=1',
        'BX_INSTRUMENTATION=1', 'adapter_runtime_stack')) {
    if ($generator.Contains($forbidden)) { throw "Forbidden stack-transfer closure expansion: $forbidden" }
}
Write-Output 'T95 S7 stack-transfer diagnostic build-closure checks passed.'
