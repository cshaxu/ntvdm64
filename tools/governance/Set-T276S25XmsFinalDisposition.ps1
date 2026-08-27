[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$xms = @($rows | Where-Object {$_.source_path -like 'xms.486/*'})
if ($xms.Count -ne 15) { throw "Expected 15 XMS rows, found $($xms.Count)." }

foreach ($row in $xms) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    switch ($row.source_path) {
        'xms.486/xms.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-host original XMS initialization/range provider -> adapter-softpc -> adapter-bochs; session owns instance state'
            $row.final_change_class = 'binding-only; preserve original suballocator and initialization sequence'
            $row.named_adapter = 'adapter-softpc; adapter-bochs; session guest-memory mapping manager'
            $row.mapping_implication = 'extended memory is a bounded Bochs guest range; no host pointer may enter the original provider'
            $row.final_evidence = 'T276 S14 XMS-IF-03, XMS-IF-06 and XMS-IF-08; xms.486/sources selects xms.c'
        }
        'xms.486/xmsa20.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-host original A20 service -> adapter-softpc -> adapter-bochs'
            $row.final_change_class = 'binding-only; preserve original AX/BL result control flow'
            $row.named_adapter = 'adapter-softpc; adapter-bochs'
            $row.mapping_implication = 'no identity mapping; typed A20 state is requested only through the machine boundary'
            $row.final_evidence = 'T276 S14 XMS-IF-02; xms.486/sources selects xmsa20.c'
        }
        'xms.486/xmsblock.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-host original XMS block provider -> adapter-softpc -> adapter-bochs; suballoc remains original support'
            $row.final_change_class = 'binding-only; preserve allocation/free/move/query algorithm'
            $row.named_adapter = 'adapter-softpc; adapter-bochs; session guest-memory mapping manager'
            $row.mapping_implication = 'all guest range transfer occurs through checked session guest-memory leases and typed machine transactions'
            $row.final_evidence = 'T276 S14 XMS-IF-03 and XMS-IF-06; xms.486/sources selects xmsblock.c'
        }
        'xms.486/xmsdisp.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-host original XMS dispatch -> adapter-mvdm-host-in ingress/completion and adapter-softpc machine facade'
            $row.final_change_class = 'binding-only; preserve selector meanings and unsupported-service failure route'
            $row.named_adapter = 'adapter-mvdm-host-in; adapter-softpc'
            $row.mapping_implication = 'BOP state crosses only fixed-width adapter frames; no native identity is exposed'
            $row.final_evidence = 'T276 S14 XMS-IF-01 and XMS-IF-04; xms.486/sources selects xmsdisp.c'
        }
        'xms.486/xmsmisc.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-host original system information/INT 15 provider -> adapter-softpc -> adapter-bochs'
            $row.final_change_class = 'binding-only; preserve original system-page and interrupt-hook control flow'
            $row.named_adapter = 'adapter-softpc; adapter-bochs; session'
            $row.mapping_implication = 'IVT and interrupt effects use typed machine operations; no guest address becomes a host pointer'
            $row.final_evidence = 'T276 S14 XMS-IF-05; xms.486/sources selects xmsmisc.c'
        }
        'xms.486/xmsumb.c' {
            $row.final_disposition = 'adapter-backed'
            $row.final_owner_or_link_boundary = 'mvdm-host original UMB provider -> adapter-softpc -> adapter-bochs; session owns the durable bounded UMB range contract'
            $row.final_change_class = 'binding-only; preserve original UMB list and B0/B1/B2 result flow'
            $row.named_adapter = 'adapter-softpc; adapter-bochs; session guest-memory mapping manager'
            $row.mapping_implication = 'durable UMB bytes resolve through the session guest-memory instance and typed machine range operations only'
            $row.final_evidence = 'T276 S14 XMS-IF-04; xms.486/sources selects xmsumb.c; enablement waits for the UMB machine seam'
        }
        'xms.486/xms.h' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'mvdm-platform-abi original XMS declaration surface supplied to the selected XMS provider and adapters'
            $row.final_change_class = 'binding-only; preserve exact data and external declaration shape'
            $row.named_adapter = 'adapter-mvdm-host-in; adapter-softpc; session'
            $row.mapping_implication = 'declarations do not carry native identity; reached fields follow the session mapping-manager rule'
            $row.final_evidence = 'T276 S14 XMS-IF-01 and XMS-IF-08'
        }
        'xms.486/i386/xmsmem86.c' {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained original x86 direct-address backend evidence; same-shaped contracts are supplied by adapter-softpc -> adapter-bochs'
            $row.final_change_class = 'none; direct-host-pointer body remains byte-exact evidence outside the final host graph'
            $row.named_adapter = 'adapter-softpc; adapter-bochs; session guest-memory mapping manager'
            $row.mapping_implication = 'historical ULONG values resolve through the session guest-memory instance; a native pointer cannot leave its checked synchronous lease'
            $row.final_evidence = 'T276 S14 XMS-IF-07: direct host-pointer casts are invalid on x64 and violate the bounded Bochs guest-RAM boundary'
        }
        'xms.486/xmsmemr.c' {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained non-x86 historical memory-backend evidence; no final Win32/x86 or Win32/x64 host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the final supported host profile'
            $row.final_evidence = 'T276 S14: xmsmemr.c is not selected by the original x86 profile'
        }
        default {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained original build-description evidence; no final host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the host runtime'
            $row.final_evidence = 'T276 S14: original XMS build-selection evidence only'
        }
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $xms | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 XMS final dispositions applied: $($counts -join ', ')."
