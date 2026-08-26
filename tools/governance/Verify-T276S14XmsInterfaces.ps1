[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations/m0-t276-s14-xms-interface-family-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 8) { throw "Expected eight XMS interface families, found $($rows.Count)." }
if (@($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'XMS interface IDs must be unique.' }
if (@($rows | Where-Object { [string]::IsNullOrWhiteSpace($_.owner) -or [string]::IsNullOrWhiteSpace($_.source_paths) }).Count -ne 0) { throw 'Every XMS interface family requires an owner and source evidence.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S14 may not enable an XMS interface.' }
if (@($rows | Where-Object { $_.interface_id -eq 'XMS-IF-07' -and $_.disposition -eq 'architecture/profile exclusion' }).Count -ne 1) { throw 'The direct host-address path must remain an explicit exclusion.' }
Write-Output 'T276 S14 XMS interface-family verification passed: 8 rows.'
