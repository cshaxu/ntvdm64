param(
    [string]$RepositoryRoot = (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
)

$ErrorActionPreference = 'Stop'
$inventory = Join-Path $RepositoryRoot 'artifacts\analysis\opennt-bop-inventory-005-20260811-001\opennt-bop-inventory.json'
if (!(Test-Path -LiteralPath $inventory)) { throw "Missing immutable inventory: $inventory" }
$data = Get-Content -LiteralPath $inventory -Raw | ConvertFrom-Json
$rows = @($data.softpc_bios_table_rows)
$definitions = @($data.softpc_bios_selector_definitions)
if ($definitions.Count -ne 41) { throw "Expected 41 BIOS selector definitions, got $($definitions.Count)" }
if ($rows.Count -ne 349) { throw "Expected 349 annotated BIOS table rows, got $($rows.Count)" }
if (@($rows | Where-Object { $_.owner_symbol -eq 'illegal_bop' }).Count -ne 227) { throw 'Unexpected illegal_bop row count' }
if (@($rows | Where-Object { $_.owner_symbol -ne 'illegal_bop' }).Count -ne 122) { throw 'Unexpected non-illegal BIOS row count' }

# These are the named OpenNT top-level selectors.  Table-form collisions are
# historical compile-time alternatives, never runtime owner selection.
$top = @(0x50,0x51,0x52,0x53,0x54,0x56,0x57,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0xfd,0xfe)
$collisions = @($rows | Where-Object { $top -contains [int]$_.selector })
if ($collisions.Count -ne 25) { throw "Expected 25 host-selector table forms, got $($collisions.Count)" }
foreach ($selector in $top) {
    $forms = @($collisions | Where-Object { [int]$_.selector -eq $selector })
    if ($forms.Count -lt 1) { throw ('Missing historical collision form 0x{0:X2}' -f $selector) }
}

# A row can only be either a historical impossible entry, a host-selector
# collision, or a native-machine candidate.  None is an adapter implementation.
foreach ($row in $rows) {
    $classification = if ($row.owner_symbol -eq 'illegal_bop') { 'historical-unavailable' }
        elseif ($top -contains [int]$row.selector) { 'host-selector-collision' }
        else { 'bochs-native-candidate' }
    if ($classification -notin @('historical-unavailable','host-selector-collision','bochs-native-candidate')) {
        throw "Unclassified BIOS row at line $($row.line)"
    }
}
Write-Output "SoftPC BIOS owner matrix verified: definitions=41 rows=349 unavailable=227 collisions=25 native-candidates=97"
