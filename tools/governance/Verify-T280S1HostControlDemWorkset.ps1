[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ledger = @(Import-Csv -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t280-s1-host-control-dem-workset-ledger.tsv') -Delimiter "`t")
$source = @(Import-Csv -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t276-final-integration-disposition-ledger.tsv') -Delimiter "`t" |
    Where-Object { $_.source_path -like 'dos/dem/*' -or $_.source_path -like 'softpc.new/host/src/*' })
if ($ledger.Count -ne 81) { throw "Expected 81 T280 workset rows; found $($ledger.Count)." }
if ($source.Count -ne 81) { throw "Expected 81 selected final-ledger rows; found $($source.Count)." }
if (@($ledger | Group-Object source_path | Where-Object Count -ne 1).Count -ne 0) {
    throw 'T280 workset contains duplicate source paths.'
}
foreach ($pair in @(@('adapter-backed', 49), @('overlay-required', 6), @('binding-only', 5), @('not-host-runtime', 21))) {
    $actual = @($ledger | Where-Object final_disposition -eq $pair[0]).Count
    if ($actual -ne $pair[1]) { throw "Expected $($pair[1]) $($pair[0]) rows; found $actual." }
}
if (@($ledger | Where-Object { $_.source_path -notlike 'dos/dem/*' -and $_.source_path -notlike 'softpc.new/host/src/*' }).Count -ne 0) {
    throw 'T280 workset contains a path outside DEM/host-control scope.'
}
foreach ($sourceRow in $source) {
    $worksetRow = @($ledger | Where-Object source_path -eq $sourceRow.source_path)
    if ($worksetRow.Count -ne 1) { throw "Missing or duplicate T280 workset row: $($sourceRow.source_path)." }
    if ($worksetRow[0].final_disposition -ne $sourceRow.final_disposition -or
        $worksetRow[0].final_owner_or_link_boundary -ne $sourceRow.final_owner_or_link_boundary -or
        $worksetRow[0].named_adapter -ne $sourceRow.named_adapter -or
        $worksetRow[0].mapping_implication -ne $sourceRow.mapping_implication) {
        throw "T280 workset diverges from final disposition: $($sourceRow.source_path)."
    }
}
foreach ($group in @('01-bop-ingress-control', '02-softpc-machine-facade', '03-dem-dispatch-data', '03-dem-namespace-file', '04-session-event-stop', '05-machine-transport-overlay')) {
    if (@($ledger | Where-Object implementation_group -eq $group).Count -eq 0) { throw "Missing implementation group: $group" }
}
Write-Host 'PASS: T280 S1 Wave 2 workset: 81 paths and all source-owner implementation groups are reconciled.'
