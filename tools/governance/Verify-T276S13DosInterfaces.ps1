[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations/m0-t276-s13-dos-interface-family-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 8) { throw "Expected eight DOS interface families, found $($rows.Count)." }
if (@($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'DOS interface IDs must be unique.' }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.source_paths) -or [string]::IsNullOrWhiteSpace($_.owner) }).Count -ne 0) { throw 'Every DOS interface family requires source evidence and an owner.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S13 interface audit may not enable a provider.' }
Write-Output "T276 S13 DOS interface-family verification passed: 8 rows."
