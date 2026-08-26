[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$dpmi = @($rows | Where-Object {$_.source_path -like 'dpmi/*'})
if ($dpmi.Count -ne 59) { throw "Expected 59 DPMI rows, found $($dpmi.Count)." }
foreach ($row in $dpmi) {
    $row.final_disposition = 'guest-only'
    $row.final_owner_or_link_boundary = 'opennt-guest-dos DOSX/DPMI guest product; loaded immutable guest bytes, never a host link input'
    $row.final_change_class = 'none; byte-exact guest source, build input or artifact retained'
    $row.named_adapter = 'adapter-bop; adapter-softpc; adapter-vdm-monitor; adapter-bochs at the historical guest boundary only'
    $row.mapping_implication = 'guest addresses remain guest numeric values; any host-facing boundary uses fixed-width adapter frames and the session guest-memory mapping-manager lease'
    $row.final_evidence = 'T276 S19: dpmi/makefile builds 16-bit 486/dosx.exe as one DPMI guest provider product, not a host static library'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output 'T276 S25 DPMI final dispositions applied: 59 guest-only rows.'
