[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$support = @($rows | Where-Object {$_.source_path -like 'softpc.new/base/support/*.c'})
if ($support.Count -ne 6) { throw "Expected 6 SoftPC support source rows, found $($support.Count)." }

foreach ($row in $support) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -eq 'softpc.new/base/support/get_env.c') {
        $row.final_disposition = 'adapter-backed'
        $row.final_owner_or_link_boundary = 'original SoftPC configuration-environment helper -> adapter-win32 capability binding; app/session own final launch inputs'
        $row.final_change_class = 'binding-only; preserve original typed environment lookup algorithm and default/error flow'
        $row.named_adapter = 'adapter-win32; session'
        $row.mapping_implication = 'no guest or host identity mapping is exposed'
        $row.final_evidence = 'T276 S12 base/support source review: isolated configuration helper, not a machine executor'
    } else {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original alternate SoftPC machine/product support evidence; Bochs and app own the corresponding final mechanics/composition'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'no runtime mapping implication'
        $row.final_evidence = 'T276 S12 base/support source review: alternate CPU, IO, BIOS timer or standalone product machinery would create a second machine executor'
    }
}
$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $support | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 SoftPC support source final dispositions applied: $($counts -join ', ')."
