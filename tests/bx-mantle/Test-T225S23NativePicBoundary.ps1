$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$machineHeader = Join-Path $repositoryRoot 'src/bx-mantle/minimal_machine.h'
$machineSource = Join-Path $repositoryRoot 'src/bx-mantle/minimal_machine.cc'
$picHeader = Join-Path $repositoryRoot 'src/bx-mantle/pic.h'
$picSource = Join-Path $repositoryRoot 'src/bx-mantle/pic.cc'
$portSource = Join-Path $repositoryRoot 'src/bx-mantle/minimal_port_space.cc'
foreach ($path in @($machineHeader, $machineSource, $picHeader, $picSource, $portSource)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing native PIC boundary input: $path" }
}
$machineHeaderText = Get-Content -LiteralPath $machineHeader -Raw
$machine = Get-Content -LiteralPath $machineSource -Raw
$pic = Get-Content -LiteralPath $picSource -Raw
$port = Get-Content -LiteralPath $portSource -Raw
foreach ($pattern in @('BX_MANTLE_MINIMAL_MACHINE_PIC_FAILED', 'bx_bool pic_owned;', 'bx_pic_c \*pic;')) {
    if ($machineHeaderText -notmatch $pattern) { throw "Missing PIC lifecycle status or ownership: $pattern" }
}
foreach ($pattern in @('BX-MANTLE-082-BEGIN', 'bx_mantle_pic_create\(\)', 'pic == NULL',
        'bx_devices\.pluginPicDevice = pic;', 'pic->init\(\)', 'pic->fini\(\)',
        'bx_devices\.pluginPicDevice = &bx_devices\.stubPic;', 'bx_mantle_pic_destroy\(pic\)')) {
    if ($machine -notmatch $pattern) { throw "Missing PIC lifecycle operation: $pattern" }
}
function Assert-Ordered([string] $first, [string] $second) {
    $a = $machine.IndexOf($first, [StringComparison]::Ordinal)
    $b = $machine.LastIndexOf($second, [StringComparison]::Ordinal)
    if ($a -lt 0 -or $b -lt 0 -or $a -ge $b) { throw "PIC lifecycle ordering violated: $first before $second" }
}
Assert-Ordered 'bx_devices.init_empty_port_space()' 'bx_mantle_pic_create()'
Assert-Ordered 'bx_mantle_pic_create()' 'pic->init()'
Assert-Ordered 'pic->fini()' 'bx_devices.pluginPicDevice = &bx_devices.stubPic;'
Assert-Ordered 'bx_devices.pluginPicDevice = &bx_devices.stubPic;' 'bx_mantle_pic_destroy(pic)'
Assert-Ordered 'bx_mantle_pic_destroy(pic)' 'bx_devices.cleanup_empty_port_space()'
foreach ($pattern in @('libpic_LTX_plugin_', 'pluginRegisterDeviceDevmodel', 'register_state\(\);', 'PLUG_',
        '(?-i:BOP)', '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:WOW)', 'adapter', 'getenv')) {
    if ($pic -match $pattern -or $machine -match $pattern) { throw "Forbidden PIC composition dependency: $pattern" }
}
foreach ($pattern in @('register_io_read_handler', 'register_io_write_handler',
        'unregister_io_read_handler', 'unregister_io_write_handler')) {
    if ($port -notmatch $pattern) { throw "Missing original port lifecycle operation: $pattern" }
}
Write-Output 't225-s23-native-pic-boundary: native PIC ownership and selector-blind mechanics verified'