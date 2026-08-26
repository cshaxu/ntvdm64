[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$overlay = @(
    'softpc.new/host/src/nt_cpu.c', 'softpc.new/host/src/nt_mem.c',
    'softpc.new/host/src/nt_emm.c', 'softpc.new/host/src/x86_emm.c',
    'softpc.new/host/src/nt_umb.c', 'softpc.new/host/src/sim32.c'
)
$excluded = @(
    'softpc.new/host/src/cpucstbs.c', 'softpc.new/host/src/nt_aorc.c',
    'softpc.new/host/src/nt_cprgs.c', 'softpc.new/host/src/nt_hfx.c',
    'softpc.new/host/src/nt_hunt.c', 'softpc.new/host/src/nt_vflop.c',
    'softpc.new/host/src/stubs.c'
)
$allControl = @(
    'softpc.new/host/src/config.c', 'softpc.new/host/src/copy_fnc.c',
    'softpc.new/host/src/cpucstbs.c', 'softpc.new/host/src/fprt.c',
    'softpc.new/host/src/nt_aorc.c', 'softpc.new/host/src/nt_bop.c',
    'softpc.new/host/src/nt_cprgs.c', 'softpc.new/host/src/nt_cpu.c',
    'softpc.new/host/src/nt_emm.c', 'softpc.new/host/src/nt_eoi.c',
    'softpc.new/host/src/nt_error.c', 'softpc.new/host/src/nt_ertbl.c',
    'softpc.new/host/src/nt_event.c', 'softpc.new/host/src/nt_hfx.c',
    'softpc.new/host/src/nt_hunt.c', 'softpc.new/host/src/nt_inthk.c',
    'softpc.new/host/src/nt_keycd.c', 'softpc.new/host/src/nt_mem.c',
    'softpc.new/host/src/nt_mess.c', 'softpc.new/host/src/nt_msscs.c',
    'softpc.new/host/src/nt_nls.c', 'softpc.new/host/src/nt_ntfun.c',
    'softpc.new/host/src/nt_pif.c', 'softpc.new/host/src/nt_reset.c',
    'softpc.new/host/src/nt_sas.c', 'softpc.new/host/src/nt_sec.c',
    'softpc.new/host/src/nt_smenu.c', 'softpc.new/host/src/nt_term.c',
    'softpc.new/host/src/nt_thred.c', 'softpc.new/host/src/nt_umb.c',
    'softpc.new/host/src/nt_unix.c', 'softpc.new/host/src/nt_vdd.c',
    'softpc.new/host/src/nt_vflop.c', 'softpc.new/host/src/nt_yoda.c',
    'softpc.new/host/src/sim32.c', 'softpc.new/host/src/stf_conf.c',
    'softpc.new/host/src/stubs.c', 'softpc.new/host/src/x86_emm.c'
)
$targets = @($rows | Where-Object {$_.source_path -in $allControl})
if ($targets.Count -ne 38) { throw "Expected 38 selected SoftPC control source rows, found $($targets.Count)." }

foreach ($row in $targets) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -in $overlay) {
        $row.final_disposition = 'overlay-required'
        $row.final_owner_or_link_boundary = 'original SoftPC control algorithm with a narrow same-shaped adapter-softpc/adapter-bochs overlay; Bochs remains sole CPU/RAM/physical-page owner'
        $row.final_change_class = 'overlay-required; replace only direct executor, native-pointer or private-section transport with typed adapter operations'
        $row.named_adapter = 'adapter-softpc; adapter-bochs; session guest-memory and host-resource mapping managers; adapter-vdm-monitor where reached'
        $row.mapping_implication = 'all historical 32-bit pointer-shaped values resolve via checked session mappings and typed machine requests; raw host addresses cannot enter the mirror body'
        $row.final_evidence = 'T276 S12 SoftPC/SAS facade baseline: original useful control algorithm mixed with non-reusable executor or direct-address transport'
    }
    elseif ($row.source_path -in $excluded) {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original alternate CPU, historical profile, virtual-FDC or debugger/product-shell evidence; no final host link input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the final host runtime'
        $row.final_evidence = 'T276 S12 source-span review: no final profile may reintroduce this second executor or unsupported historical product shell'
    }
    else {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'opennt-mvdm-host original monitor, BOP, session, host-capability, configuration, VDD or error provider -> named adapters and session; Bochs receives only typed mechanical requests'
        $row.final_change_class = 'binding-only; preserve original control flow, ordering, failure routes and data shapes'
        $row.named_adapter = 'adapter-bop; adapter-softpc; adapter-bochs; adapter-win32; adapter-vdm-monitor; adapter-vdd; session as reached'
        $row.mapping_implication = 'guest data uses only session guest-memory mapping; callbacks/resources use the session completion or host-resource instance, never native identity in MVDM fields'
        $row.final_evidence = 'T276 S12 monitor/session, Win32 capability and BOP/VDD/debugger source-span owner baselines'
    }
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $targets | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 SoftPC control source final dispositions applied: $($counts -join ', ')."
