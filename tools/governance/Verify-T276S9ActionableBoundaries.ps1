[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$formsPath = Join-Path $operations 'm0-t276-s8-original-provider-form-ledger.tsv'
$ledgerPath = Join-Path $operations 'm0-t276-s9-actionable-boundary-abi-failure-ledger.tsv'
foreach ($path in @($formsPath, $ledgerPath)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing required ledger: $path" }
}

$forms = Import-Csv -LiteralPath $formsPath -Delimiter "`t"
$ledger = Import-Csv -LiteralPath $ledgerPath -Delimiter "`t"
$actionableForms = @('no-selected-provider-body','no-selected-provider-body-profile-guarded','toolchain-runtime-import','selected-callback-global')
$expected = @($forms | Where-Object { $_.selected_source_form -in $actionableForms })
if ($expected.Count -ne 12) { throw "Expected 12 non-body S8 rows, found $($expected.Count)." }
if ($ledger.Count -ne $expected.Count) { throw "S9 row count $($ledger.Count) does not match expected $($expected.Count)." }
foreach ($form in $expected) {
    $row = @($ledger | Where-Object { $_.worklist_id -eq $form.worklist_id })
    if ($row.Count -ne 1) { throw "S9 must have exactly one row for $($form.worklist_id)." }
    if ($row[0].lexical_name -ne $form.lexical_name -or $row[0].declaration_path -ne $form.declaration_path -or $row[0].source_form -ne $form.selected_source_form) {
        throw "S9 identity/form mismatch for $($form.worklist_id)."
    }
    foreach ($column in 'call_abi_evidence','availability_or_guard_evidence','failure_or_result_evidence','package_prerequisite','owner_decision','recovery_state','notes') {
        if ([string]::IsNullOrWhiteSpace($row[0].$column)) { throw "Empty $column for $($form.worklist_id)." }
    }
    if ($row[0].owner_decision -ne 'unreviewed' -or $row[0].recovery_state -ne 'unreviewed') {
        throw "S9 must not make an implementation or ownership decision for $($form.worklist_id)."
    }
}

Write-Output "T276 S9 actionable-boundary verification passed: $($ledger.Count) rows."
