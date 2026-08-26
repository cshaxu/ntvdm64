[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'softpc.new/base/support/*.c'})
if ($rows.Count -ne 6) { throw "Expected 6 SoftPC support source rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No SoftPC support source row may remain unresolved.' }
if (($rows | Where-Object {$_.source_path -eq 'softpc.new/base/support/get_env.c'}).final_disposition -ne 'adapter-backed') { throw 'get_env.c must be adapter-backed.' }
Write-Output "T276 S25 SoftPC support source final-disposition verification passed: $($rows.Count) rows."
