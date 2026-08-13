[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}
$source = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src\bochs\cpu\exception.cc') -Raw
$default = '#ifndef BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC' + "`n" +
    '#define BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC 0' + "`n" + '#endif'
if (-not $source.Contains($default)) { throw 'Missing BX-TRACE-060 default-off macro.' }
$branchStart = $source.IndexOf('if (result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH)')
$branchEnd = $source.IndexOf('if (result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP)', $branchStart)
if ($branchStart -lt 0 -or $branchEnd -lt $branchStart) { throw 'Missing generic pass-through branch.' }
$branch = $source.Substring($branchStart, $branchEnd - $branchStart)
$traceStart = $branch.IndexOf('#if BX_NTVDM_ENABLE_ADAPTER_STATE_DIAGNOSTIC')
$traceEnd = $branch.IndexOf('#endif', $traceStart)
if ($traceStart -lt 0 -or $traceEnd -lt $traceStart) { throw 'Missing state-trace guard.' }
$trace = $branch.Substring($traceStart, $traceEnd - $traceStart)
foreach ($required in @('bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state', 'diagnostic_state.installed', 'diagnostic_state.has_boot_namespace_provider', 'diagnostic_state.pending_kind', 'ntdos64 adapter lifecycle')) {
    if (-not $trace.Contains($required)) { throw "Missing state-trace term: $required" }
}
foreach ($forbidden in @('instruction_window', 'bytes[', 'selector', 'service', 'guest_', 'payload', 'take_pending', 'queue_', 'set_reg', 'set_CF', 'response->', 'RIP =', 'return 1')) {
    if ($trace.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "Forbidden state-trace term: $forbidden" }
}
$traceIndex = $branchStart + $traceStart
$passThroughIndex = $source.IndexOf('ntdos64 adapter boundary passed through', $branchStart)
$returnIndex = $source.IndexOf('return 0;', $passThroughIndex)
if ($passThroughIndex -lt $traceIndex -or $returnIndex -lt $passThroughIndex -or $returnIndex -ge $branchEnd) {
    throw 'State trace does not preserve append-only pass-through order.'
}
Write-Output 'Bochs adapter-state trace boundary verification passed.'
