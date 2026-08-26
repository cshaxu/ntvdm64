[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {
    $_.final_evidence -like 'T276 S12 SoftPC/SAS facade baseline*' -or
    $_.final_evidence -like 'T276 S12 source-span review*' -or
    $_.final_evidence -eq 'T276 S12 monitor/session, Win32 capability and BOP/VDD/debugger source-span owner baselines'
})
if ($rows.Count -ne 38) { throw "Expected 38 audited SoftPC control rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -eq 'unresolved'}).Count) { throw 'No selected SoftPC control source may remain unresolved.' }
$overlay = @('softpc.new/host/src/nt_cpu.c','softpc.new/host/src/nt_mem.c','softpc.new/host/src/nt_emm.c','softpc.new/host/src/x86_emm.c','softpc.new/host/src/nt_umb.c','softpc.new/host/src/sim32.c')
if (@($rows | Where-Object {$_.source_path -in $overlay -and $_.final_disposition -ne 'overlay-required'}).Count) { throw 'Mixed original SoftPC facade bodies require overlay disposition.' }
Write-Output "T276 S25 SoftPC control-source final-disposition verification passed: $($rows.Count) rows."
