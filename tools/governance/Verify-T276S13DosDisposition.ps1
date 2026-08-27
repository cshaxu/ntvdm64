[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$ledgerPath = Join-Path $operations 'm0-t276-s13-dos-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $ledgerPath)) { throw "Missing output: $ledgerPath" }
$rows = @(Import-Csv -LiteralPath $ledgerPath -Delimiter "`t")
if ($rows.Count -ne 40) { throw "Expected 40 DOS disposition rows, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) { throw 'Every DOS path must have exactly one disposition.' }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.provisional_component_owner) -or [string]::IsNullOrWhiteSpace($_.required_boundary_owner) }).Count -ne 0) { throw 'Every DOS path requires a component owner and boundary disposition.' }
$sources = @($rows | Where-Object file_kind -eq 'source')
if ($sources.Count -ne 27) { throw "Expected 27 DOS source units, found $($sources.Count)." }
if (@($sources | Where-Object { $_.provisional_component_owner -ne 'mvdm-host' }).Count -ne 0) { throw 'Every DOS source unit must remain in the mvdm-host audit carrier.' }
if (@($sources | Where-Object { $_.original_literal_build_membership -ne 'observed-in-literal-sources' }).Count -ne 0) { throw 'Every DOS source unit must have literal historical SOURCES evidence.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S13 may not enable a source body.' }
Write-Output "T276 S13 dos disposition verification passed: 40 paths, 27 source units."
