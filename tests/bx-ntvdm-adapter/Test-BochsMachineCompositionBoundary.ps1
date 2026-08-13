[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$exception = Join-Path $RepositoryRoot 'src\bochs\cpu\exception.cc'
if (-not (Test-Path -LiteralPath $exception -PathType Leaf)) {
    throw "Missing seam file: $exception"
}
$text = Get-Content -LiteralPath $exception -Raw
$adapter = Get-Content (Join-Path $RepositoryRoot 'src\bx-ntvdm-adapter\bx_ntvdm_machine_composition_v2.c') -Raw
if ($text -notmatch '#ifndef BX_NTVDM_ENABLE_MACHINE_COMPOSITION\s*\r?\n#define BX_NTVDM_ENABLE_MACHINE_COMPOSITION 0') {
    throw 'Machine-composition seam is not default-off.'
}
$start = $text.IndexOf('#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION')
if ($start -lt 0) { throw 'Missing machine-composition seam block.' }
$end = $text.IndexOf('#endif', $start)
if ($end -lt $start) { throw 'Unable to isolate machine-composition seam block.' }
$block = $text.Substring($start, ($end + '#endif'.Length) - $start)
foreach ($required in @(
    'bx_ntvdm_machine_composition_bridge',
    'bx_ntvdm_machine_composition_v2_dispatch',
    'bx_ntvdm_machine_composition_read8',
    'bx_ntvdm_machine_composition_write8',
    'bx_ntvdm_machine_composition_store8',
    'bx_ntvdm_machine_composition_load8',
    'bx_ntvdm_machine_composition_report',
    'bx_devices.inp',
    'bx_devices.outp',
    'copy_to_ordinary_ram',
    'copy_from_ordinary_ram')) {
    if (-not $block.Contains($required)) { throw "Missing required mechanical token: $required" }
}
foreach ($forbidden in @('selector', 'OpenNT', 'SoftPC', 'CCPU', 'DOS',
        'WOW', 'DEM', 'adapter', 'longjmp', '0xc4', '0x02', '0x06',
        'unexpected_int', 'illegal_op_int', 'prev_rip + 3')) {
    if ($block -match ('(?i)' + [regex]::Escape($forbidden))) {
        throw "Forbidden semantic/control token in Bochs seam: $forbidden"
    }
}
if ($text -notmatch 'BX_NTVDM_CPU_RESULT_V2_RESUME' -or
    $text -notmatch 'RIP = result.resume_rip') {
    throw 'Generic machine-composition result application is missing.'
}
foreach ($required in @('window->bytes[0] != 0xc4u', 'window->bytes[2] == 0x02u',
        'window->bytes[2] == 0x06u', 'unexpected_int', 'illegal_op_int',
        'bx_ntvdm_cpu_result_v2_resume')) {
    if (-not $adapter.Contains($required)) { throw "Adapter machine plane lacks selection token: $required" }
}
Write-Host 'Bochs machine-composition boundary verification passed.'
