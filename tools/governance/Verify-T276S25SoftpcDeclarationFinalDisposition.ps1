[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {
    $_.source_path -like 'softpc.new/base/inc/*' -or
    $_.source_path -like 'softpc.new/host/inc/*' -or
    $_.source_path -in @('softpc.new/host/link/nt_uis.h', 'softpc.new/host/src/makefile.inc', 'softpc.new/obj.vdm/cdefine.inc')
})
if ($rows.Count -ne 339) { throw "Expected 339 selected SoftPC declaration rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.final_disposition -ne 'binding-only'}).Count) { throw 'Every selected SoftPC declaration carrier must be binding-only.' }
if (@($rows | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw 'Every SoftPC declaration row needs the final audited implementation state.' }
Write-Output "T276 S25 SoftPC declaration final-disposition verification passed: $($rows.Count) rows."
