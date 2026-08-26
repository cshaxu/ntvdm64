[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.package_root -eq 'wow32'})
$headers = @($rows | Where-Object {$_.file_kind -eq 'declaration'})
if ($headers.Count -ne 105 -or @($headers | Where-Object {$_.final_disposition -ne 'binding-only'}).Count) { throw 'Every selected WOW32 declaration carrier must be binding-only.' }
$excluded = @($rows | Where-Object {$_.final_evidence -eq 'T276 S22 WOW32 condition-aware build selection: unselected source or build/resource evidence only'})
if ($excluded.Count -ne 19 -or @($excluded | Where-Object {$_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'Every selected WOW32 unselected/build/resource row must be not-host-runtime.' }
Write-Output "T276 S25 WOW32 carrier final-disposition verification passed: $($headers.Count + $excluded.Count) rows."
