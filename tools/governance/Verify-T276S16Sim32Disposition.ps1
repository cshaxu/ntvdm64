[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$path=Join-Path (Resolve-Path -LiteralPath $RepositoryRoot).Path 'docs/etc/operations/m0-t276-s16-sim32-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows=@(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 5) { throw "Expected five SIM32 paths, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) { throw 'Every SIM32 path must have one disposition.' }
if (@($rows | Where-Object { $_.source_path -eq 'sim32/sim32.c' -and $_.provisional_disposition -eq 'historical-product-shell exclusion; API-form recovery only' }).Count -ne 1) { throw 'SIM32 serial product shell must remain excluded while API form is recovered.' }
if (@($rows | Where-Object { $_.source_path -eq 'sim32/sim32.h' -and $_.required_boundary_owner -like '*mapping manager*' }).Count -ne 1) { throw 'SIM32 declarations must require the unique mapping manager.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S16 may not enable a SIM32 body.' }
Write-Output 'T276 S16 SIM32 disposition verification passed: 5 paths, one product-shell exclusion and one mapping-manager API recovery contract.'
