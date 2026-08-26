[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'vdmredir/*'})
if ($rows.Count -ne 25) { throw "Expected 25 Redirector rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No Redirector row may remain unresolved after final package review.' }
if (@($rows | Where-Object {$_.file_kind -eq 'source' -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'Every Redirector source body must be adapter-backed.' }
if (@($rows | Where-Object {$_.file_kind -eq 'declaration' -and $_.final_disposition -ne 'binding-only'}).Count) { throw 'Every Redirector declaration must be binding-only.' }
Write-Output "T276 S25 Redirector final-disposition verification passed: $($rows.Count) rows."
