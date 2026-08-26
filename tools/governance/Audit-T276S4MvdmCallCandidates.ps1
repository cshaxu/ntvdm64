[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputDirectory = 'docs/etc/operations'
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$operationsRoot = Join-Path $repositoryRoot $OutputDirectory
$mirrorRoot = Join-Path $repositoryRoot 'src/opennt-mvdm-host'
$fileLedgerPath = Join-Path $operationsRoot 'mvdm-file-recovery-ledger.tsv'
$declarationPath = Join-Path $operationsRoot 'm0-t276-s3-textual-declaration-ledger.tsv'
$callPath = Join-Path $operationsRoot 'm0-t276-s4-textual-call-candidate-ledger.tsv'
$unitPath = Join-Path $operationsRoot 'm0-t276-s4-translation-unit-call-scan-ledger.tsv'
$evidencePath = Join-Path $operationsRoot 'm0-t276-s4-caller-callee-correlation-001.md'

foreach ($path in @($fileLedgerPath, $declarationPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T276 input ledger: $path" }
}

$files = Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Sort-Object target_path
$sources = @($files | Where-Object { $_.file_kind -eq 'source' })
if ($sources.Count -ne 612) { throw "Expected 612 selected source files; found $($sources.Count)." }
$declarations = @(Import-Csv -LiteralPath $declarationPath -Delimiter "`t")
if ($declarations.Count -ne 4803) { throw "Expected 4,803 declaration candidates; found $($declarations.Count)." }
$byName = @{}
foreach ($declaration in $declarations) {
    $name = $declaration.lexical_name.ToLowerInvariant()
    if (-not $byName.ContainsKey($name)) { $byName[$name] = New-Object System.Collections.Generic.List[string] }
    $byName[$name].Add($declaration.declaration_id)
}

$keywords = @('if','for','while','switch','return','sizeof','case','do','typedef','defined','__asm','asm','catch')
$calls = New-Object System.Collections.Generic.List[object]
$units = New-Object System.Collections.Generic.List[object]
$callIndex = 0
foreach ($source in $sources) {
    $extension = [System.IO.Path]::GetExtension($source.target_path).ToLowerInvariant()
    if ($extension -notin @('.c', '.cc', '.cpp')) {
        $units.Add([pscustomobject]@{
            file_id = $source.file_id; target_path = $source.target_path; package_root = $source.package_root
            scan_state = 'not-c-like-lexical-scan'; candidate_count = 0; evidence = $source.target_path
        })
        continue
    }
    $absolutePath = Join-Path $mirrorRoot ($source.target_path -replace '/', '\\')
    $text = Get-Content -LiteralPath $absolutePath -Raw
    if ($null -eq $text) { $text = '' }
    # Replace comments with spaces while retaining newline positions. Preprocessor
    # and definition forms remain candidates by design; this is not a C parser.
    $scanText = [regex]::Replace($text, '(?s)/\*.*?\*/', { param($m) [regex]::Replace($m.Value, '[^\r\n]', ' ') })
    $scanText = [regex]::Replace($scanText, '(?m)//[^\r\n]*', { param($m) [regex]::Replace($m.Value, '[^\r\n]', ' ') })
    $unitCount = 0
    foreach ($match in [regex]::Matches($scanText, '\b([A-Za-z_][A-Za-z0-9_]*)\s*\(')) {
        $name = $match.Groups[1].Value
        if ($keywords -contains $name.ToLowerInvariant()) { continue }
        $callIndex++; $unitCount++
        $nameKey = $name.ToLowerInvariant()
        $candidateIds = if ($byName.ContainsKey($nameKey)) { @($byName[$nameKey]) } else { @() }
        $correlation = if ($candidateIds.Count -eq 0) { 'no-selected-extern-candidate' }
            elseif ($candidateIds.Count -eq 1) { 'unique-declaration-name-candidate' }
            else { 'ambiguous-declaration-name-candidate' }
        $line = 1 + ([regex]::Matches($scanText.Substring(0, $match.Index), "`n")).Count
        $calls.Add([pscustomobject]@{
            call_id = ('MVDM-CALL-{0:D6}' -f $callIndex)
            caller_file_id = $source.file_id
            caller_path = $source.target_path
            caller_package = $source.package_root
            source_line = $line
            lexical_callee = $name
            correlation = $correlation
            declaration_candidate_ids = ($candidateIds -join ';')
            reachability = 'unreviewed-textual-call'
            evidence = $source.target_path + ':' + $line
        })
    }
    $units.Add([pscustomobject]@{
        file_id = $source.file_id; target_path = $source.target_path; package_root = $source.package_root
        scan_state = 'c-like-lexical-scan'; candidate_count = $unitCount; evidence = $source.target_path
    })
}
$calls | Export-Csv -LiteralPath $callPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$units | Export-Csv -LiteralPath $unitPath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$correlations = @{}
foreach ($call in $calls) { if (-not $correlations.ContainsKey($call.correlation)) { $correlations[$call.correlation] = 0 }; $correlations[$call.correlation]++ }
$scanStates = @{}
foreach ($unit in $units) { if (-not $scanStates.ContainsKey($unit.scan_state)) { $scanStates[$unit.scan_state] = 0 }; $scanStates[$unit.scan_state]++ }
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('# M0 T276 S4 textual caller/callee correlation evidence')
$lines.Add('')
$lines.Add('## Scope and method')
$lines.Add('')
$lines.Add('All 612 selected translation units are accounted for. C/C++ files receive a comment-stripped lexical identifier-before-parenthesis scan. Assembly units are recorded but not projected into a false C-call model. Candidate callee names are matched only against the S3 lexical declaration names; this is neither a compiler symbol table nor a proof of an external, reachable call.')
$lines.Add('')
$lines.Add('## Counts')
$lines.Add('')
$lines.Add('- Selected translation units: ' + $units.Count)
$lines.Add('- Lexical call/definition candidates: ' + $calls.Count)
$lines.Add('')
$lines.Add('### Translation-unit scan states')
$lines.Add('')
foreach ($name in ($scanStates.Keys | Sort-Object)) { $lines.Add('- `' + $name + '`: ' + $scanStates[$name]) }
$lines.Add('')
$lines.Add('### Name-correlation outcomes')
$lines.Add('')
foreach ($name in ($correlations.Keys | Sort-Object)) { $lines.Add('- `' + $name + '`: ' + $correlations[$name]) }
$lines.Add('')
$lines.Add('## Limits')
$lines.Add('')
$lines.Add('- Calls, function definitions, macro expansions and function-pointer forms can share this lexical shape; candidates require S5 source-level review.')
$lines.Add('- A unique name match does not establish ABI, linkage, direct availability, adapter ownership or a package dependency.')
$lines.Add('- Assembly exports/imports, macro-generated calls and build-condition reachability remain explicit follow-up evidence.')
[System.IO.File]::WriteAllLines($evidencePath, $lines)
Write-Host "Accounted for $($units.Count) translation units and observed $($calls.Count) lexical caller/callee candidates."
