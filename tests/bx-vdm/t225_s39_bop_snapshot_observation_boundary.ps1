param(
    [Parameter(Mandatory = $true)]
    [string] $RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$coreAndMantle = @(
    (Join-Path $root 'src/bx-core'),
    (Join-Path $root 'src/bx-mantle')
)

foreach ($path in $coreAndMantle) {
    $match = Get-ChildItem -LiteralPath $path -Recurse -File |
        Select-String -SimpleMatch -Pattern 'bx_ntvdm_bop_sequence_observation_v1' -ErrorAction SilentlyContinue
    if ($null -ne $match) { throw "BOP snapshot observer crossed its bx-vdm/CLI boundary: $path" }
}

$header = Get-Content -LiteralPath (Join-Path $root 'src/bx-vdm/bop-v1/bx_ntvdm_bop_sequence_observation_v1.h') -Raw
$observer = Get-Content -LiteralPath (Join-Path $root 'src/bx-vdm/bop-v1/bx_ntvdm_bop_sequence_observation_v1.c') -Raw
$cli = Get-Content -LiteralPath (Join-Path $root 'src/cli/ntdos64_native_cli.c') -Raw

foreach ($required in @(
    'BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_VERSION UINT32_C(2)',
    'uint32_t eax, ebx, ecx, edx, esi, edi, eflags;',
    'uint16_t ds, es, ss;'
)) {
    if (!$header.Contains($required)) { throw "Missing versioned snapshot ABI fragment: $required" }
}
foreach ($required in @('slot->eax = event->eax', 'slot->eflags = event->eflags', 'slot->ss = event->ss')) {
    if (!$observer.Contains($required)) { throw "Missing exact snapshot copy: $required" }
}
foreach ($required in @('eax=%08x', 'eflags', 'ds=%04x', 'ss=%04x')) {
    if (!$cli.Contains($required)) { throw "Missing CLI snapshot presentation fragment: $required" }
}

Write-Output 't225-s39-bop-snapshot-boundary=pass'