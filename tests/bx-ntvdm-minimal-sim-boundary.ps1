$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$headerPath = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_minimal_sim.h'
$sourcePath = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_minimal_sim.cc'

foreach ($path in @($headerPath, $sourcePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing BX-MACH-023 source: $path"
    }
}

$source = Get-Content -LiteralPath $sourcePath -Raw
$required = @(
    'bx_init_siminterface\(\);',
    'SIM->get_param\("\."\)',
    'new bx_list_c\(root, "cpu", "CPU Options"\)',
    'new bx_param_bool_c\(cpu,',
    '"reset_on_triple_fault"',
    'BX_NTVDM_MINIMAL_SIM_RESET_PARAM_NOT_FALSE'
)
foreach ($pattern in $required) {
    if ($source -notmatch $pattern) {
        throw "Missing required BX-MACH-023 invariant: $pattern"
    }
}

$forbidden = @(
    'config\.cc', 'bx_gui', 'text_', 'PLUG_', 'bx_load_',
    'DEV_', 'bx_devices', 'cmos', 'timer', 'adapter', 'OpenNT',
    'DOS', 'WOW', 'BOP'
)
foreach ($pattern in $forbidden) {
    if ($source -match $pattern) {
        throw "Forbidden BX-MACH-023 dependency: $pattern"
    }
}

Write-Output 'bx-ntvdm-minimal-sim-boundary: source invariants verified'
