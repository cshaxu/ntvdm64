[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -like 'xms.486/*'})
if ($rows.Count -ne 15) { throw "Expected 15 XMS rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No XMS row may remain unresolved after the final package audit.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every XMS row needs the final audited implementation state.' }
if (@($rows | Where-Object {$_.source_path -in @('xms.486/xms.c','xms.486/xmsa20.c','xms.486/xmsblock.c','xms.486/xmsdisp.c','xms.486/xmsmisc.c','xms.486/xmsumb.c') -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'Selected original XMS provider bodies must be adapter-backed.' }
if (@($rows | Where-Object {$_.source_path -eq 'xms.486/i386/xmsmem86.c' -and $_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'The direct-address xmsmem86 body must remain out of the final host graph.' }
Write-Output "T276 S25 XMS final-disposition verification passed: $($rows.Count) rows."
