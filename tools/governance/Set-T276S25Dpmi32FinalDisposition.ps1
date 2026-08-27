[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$dpmi = @($rows | Where-Object {$_.source_path -like 'dpmi32/*'})
if ($dpmi.Count -ne 25) { throw "Expected 25 dpmi32 rows, found $($dpmi.Count)." }

$x86Bodies = @(
    'dpmi32/buffer.c', 'dpmi32/data.c', 'dpmi32/debug.c', 'dpmi32/dpmi32.c',
    'dpmi32/dpmiint.c', 'dpmi32/dpmiselr.c', 'dpmi32/i386/dpmi386.c',
    'dpmi32/i386/dpmimem.c', 'dpmi32/i386/dpmimisc.c', 'dpmi32/int21map.c',
    'dpmi32/modesw.c', 'dpmi32/register.c', 'dpmi32/savestat.c',
    'dpmi32/stack.c', 'dpmi32/vxd.c', 'dpmi32/xmem.c'
)
$declarations = @(
    'dpmi32/dpmi32p.h', 'dpmi32/dpmidata.h', 'dpmi32/dpmiint.h',
    'dpmi32/precomp.h', 'dpmi32/xlathlp.h'
)

foreach ($row in $dpmi) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -in $x86Bodies) {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'mvdm-host original DPMI32 provider -> adapter-mvdm-host-out/softpc -> adapter-bochs; adapter-mvdm-host-out/monitor owns only monitor callback binding'
        $row.final_change_class = 'binding-only; preserve original DPMI selector, interrupt, mode-switch, LDT and VDM_TIB control flow'
        $row.named_adapter = 'adapter-mvdm-host-out/softpc; adapter-bochs; adapter-mvdm-host-out/monitor; session guest-memory mapping manager'
        $row.mapping_implication = 'Sim32GetVDMPointer-style ranges resolve only through synchronous bounded guest-memory leases or copies; no native host pointer may enter DPMI fields'
        $row.final_evidence = 'T276 S20 DPMI32 package baseline: selected original x86 monitor provider body'
    }
    elseif ($row.source_path -in $declarations) {
        $row.final_disposition = 'binding-only'
        $row.final_owner_or_link_boundary = 'mvdm-platform-abi original DPMI32 declaration surface supplied unchanged to the selected provider and same-shaped adapters'
        $row.final_change_class = 'binding-only; preserve original declaration, layout and calling-convention shape'
        $row.named_adapter = 'adapter-mvdm-host-out/softpc; adapter-mvdm-host-out/monitor; session'
        $row.mapping_implication = 'reached guest addresses use the session guest-memory instance; declarations never expose native identities'
        $row.final_evidence = 'T276 S20 DPMI32 declaration/facade prerequisite'
    }
    elseif ($row.source_path -in @('dpmi32/dpmimemr.c', 'dpmi32/dpmimscr.c')) {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original non-x86 DPMI32 backend evidence; no final Win32 x86/x64 host link input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the supported x86/x64 host profiles'
        $row.final_evidence = 'T276 S20 DPMI32 package baseline: RISC-only provider body'
    }
    else {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original DPMI32 build-description evidence; no final host link input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'not applicable to the host runtime'
        $row.final_evidence = 'T276 S20 DPMI32 build-selection evidence only'
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $dpmi | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 DPMI32 final dispositions applied: $($counts -join ', ')."
