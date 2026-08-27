[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$headers = @($rows | Where-Object {
    $_.source_path -like 'softpc.new/base/inc/*' -or
    $_.source_path -like 'softpc.new/host/inc/*' -or
    $_.source_path -in @('softpc.new/host/link/nt_uis.h', 'softpc.new/host/src/makefile.inc', 'softpc.new/obj.vdm/cdefine.inc')
})
if ($headers.Count -ne 339) { throw "Expected 339 selected SoftPC declaration rows, found $($headers.Count)." }

foreach ($row in $headers) {
    $row.final_disposition = 'binding-only'
    $row.final_owner_or_link_boundary = 'original SoftPC declaration surface retained for same-shaped adapter-mvdm-host-out/softpc or adapter-bochs binding; never a direct MVDM-to-Bochs object dependency'
    $row.final_change_class = 'binding-only; preserve original names, parameter layouts, constants and calling conventions'
    $row.named_adapter = 'adapter-mvdm-host-out/softpc; adapter-bochs where a machine facade is reached'
    $row.mapping_implication = 'pointer-shaped MVDM fields bind only through the session mapping-manager rules; declaration inclusion alone does not establish an identity mapping'
    $row.final_evidence = 'T276 S12 SoftPC original-subpackage and facade-interface baselines: declaration carrier, not a parallel machine executor'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output "T276 S25 SoftPC declaration final dispositions applied: binding-only=$($headers.Count)."
