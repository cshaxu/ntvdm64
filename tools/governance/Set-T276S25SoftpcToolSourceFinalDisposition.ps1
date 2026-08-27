[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$toolSources = @($rows | Where-Object {
    $_.final_disposition -eq 'unresolved' -and $_.file_kind -eq 'source' -and
    ($_.source_path -like 'softpc.new/base/dasm386/*' -or $_.source_path -like 'softpc.new/base/debug/*' -or $_.source_path -eq 'softpc.new/debugger/vdmdebug.c')
})
$entrySources = @($rows | Where-Object {
    $_.final_disposition -eq 'unresolved' -and $_.file_kind -eq 'source' -and
    $_.source_path -in @('softpc.new/obj.vdm/c_regs.c', 'softpc.new/obj.vdm/ntvdm.c')
})
if ($toolSources.Count -ne 12) { throw "Expected 12 SoftPC debugger/disassembler sources, found $($toolSources.Count)." }
if ($entrySources.Count -ne 2) { throw "Expected 2 historical product-entry sources, found $($entrySources.Count)." }

foreach ($row in $toolSources) {
    $row.final_disposition = 'tool-only'
    $row.final_owner_or_link_boundary = 'original SoftPC debugger/disassembler tool source; separately recoverable only under a future whole-tool admission'
    $row.final_change_class = 'none; byte-exact source mirror retained'
    $row.named_adapter = 'none; future adapter-mvdm-host-out/debugger only after a complete tool boundary is admitted'
    $row.mapping_implication = 'no main-runtime mapping implication'
    $row.final_evidence = 'T276 S12 original SoftPC debugger/disassembler subpackage classification'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
foreach ($row in $entrySources) {
    $row.final_disposition = 'not-host-runtime'
    $row.final_owner_or_link_boundary = 'retained historical standalone NTVDM product-entry evidence; app owns final product composition'
    $row.final_change_class = 'none; byte-exact source mirror retained'
    $row.named_adapter = 'none'
    $row.mapping_implication = 'no main-runtime mapping implication'
    $row.final_evidence = 'T276 S12 original host-product composition classification; final app replaces standalone entry scaffolding'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output "T276 S25 SoftPC tool/product source final dispositions applied: tool-only=$($toolSources.Count), not-host-runtime=$($entrySources.Count)."
