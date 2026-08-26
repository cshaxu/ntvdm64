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
$packageLedgerPath = Join-Path $operationsRoot 'mvdm-package-dependency-ledger.tsv'
$profileLedgerPath = Join-Path $operationsRoot 'mvdm-build-profile-ledger.tsv'
$edgePath = Join-Path $operationsRoot 'm0-t276-s2-translation-unit-include-ledger.tsv'
$buildDescriptionPath = Join-Path $operationsRoot 'm0-t276-s2-original-build-description-ledger.tsv'
$translationUnitPath = Join-Path $operationsRoot 'm0-t276-s2-translation-unit-build-ledger.tsv'
$evidencePath = Join-Path $operationsRoot 'm0-t276-s2-build-input-include-graph-001.md'

foreach ($path in @($fileLedgerPath, $packageLedgerPath, $profileLedgerPath)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing T276 input ledger: $path"
    }
}

function Normalize-RelativePath([string]$Path) {
    $parts = New-Object System.Collections.Generic.List[string]
    foreach ($part in ($Path -replace '/', '\\' -split '\\')) {
        if ([string]::IsNullOrWhiteSpace($part) -or $part -eq '.') { continue }
        if ($part -eq '..') {
            if ($parts.Count -gt 0) { $parts.RemoveAt($parts.Count - 1) }
            continue
        }
        $parts.Add($part)
    }
    return ($parts -join '/')
}

function Classify-SystemInclude([string]$Spelling, [string]$Delimiter) {
    if ($Delimiter -eq '<') {
        if ($Spelling -match '(?i)^(windows|winternl|ntsecapi|wincon|winsock|rpc|lm|shellapi|commctrl|objbase|ole|tchar|strsafe)\\.h$') {
            return 'win32-or-nt-sdk-textual'
        }
        return 'toolchain-or-sdk-textual'
    }
    return 'unresolved-textual'
}

$files = Import-Csv -LiteralPath $fileLedgerPath -Delimiter "`t" | Sort-Object target_path
if ($files.Count -ne 1689) { throw "Expected 1,689 file-ledger rows; found $($files.Count)." }
if (($files | Where-Object { $_.mirror_identity -ne 'byte-exact-verified' }).Count -ne 0) {
    throw 'S2 requires the S1 byte-exact mirror baseline.'
}

$byPath = @{}
foreach ($file in $files) { $byPath[$file.target_path.ToLowerInvariant()] = $file }

$sourceFiles = @($files | Where-Object { $_.file_kind -eq 'source' })
$buildInputs = @($files | Where-Object {
    [System.IO.Path]::GetFileName($_.target_path) -match '^(?i:dirs|makefil0|makefile|sources)$'
})

