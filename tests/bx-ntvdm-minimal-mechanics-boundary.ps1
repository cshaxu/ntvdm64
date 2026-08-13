$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$memoryHeader = Join-Path $repositoryRoot 'src/bx-core/memory/memory.h'
$memorySource = Join-Path $repositoryRoot 'src/bx-core/memory/misc_mem.cc'
$iodevHeader = Join-Path $repositoryRoot 'src/bochs/iodev/iodev.h'
$portSpaceSource = Join-Path $repositoryRoot 'src/bx-mantle/minimal_port_space.cc'
$devicesSource = Join-Path $repositoryRoot 'src/bochs/iodev/devices.cc'

function Get-RegisteredBlock([string] $path, [string] $name) {
    $text = Get-Content -LiteralPath $path -Raw
    $match = [regex]::Match($text, "(?s)// $name-BEGIN\r?\n(.*?)// $name-END")
    if (-not $match.Success) {
        throw "Missing $name source block in $path"
    }
    return $match.Groups[1].Value
}

$memoryDeclaration = Get-Content -LiteralPath $memoryHeader -Raw
$iodevDeclaration = Get-Content -LiteralPath $iodevHeader -Raw
if ($memoryDeclaration -notmatch 'bx_bool init_memory_without_sim\(Bit64u guest, Bit64u host\);') {
    throw 'Missing private BX-MEM-024 declaration'
}
foreach ($pattern in @('bx_bool init_empty_port_space\(void\);', 'bx_bool cleanup_empty_port_space\(void\);')) {
    if ($iodevDeclaration -notmatch $pattern) {
        throw "Missing private BX-IO-025 declaration: $pattern"
    }
}

$memory = Get-RegisteredBlock $memorySource 'BX-MEM-024'
foreach ($pattern in @('actual_vector != NULL', 'alloc_vector_aligned', 'rom =', 'bogus =',
        'blocks = new Bit8u\*', 'memory_handlers = new struct memory_handler_struct',
        'pci_enabled = 0', 'smram_available = 0', 'smram_enable = 0',
        'smram_restricted = 0', 'return 0;', 'return 1;')) {
    if ($memory -notmatch $pattern) {
        throw "Missing BX-MEM-024 invariant: $pattern"
    }
}
foreach ($pattern in @('(?-i:\bSIM->)', '(?-i:BXPN_)', '(?-i:register_state)',
        '(?-i:load_ROM)', '(?-i:PCI)', '(?-i:VGA)', '(?-i:adapter)',
        '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:BOP)', '(?-i:host pointer)')) {
    if ($memory -match $pattern) {
        throw "Forbidden BX-MEM-024 dependency: $pattern"
    }
}

$io = Get-RegisteredBlock $portSpaceSource 'BX-IO-025'
foreach ($pattern in @('register_default_io_read_handler', 'register_default_io_write_handler',
        'new struct io_handler_struct \*\[PORTS\]', 'read_port_to_handler\[i\] = &io_read_handlers',
        'write_port_to_handler\[i\] = &io_write_handlers', 'cleanup_empty_port_space',
        'read_port_to_handler\[i\] != &io_read_handlers',
        'write_port_to_handler\[i\] != &io_write_handlers', 'delete \[\] read_port_to_handler',
        'delete \[\] write_port_to_handler', 'return 0;', 'return 1;')) {
    if ($io -notmatch $pattern) {
        throw "Missing BX-IO-025 invariant: $pattern"
    }
}
foreach ($pattern in @('(?-i:\bSIM->)', '(?-i:bx_virt_timer)', '(?-i:bx_slowdown_timer)',
        '(?-i:PLUG_)', '(?-i:DEV_)', '(?-i:CMOS)', '(?-i:timer)',
        '(?-i:init_plugins)', '(?-i:register_timer)', '(?-i:port92)', '(?-i:bx_gui)',
        '(?-i:adapter)', '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:WOW)', '(?-i:BOP)')) {
    if ($io -match $pattern) {
        throw "Forbidden BX-IO-025 dependency: $pattern"
    }
}

$mantle = Get-Content -LiteralPath $portSpaceSource -Raw
$devices = Get-Content -LiteralPath $devicesSource -Raw
foreach ($pattern in @('bx_devices_c::bx_devices_c\(\)', 'bx_devices_c::~bx_devices_c\(\)',
        'void bx_devices_c::init_stubs\(\)', 'bx_devices_c::inp\(Bit16u addr, unsigned io_len\)',
        'bx_devices_c::outp\(Bit16u addr, Bit32u value, unsigned io_len\)')) {
    if ($mantle -notmatch $pattern) {
        throw "Missing BX-MANTLE-065 original member in mantle: $pattern"
    }
    if ($devices -match $pattern) {
        throw "BX-MANTLE-065 member remains in full device source: $pattern"
    }
}
foreach ($pattern in @('bx_devices_c::init\(BX_MEM_C \*newmem\)',
        'void bx_devices_c::reset\(unsigned type\)', 'void bx_devices_c::exit\(\)')) {
    if ($devices -notmatch $pattern) {
        throw "Full-device-only member unexpectedly missing: $pattern"
    }
}

Write-Output 'bx-ntvdm-minimal-mechanics-boundary: source and lifecycle invariants verified'
