[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$fileLedger = Import-Csv -LiteralPath (Join-Path $operations 'm0-t276-s11-w0-w1-file-disposition-ledger.tsv') -Delimiter "`t"
$familyLedger = Import-Csv -LiteralPath (Join-Path $operations 'm0-t276-s11-w0-w1-source-interface-family-ledger.tsv') -Delimiter "`t"
$callLedger = Import-Csv -LiteralPath (Join-Path $operations 'm0-t276-s11-w0-w1-call-accounting-ledger.tsv') -Delimiter "`t"
$runtimeSupportLedger = Import-Csv -LiteralPath (Join-Path $operations 'm0-t276-s11-w0-w1-runtime-support-binding-ledger.tsv') -Delimiter "`t"
$roots = @('dirs', 'makefil0', 'inc', 'oemuni', 'suballoc', 'vdmutils')
if ($fileLedger.Count -ne 140) { throw "Expected 140 W0/W1 file rows, found $($fileLedger.Count)." }
if (($fileLedger | Group-Object file_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate W0/W1 file identity.' }
$actualRoots = (($fileLedger.package_root | Sort-Object -Unique) -join ',')
$expectedRoots = (($roots | Sort-Object -Unique) -join ',')
if ($actualRoots -ne $expectedRoots) { throw 'W0/W1 file roots are incomplete.' }
foreach ($row in $fileLedger) {
    foreach ($column in 'original_build_membership','provisional_composition_disposition','source_role_and_reason','original_evidence','interface_state','final_state') {
        if ([string]::IsNullOrWhiteSpace($row.$column)) { throw "Empty $column for $($row.target_path)." }
    }
    if ($row.final_component_owner -notin @('opennt-mvdm-support', 'opennt-mvdm-tools')) { throw "Unexpected final owner for $($row.target_path)." }
    if ($row.final_state -ne 'provisional-not-enabled') { throw "S11 cannot enable $($row.target_path)." }
}
if ($familyLedger.Count -ne 14) { throw "Expected 14 source-interface family rows, found $($familyLedger.Count)." }
if (($familyLedger | Group-Object source_path, original_interface_family | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate source-interface family row.' }
if (($callLedger | Group-Object call_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate W0/W1 call-accounting identity.' }
if ($callLedger.Count -eq 0) { throw 'W0/W1 call-accounting ledger is empty.' }
if (($callLedger | Where-Object { $_.call_accounting_state -notlike 'accounted-lexically*' }).Count -ne 0) { throw 'Unexpected S11 call-accounting state.' }
if ($runtimeSupportLedger.Count -ne 3) { throw "Expected three original runtime-support bindings, found $($runtimeSupportLedger.Count)." }
if (($runtimeSupportLedger | Group-Object binding_id | Where-Object Count -ne 1).Count -ne 0) { throw 'Duplicate W1 runtime-support binding identity.' }
if (($runtimeSupportLedger | Where-Object { $_.state -notlike '*-pending-not-enabled' }).Count -ne 0) { throw 'S11 runtime-support binding must remain unenabled.' }
if (@($fileLedger | Where-Object final_component_owner -eq 'opennt-mvdm-support').Count -ne 112) { throw 'Expected 112 support-owned W0/W1 paths.' }
if (@($fileLedger | Where-Object final_component_owner -eq 'opennt-mvdm-tools').Count -ne 28) { throw 'Expected 28 tool-owned W0/W1 paths.' }
Write-Output "T276 S11 W0/W1 disposition verification passed: $($fileLedger.Count) files, $($familyLedger.Count) interface-family rows, $($callLedger.Count) lexical calls, $($runtimeSupportLedger.Count) runtime-support bindings."
