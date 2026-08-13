[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path $MyInvocation.MyCommand.Path))
}
$header = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src\bx-vdm\bx_ntvdm_adapter_runtime.h') -Raw
$source = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src\bx-vdm\bx_ntvdm_adapter_runtime.c') -Raw
$start = $header.IndexOf('typedef struct bx_ntvdm_adapter_runtime_diagnostic_state_v1')
$end = $header.IndexOf('} bx_ntvdm_adapter_runtime_diagnostic_state_v1;', $start)
if ($start -lt 0 -or $end -lt $start) { throw 'Missing copied diagnostic-state ABI.' }
$record = $header.Substring($start, $end - $start)
foreach ($field in @('uint32_t version;', 'uint32_t installed;', 'uint32_t has_boot_namespace_provider;', 'uint32_t pending_kind;', 'uint32_t boot_file_diagnostic;')) {
    if (-not $record.Contains($field)) { throw "Missing fixed-width lifecycle field: $field" }
}
foreach ($forbidden in @('selector', 'service', 'address', 'payload', '*', 'pointer', 'transaction')) {
    if ($record.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "Forbidden diagnostic ABI term: $forbidden" }
}
$functionStart = $source.IndexOf('int bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state')
$functionEnd = $source.IndexOf("`n}", $functionStart)
if ($functionStart -lt 0 -or $functionEnd -lt $functionStart) { throw 'Missing diagnostic copy implementation.' }
$function = $source.Substring($functionStart, $functionEnd - $functionStart)
foreach ($required in @('out->version', 'out->installed', 'out->has_boot_namespace_provider', 'out->pending_kind', 'out->boot_file_diagnostic', 'return 1;')) {
    if (-not $function.Contains($required)) { throw "Missing copy implementation term: $required" }
}
foreach ($forbidden in @('dispatch(', 'take_pending', 'queue_', 'clear_pending', '_reset(', 'memcpy', 'selector', 'service')) {
    if ($function.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) { throw "State observer is not side-effect-free: $forbidden" }
}
Write-Output 'Adapter runtime diagnostic-state boundary verification passed.'
