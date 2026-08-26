[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-softpc-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing S12 ledger: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 998) { throw "Expected 998 S12 rows, found $($rows.Count)." }
if (($rows | Group-Object file_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate S12 file identity.' }
if (($rows | Where-Object { $_.state -ne 'provisional-not-enabled' }).Count -ne 0) { throw 'S12 must not enable a source path.' }
if (@($rows | Where-Object provisional_composition_disposition -eq 'bochs-replacement-profile-excluded').Count -eq 0) { throw 'Missing Bochs replacement exclusion rows.' }
if (@($rows | Where-Object final_component_owner -eq 'tools/opennt').Count -eq 0) { throw 'Missing historical build-tool rows.' }
if (@($rows | Where-Object final_component_owner -eq 'opennt-mvdm-firmware').Count -ne 58) { throw 'Expected 58 separately mirrored firmware-input rows.' }
foreach ($row in $rows) {
    foreach ($column in 'original_build_membership','original_subpackage_role','final_component_owner','provisional_composition_disposition','source_derived_reason','evidence') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.target_path)." }
    }
}
Write-Output "T276 S12 softpc disposition verification passed: $($rows.Count) paths."
