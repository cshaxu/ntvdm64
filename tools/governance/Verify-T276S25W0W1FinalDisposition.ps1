[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.package_root -in @('dirs', 'makefil0', 'inc', 'oemuni', 'suballoc', 'vdmutils')})
if ($rows.Count -ne 140) { throw "Expected 140 W0/W1 rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No W0/W1 row may remain unresolved after this final-disposition review.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every W0/W1 row needs the final audited implementation state.' }
if (@($rows | Where-Object {$_.source_path -like 'vdmutils/*' -and $_.final_disposition -ne 'tool-only'}).Count) { throw 'Every vdmutils row must be tool-only.' }
if (@($rows | Where-Object {$_.source_path -like 'inc/*' -and $_.final_disposition -ne 'binding-only'}).Count) { throw 'Every inc carrier must be binding-only.' }
if (@($rows | Where-Object {$_.source_path -in @('oemuni/file.c','oemuni/process.c') -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'The two oemuni library bodies must be adapter-backed.' }
if (@($rows | Where-Object {$_.source_path -eq 'suballoc/suballoc.c' -and $_.final_disposition -ne 'binding-only'}).Count) { throw 'suballoc.c must be binding-only.' }
Write-Output "T276 S25 W0/W1 final-disposition verification passed: $($rows.Count) rows."