# Parse original NT build `sources` files as historical evidence only. The
# parser intentionally preserves raw values and does not expand macros or
# assert that a historic target is buildable today.
$buildDescriptions = New-Object System.Collections.Generic.List[object]
$buildDescriptionIndex = 0
$sourceToDescriptions = @{}
$declaredSourceTokens = 0
foreach ($buildInput in $buildInputs) {
    $buildInputName = [System.IO.Path]::GetFileName($buildInput.target_path)
    $absolutePath = Join-Path $mirrorRoot ($buildInput.target_path -replace '/', '\\')
    $rawText = Get-Content -LiteralPath $absolutePath -Raw
    if (-not $buildInputName.Equals('sources', [System.StringComparison]::OrdinalIgnoreCase)) {
        $buildDescriptionIndex++
        $buildDescriptions.Add([pscustomobject]@{
            build_description_id = ('MVDM-BUILD-{0:D4}' -f $buildDescriptionIndex)
            package_root = $buildInput.package_root
            build_control_path = $buildInput.target_path
            input_kind = 'non-sources-build-control'
            target_name = ''
            target_type = ''
            includes_raw = ''
            sources_raw = ''
            declared_source_count = 0
            selected_source_file_ids = ''
            parser_state = 'recorded-not-parsed-as-nt-sources'
            evidence = $buildInput.target_path
        })
        continue
    }

    $flattened = [regex]::Replace($rawText, '\\\s*\r?\n\s*', ' ')
    function Get-OriginalBuildValue([string]$Name) {
        $match = [regex]::Match($flattened, '(?mi)^\s*' + [regex]::Escape($Name) + '\s*=\s*(.*)$')
        if ($match.Success) { return $match.Groups[1].Value.Trim() }
        return ''
    }
    $targetName = Get-OriginalBuildValue 'TARGETNAME'
    $targetType = Get-OriginalBuildValue 'TARGETTYPE'
    $includesRaw = Get-OriginalBuildValue 'INCLUDES'
    $sourcesRaw = Get-OriginalBuildValue 'SOURCES'
    $selectedSourceIds = New-Object System.Collections.Generic.List[string]
    foreach ($tokenMatch in [regex]::Matches($sourcesRaw, '(?i)[A-Za-z0-9_./\\-]+\.(c|cc|cpp|asm|s)\b')) {
        $declaredSourceTokens++
        $candidate = Normalize-RelativePath ((Split-Path -Parent $buildInput.target_path) + '/' + $tokenMatch.Value)
        $candidateKey = $candidate.ToLowerInvariant()
        if ($byPath.ContainsKey($candidateKey) -and $byPath[$candidateKey].file_kind -eq 'source') {
            $selectedSourceIds.Add($byPath[$candidateKey].file_id)
        }
    }
    $buildDescriptionIndex++
    $buildDescriptionId = ('MVDM-BUILD-{0:D4}' -f $buildDescriptionIndex)
    foreach ($sourceId in ($selectedSourceIds | Select-Object -Unique)) {
        if (-not $sourceToDescriptions.ContainsKey($sourceId)) {
            $sourceToDescriptions[$sourceId] = New-Object System.Collections.Generic.List[string]
        }
        $sourceToDescriptions[$sourceId].Add($buildDescriptionId)
    }
    $buildDescriptions.Add([pscustomobject]@{
        build_description_id = $buildDescriptionId
        package_root = $buildInput.package_root
        build_control_path = $buildInput.target_path
        input_kind = 'original-nt-sources'
        target_name = $targetName
        target_type = $targetType
        includes_raw = $includesRaw
        sources_raw = $sourcesRaw
        declared_source_count = $selectedSourceIds.Count
        selected_source_file_ids = ($selectedSourceIds | Select-Object -Unique) -join ';'
        parser_state = 'literal-macro-unexpanded'
        evidence = $buildInput.target_path
    })
}
$translationUnits = foreach ($source in $sourceFiles) {
    $descriptionIds = if ($sourceToDescriptions.ContainsKey($source.file_id)) {
        ($sourceToDescriptions[$source.file_id] | Select-Object -Unique) -join ';'
    } else { '' }
    [pscustomobject]@{
        file_id = $source.file_id
        target_path = $source.target_path
        package_root = $source.package_root
        original_build_description_ids = $descriptionIds
        original_build_membership = if ([string]::IsNullOrEmpty($descriptionIds)) { 'not-observed-in-literal-sources' } else { 'observed-in-literal-sources' }
        build_state = 'unreviewed'
        evidence = 'm0-t276-s2-original-build-description-ledger.tsv'
    }
}
$buildDescriptions | Export-Csv -LiteralPath $buildDescriptionPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$translationUnits | Export-Csv -LiteralPath $translationUnitPath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$edges = New-Object System.Collections.Generic.List[object]
$edgeIndex = 0
foreach ($source in $sourceFiles) {
    $absolutePath = Join-Path $mirrorRoot ($source.target_path -replace '/', '\\')
    if (-not (Test-Path -LiteralPath $absolutePath -PathType Leaf)) {
        throw "Verified source mirror is missing: $($source.target_path)"
    }
    $lineNumber = 0
    foreach ($line in (Get-Content -LiteralPath $absolutePath)) {
        $lineNumber++
        $match = [regex]::Match($line, '^\s*#\s*include\s*([<\"])([^>\"]+)[>\"]')
        if (-not $match.Success) { continue }
        $delimiter = $match.Groups[1].Value
        $spelling = $match.Groups[2].Value.Trim()
        $resolution = Classify-SystemInclude $spelling $delimiter
        $resolvedPath = ''
        $resolvedPackage = ''
        if ($delimiter -eq '"') {
            $candidate = Normalize-RelativePath ((Split-Path -Parent $source.target_path) + '/' + $spelling)
            $candidateKey = $candidate.ToLowerInvariant()
            if ($byPath.ContainsKey($candidateKey)) {
                $resolution = 'selected-mvdm-relative'
                $resolvedPath = $byPath[$candidateKey].target_path
                $resolvedPackage = $byPath[$candidateKey].package_root
            }
        }
        $edgeIndex++
        $edges.Add([pscustomobject]@{
            edge_id = ('MVDM-INCLUDE-{0:D5}' -f $edgeIndex)
            caller_file_id = $source.file_id
            caller_path = $source.target_path
            caller_package = $source.package_root
            source_line = $lineNumber
            delimiter = $delimiter
            include_spelling = $spelling
            textual_resolution = $resolution
            resolved_selected_path = $resolvedPath
            resolved_selected_package = $resolvedPackage
            evidence = $source.target_path + ':' + $lineNumber
        })
    }
}

