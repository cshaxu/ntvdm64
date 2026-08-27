[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$vdd = @($rows | Where-Object {$_.source_path -like 'vdd/*'})
if ($vdd.Count -ne 47) { throw "Expected 47 VDD rows, found $($vdd.Count)." }
foreach ($row in $vdd) {
    $row.final_disposition = 'not-host-runtime'
    $row.final_owner_or_link_boundary = 'retained mvdm-host mirror evidence; no final host link input'
    $row.final_change_class = 'none; byte-exact source mirror retained'
    $row.named_adapter = 'adapter-vdd future product boundary only; sample is not promoted'
    $row.mapping_implication = 'not applicable to final product; any separately admitted VDD reuse must use session guest-memory and host-resource mapping-manager instances'
    $row.final_evidence = 'vdd root contains only samples/guest sample halves; original sample SOURCES and T276 S24 audit'
    $row.final_audit_state = 'final-disposition-audited; not implemented'
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
Write-Output 'T276 S24 VDD final disposition applied: 47 not-host-runtime sample rows.'
