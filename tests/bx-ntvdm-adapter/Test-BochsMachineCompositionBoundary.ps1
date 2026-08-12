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
if ($text -notmatch '#ifndef BX_NTVDM_ENABLE_MACHINE_COMPOSITION\s*\r?\n#define BX_NTVDM_ENABLE_MACHINE_COMPOSITION 0') {
    throw 'Machine-composition seam is not default-off.'
}
$start = $text.IndexOf('#if BX_NTVDM_ENABLE_MACHINE_COMPOSITION')
if ($start -lt 0) { throw 'Missing machine-composition seam block.' }
$end = $text.IndexOf('#endif', $start)
if ($end -lt $start) { throw 'Unable to isolate machine-composition seam block.' }
$block = $text.Substring($start, ($end + '#endif'.Length) - $start)
foreach ($required in @(
    'bx_ntvdm_machine_composition_probe',
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
        'WOW', 'DEM', 'adapter', 'longjmp', '0xc4')) {
    if ($block -match ('(?i)' + [regex]::Escape($forbidden))) {
        throw "Forbidden semantic/control token in Bochs seam: $forbidden"
    }
}
if ($text -notmatch 'machine composition outcome=' -or
    $text -notmatch 'BX_NTVDM_MACHINE_BOP_V1_HANDLED_RESUME') {
    throw 'Machine-composition outcome/resume contract is missing.'
}
Write-Host 'Bochs machine-composition boundary verification passed.'
