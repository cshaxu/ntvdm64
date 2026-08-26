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
$interfaceLedgerPath = Join-Path $operationsRoot 'mvdm-interface-recovery-ledger.tsv'
$candidatePath = Join-Path $operationsRoot 'm0-t276-s3-textual-declaration-ledger.tsv'
$evidencePath = Join-Path $operationsRoot 'm0-t276-s3-interface-declaration-baseline-001.md'

foreach ($path in @($fileLedgerPath, $interfaceLedgerPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing T276 input ledger: $path" }
}

function Get-CandidateName([string]$Declaration) {
    if ([string]::IsNullOrEmpty($Declaration)) { return '' }
    $pointerName = [regex]::Match($Declaration, '\(\s*\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)')
    if ($pointerName.Success) { return $pointerName.Groups[1].Value }
    $functionNames = [regex]::Matches($Declaration, '\b([A-Za-z_][A-Za-z0-9_]*)\s*\(')
    if ($functionNames.Count -gt 0) { return $functionNames[$functionNames.Count - 1].Groups[1].Value }
    $allNames = [regex]::Matches($Declaration, '\b([A-Za-z_][A-Za-z0-9_]*)\b')
    if ($allNames.Count -gt 0) { return $allNames[$allNames.Count - 1].Groups[1].Value }
    return ''
}

$files = Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Sort-Object target_path
if ($files.Count -ne 1689) { throw "Expected 1,689 file-ledger rows; found $($files.Count)." }
$declarationFiles = @($files | Where-Object { $_.file_kind -eq 'declaration' })
if ($declarationFiles.Count -ne 857) { throw "Expected 857 declaration files; found $($declarationFiles.Count)." }
if (($declarationFiles | Where-Object { $_.mirror_identity -ne 'byte-exact-verified' }).Count -ne 0) {
    throw 'S3 requires the S1 byte-exact declaration baseline.'
}

$existing = @(Import-Csv -LiteralPath $interfaceLedgerPath -Delimiter "`t")
$families = @($existing | Where-Object { $_.row_kind -eq 'family-unexpanded' })
if ($families.Count -ne 12) { throw "Expected twelve unexpanded family debt rows; found $($families.Count)." }

$candidates = New-Object System.Collections.Generic.List[object]
$candidateIndex = 0
$nameless = 0
foreach ($file in $declarationFiles) {
    $absolutePath = Join-Path $mirrorRoot ($file.target_path -replace '/', '\\')
    if (-not (Test-Path -LiteralPath $absolutePath -PathType Leaf)) { throw "Missing verified declaration mirror: $($file.target_path)" }
    $text = Get-Content -LiteralPath $absolutePath -Raw
    if ($null -eq $text) { $text = '' }
    # This is deliberately lexical: comments/macros/conditional declarations
    # remain visible as candidates instead of being silently inferred away.
    foreach ($match in [regex]::Matches($text, '(?ms)\bextern\b(?:(?!;).){0,2048};')) {
        $candidateIndex++
        $raw = [regex]::Replace($match.Value, '\s+', ' ').Trim()
        $name = Get-CandidateName $raw
        if ([string]::IsNullOrWhiteSpace($name)) { $nameless++ }
        $line = 1 + ([regex]::Matches($text.Substring(0, $match.Index), "`n")).Count
        $kind = if ($raw -match '\(') { 'function-or-callback-textual' } else { 'global-or-type-textual' }
        $candidates.Add([pscustomobject]@{
            declaration_id = ('MVDM-DECL-{0:D5}' -f $candidateIndex)
            file_id = $file.file_id
            declaration_path = $file.target_path
            package_root = $file.package_root
            source_line = $line
            declaration_kind = $kind
            lexical_name = $name
            raw_declaration = $raw
            reachability = 'unreviewed-textual-declaration'
            availability = 'unreviewed'
            evidence = $file.target_path + ':' + $line
        })
    }
}

$candidates | Export-Csv -LiteralPath $candidatePath -Delimiter "`t" -NoTypeInformation -Encoding utf8

# The interface-recovery ledger is the authoritative parent tracker.  S3 adds
# only neutral children: they are declarations, not claims that a caller reaches
# them or that an adapter is needed.
$interfaceChildren = foreach ($candidate in $candidates) {
    [pscustomobject]@{
        interface_id = $candidate.declaration_id
        parent_interface_id = ''
        row_kind = 'textual-declaration-candidate'
        caller = ''
        declaration_evidence = $candidate.evidence
        required_interface = if ([string]::IsNullOrWhiteSpace($candidate.lexical_name)) { '<lexically-unnamed>' } else { $candidate.lexical_name }
        abi_layout = $candidate.declaration_kind
        owner = 'unassigned-pending-caller-resolution'
        availability = 'unreviewed'
        recovery_disposition = 'unreviewed'
        failure_contract = 'unreviewed; declaration alone does not prove a required runtime interface'
        successor = 'T276 S4 caller-to-interface resolution'
        focused_evidence = $candidate.declaration_id
    }
}
@($families) + @($interfaceChildren) | Export-Csv -LiteralPath $interfaceLedgerPath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$byPackage = $candidates | Group-Object package_root | Sort-Object Name
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('# M0 T276 S3 original textual interface-declaration evidence')
$lines.Add('')
$lines.Add('## Scope and method')
$lines.Add('')
$lines.Add('The extractor scans all byte-exact selected header/inc/gi mirrors for a bounded lexical `extern ... ;` form. It records each source occurrence with original file and line evidence. It neither preprocesses conditionals/macros nor resolves a declaration to a caller, library, adapter or available modern implementation.')
$lines.Add('')
$lines.Add('## Counts')
$lines.Add('')
$lines.Add('- Selected declaration files: ' + $declarationFiles.Count)
$lines.Add('- Existing unexpanded interface-family rows retained: ' + $families.Count)
$lines.Add('- Lexical `extern` declaration candidates: ' + $candidates.Count)
$lines.Add('- Candidates without an unambiguous lexical name: ' + $nameless)
$lines.Add('')
$lines.Add('### Candidates by declaration package')
$lines.Add('')
foreach ($group in $byPackage) { $lines.Add('- `' + $group.Name + '`: ' + $group.Count) }
$lines.Add('')
$lines.Add('## Limits')
$lines.Add('')
$lines.Add('- An `extern` declaration is not evidence that any selected translation unit reaches it.')
$lines.Add('- Macro-expanded declarations, static definitions, assembly exports, import libraries and call-site calling conventions require later package/caller analysis.')
$lines.Add('- The candidate ledger preserves raw declaration text to make S4 resolution auditable rather than silently guessing an adapter owner.')
$lines.Add('- No final composition classification, adapter requirement, x86/x64 build decision or enabled package follows from this extraction.')
[System.IO.File]::WriteAllLines($evidencePath, $lines)

Write-Host "Observed $($candidates.Count) lexical extern declaration candidates across $($declarationFiles.Count) declaration files; retained $($families.Count) unexpanded family rows."
