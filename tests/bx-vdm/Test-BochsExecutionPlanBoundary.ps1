[CmdletBinding()]
param([string]$RepositoryRoot = '')

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$mainPath = Join-Path $RepositoryRoot 'refs\bochs\main.cc'
$cpuPath = Join-Path $RepositoryRoot 'src\bx-core\cpu\init.cc'
foreach ($path in @($mainPath, $cpuPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing source: $path" }
}
$main = Get-Content -LiteralPath $mainPath -Raw
if ($main -notmatch 'bx_ntvdm_adapter_runtime\.h') {
    throw 'Bochs execution-plan seam does not include the admitted adapter runtime endpoint.'
}
$executionGuard = '#if defined(BX_NTVDM_ENABLE_EXECUTION_PLAN) && BX_NTVDM_ENABLE_EXECUTION_PLAN'
$deferredGuard = '#if defined(BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN) && BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN'
if ($main.IndexOf($executionGuard, [System.StringComparison]::Ordinal) -lt 0 -or
    $main.IndexOf($deferredGuard, [System.StringComparison]::Ordinal) -lt 0) {
    throw 'Execution-plan and deferred-startup adapter seams must both be explicit default-off opt-ins.'
}
$runtimeInclude = $main.IndexOf('#include "bop-v1/bx_ntvdm_adapter_runtime.h"', [System.StringComparison]::Ordinal)
$runtimeGuard = $main.LastIndexOf('#if (defined(BX_NTVDM_ENABLE_EXECUTION_PLAN)', $runtimeInclude, [System.StringComparison]::Ordinal)
if ($runtimeInclude -lt 0 -or $runtimeGuard -lt 0) {
    throw 'Adapter runtime include escapes the combined default-off guard.'
}
$helperStart = $main.IndexOf('static int bx_apply_requested_execution_plan(void)')
$helperEnd = $main.IndexOf('bx_startup_flags_t bx_startup_flags;', $helperStart)
if ($helperStart -lt 0 -or $helperEnd -lt $helperStart) { throw 'Missing unique execution-plan consumer.' }
$helper = $main.Substring($helperStart, $helperEnd - $helperStart)
$ordered = @(
    'bx_ntvdm_adapter_runtime_v1_prepare_execution_plan_from_environment',
    'bx_ntvdm_startup_plan_v1_preflight',
    'ordinary_ram_readable',
    'copy_from_ordinary_ram',
    'copy_to_ordinary_ram',
    'payload_write.guest_physical_address',
    'copy_to_ordinary_ram',
    'preserved_state_address',
    'apply_real_mode_entry'
)
$offset = 0
foreach ($token in $ordered) {
    $next = $helper.IndexOf($token, $offset)
    if ($next -lt $offset) { throw "Missing or misordered execution-plan token: $token" }
    $offset = $next + $token.Length
}
foreach ($forbidden in @('OpenNT', 'DOS', 'WOW', 'DEM', 'BOP', 'BIOS', 'firmware', 'profile', 'COMMAND', 'service', 'vector')) {
    if ($helper -match [regex]::Escape($forbidden)) { throw "Execution-plan consumer contains forbidden semantic token: $forbidden" }
}
$hardware = $main.IndexOf('bx_init_hardware();')
$seam = $main.IndexOf('if (bx_apply_requested_execution_plan() < 0)', $hardware)
$cpuLoop = $main.IndexOf('BX_CPU(0)->cpu_loop()', $hardware)
if ($hardware -lt 0 -or $seam -lt $hardware -or $cpuLoop -lt $seam) {
    throw 'Execution-plan seam is not uniquely after hardware initialization and before CPU loop.'
}
$callGuard = $main.LastIndexOf($executionGuard, $seam, [System.StringComparison]::Ordinal)
if ($callGuard -lt $hardware) {
    throw 'Execution-plan call is not enclosed by its default-off opt-in guard.'
}
$cpu = Get-Content -LiteralPath $cpuPath -Raw
if ($cpu -notmatch 'void BX_CPU_C::apply_real_mode_entry\(' -or
    $cpu -notmatch 'load_seg_reg\(&sregs\[BX_SEG_REG_CS\], cs\);\s*RIP=eip;') {
    throw 'CPU real-mode entry delta is incomplete.'
}
Write-Host 'Bochs execution-plan boundary: single generic pre-loop consumer verified.'
