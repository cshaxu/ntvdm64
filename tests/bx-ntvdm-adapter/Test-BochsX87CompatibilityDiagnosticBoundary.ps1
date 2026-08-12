[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$sourcePath = Join-Path $RepositoryRoot 'src\bochs\fpu\fpu.cc'
if (-not (Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
    throw "Missing FPU source: $sourcePath"
}

$source = Get-Content -LiteralPath $sourcePath -Raw
$default = '#ifndef BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC' + "`n" +
           '#define BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC 0' + "`n" +
           '#endif'
if (-not $source.Contains($default)) {
    throw 'Missing BX-TRACE-035 local default-off macro invariant.'
}

$functionStart = $source.IndexOf('void BX_CPU_C::FPU_check_pending_exceptions(void)', [System.StringComparison]::Ordinal)
$functionEnd = $source.IndexOf('bx_address BX_CPU_C::fpu_save_environment', $functionStart, [System.StringComparison]::Ordinal)
if ($functionStart -lt 0 -or $functionEnd -lt $functionStart) {
    throw 'Missing unique FPU compatibility diagnostic owner.'
}
$function = $source.Substring($functionStart, $functionEnd - $functionStart)
$guardStart = $function.IndexOf('#if BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC', [System.StringComparison]::Ordinal)
$guardEnd = $function.IndexOf('#endif', $guardStart, [System.StringComparison]::Ordinal)
$record = $function.IndexOf('ntdos64 x87-compat-diagnostic', [System.StringComparison]::Ordinal)
$legacy = $function.IndexOf('math_abort: MSDOS compatibility FPU exception', [System.StringComparison]::Ordinal)
$irq = $function.IndexOf('DEV_pic_raise_irq(13);', [System.StringComparison]::Ordinal)
if ($guardStart -lt 0 -or $guardEnd -lt $guardStart -or $record -lt $guardStart -or $record -gt $guardEnd -or
    $legacy -lt $guardEnd -or $irq -lt $legacy) {
    throw 'BX-TRACE-035 does not preserve guard, original diagnostic, and IRQ13 ordering.'
}

$guard = $function.Substring($guardStart, $guardEnd - $guardStart)
foreach ($token in @('BX_CPU_ID', 'sregs[BX_SEG_REG_CS].selector.value', 'RIP', 'cr0.get32()',
        'get_control_word()', 'get_partial_status()', 'get_tag_word()', '.tos', '.foo', '.fcs', '.fip', '.fds', '.fdp')) {
    if (-not $guard.Contains($token)) {
        throw "Missing fixed native diagnostic field: $token"
    }
}
if ($guard.Contains('BX_CPU_THIS_PTR RIP')) {
    throw 'RIP is already a CPU-access macro and must not receive a duplicated owner prefix.'
}
foreach ($forbidden in @('bx_ntvdm_', 'adapter', 'OpenNT', 'DOS', 'WOW', 'DEM', 'BOP', 'BIOS',
        'firmware', 'profile', 'guest_', 'copy_', 'resume', 'stop', 'malloc', 'new ')) {
    $pattern = '(?i)(?<![A-Za-z0-9_])' + [regex]::Escape($forbidden) + '(?![A-Za-z0-9_])'
    if ($guard -match $pattern) {
        throw "Forbidden boundary-expanding token in diagnostic guard: $forbidden"
    }
}

Write-Host 'Bochs x87 compatibility diagnostic boundary verification passed.'
