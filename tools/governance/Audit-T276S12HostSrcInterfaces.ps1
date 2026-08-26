[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$hostPath = Join-Path $operations 'm0-t276-s12-host-src-disposition-ledger.tsv'
$callPath = Join-Path $operations 'm0-t276-s4-textual-call-candidate-ledger.tsv'
$boundaryPath = Join-Path $operations 'm0-t276-s5-unique-call-disposition-ledger.tsv'
foreach ($path in @($hostPath, $callPath, $boundaryPath)) { if (-not (Test-Path -LiteralPath $path)) { throw "Missing T276 input: $path" } }

$hostRows = @(Import-Csv -LiteralPath $hostPath -Delimiter "`t" | Sort-Object source_path)
$calls = @(Import-Csv -LiteralPath $callPath -Delimiter "`t")
$boundaries = @(Import-Csv -LiteralPath $boundaryPath -Delimiter "`t")
if ($hostRows.Count -ne 56) { throw "Expected 56 host/src rows, found $($hostRows.Count)." }

$callsByPath = @{}
foreach ($call in $calls) {
    if (-not $callsByPath.ContainsKey($call.caller_path)) { $callsByPath[$call.caller_path] = [System.Collections.Generic.List[object]]::new() }
    $callsByPath[$call.caller_path].Add($call)
}
$boundariesByPath = @{}
foreach ($boundary in $boundaries) {
    if (-not $boundariesByPath.ContainsKey($boundary.caller_path)) { $boundariesByPath[$boundary.caller_path] = [System.Collections.Generic.List[object]]::new() }
    $boundariesByPath[$boundary.caller_path].Add($boundary)
}

$out = foreach ($sourceRow in $hostRows) {
    $pathCalls = if ($callsByPath.ContainsKey($sourceRow.source_path)) { @($callsByPath[$sourceRow.source_path]) } else { @() }
    $pathBoundaries = if ($boundariesByPath.ContainsKey($sourceRow.source_path)) { @($boundariesByPath[$sourceRow.source_path]) } else { @() }
    [pscustomobject][ordered]@{
        file_id = $sourceRow.file_id
        source_path = $sourceRow.source_path
        original_role_group = $sourceRow.original_role_group
        lexical_call_count = $pathCalls.Count
        lexical_unique_declaration_count = @($pathCalls | Where-Object correlation -eq 'unique-declaration-candidate').Count
        lexical_ambiguous_declaration_count = @($pathCalls | Where-Object correlation -eq 'ambiguous-declaration-candidate').Count
        lexical_no_selected_extern_count = @($pathCalls | Where-Object correlation -eq 'no-selected-extern-candidate').Count
        selected_definition_name_observed_count = @($pathBoundaries | Where-Object disposition -eq 'selected-definition-name-observed').Count
        external_boundary_candidate_count = @($pathBoundaries | Where-Object disposition -eq 'external-boundary-candidate').Count
        interface_audit_state = 'coverage-only; source-form/ABI/failure review remains required before any binding decision'
        evidence = "m0-t276-s4-textual-call-candidate-ledger.tsv; m0-t276-s5-unique-call-disposition-ledger.tsv; $($sourceRow.source_path)"
    }
}

$out | ConvertTo-Csv -NoTypeInformation -Delimiter "`t" | Set-Content -LiteralPath (Join-Path $operations 'm0-t276-s12-host-src-interface-density-ledger.tsv') -Encoding utf8
Write-Output "T276 S12 host/src interface density generated: $($out.Count) source rows; $((($out | Measure-Object lexical_call_count -Sum).Sum)) lexical calls."
