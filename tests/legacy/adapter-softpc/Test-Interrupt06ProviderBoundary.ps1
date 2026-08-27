$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$headerPath = Join-Path $repositoryRoot 'src/adapter-softpc/interrupt06_provider.h'
$sourcePath = Join-Path $repositoryRoot 'src/adapter-softpc/interrupt06_provider.c'
foreach ($path in @($headerPath, $sourcePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing interrupt-06 adapter input: $path" }
}
$header = Get-Content -LiteralPath $headerPath -Raw
$source = Get-Content -LiteralPath $sourcePath -Raw
$code = [regex]::Replace($source, '(?s)/\*.*?\*/|//.*?(\r?\n|$)', '')
foreach ($required in @(
    'INT06_BDA_INTERRUPT_CAUSE 0x046bu',
    'INT06_PROVIDER_OPERATION_RAM_READ8',
    'INT06_PROVIDER_OPERATION_PORT_WRITE8',
    'int06_provider_begin',
    'int06_provider_begin_unexpected',
    's->phase = P_MASTER_OCW3',
    'P_IP_LO', 'P_WRITE_CS_LO', 'P_MASTER_OCW3', 'P_SLAVE_EOI', 'P_BDA',
    's->saved_ip == 0xffffu',
    's->master_isr == 0u ? 0xffu : s->master_isr',
    'INT06_PROVIDER_STATUS_FAILED'
)) {
    if ($header.IndexOf($required, [StringComparison]::OrdinalIgnoreCase) -lt 0 -and $source.IndexOf($required, [StringComparison]::OrdinalIgnoreCase) -lt 0) {
        throw "Missing S23 source-derived INT06 invariant: $required"
    }
}
foreach ($forbidden in @('bx_', 'bochs', 'callback', 'CCPU', 'SAS', 'BOP', 'selector', 'Windows.h', 'Win32', 'LoadLibrary', '__declspec(thread)', 'Tls')) {
    if ($header.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0 -or $code.IndexOf($forbidden, [StringComparison]::OrdinalIgnoreCase) -ge 0) {
        throw "Forbidden interrupt-06 adapter dependency: $forbidden"
    }
}
Write-Output 'interrupt06-provider-boundary: source invariants verified'
