[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'dos/*'})
if ($rows.Count -ne 40) { throw "Expected 40 dos rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No dos row may remain unresolved after the final package audit.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every dos row needs the final audited implementation state.' }
if (@($rows | Where-Object {$_.file_kind -eq 'source' -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'Every selected original DEM/COMMAND provider body must be adapter-backed.' }
if (@($rows | Where-Object {$_.file_kind -eq 'declaration' -and $_.final_disposition -ne 'binding-only'}).Count) { throw 'Every DEM/COMMAND declaration carrier must be binding-only.' }
Write-Output "T276 S25 DOS final-disposition verification passed: $($rows.Count) rows."
