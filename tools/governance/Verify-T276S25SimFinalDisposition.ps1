[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'sim16/*' -or $_.source_path -like 'sim32/*'})
if ($rows.Count -ne 13) { throw "Expected 13 SIM16/SIM32 rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No SIM16/SIM32 row may remain unresolved after final package review.' }
if (@($rows | Where-Object {$_.source_path -eq 'sim32/sim32.h' -and $_.final_disposition -ne 'binding-only'}).Count) { throw 'sim32.h must remain a binding-only facade declaration.' }
if (@($rows | Where-Object {$_.source_path -in @('sim16/sim16.asm','sim16/siminit.asm','sim32/sim32.c') -and $_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'Historical SIM serial product-shell bodies cannot enter the final host runtime.' }
Write-Output "T276 S25 SIM final-disposition verification passed: $($rows.Count) rows."