$edges | Export-Csv -LiteralPath $edgePath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$packageRows = Import-Csv -LiteralPath $packageLedgerPath -Delimiter "`t" | Sort-Object package_root
$profileRows = Import-Csv -LiteralPath $profileLedgerPath -Delimiter "`t" | Sort-Object package_root
foreach ($package in $packageRows) {
    $packageSources = @($sourceFiles | Where-Object { $_.package_root -eq $package.package_root })
    $packageBuildInputs = @($buildInputs | Where-Object { $_.package_root -eq $package.package_root })
    $packageEdges = @($edges | Where-Object { $_.caller_package -eq $package.package_root })
    $package.dependency_state = 'observed-textual-include-graph'
    $package.evidence = ('m0-t276-s2-translation-unit-include-ledger.tsv; source={0}; build-input={1}; include={2}' -f $packageSources.Count, $packageBuildInputs.Count, $packageEdges.Count)
}
foreach ($profile in $profileRows) {
    $packageSources = @($sourceFiles | Where-Object { $_.package_root -eq $profile.package_root })
    $packageBuildInputs = @($buildInputs | Where-Object { $_.package_root -eq $profile.package_root })
    $profile.original_inputs = ('observed-source={0}; observed-build-control={1}' -f $packageSources.Count, $packageBuildInputs.Count)
    $profile.include_and_library_closure = 'observed-textual-includes; library-closure-unreviewed'
    $profile.profile_state = 'unreviewed'
}
$packageRows | Export-Csv -LiteralPath $packageLedgerPath -Delimiter "`t" -NoTypeInformation -Encoding utf8
$profileRows | Export-Csv -LiteralPath $profileLedgerPath -Delimiter "`t" -NoTypeInformation -Encoding utf8

$resolutionCounts = @{}
foreach ($edge in $edges) {
    if (-not $resolutionCounts.ContainsKey($edge.textual_resolution)) { $resolutionCounts[$edge.textual_resolution] = 0 }
    $resolutionCounts[$edge.textual_resolution]++
}
$packageEdgeCounts = $edges | Group-Object caller_package | Sort-Object Name
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('# M0 T276 S2 original build-input and include-graph evidence')
$lines.Add('')
$lines.Add('## Scope and method')
$lines.Add('')
$lines.Add('This evidence parses only byte-exact selected mirror files. It inventories source translation units, selected original build-control files named `dirs`, `makefil0`, `makefile` or `sources`, and literal C/C++ preprocessor include lines. A textual include is not a compiler-resolved include path, library dependency, link edge or interface-recovery conclusion.')
$lines.Add('')
$lines.Add('## Counts')
$lines.Add('')
$lines.Add('- Selected file-ledger rows: ' + $files.Count)
$lines.Add('- Source translation units: ' + $sourceFiles.Count)
$lines.Add('- Selected original build-control inputs: ' + $buildInputs.Count)
$lines.Add('- Original NT `sources` descriptions: ' + @($buildDescriptions | Where-Object { $_.input_kind -eq 'original-nt-sources' }).Count)
$lines.Add('- Literal declared source tokens: ' + $declaredSourceTokens)
$lines.Add('- Selected source files observed in a literal `sources` description: ' + @($translationUnits | Where-Object { $_.original_build_membership -eq 'observed-in-literal-sources' }).Count)
$lines.Add('- Selected source files not observed in a literal `sources` description: ' + @($translationUnits | Where-Object { $_.original_build_membership -eq 'not-observed-in-literal-sources' }).Count)
$lines.Add('- Literal include edges: ' + $edges.Count)
$lines.Add('- Package roots: ' + $packageRows.Count)
$lines.Add('')
$lines.Add('### Textual include classifications')
$lines.Add('')
foreach ($name in ($resolutionCounts.Keys | Sort-Object)) { $lines.Add('- `' + $name + '`: ' + $resolutionCounts[$name]) }
$lines.Add('')
$lines.Add('### Include edges by caller package')
$lines.Add('')
foreach ($group in $packageEdgeCounts) { $lines.Add('- `' + $group.Name + '`: ' + $group.Count) }
$lines.Add('')
$lines.Add('## Limits')
$lines.Add('')
$lines.Add('- Quoted include resolution is intentionally limited to a normalized path relative to the caller that exactly matches a selected mirror path.')
$lines.Add('- Angle-bracket classification is lexical and does not claim a currently available SDK declaration.')
$lines.Add('- Original build descriptions may use macros, generated sources, assembler tooling, conditional compilation and external library inputs that require S3 symbol/interface and later build-profile audit.')
$lines.Add('- `SOURCES` parsing is literal and macro-unexpanded: it is a historical input record, not a complete target recipe.')
$lines.Add('- No final composition classification, adapter requirement, x86/x64 build decision or enabled package follows from this extraction.')
[System.IO.File]::WriteAllLines($evidencePath, $lines)

Write-Host "Observed $($sourceFiles.Count) translation units, $($buildInputs.Count) build-control inputs and $($edges.Count) textual include edges across $($packageRows.Count) package roots."
