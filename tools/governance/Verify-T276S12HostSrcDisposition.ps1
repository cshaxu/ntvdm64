[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-host-src-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing S12 host/src ledger: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 56) { throw "Expected 56 host/src source rows, found $($rows.Count)." }
if (($rows | Group-Object file_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate host/src source identity.' }
if (($rows | Where-Object { $_.state -ne 'provisional-not-enabled' }).Count -ne 0) { throw 'Host/src audit must not enable a source.' }
if (@($rows | Where-Object original_role_group -eq 'opennt-bop-dispatch').Count -ne 1) { throw 'Expected exactly one original BOP dispatch unit.' }
if (@($rows | Where-Object original_role_group -eq 'literal-build-unobserved-auxiliary').Count -ne 10) { throw 'Expected ten literal-build-unobserved source rows.' }
foreach ($row in $rows) {
    foreach ($column in 'original_literal_build_membership','original_role_group','required_boundary_owner','original_contract_summary','provisional_disposition','evidence') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.source_path)." }
    }
}
Write-Output "T276 S12 host/src disposition verification passed: $($rows.Count) source rows."
