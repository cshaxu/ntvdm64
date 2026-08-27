[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'vdd/*'})
if ($rows.Count -ne 47) { throw "Expected 47 VDD final rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'Every VDD sample row must remain not-host-runtime.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every VDD sample row needs final audited implementation state.' }
if (@($rows | Where-Object {$_.named_adapter -notmatch 'adapter-mvdm-host-out/vdd'}).Count) { throw 'Every VDD sample row must record the future adapter-mvdm-host-out/vdd boundary.' }
Write-Output 'T276 S24 VDD final-disposition verification passed: 47 sample rows are not-host-runtime.'
