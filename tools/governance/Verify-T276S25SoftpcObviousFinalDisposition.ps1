[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RepositoryRoot)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path $RepositoryRoot).Path
$ops = Join-Path $root 'docs/etc/operations'
$rows = @(Import-Csv (Join-Path $ops 'm0-t276-final-integration-disposition-ledger.tsv') -Delimiter "`t")
$source = @(Import-Csv (Join-Path $ops 'm0-t276-s12-softpc-file-disposition-ledger.tsv') -Delimiter "`t")
$groups = @{
    'bochs-replacement-profile-excluded' = 'not-host-runtime'
    'adapter-bochs-firmware-review' = 'firmware-only'
    'tool-profile-excluded' = 'tool-only'
    'profile-excluded' = 'not-host-runtime'
    'build-control-only' = 'not-host-runtime'
}
$checked = 0
foreach ($pair in $groups.GetEnumerator()) {
    $paths = @($source | Where-Object {$_.provisional_composition_disposition -eq $pair.Key} | Select-Object -ExpandProperty target_path)
    $final = @($rows | Where-Object {$paths -contains $_.source_path})
    if ($final.Count -ne $paths.Count) { throw "Missing final rows for $($pair.Key)." }
    if (@($final | Where-Object {$_.final_disposition -ne $pair.Value}).Count) { throw "Final disposition mismatch for $($pair.Key)." }
    if (@($final | Where-Object {$_.final_audit_state -ne 'final-disposition-audited; not implemented'}).Count) { throw "Final audit state mismatch for $($pair.Key)." }
    $checked += $final.Count
}
if ($checked -ne 558) { throw "Expected 558 verified SoftPC rows, found $checked." }
Write-Output "T276 S25 conclusive SoftPC final-disposition verification passed: $checked rows."
