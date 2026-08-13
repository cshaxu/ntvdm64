$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$machineHeader = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_minimal_machine.h'
$machineSource = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_minimal_machine.cc'
$memoryHeader = Join-Path $repositoryRoot 'src/bochs/memory/memory.h'
$iodevHeader = Join-Path $repositoryRoot 'src/bochs/iodev/iodev.h'

foreach ($path in @($machineHeader, $machineSource, $memoryHeader, $iodevHeader)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing BX-MACH-026 source: $path"
    }
}

$header = Get-Content -LiteralPath $machineHeader -Raw
$source = Get-Content -LiteralPath $machineSource -Raw
$cleanupMatch = [regex]::Match($source, '(?s)bx_ntvdm_minimal_machine_status bx_ntvdm_minimal_machine_c::cleanup\(void\)\s*\{(.*?)\n\}')
if (-not $cleanupMatch.Success) {
    throw 'Missing BX-MACH-026 cleanup implementation'
}
$cleanupSource = $cleanupMatch.Groups[1].Value
$memory = Get-Content -LiteralPath $memoryHeader -Raw
$iodev = Get-Content -LiteralPath $iodevHeader -Raw

foreach ($pattern in @(
        'class bx_ntvdm_minimal_machine_c',
        'bx_ntvdm_minimal_machine_status initialize\(Bit64u guest, Bit64u host\);',
        'bx_ntvdm_minimal_machine_status cleanup\(void\);',
        'bx_bool attempted;', 'bx_bool memory_owned;', 'bx_bool port_space_owned;')) {
    if ($header -notmatch $pattern) {
        throw "Missing BX-MACH-026 API invariant: $pattern"
    }
}

foreach ($declaration in @($memory, $iodev)) {
    if ($declaration -notmatch 'friend class bx_ntvdm_minimal_machine_c;') {
        throw 'Missing BX-MACH-026 friend declaration'
    }
}

foreach ($pattern in @(
        'logfunctions \*pluginlog = &bx_ntvdm_minimal_machine_log;',
        'bx_pc_system_c bx_pc_system;', 'bx_debug_t bx_dbg;',
        'BOCHSAPI BX_CPU_C bx_cpu;', 'BOCHSAPI BX_MEM_C bx_mem;',
        'bx_bool bx_user_quit = 0;',
        'SAFE_GET_IOFUNC\(\);', 'SAFE_GET_GENLOG\(\);',
        'bx_ntvdm_minimal_sim_initialize\(\)',
        'bx_mem\.init_memory_without_sim\(guest, host\)',
        'bx_devices\.init_empty_port_space\(\)',
        'bx_cpu\.initialize\(\);', 'bx_cpu\.reset\(BX_RESET_HARDWARE\);',
        'bx_devices\.cleanup_empty_port_space\(\)', 'bx_mem\.cleanup_memory\(\)')) {
    if ($source -notmatch $pattern) {
        throw "Missing BX-MACH-026 source invariant: $pattern"
    }
}

function Assert-Ordered([string] $first, [string] $second) {
    $firstIndex = $source.IndexOf($first, [System.StringComparison]::Ordinal)
    $secondIndex = $source.IndexOf($second, [System.StringComparison]::Ordinal)
    if ($firstIndex -lt 0 -or $secondIndex -lt 0 -or $firstIndex -ge $secondIndex) {
        throw "BX-MACH-026 lifecycle order violated: $first before $second"
    }
}

Assert-Ordered 'SAFE_GET_IOFUNC();' 'SAFE_GET_GENLOG();'
Assert-Ordered 'SAFE_GET_GENLOG();' 'bx_ntvdm_minimal_sim_initialize()'
Assert-Ordered 'bx_ntvdm_minimal_sim_initialize()' 'bx_mem.init_memory_without_sim(guest, host)'
Assert-Ordered 'bx_mem.init_memory_without_sim(guest, host)' 'bx_devices.init_empty_port_space()'
Assert-Ordered 'bx_devices.init_empty_port_space()' 'bx_cpu.initialize();'
Assert-Ordered 'bx_cpu.initialize();' 'bx_cpu.reset(BX_RESET_HARDWARE);'
if ($cleanupSource.IndexOf('bx_devices.cleanup_empty_port_space()', [System.StringComparison]::Ordinal) -ge
        $cleanupSource.IndexOf('bx_mem.cleanup_memory()', [System.StringComparison]::Ordinal)) {
    throw 'BX-MACH-026 cleanup does not release port space before memory'
}

foreach ($pattern in @(
        'main\.cc', 'bx_init_main', 'bx_begin_simulation', 'bx_init_options',
        'bx_init_hardware', 'bx_pc_system\.initialize', 'bx_pc_system\.Reset',
        'cpu_loop', 'load_ROM', 'load_RAM', 'SIM->configuration_interface',
        'bx_gui', 'PLUG_', 'DEV_init', 'DEV_reset', 'CMOS', 'timer', 'adapter',
        'OpenNT', 'DOS', 'WOW', 'DEM', 'BOP', 'CLI', 'extern "C"')) {
    if ($source -match $pattern -or $header -match $pattern) {
        throw "Forbidden BX-MACH-026 dependency: $pattern"
    }
}

Write-Output 'bx-ntvdm-minimal-machine-boundary: composition invariants verified'
