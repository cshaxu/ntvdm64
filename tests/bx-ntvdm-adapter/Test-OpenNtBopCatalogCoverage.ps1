$ErrorActionPreference = 'Stop'

$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$inventoryPath = Join-Path $repositoryRoot 'artifacts\analysis\opennt-bop-inventory-005-20260811-001\opennt-bop-inventory.json'
$catalogPath = Join-Path $repositoryRoot 'src\bx-ntvdm-adapter\bx_ntvdm_bop_catalog_v1.c'
if (-not (Test-Path -LiteralPath $inventoryPath)) { throw "Missing pinned BOP inventory: $inventoryPath" }

$inventory = Get-Content -LiteralPath $inventoryPath -Raw | ConvertFrom-Json
$catalog = Get-Content -LiteralPath $catalogPath -Raw
$topLevelValues = @($inventory.top_level_selectors.value) | Sort-Object -Unique
foreach ($value in $topLevelValues) {
    $case = 'case 0x{0:x2}u:' -f [uint32]$value
    if (-not $catalog.Contains($case)) {
        throw "Catalog does not classify source-defined BOP selector $case"
    }
}

$tableValues = @($inventory.softpc_bios_table_rows.selector) | Sort-Object -Unique
if ($tableValues.Count -ne 256 -or ($tableValues | Where-Object { $_ -lt 0 -or $_ -gt 255 }).Count -ne 0) {
    throw 'Pinned SoftPC BIOS[] inventory does not prove a complete byte-selector table.'
}
if ($catalog -notmatch 'default:\s*return BX_NTVDM_BOP_CATALOG_V1_SOURCE_SOFTPC_BIOS') {
    throw 'Catalog does not classify every historical SoftPC BIOS[] table selector.'
}
$namedBiosValues = @($inventory.softpc_bios_selector_definitions.value) | Sort-Object -Unique
if ($namedBiosValues.Count -eq 0) {
    throw 'Pinned inventory does not contain named SoftPC BIOS selector definitions.'
}

if ($catalog -notmatch 'SOURCE_UNCATALOGUED' -or $catalog -notmatch 'identity->is_bop = 1u') {
    throw 'Catalog does not retain the uncatalogued BOP observation path.'
}

Write-Output "OpenNT BOP catalog coverage verified for $($topLevelValues.Count) top-level, $($namedBiosValues.Count) named BIOS and $($tableValues.Count) BIOS-table selectors."
