[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$machineBodies = @(
    'softpc.new/host/src/nt_cga.c', 'softpc.new/host/src/nt_ega.c',
    'softpc.new/host/src/nt_vga.c', 'softpc.new/host/src/nt_munge.c',
    'softpc.new/host/src/nt_graph.c', 'softpc.new/host/src/nt_det.c',
    'softpc.new/host/src/nt_fulsc.c', 'softpc.new/host/src/nt_rflop.c',
    'softpc.new/host/src/nt_sound.c', 'softpc.new/host/src/nt_timer.c'
)
$endpointBodies = @(
    'softpc.new/host/src/nt_hosts.c', 'softpc.new/host/src/nt_input.c',
    'softpc.new/host/src/nt_mouse.c', 'softpc.new/host/src/nt_com.c',
    'softpc.new/host/src/nt_wcom.c', 'softpc.new/host/src/nt_lpt.c',
    'softpc.new/host/src/nt_fdisk.c', 'softpc.new/host/src/nt_rez.c'
)
$targets = @($rows | Where-Object {$_.source_path -in ($machineBodies + $endpointBodies)})
if ($targets.Count -ne 18) { throw "Expected 18 SoftPC device source rows, found $($targets.Count)." }

foreach ($row in $targets) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -in $machineBodies) {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original SoftPC native device/executor evidence; Bochs is the sole final video, FDC/DMA, PIT/PIC or PPI/sound owner'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'no MVDM identity mapping is permitted into a parallel machine implementation'
        $row.final_evidence = 'T276 S12 device-bridge baseline: original device execution is a Bochs-replacement exclusion'
    } else {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'opennt-mvdm-host original host endpoint/control provider -> adapter-win32 or adapter-wow -> adapter-bochs typed device request; app supplies presentation policy'
        $row.final_change_class = 'binding-only; preserve original endpoint/control flow while excluding direct controller/object access'
        $row.named_adapter = 'adapter-win32; adapter-wow where reached; adapter-bochs; session host-resource mapping manager'
        $row.mapping_implication = 'host handles/resources remain opaque session host-resource identities; Bochs receives only typed endpoint/device operations'
        $row.final_evidence = 'T276 S12 device-bridge baseline: original host endpoint/provider body with named public capability and Bochs profile boundary'
    }
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $targets | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 SoftPC device source final dispositions applied: $($counts -join ', ')."
