$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$source = Get-Content (Join-Path $repository 'src\bochs\cpu\exception.cc') -Raw
$start = $source.IndexOf('void BX_CPU_C::real_mode_int')
$end = $source.IndexOf('void BX_CPU_C::interrupt(', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Unable to isolate real_mode_int.' }
$block = $source.Substring($start, $end - $start)
foreach ($required in @('BX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC',
        'ntdos64 real-mode vector', 'system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector)',
        'system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2)',
        'old-cs=', 'old-ip=', 'old-ss=', 'old-sp=', 'target-cs=', 'target-ip=')) {
    if (-not $block.Contains($required)) { throw "Missing real-mode vector diagnostic element: $required" }
}
if ($block.IndexOf('ntdos64 real-mode vector') -lt $block.IndexOf('system_read_word(BX_CPU_THIS_PTR idtr.base + 4 * vector + 2)')) {
    throw 'Vector diagnostic must use the already-read IVT target.'
}
foreach ($forbidden in @('bx_ntvdm_', 'adapter', 'BOP', 'OpenNT', 'DOS', 'SVC_',
        'copy_from_ordinary_ram', 'copy_to_ordinary_ram', 'set_reg', 'set_CF',
        'EIP =', 'load_seg_reg', 'interrupt(')) {
    $diagnosticStart = $block.LastIndexOf('#if BX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC')
    $diagnosticEnd = $block.IndexOf('#endif', $diagnosticStart)
    $diagnostic = $block.Substring($diagnosticStart, $diagnosticEnd - $diagnosticStart + 6)
    if ($diagnostic.Contains($forbidden)) { throw "Forbidden term in vector diagnostic: $forbidden" }
}
$generator = Get-Content (Join-Path $repository 'tools\New-T95S7RuntimeTraceBuildRoot.ps1') -Raw
if (-not $generator.Contains('/DBX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC=1')) {
    throw 'Runtime trace build shim does not enable the registered diagnostic.'
}
Write-Output 'Bochs real-mode vector diagnostic boundary checks passed.'
