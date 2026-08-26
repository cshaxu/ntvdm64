[CmdletBinding()]
param([Parameter(Mandatory = $true)][string]$RepositoryRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s15-sim16-file-disposition-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing output: $path" }
$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 8) { throw "Expected eight SIM16 paths, found $($rows.Count)." }
if (@($rows | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) { throw 'Every SIM16 path must have one disposition.' }
if (@($rows | Where-Object { $_.provisional_disposition -eq 'WOW-profile-gated host-product body' }).Count -ne 2) { throw 'Expected both SIM16 assembler bodies to remain WOW-profile gated.' }
if (@($rows | Where-Object state -ne 'provisional-not-enabled').Count -ne 0) { throw 'S15 may not enable a SIM16 body.' }
Write-Output 'T276 S15 SIM16 disposition verification passed: 8 paths, 2 WOW-profile-gated bodies.'
