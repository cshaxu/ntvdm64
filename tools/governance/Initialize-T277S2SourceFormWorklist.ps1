[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
function Read-Tsv([string]$Name) { @(Import-Csv -LiteralPath (Join-Path $ops $Name) -Delimiter "`t") }
function Write-Tsv([object[]]$Rows, [string]$Name) { @($Rows) | Export-Csv -LiteralPath (Join-Path $ops $Name) -Delimiter "`t" -NoTypeInformation -Encoding utf8 }

$files = Read-Tsv 'm0-t277-file-recovery-ledger.tsv'
$interfaces = Read-Tsv 'm0-t277-interface-recovery-ledger.tsv'
$fileById = @{}
foreach ($file in $files) { $fileById[$file.file_id] = $file }
$eligible = @('adapter-backed', 'binding-only', 'overlay-required')
$order = @{ 'suballoc' = 10; 'xms.486' = 20; 'dpmi32' = 30; 'dos' = 40; 'softpc.new' = 50; 'vdmredir' = 60; 'wow32' = 70 }

$correlations = @($interfaces | Where-Object { $_.row_kind -eq 'call-declaration-correlation' })
$items = New-Object System.Collections.Generic.List[object]
foreach ($row in $correlations) {
    $file = $fileById[$row.caller_file_id]
    if ($null -eq $file) { throw "Missing caller file: $($row.caller_file_id)" }
    if ($file.final_disposition -notin $eligible) { continue }
    $priority = if ($order.ContainsKey($row.caller_package)) { $order[$row.caller_package] } else { 999 }
    $items.Add([pscustomobject]@{
        work_item_id = 'MVDM-T277-S2-' + $row.interface_id
        review_order = $priority
        caller_file_id = $row.caller_file_id
        caller_path = $row.caller_path
        caller_package = $row.caller_package
        caller_line = $row.source_line
        caller_final_disposition = $file.final_disposition
        symbol = $row.symbol
        declaration_path = $row.declaration_path
        declaration_package = $row.declaration_package
        correlation = $row.correlation
        source_form_state = 'pending-original-caller-and-provider-form-review'
        abi_owner_state = 'unassigned-pending-source-form-review'
        failure_contract_state = 'unreviewed'
        evidence = $row.evidence
    })
}
$items = @($items | Sort-Object review_order, caller_package, caller_path, caller_line, symbol)
Write-Tsv $items 'm0-t277-s2-source-form-worklist.tsv'

$summary = @(
    [pscustomobject]@{ summary_id = 'MVDM-T277-S2-ALL'; population = 'all caller/declaration correlations'; count = $correlations.Count; state = 'preserved T277 S1 population'; evidence = 'm0-t277-interface-recovery-ledger.tsv' },
    [pscustomobject]@{ summary_id = 'MVDM-T277-S2-WORKLIST'; population = 'future-host caller correlations'; count = $items.Count; state = 'source-form review worklist'; evidence = 'm0-t277-s2-source-form-worklist.tsv' }
)
foreach ($group in $items | Group-Object caller_package | Sort-Object Name) {
    $summary += [pscustomobject]@{ summary_id = 'MVDM-T277-S2-PACKAGE-' + $group.Name.ToUpperInvariant().Replace('.', '-'); population = 'future-host caller package: ' + $group.Name; count = $group.Count; state = 'source-form review worklist'; evidence = 'm0-t277-s2-source-form-worklist.tsv' }
}
foreach ($group in $correlations | Group-Object reachability_state | Sort-Object Name) {
    $summary += [pscustomobject]@{ summary_id = 'MVDM-T277-S2-STATE-' + $group.Name.ToUpperInvariant().Replace('-', '_'); population = 'all correlations state: ' + $group.Name; count = $group.Count; state = 'preserved evidence'; evidence = 'm0-t277-interface-recovery-ledger.tsv' }
}
Write-Tsv $summary 'm0-t277-s2-source-form-summary.tsv'
Write-Host "Wrote $($items.Count) S2 work items and $($summary.Count) summary rows."
