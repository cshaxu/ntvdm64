$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$headerPath = Join-Path $repositoryRoot 'src/opennt/local/compat/host/opennt_int06_provider_v1.h'
$sourcePath = Join-Path $repositoryRoot 'src/opennt/local/compat/host/opennt_int06_provider_v1.c'
foreach ($path in @($headerPath, $sourcePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing S23 opennt-host input: $path" }
}
$header = Get-Content -LiteralPath $headerPath -Raw
$source = Get-Content -LiteralPath $sourcePath -Raw
$code = [regex]::Replace($source, '(?s)/\*.*?\*/|//.*?(\r?\n|$)', '')
foreach ($required in @(
    'OPENNT_INT06_BDA_INTERRUPT_CAUSE 0x046bu',
    'OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_READ8',
    'OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_WRITE8',
    'opennt_int06_provider_v1_begin',
    'opennt_int06_provider_v1_begin_unexpected',
    's->phase = P_MASTER_OCW3',
    'P_IP_LO', 'P_WRITE_CS_LO', 'P_MASTER_OCW3', 'P_SLAVE_EOI', 'P_BDA',
    's->saved_ip == 0xffffu',
    's->master_isr == 0u ? 0xffu : s->master_isr',
    'OPENNT_INT06_PROVIDER_V1_STATUS_FAILED'
)) {
    if ($header.IndexOf($required, [StringComparison]::OrdinalIgnoreCase) -lt 0 -and $source.IndexOf($required, [StringComparison]::OrdinalIgnoreCase) -lt 0) {
        throw "Missing S23 source-derived INT06 invariant: $required"
    }
}
foreach ($forbidden in @('bx_', 'bochs', 'callback', 'CCPU', 'SAS', 'BOP', 'selector', 'Windows.h', 'Win32', 'LoadLibrary', '__declspec(thread)', 'Tls')) {
    if ($header.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0 -or $code.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Forbidden opennt-host boundary dependency: $forbidden"
    }
}
Write-Output 't225-s23-opennt-int06-provider-boundary: source invariants verified'
