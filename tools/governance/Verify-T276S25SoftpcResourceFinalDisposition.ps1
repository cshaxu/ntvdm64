[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.package_root -eq 'softpc.new' -and $_.file_kind -eq 'resource-or-other' -and $_.final_evidence -like 'T276 S12 original SoftPC *'})
if ($rows.Count -ne 25) { throw "Expected 25 audited SoftPC resource rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No selected SoftPC resource row may remain unresolved.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every audited SoftPC resource row needs the final audited implementation state.' }
Write-Output "T276 S25 SoftPC resource final-disposition verification passed: $($rows.Count) rows."
