$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$machineHeader = Join-Path $repositoryRoot 'src/adapter-bochs/minimal_machine.h'
$machineSource = Join-Path $repositoryRoot 'src/adapter-bochs/minimal_machine.cc'
$picHeader = Join-Path $repositoryRoot 'src/bochs-core/iodev/pic.h'
$picSource = Join-Path $repositoryRoot 'src/bochs-core/iodev/pic.cc'
$picAssembly = Join-Path $repositoryRoot 'src/adapter-bochs/minimal_pic.cc'
$picOverlay = Join-Path $repositoryRoot 'src/bochs-core-overlay/iodev/pic_lifecycle.cc'
$portSource = Join-Path $repositoryRoot 'src/bochs-core-overlay/iodev/minimal_port_space.cc'
foreach ($path in @($machineHeader, $machineSource, $picHeader, $picSource, $picAssembly, $picOverlay, $portSource)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing native PIC boundary input: $path" }
}
$machineHeaderText = Get-Content -LiteralPath $machineHeader -Raw
$machine = Get-Content -LiteralPath $machineSource -Raw
$pic = Get-Content -LiteralPath $picSource -Raw
$assembly = Get-Content -LiteralPath $picAssembly -Raw
$overlay = Get-Content -LiteralPath $picOverlay -Raw
$port = Get-Content -LiteralPath $portSource -Raw
foreach ($pattern in @('BX_MACHINE_MINIMAL_MACHINE_PIC_FAILED', 'bx_bool pic_owned;', 'bx_pic_c \*pic;')) {
    if ($machineHeaderText -notmatch $pattern) { throw "Missing PIC lifecycle status or ownership: $pattern" }
}
foreach ($pattern in @('BX-MACHINE-082-BEGIN', 'minimal_pic_create\(\)', 'pic == NULL',
        'bx_devices\.pluginPicDevice = pic;', 'pic->init\(\)', 'pic->fini\(\)',
        'bx_devices\.pluginPicDevice = &bx_devices\.stubPic;', 'minimal_pic_destroy\(pic\)')) {
    if ($machine -notmatch $pattern) { throw "Missing PIC lifecycle operation: $pattern" }
}
function Assert-Ordered([string] $first, [string] $second) {
    $a = $machine.IndexOf($first, [StringComparison]::Ordinal)
    $b = $machine.LastIndexOf($second, [StringComparison]::Ordinal)
    if ($a -lt 0 -or $b -lt 0 -or $a -ge $b) { throw "PIC lifecycle ordering violated: $first before $second" }
}
Assert-Ordered 'bx_devices.init_empty_port_space()' 'minimal_pic_create()'
Assert-Ordered 'minimal_pic_create()' 'pic->init()'
Assert-Ordered 'pic->fini()' 'bx_devices.pluginPicDevice = &bx_devices.stubPic;'
Assert-Ordered 'bx_devices.pluginPicDevice = &bx_devices.stubPic;' 'minimal_pic_destroy(pic)'
Assert-Ordered 'minimal_pic_destroy(pic)' 'bx_devices.cleanup_empty_port_space()'
foreach ($pattern in @('libpic_LTX_plugin_', 'pluginRegisterDeviceDevmodel', 'register_state\(\);', 'PLUG_',
        '(?-i:BOP)', '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:WOW)', 'getenv')) {
    if ($machine -match $pattern -or $assembly -match $pattern -or $overlay -match $pattern) { throw "Forbidden PIC composition dependency: $pattern" }
}
if ($overlay -notmatch 'bochs_core_overlay_pic_fini') { throw 'Missing private PIC overlay teardown body.' }
foreach ($pattern in @('register_io_read_handler', 'register_io_write_handler',
        'unregister_io_read_handler', 'unregister_io_write_handler')) {
    if ($port -notmatch $pattern) { throw "Missing original port lifecycle operation: $pattern" }
}
Write-Output 't225-s23-native-pic-boundary: native PIC ownership and selector-blind mechanics verified'
