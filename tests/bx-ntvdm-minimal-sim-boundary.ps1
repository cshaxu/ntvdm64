$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$headerPath = Join-Path $repositoryRoot 'src/adapter-bochs/minimal_sim.h'
$sourcePath = Join-Path $repositoryRoot 'src/adapter-bochs/minimal_sim.cc'

foreach ($path in @($headerPath, $sourcePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing BX-MACH-023 source: $path"
    }
}

$source = Get-Content -LiteralPath $sourcePath -Raw
$required = @(
    'class bx_mantle_minimal_sim_c : public bx_simulator_interface_c',
    'bx_cpudb_pentium_mmx',
    'Bit32u apic_id_mask = 0x0f',
    'bx_bool simulate_xapic = 0',
    'BX_CPU_LEVEL != 5',
    'BX_SUPPORT_X86_64',
    'new bx_list_c\(root_param, "cpu", "CPU Options"\)',
    '"model"',
    'new bx_param_bool_c\(cpu,',
    '"reset_on_triple_fault"',
    '"ignore_bad_msrs"',
    '"msrs"',
    'BX_MANTLE_MINIMAL_SIM_RESET_PARAM_NOT_FALSE'
)
foreach ($pattern in $required) {
    if ($source -notmatch $pattern) {
        throw "Missing required BX-MACH-023 invariant: $pattern"
    }
}

$forbidden = @(
    'bx_init_siminterface', 'config\.cc', 'bx_gui', 'text_', 'PLUG_', 'bx_load_',
    'DEV_', 'bx_devices', 'cmos', 'timer', 'adapter', 'OpenNT',
    'DOS', 'WOW', 'BOP'
)
foreach ($pattern in $forbidden) {
    if ($source -match $pattern) {
        throw "Forbidden BX-MACH-023 dependency: $pattern"
    }
}

Write-Output 'bx-ntvdm-minimal-sim-boundary: source invariants verified'