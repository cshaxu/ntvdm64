[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'dpmi/*'})
if ($rows.Count -ne 59) { throw "Expected 59 DPMI rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -ne 'guest-only'}).Count) { throw 'Every DPMI path must remain guest-only.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every DPMI row needs the final audited implementation state.' }
Write-Output 'T276 S25 DPMI final-disposition verification passed: 59 guest-only rows.'
