[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$path = Join-Path $root 'docs/etc/operations/m0-t276-s12-device-bridge-interface-ledger.tsv'
if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 device-bridge interface ledger: $path" }

$rows = @(Import-Csv -LiteralPath $path -Delimiter "`t")
if ($rows.Count -ne 18) { throw "Expected 18 device-bridge rows, found $($rows.Count)." }
if (($rows | Group-Object interface_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate device-bridge interface identity.' }
if (($rows | Where-Object state -ne 'reviewed-provisional-not-enabled').Count -ne 0) { throw 'Device-bridge audit must not enable a source or adapter.' }
foreach ($source in @('nt_cga.c','nt_com.c','nt_det.c','nt_ega.c','nt_fdisk.c','nt_fulsc.c','nt_graph.c','nt_hosts.c','nt_input.c','nt_lpt.c','nt_mouse.c','nt_munge.c','nt_rez.c','nt_rflop.c','nt_sound.c','nt_timer.c','nt_vga.c','nt_wcom.c')) {
    if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source").Count -ne 1) { throw "Missing or duplicate audited source: $source" }
}
foreach ($source in @('nt_cga.c','nt_ega.c','nt_vga.c','nt_munge.c','nt_rflop.c','nt_timer.c')) {
    if (@($rows | Where-Object source_path -eq "softpc.new/host/src/$source" | Where-Object provisional_disposition -notmatch 'Bochs-replacement').Count -ne 0) { throw "$source must retain an explicit Bochs-replacement exclusion." }
}
foreach ($row in $rows) {
    foreach ($column in 'source_span','device_or_interface_family','original_contract','historical_dependency','single_recovery_owner','provisional_disposition','predecessor_gate','evidence') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.interface_id)." }
    }
}
Write-Output "T276 S12 device-bridge interface verification passed: $($rows.Count) rows."
