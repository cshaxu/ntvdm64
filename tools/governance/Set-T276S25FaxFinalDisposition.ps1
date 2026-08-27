[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$fax = @($rows | Where-Object {$_.source_path -like 'fax/*'})
if ($fax.Count -ne 21) { throw "Expected 21 fax rows, found $($fax.Count)." }
$uiBodies = @('fax/wowfaxui/wfhelper.c', 'fax/wowfaxui/wfsheets.c', 'fax/wowfaxui/wfupgrad.c', 'fax/wowfaxui/wowfaxui.c')
$declarations = @('fax/faxcomm.h', 'fax/wowfax/wowfaxdd.h', 'fax/wowfaxui/wfsheets.h', 'fax/wowfaxui/wowfaxui.h')

foreach ($row in $fax) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -in $uiBodies) {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'mvdm-host original WOWFAXUI provider -> adapter-wow -> adapter-win32 and session copied broker records'
        $row.final_change_class = 'binding-only; preserve original UI/provider flow while replacing private shared-section transport'
        $row.named_adapter = 'adapter-wow; adapter-win32; session host-resource mapping manager'
        $row.mapping_implication = 'historical 32-bit shared-section offsets are fixed-width copied broker fields; PVOID-to-DWORD identity conversion is prohibited'
        $row.final_evidence = 'T276 S23 FAX package baseline: original user-mode WOWFAXUI provider body'
    }
    elseif ($row.source_path -in $declarations) {
        $row.final_disposition = 'binding-only'
        $row.final_owner_or_link_boundary = 'mvdm-platform-abi original WOWFAX declaration surface supplied to the selected UI provider and adapter-wow binding'
        $row.final_change_class = 'binding-only; preserve declaration and offset field shape'
        $row.named_adapter = 'adapter-wow; adapter-win32; session'
        $row.mapping_implication = 'shared data is copied through session-owned fixed-width records, never direct native pointer identity'
        $row.final_evidence = 'T276 S23 FAX declaration prerequisite'
    }
    else {
        $row.final_disposition = 'not-host-runtime'
        if ($row.source_path -eq 'fax/wowfax/wowfax.c') {
            $row.final_owner_or_link_boundary = 'retained original private CSRSS/GDI-driver evidence; no final user-mode host link input'
            $row.final_evidence = 'T276 S23 FAX package baseline: graphics-engine driver cannot be silently replaced by a user-mode printer helper'
        } else {
            $row.final_owner_or_link_boundary = 'retained original FAX driver/product build or resource evidence; no final host link input'
            $row.final_evidence = 'T276 S23 FAX package baseline: product/build/resource evidence only'
        }
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the final host runtime'
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $fax | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 FAX final dispositions applied: $($counts -join ', ')."
