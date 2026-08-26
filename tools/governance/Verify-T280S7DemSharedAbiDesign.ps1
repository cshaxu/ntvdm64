[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$plan = Join-Path $root 'docs/etc/operations/m0-t280-s7-dem-shared-abi-design-plan-001.md'
$ledger = Join-Path $root 'docs/etc/operations/m0-t280-s7-dem-shared-abi-ledger.tsv'
$evidence = Join-Path $root 'docs/etc/operations/m0-t280-s7-dem-shared-abi-evidence-001.md'
foreach ($path in @($plan, $ledger, $evidence)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T280 S7 artifact: $path" }
}
$expected = @('DEM-build-form','disk-space-name','NT-directory-and-file-records','DASD-storage','SoftPC-guest-memory')
$rows = Import-Csv -LiteralPath $ledger -Delimiter "`t"
if ($rows.Count -ne $expected.Count) { throw "Expected $($expected.Count) S7 family rows; found $($rows.Count)." }
foreach ($family in $expected) {
    $row = @($rows | Where-Object family -eq $family)
    if ($row.Count -ne 1) { throw "Expected one S7 ledger row for $family." }
    foreach ($field in @('original_source_form','DEM_consumers','current_modern_finding','future_owner','mapping_manager_disposition','minimal_follow_on')) {
        if ([string]::IsNullOrWhiteSpace($row[0].$field)) { throw "$family has no $field evidence." }
    }
}
if (($rows | Where-Object family -eq 'SoftPC-guest-memory').mapping_manager_disposition -notmatch 'guest_memory') {
    throw 'SoftPC guest-memory row does not retain the session mapping-manager boundary.'
}
foreach ($family in @('NT-directory-and-file-records','DASD-storage')) {
    if (($rows | Where-Object family -eq $family).mapping_manager_disposition -notmatch 'host-local') {
        throw "$family does not explicitly exclude host-local records from mapping managers."
    }
}
Write-Host 'PASS: T280 S7 has five source-shaped DEM ABI families with one mapping boundary.'
