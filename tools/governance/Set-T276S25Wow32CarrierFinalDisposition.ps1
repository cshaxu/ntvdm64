[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$headers = @($rows | Where-Object {$_.package_root -eq 'wow32' -and $_.file_kind -eq 'declaration'})
$excluded = @($rows | Where-Object {$_.package_root -eq 'wow32' -and $_.final_disposition -eq 'unresolved' -and ($_.file_kind -eq 'resource-or-other' -or $_.source_path -in @('wow32/debug.c','wow32/i386/callpr32.asm','wow32/i386/fastwow.asm','wow32/wctlm32.c','wow32/wcurcash.c','wow32/wddeml32.c','wow32/wkglobal.c','wow32/wklocal.c'))})
if ($headers.Count -ne 105) { throw "Expected 105 WOW32 declaration rows, found $($headers.Count)." }
if ($excluded.Count -ne 19) { throw "Expected 19 WOW32 unselected/build/resource rows, found $($excluded.Count)." }
foreach ($row in $headers) {
    $row.final_disposition = 'binding-only'
    $row.final_owner_or_link_boundary = 'original WOW32 declaration surface retained for same-shaped adapter-wow and adapter-win32 bindings'
    $row.final_change_class = 'binding-only; preserve declaration, layout and calling-convention shape'
    $row.named_adapter = 'adapter-wow; adapter-win32; adapter-softpc; adapter-vdm-monitor; session where reached'
    $row.mapping_implication = 'pointer and handle fields bind through session mapping-manager instances; no native identity is represented by a 32-bit WOW value'
    $row.final_evidence = 'T276 S22 WOW32 package baseline: declaration carrier'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
foreach ($row in $excluded) {
    $row.final_disposition = 'not-host-runtime'
    $row.final_owner_or_link_boundary = 'retained original unselected WOW32 source or build/resource evidence; no final host link input'
    $row.final_change_class = 'none; byte-exact source mirror retained'
    $row.named_adapter = 'none'
    $row.mapping_implication = 'not applicable to the final host runtime'
    $row.final_evidence = 'T276 S22 WOW32 condition-aware build selection: unselected source or build/resource evidence only'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output "T276 S25 WOW32 carrier final dispositions applied: binding-only=$($headers.Count), not-host-runtime=$($excluded.Count)."
