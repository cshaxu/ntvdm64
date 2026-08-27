[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$sim = @($rows | Where-Object {$_.source_path -like 'sim16/*' -or $_.source_path -like 'sim32/*'})
if ($sim.Count -ne 13) { throw "Expected 13 SIM16/SIM32 rows, found $($sim.Count)." }
foreach ($row in $sim) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    switch ($row.source_path) {
        'sim32/sim32.h' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'adapter-mvdm-host-out/monitor same-shaped SIM32 public facade -> session guest-memory mapping manager -> adapter-mvdm-host-out/softpc -> adapter-bochs'
            $row.final_change_class = 'binding-only; retain original API declarations and names while replacing the private serial transport'
            $row.named_adapter = 'adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/softpc; adapter-bochs'
            $row.mapping_implication = 'Sim32 address forms resolve only through a bounded session guest-memory mapping-manager lease; no raw guest pointer escapes'
            $row.final_evidence = 'T276 S16: sim32.h is the original public monitor API form; SIM32 serial product shell is explicitly excluded'
        }
        'sim16/incs.inc' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'adapter-mvdm-host-out/wow declaration evidence for a separately admitted complete WOW package'
            $row.final_change_class = 'binding-only; retain original 16-bit declaration/segment shape outside a current host graph'
            $row.named_adapter = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/monitor; adapter-bochs'
            $row.mapping_implication = 'a future admitted caller uses session guest-memory leases and copied monitor records; these declarations expose no native identity'
            $row.final_evidence = 'T276 S15: original SIM16 declaration surface; only a complete WOW package may revive the peer protocol'
        }
        'sim16/segdefs.inc' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'adapter-mvdm-host-out/wow declaration evidence for a separately admitted complete WOW package'
            $row.final_change_class = 'binding-only; retain original 16-bit declaration/segment shape outside a current host graph'
            $row.named_adapter = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/monitor; adapter-bochs'
            $row.mapping_implication = 'a future admitted caller uses session guest-memory leases and copied monitor records; these declarations expose no native identity'
            $row.final_evidence = 'T276 S15: original SIM16 declaration surface; only a complete WOW package may revive the peer protocol'
        }
        'sim16/sim16.inc' {
            $row.final_disposition = 'binding-only'
            $row.final_owner_or_link_boundary = 'adapter-mvdm-host-out/wow declaration evidence for a separately admitted complete WOW package'
            $row.final_change_class = 'binding-only; retain original 16-bit declaration/segment shape outside a current host graph'
            $row.named_adapter = 'adapter-mvdm-host-out/wow; adapter-mvdm-host-out/monitor; adapter-bochs'
            $row.mapping_implication = 'a future admitted caller uses session guest-memory leases and copied monitor records; these declarations expose no native identity'
            $row.final_evidence = 'T276 S15: original SIM16 declaration surface; only a complete WOW package may revive the peer protocol'
        }
        'sim16/sim16.asm' {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained historical WOW serial/monitor product-shell evidence; public API form is recovered by adapter-mvdm-host-out/monitor or adapter-mvdm-host-out/wow, not by importing this body'
            $row.final_change_class = 'none; byte-exact source mirror retained outside the final host graph'
            $row.named_adapter = 'adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/wow; adapter-mvdm-host-out/softpc; adapter-bochs'
            $row.mapping_implication = 'private serial transport and raw pointer behavior are excluded; admitted facade calls use session guest-memory mapping-manager leases only'
            $row.final_evidence = 'T276 S15/S16: SIM16/SIM32 are historical WOW serial product shells, not machine owners or guest images'
        }
        'sim16/siminit.asm' {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained historical WOW serial/monitor product-shell evidence; public API form is recovered by adapter-mvdm-host-out/monitor or adapter-mvdm-host-out/wow, not by importing this body'
            $row.final_change_class = 'none; byte-exact source mirror retained outside the final host graph'
            $row.named_adapter = 'adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/wow; adapter-mvdm-host-out/softpc; adapter-bochs'
            $row.mapping_implication = 'private serial transport and raw pointer behavior are excluded; admitted facade calls use session guest-memory mapping-manager leases only'
            $row.final_evidence = 'T276 S15/S16: SIM16/SIM32 are historical WOW serial product shells, not machine owners or guest images'
        }
        'sim32/sim32.c' {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained historical WOW serial/monitor product-shell evidence; public API form is recovered by adapter-mvdm-host-out/monitor or adapter-mvdm-host-out/wow, not by importing this body'
            $row.final_change_class = 'none; byte-exact source mirror retained outside the final host graph'
            $row.named_adapter = 'adapter-mvdm-host-out/monitor; adapter-mvdm-host-out/wow; adapter-mvdm-host-out/softpc; adapter-bochs'
            $row.mapping_implication = 'private serial transport and raw pointer behavior are excluded; admitted facade calls use session guest-memory mapping-manager leases only'
            $row.final_evidence = 'T276 S15/S16: SIM16/SIM32 are historical WOW serial product shells, not machine owners or guest images'
        }
        default {
            $row.final_disposition = 'not-host-runtime'
            $row.final_owner_or_link_boundary = 'retained original 16-bit/SIM build-product evidence; no final host link input'
            $row.final_change_class = 'none; byte-exact source mirror retained'
            $row.named_adapter = 'none'
            $row.mapping_implication = 'not applicable to the final host runtime'
            $row.final_evidence = 'T276 S15/S16: original build/export/macro evidence only'
        }
    }
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $sim | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 SIM final dispositions applied: $($counts -join ', ')."
