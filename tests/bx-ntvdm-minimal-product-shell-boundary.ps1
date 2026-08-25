$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$sourcePath = Join-Path $repositoryRoot 'src/adapter-bochs/minimal_product_shell.cc'
$pcSystemPath = Join-Path $repositoryRoot 'src/bochs-core/pc_system.cc'

$source = Get-Content -LiteralPath $sourcePath -Raw
$pcSystem = Get-Content -LiteralPath $pcSystemPath -Raw
foreach ($pattern in @(
    'bx_gui_c \*bx_gui = NULL',
    'int bx_atexit\(void\)',
    'bx_param_num_c::text_ask\(FILE \*, FILE \*\) \{ return -1; \}',
    'bx_list_c::text_ask\(FILE \*, FILE \*\) \{ return -1; \}',
    'int bx_pc_system_c::Reset\(unsigned type\)',
    'return type == BX_RESET_HARDWARE \? -1 : 0;',
    'void bx_pc_system_c::exit\(void\)',
    'numTimers = 1 \+ BX_SUPPORT_APIC;'
)) {
    if ($source -notmatch $pattern) { throw "Missing finite product-shell invariant: $pattern" }
}
foreach ($pattern in @('bx_devices\.exit', 'bx_gui->', 'DEV_reset_devices', 'PLUG_',
        '(?-i:adapter)', '(?-i:OpenNT)', '(?-i:DOS)', '(?-i:WOW)', '(?-i:BOP)')) {
    if ($source -match $pattern) { throw "Forbidden finite product-shell dependency: $pattern" }
}
foreach ($pattern in @('int bx_pc_system_c::Reset\(unsigned type\)', 'void bx_pc_system_c::exit\(void\)')) {
    if ($pcSystem -match $pattern) { throw "Product-bound PC-time source still defines: $pattern" }
}

Write-Output 'bx-ntvdm-minimal-product-shell-boundary: finite shell invariants verified'
