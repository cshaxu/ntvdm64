[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'docs/etc/operations'
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operations = Join-Path $root $OutputDirectory
$mirrorRoot = Join-Path $root 'src/opennt-mvdm-host'
$worklistPath = Join-Path $operations 'm0-t276-s6-boundary-family-worklist.tsv'
$ledgerPath = Join-Path $operations 'm0-t276-s7-original-provider-occurrence-ledger.tsv'
$evidencePath = Join-Path $operations 'm0-t276-s7-provider-occurrence-baseline-001.md'
if (-not (Test-Path -LiteralPath $worklistPath)) { throw "Missing S6 worklist: $worklistPath" }

$sources = Get-ChildItem -LiteralPath $mirrorRoot -Recurse -File |
    Where-Object { $_.Extension -in '.c','.cc','.cpp' } |
    Sort-Object FullName
$worklist = Import-Csv -LiteralPath $worklistPath -Delimiter "`t"
$rows = foreach ($item in $worklist) {
    $pattern = '(?<![A-Za-z0-9_])' + [regex]::Escape($item.lexical_name) + '(?![A-Za-z0-9_])'
    $matches = foreach ($source in $sources) {
        $text = Get-Content -LiteralPath $source.FullName -Raw
        if ([string]::IsNullOrEmpty($text)) { continue }
        if ([regex]::IsMatch($text, $pattern)) { $source }
    }
    $relativePaths = @($matches | ForEach-Object {
        $_.FullName.Substring($mirrorRoot.Length).TrimStart('\').Replace('\','/')
    } | Sort-Object -Unique)
    [pscustomobject]@{
        worklist_id = $item.worklist_id
        lexical_name = $item.lexical_name
        declaration_path = $item.declaration_path
        source_occurrence_count = $relativePaths.Count
        source_paths = $relativePaths -join ';'
        review_state = 'unreviewed-source-semantic'
        evidence = 'case-sensitive selected source occurrence scan'
    }
}

if (($rows | Where-Object { $_.source_occurrence_count -eq 0 }).Count -ne 0) {
    throw 'At least one S6 worklist symbol has no case-sensitive selected-source occurrence.'
}
$rows | Export-Csv -LiteralPath $ledgerPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$lines = @(
    '# M0 T276 S7 original provider occurrence baseline',
    '',
    'The accompanying `m0-t276-s7-original-provider-occurrence-ledger.tsv` records case-sensitive selected C/C++ source occurrences for each S6 worklist symbol.  It is a source-navigation aid only: an occurrence can be a definition, call, macro, variable declaration or comment-adjacent context.  It does not assign an owner, adapter, ABI result, composition class or build disposition.',
    '',
    "All $($rows.Count) S6 worklist symbols have at least one selected-source occurrence.  S8 must classify each recorded occurrence by original declaration/definition form before accepting or rejecting an adapter boundary."
)
[System.IO.File]::WriteAllLines($evidencePath, $lines)
Write-Host "Recorded case-sensitive provider occurrences for $($rows.Count) worklist rows."
