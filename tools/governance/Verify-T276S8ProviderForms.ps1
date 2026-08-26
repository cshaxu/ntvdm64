[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root 'docs/etc/operations'
$worklistPath = Join-Path $operations 'm0-t276-s6-boundary-family-worklist.tsv'
$formsPath = Join-Path $operations 'm0-t276-s8-original-provider-form-ledger.tsv'

foreach ($path in @($worklistPath, $formsPath)) {
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing required ledger: $path" }
}

$worklist = Import-Csv -LiteralPath $worklistPath -Delimiter "`t"
$forms = Import-Csv -LiteralPath $formsPath -Delimiter "`t"
if ($worklist.Count -ne 26) { throw "Expected 26 S6 worklist rows, found $($worklist.Count)." }
if ($forms.Count -ne $worklist.Count) { throw "Provider-form row count $($forms.Count) does not match worklist count $($worklist.Count)." }

$duplicate = $forms | Group-Object worklist_id | Where-Object Count -ne 1
if ($duplicate) { throw "Provider-form ledger contains duplicate worklist IDs." }

foreach ($row in $worklist) {
    $form = $forms | Where-Object worklist_id -eq $row.worklist_id
    if (-not $form) { throw "Missing provider-form row for $($row.worklist_id)." }
    if ($form.lexical_name -ne $row.lexical_name -or $form.declaration_path -ne $row.declaration_path) {
        throw "Provider-form identity mismatch for $($row.worklist_id)."
    }
    foreach ($column in 'declared_shape','selected_source_form','selected_source_evidence','original_semantic_boundary','next_semantic_review','recovery_state','notes') {
        if ([string]::IsNullOrWhiteSpace($form.$column)) { throw "Empty $column for $($row.worklist_id)." }
    }
    if ($form.recovery_state -ne 'unreviewed') { throw "S8 must not make a final recovery decision: $($row.worklist_id)." }
}

$allowed = @(
    'selected-original-body',
    'selected-original-machine-body',
    'selected-original-machine-body-and-indirection',
    'selected-alternative-machine-bodies',
    'selected-profile-alternative-bodies',
    'selected-callback-global',
    'toolchain-runtime-import',
    'no-selected-provider-body',
    'no-selected-provider-body-profile-guarded'
)
if (($forms.selected_source_form | Where-Object { $_ -notin $allowed }).Count -ne 0) {
    throw 'Provider-form ledger contains an unapproved source-form classification.'
}

Write-Output "T276 S8 provider-form verification passed: $($forms.Count) rows, $($forms.selected_source_form | Sort-Object -Unique | Measure-Object | Select-Object -ExpandProperty Count) form classes."
