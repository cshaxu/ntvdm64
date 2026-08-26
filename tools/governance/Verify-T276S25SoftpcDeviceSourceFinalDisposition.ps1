[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$path = Join-Path (Resolve-Path $RepositoryRoot).Path 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv'
$machine = @('softpc.new/host/src/nt_cga.c','softpc.new/host/src/nt_ega.c','softpc.new/host/src/nt_vga.c','softpc.new/host/src/nt_munge.c','softpc.new/host/src/nt_graph.c','softpc.new/host/src/nt_det.c','softpc.new/host/src/nt_fulsc.c','softpc.new/host/src/nt_rflop.c','softpc.new/host/src/nt_sound.c','softpc.new/host/src/nt_timer.c')
$endpoint = @('softpc.new/host/src/nt_hosts.c','softpc.new/host/src/nt_input.c','softpc.new/host/src/nt_mouse.c','softpc.new/host/src/nt_com.c','softpc.new/host/src/nt_wcom.c','softpc.new/host/src/nt_lpt.c','softpc.new/host/src/nt_fdisk.c','softpc.new/host/src/nt_rez.c')
$rows = @(Import-Csv $path -Delimiter "`t" | Where-Object {$_.source_path -in ($machine + $endpoint)})
if ($rows.Count -ne 18) { throw "Expected 18 SoftPC device source rows, found $($rows.Count)." }
if (@($rows | Where-Object {$_.source_path -in $machine -and $_.final_disposition -ne 'not-host-runtime'}).Count) { throw 'SoftPC native device executor bodies must remain outside the final host graph.' }
if (@($rows | Where-Object {$_.source_path -in $endpoint -and $_.final_disposition -ne 'adapter-backed'}).Count) { throw 'SoftPC host endpoint bodies must be adapter-backed.' }
Write-Output "T276 S25 SoftPC device source final-disposition verification passed: $($rows.Count) rows."
