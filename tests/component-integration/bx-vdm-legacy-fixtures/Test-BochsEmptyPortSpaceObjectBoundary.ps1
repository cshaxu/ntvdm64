[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path))
}

$devicesPath = Join-Path $RepositoryRoot 'refs\bochs\iodev\devices.cc'
$isolatedPath = Join-Path $RepositoryRoot 'src\bx-mantle\minimal_port_space.cc'
foreach ($path in @($devicesPath, $isolatedPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing BX-IO-029 source: $path"
    }
}

$devices = Get-Content -LiteralPath $devicesPath -Raw
$isolated = Get-Content -LiteralPath $isolatedPath -Raw
$definitions = @(
    'bx_bool\s+bx_devices_c::init_empty_port_space\s*\(\s*void\s*\)',
    'Bit32u\s+bx_devices_c::default_read_handler\s*\(',
    'void\s+bx_devices_c::default_write_handler\s*\(',
    'bx_bool\s+bx_devices_c::register_default_io_read_handler\s*\(',
    'bx_bool\s+bx_devices_c::register_default_io_write_handler\s*\(',
    'bx_bool\s+bx_devices_c::cleanup_empty_port_space\s*\(\s*void\s*\)'
)
foreach ($definition in $definitions) {
    $isolatedCount = [regex]::Matches($isolated, $definition).Count
    $devicesCount = [regex]::Matches($devices, $definition).Count
    if ($isolatedCount -ne 1) {
        throw "BX-IO-029 requires exactly one isolated definition for pattern '$definition'; found $isolatedCount."
    }
    if ($devicesCount -ne 0) {
        throw "BX-IO-029 requires no relocated definition in devices.cc for pattern '$definition'; found $devicesCount."
    }
}

if ($isolated -match '(?i)bx_ntvdm|adapter|opennt') {
    throw 'BX-IO-029 isolated source must not contain adapter or OpenNT references.'
}

Write-Host 'Bochs empty-port source-object boundary verification passed.'
