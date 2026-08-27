[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$ledger = Join-Path $RepositoryRoot 'docs/etc/operations/m0-t282-s9-command-environment-guest-seam-ledger-001.tsv'
$rows = Import-Csv -LiteralPath $ledger -Delimiter "`t"
$required = @('CMD-SEAM-001','CMD-SEAM-002','CMD-SEAM-003','CMD-SEAM-004')
foreach ($id in $required) {
    $row = @($rows | Where-Object { $_.id -eq $id })
    if ($row.Count -ne 1 -or [string]::IsNullOrWhiteSpace($row[0].minimum_span_and_termination) -or
        [string]::IsNullOrWhiteSpace($row[0].safe_disposition) -or
        [string]::IsNullOrWhiteSpace($row[0].source_failure_or_divergence)) {
        throw "Missing complete command environment guest seam record: $id"
    }
}
if (@($rows | Where-Object { $_.safe_disposition -match 'GetVDMAddr' }).Count -ne 0) {
    throw 'A naked GetVDMAddr route is not a permitted safe disposition.'
}
Write-Host "PASS: T282 S9 COMMAND environment guest seam ($($rows.Count) rows)"
