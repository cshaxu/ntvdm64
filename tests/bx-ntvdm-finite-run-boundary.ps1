$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$headerPath = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_finite_run.h'
$sourcePath = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_finite_run.cc'
$toolPath = Join-Path $repositoryRoot 'tools/Invoke-T198S3FiniteNativeRunProbe.ps1'
foreach ($path in @($headerPath, $sourcePath, $toolPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T198 S3 input: $path" }
}

$header = Get-Content -LiteralPath $headerPath -Raw
$source = Get-Content -LiteralPath $sourcePath -Raw
$tool = Get-Content -LiteralPath $toolPath -Raw
foreach ($pattern in @(
        'enum bx_ntvdm_finite_run_status', 'BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET',
        'struct bx_ntvdm_finite_run_request', 'const Bit8u \*entry_bytes;',
        'Bit64u instruction_tick_budget;', 'Bit32u ips;',
        'bx_ntvdm_run_finite_bare_bytes')) {
    if ($header -notmatch $pattern) { throw "Missing finite-run contract invariant: $pattern" }
}
foreach ($pattern in @(
        'bx_ntvdm_minimal_machine_c machine;', 'machine\.initialize\(0x100000, 0x100000\)',
        'bx_mem\.copy_to_ordinary_ram', 'bx_pc_system\.initialize\(request->ips\)',
        'bx_cpu\.apply_real_mode_entry', 'bx_pc_system\.register_timer_ticks',
        'bx_ntvdm_finite_run_stop', 'bx_pc_system\.kill_bochs_request = 1;',
        'bx_cpu\.cpu_loop\(\);', 'bx_pc_system\.unregisterTimer', 'machine\.cleanup\(\)')) {
    if ($source -notmatch $pattern) { throw "Missing finite-run source invariant: $pattern" }
}
foreach ($pattern in @('bx_user_quit', 'main\.cc', 'bx-vdm', '(?-i:adapter)',
        '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:WOW)', '(?-i:BOP)', 'SIM->', 'bx_gui')) {
    if ($header -match $pattern -or $source -match $pattern) {
        throw "Forbidden finite-run dependency: $pattern"
    }
}
foreach ($pattern in @('WholeCpu5Core', '0xf4', 'instructionTickBudget = 64',
        "'main.cc'", "'adapter'", "'OpenNT'", "'BOP'", "'CLI'")) {
    if ($tool -notmatch $pattern) { throw "Missing finite-run probe invariant: $pattern" }
}

Write-Output 'bx-ntvdm-finite-run-boundary: selector-blind finite mantle invariants verified'
