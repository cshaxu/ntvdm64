[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$path = Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv'
$rows = @(Import-Csv $path -Delimiter "`t")
$resources = @($rows | Where-Object {$_.package_root -eq 'softpc.new' -and $_.file_kind -eq 'resource-or-other' -and $_.final_disposition -eq 'unresolved'})
if ($resources.Count -ne 25) { throw "Expected 25 unresolved SoftPC resource rows, found $($resources.Count)." }

foreach ($row in $resources) {
    $row.final_audit_state = 'final-disposition-audited; not implemented'
    if ($row.source_path -like 'softpc.new/base/dasm386/*' -or $row.source_path -like 'softpc.new/base/debug/*' -or $row.source_path -like 'softpc.new/debugger/*') {
        $row.final_disposition = 'tool-only'
        $row.final_owner_or_link_boundary = 'retained original SoftPC disassembly/debugger product build evidence; separately recoverable tool, never main host-runtime input'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none; a future adapter-mvdm-host-out/debugger requires separate whole-tool admission'
        $row.mapping_implication = 'no runtime mapping implication'
        $row.final_evidence = 'T276 S12 original SoftPC debugger/disassembler subpackage classification'
    } else {
        $row.final_disposition = 'not-host-runtime'
        $row.final_owner_or_link_boundary = 'retained original standalone SoftPC/NTVDM product build, resource or launch metadata; app owns the final product composition'
        $row.final_change_class = 'none; byte-exact source mirror retained'
        $row.named_adapter = 'none'
        $row.mapping_implication = 'no runtime mapping implication'
        $row.final_evidence = 'T276 S12 original SoftPC host/app build-control evidence; no final host link input'
    }
}

$rows | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content $path -Encoding utf8
$counts = $resources | Group-Object final_disposition | Sort-Object Name | ForEach-Object { "$($_.Name)=$($_.Count)" }
Write-Output "T276 S25 SoftPC resource final dispositions applied: $($counts -join ', ')."
