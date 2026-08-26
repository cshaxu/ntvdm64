[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$hostPath = Join-Path $operations 'm0-t276-s12-host-src-disposition-ledger.tsv'
$densityPath = Join-Path $operations 'm0-t276-s12-host-src-interface-density-ledger.tsv'
foreach ($path in @($hostPath, $densityPath)) { if (-not (Test-Path -LiteralPath $path)) { throw "Missing S12 input: $path" } }
$sourceRows = @(Import-Csv -LiteralPath $hostPath -Delimiter "`t")
$density = @(Import-Csv -LiteralPath $densityPath -Delimiter "`t")
if ($density.Count -ne 56) { throw "Expected 56 density rows, found $($density.Count)." }
if (($density | Group-Object file_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate host/src density identity.' }
foreach ($source in $sourceRows) {
    $row = @($density | Where-Object file_id -eq $source.file_id)
    if ($row.Count -ne 1) { throw "Missing density row for $($source.source_path)." }
    if ($row[0].original_role_group -ne $source.original_role_group) { throw "Group mismatch for $($source.source_path)." }
    foreach ($column in 'lexical_call_count','lexical_unique_declaration_count','lexical_ambiguous_declaration_count','lexical_no_selected_extern_count','selected_definition_name_observed_count','external_boundary_candidate_count') {
        if ([int]$row[0].$column -lt 0) { throw "Negative $column for $($source.source_path)." }
    }
    if ($row[0].interface_audit_state -ne 'coverage-only; source-form/ABI/failure review remains required before any binding decision') { throw "Unexpected interface state for $($source.source_path)." }
}
Write-Output "T276 S12 host/src interface-density verification passed: $($density.Count) source rows."
