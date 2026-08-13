$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot
$headerPath = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_generic_ud_bridge.h'
$sourcePath = Join-Path $repositoryRoot 'src/bx-mantle/bx_ntvdm_generic_ud_bridge.cc'
foreach ($path in @($headerPath, $sourcePath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing generic #UD bridge input: $path" }
}
$header = Get-Content -LiteralPath $headerPath -Raw
$source = Get-Content -LiteralPath $sourcePath -Raw
foreach ($pattern in @('struct bx_ntvdm_generic_ud_event_v1',
        'struct bx_ntvdm_generic_ud_outcome_v1',
        'BX_NTVDM_GENERIC_UD_PASS_THROUGH', 'BX_NTVDM_GENERIC_UD_RESUME',
        'BX_NTVDM_GENERIC_UD_STOP', 'bx_ntvdm_mantle_generic_ud_bridge_v1')) {
    if ($header -notmatch $pattern) { throw "Missing generic #UD bridge invariant: $pattern" }
}
foreach ($pattern in @('bx-vdm', '(?-i:adapter)', '(?-i:BOP)', '(?-i:OpenNT)',
        '(?-i:DOS)', '(?-i:WOW)', 'getenv', 'SIM->', 'bx_gui', 'callback')) {
    if ($header -match $pattern -or $source -match $pattern) {
        throw "Forbidden generic #UD bridge dependency: $pattern"
    }
}
if ($source -notmatch 'return 0;') { throw 'The uncomposed generic #UD bridge must decline.' }
Write-Output 'bx-mantle-generic-ud-bridge-boundary: fixed uncomposed bridge verified'
