[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'docs/etc/operations'
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) { $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot) }
$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operationsRoot = Join-Path $repositoryRoot $OutputDirectory
$mirrorRoot = Join-Path $repositoryRoot 'src/opennt-mvdm-host'
$fileLedgerPath = Join-Path $operationsRoot 'mvdm-file-recovery-ledger.tsv'
$declarationPath = Join-Path $operationsRoot 'm0-t276-s3-textual-declaration-ledger.tsv'
$callPath = Join-Path $operationsRoot 'm0-t276-s4-textual-call-candidate-ledger.tsv'
$definitionPath = Join-Path $operationsRoot 'm0-t276-s5-selected-definition-ledger.tsv'
$boundaryPath = Join-Path $operationsRoot 'm0-t276-s5-unique-call-disposition-ledger.tsv'
$evidencePath = Join-Path $operationsRoot 'm0-t276-s5-external-boundary-baseline-001.md'

foreach ($path in @($fileLedgerPath, $declarationPath, $callPath)) { if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T276 input: $path" } }
$files = Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Sort-Object target_path
$sources = @($files | Where-Object { $_.file_kind -eq 'source' })
$calls = @(Import-Csv -LiteralPath $callPath -Delimiter "`t")
$declarations = @(Import-Csv -LiteralPath $declarationPath -Delimiter "`t")
if ($sources.Count -ne 612) { throw "Expected 612 sources; found $($sources.Count)." }
if ($calls.Count -ne 89924) { throw "Expected 89,924 calls; found $($calls.Count)." }

$keywords = @('if','for','while','switch','return','sizeof','case','do','typedef','defined','__asm','asm','catch')
$definitions = New-Object System.Collections.Generic.List[object]
$definitionIndex = 0
$definitionNames = @{}
foreach ($source in $sources) {
    if ([System.IO.Path]::GetExtension($source.target_path).ToLowerInvariant() -notin @('.c','.cc','.cpp')) { continue }
    $path = Join-Path $mirrorRoot ($source.target_path -replace '/', '\\')
    $text = Get-Content -LiteralPath $path -Raw
    if ($null -eq $text) { $text = '' }
    $scanText = [regex]::Replace($text, '(?s)/\*.*?\*/', { param($m) [regex]::Replace($m.Value, '[^\r\n]', ' ') })
    $scanText = [regex]::Replace($scanText, '(?m)//[^\r\n]*', { param($m) [regex]::Replace($m.Value, '[^\r\n]', ' ') })
    # Preserve both ANSI and the original K&R definition form.  A K&R header
    # may have parameter declaration lines between `name(args)` and `{`.
    foreach ($match in [regex]::Matches($scanText, '(?ms)\b([A-Za-z_][A-Za-z0-9_]*)\s*\([^(){}]{0,2048}\)\s*(?:(?:[A-Za-z_][A-Za-z0-9_ \t\*,\r\n]*;)\s*){0,32}\{')) {
        $name = $match.Groups[1].Value
        if ($keywords -contains $name.ToLowerInvariant()) { continue }
        $definitionIndex++
        $line = 1 + ([regex]::Matches($scanText.Substring(0, $match.Index), "`n")).Count
        $record = [pscustomobject]@{
            definition_id = ('MVDM-DEF-{0:D5}' -f $definitionIndex); file_id = $source.file_id
            definition_path = $source.target_path; package_root = $source.package_root; source_line = $line
            lexical_name = $name; provenance = 'selected-c-or-cpp-source-lexical-definition'; evidence = $source.target_path + ':' + $line
        }
        $definitions.Add($record)
        $key = $name.ToLowerInvariant()
        if (-not $definitionNames.ContainsKey($key)) { $definitionNames[$key] = New-Object System.Collections.Generic.List[string] }
        $definitionNames[$key].Add($record.definition_id)
    }
}
$definitions | Export-Csv -LiteralPath $definitionPath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$declarationById = @{}
foreach ($declaration in $declarations) { $declarationById[$declaration.declaration_id] = $declaration }
$uniqueCalls = @($calls | Where-Object { $_.correlation -eq 'unique-declaration-name-candidate' })
$boundaries = New-Object System.Collections.Generic.List[object]
$boundaryIndex = 0
foreach ($call in $uniqueCalls) {
    $boundaryIndex++
    $declaration = $declarationById[$call.declaration_candidate_ids]
    if ($null -eq $declaration) { throw "S4 unique candidate lacks S3 declaration: $($call.call_id)" }
    $key = $call.lexical_callee.ToLowerInvariant()
    $disposition = if ($definitionNames.ContainsKey($key)) { 'selected-definition-name-observed' } else { 'external-boundary-candidate' }
    $boundaries.Add([pscustomobject]@{
        disposition_id = ('MVDM-BOUNDARY-{0:D5}' -f $boundaryIndex); call_id = $call.call_id
        caller_path = $call.caller_path; caller_package = $call.caller_package; caller_line = $call.source_line
        lexical_callee = $call.lexical_callee; declaration_id = $declaration.declaration_id
        declaration_path = $declaration.declaration_path; declaration_package = $declaration.package_root
        disposition = $disposition
        selected_definition_ids = if ($definitionNames.ContainsKey($key)) { ($definitionNames[$key] | Select-Object -Unique) -join ';' } else { '' }
        owner = 'unreviewed'; recovery_state = 'unreviewed'; evidence = $call.evidence + '; ' + $declaration.evidence
    })
}
$boundaries | Export-Csv -LiteralPath $boundaryPath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$counts = @{}; foreach ($row in $boundaries) { if (-not $counts.ContainsKey($row.disposition)) { $counts[$row.disposition] = 0 }; $counts[$row.disposition]++ }
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('# M0 T276 S5 selected-definition and external-boundary evidence')
$lines.Add('')
$lines.Add('## Method')
$lines.Add('')
$lines.Add('S5 scans selected C/C++ source for a lexical function-definition form, then disposes every S4 unique-declaration-name call candidate. A name observed in any selected source definition is retained as a selected-definition-name observation; all other unique declaration calls are external-boundary candidates. Neither outcome proves linkage, calling convention, selected provider, reachability after preprocessing, or adapter ownership.')
$lines.Add('')
$lines.Add('## Counts')
$lines.Add('')
$lines.Add('- Selected C/C++ lexical definition occurrences: ' + $definitions.Count)
$lines.Add('- S4 unique-declaration-name call candidates disposed: ' + $boundaries.Count)
foreach ($name in ($counts.Keys | Sort-Object)) { $lines.Add('- `' + $name + '`: ' + $counts[$name]) }
$lines.Add('')
$lines.Add('## Limits')
$lines.Add('')
$lines.Add('- Static/conditional/macro forms and assembly symbols require source-level package review.')
$lines.Add('- A selected definition with the same spelling can be a different linkage scope; it is not a provider decision.')
$lines.Add('- `external-boundary-candidate` is an audit queue, not an adapter authorization.')
[System.IO.File]::WriteAllLines($evidencePath, $lines)
Write-Host "Observed $($definitions.Count) selected C/C++ definition candidates and disposed $($boundaries.Count) unique call candidates."
