param(
    [Parameter(Mandatory = $true)]
    [string]$CandidatePath,

    [Parameter(Mandatory = $true)]
    [string]$OutputDirectory,

    [string]$ProvenanceNote = ''
)

$ErrorActionPreference = 'Stop'

$candidate = Get-Item -LiteralPath $CandidatePath -ErrorAction Stop
if ($candidate.PSIsContainer) {
    throw "CandidatePath must name one archive, map, listing, build log, or generator input file: $CandidatePath"
}

$llvmBin = 'C:\Program Files\LLVM\bin'
$llvmAr = Join-Path $llvmBin 'llvm-ar.exe'
$llvmNm = Join-Path $llvmBin 'llvm-nm.exe'
$llvmReadObj = Join-Path $llvmBin 'llvm-readobj.exe'

New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
$outputRoot = (Resolve-Path -LiteralPath $OutputDirectory).Path
$hash = (Get-FileHash -LiteralPath $candidate.FullName -Algorithm SHA256).Hash
$extension = $candidate.Extension.ToLowerInvariant()
$isArchive = $extension -in @('.lib', '.a')
$members = @()
$machine = @()
$definedSymbols = @()
$undefinedSymbols = @()
$textHits = @()

if ($isArchive) {
    if (-not (Test-Path -LiteralPath $llvmAr -PathType Leaf) -or
        -not (Test-Path -LiteralPath $llvmNm -PathType Leaf) -or
        -not (Test-Path -LiteralPath $llvmReadObj -PathType Leaf)) {
        throw 'LLVM archive inspection tools were not found under C:\Program Files\LLVM\bin.'
    }

    $members = @(& $llvmAr t $candidate.FullName)
    $machine = @(& $llvmReadObj --file-headers $candidate.FullName 2>&1 |
        Select-String -Pattern 'Machine:' |
        ForEach-Object { $_.ToString().Trim() } |
        Sort-Object -Unique)
    $nm = @(& $llvmNm --format=posix --defined-only $candidate.FullName 2>&1)
    $definedSymbols = @($nm | ForEach-Object { ($_ -split '\s+')[0] } |
        Where-Object { $_ -and $_ -notmatch '^(.*:|.*\.obj:)$' } |
        Sort-Object -Unique)
    $undefinedOutput = @(& $llvmNm --format=posix --undefined-only $candidate.FullName 2>&1)
    $undefinedSymbols = @($undefinedOutput | ForEach-Object { ($_ -split '\s+')[0] } |
        Where-Object { $_ -and $_ -notmatch '^(.*:|.*\.obj:)$' } |
        Sort-Object -Unique)
} else {
    $keywords = 'ccpu486\.lib|ccpu386\.lib|cvidc\.lib|EvPtrs\.edl|Vglob\.edl|Evid\.edl|host[/\\]genPg|C_Video|\bVideo\b|c_effective_addr|c_sas_touch|c_VirtualiseInstruction'
    $textHits = @(Select-String -LiteralPath $candidate.FullName -Pattern $keywords -AllMatches -ErrorAction SilentlyContinue |
        ForEach-Object {
            [pscustomobject]@{
                Line = $_.LineNumber
                Text = $_.Line.Trim()
            }
        })
}

$watchSymbols = @('Video', 'C_Video', 'c_effective_addr', 'c_sas_touch', 'c_VirtualiseInstruction')
$watchSummary = foreach ($symbol in $watchSymbols) {
    [pscustomobject]@{
        Symbol = $symbol
        Defined = @($definedSymbols | Where-Object { $_ -match "(^|_)$([regex]::Escape($symbol))$" }).Count -gt 0
        Undefined = @($undefinedSymbols | Where-Object { $_ -match "(^|_)$([regex]::Escape($symbol))$" }).Count -gt 0
        TextMentions = @($textHits | Where-Object { $_.Text -match [regex]::Escape($symbol) }).Count
    }
}

$record = [pscustomobject]@{
    Schema = 'runner.softpc-composition-candidate.v1'
    InspectedAtUtc = [DateTime]::UtcNow.ToString('o')
    Candidate = [pscustomobject]@{
        Path = $candidate.FullName
        Name = $candidate.Name
        SizeBytes = $candidate.Length
        Sha256 = $hash
        Extension = $extension
        ProvenanceNote = $ProvenanceNote
    }
    Classification = if ($isArchive) { 'archive' } else { 'text-or-generator-evidence' }
    ArchiveMembers = @($members)
    MachineEvidence = @($machine)
    DefinedSymbols = @($definedSymbols)
    UndefinedSymbols = @($undefinedSymbols)
    TextHits = @($textHits)
    WatchSymbols = @($watchSummary)
    Admission = 'Inspection only. This record does not admit the candidate to an OpenNT component, CMake, a fixture, or runtime.'
}

$stem = [IO.Path]::GetFileNameWithoutExtension($candidate.Name)
$jsonPath = Join-Path $outputRoot "$stem.softpc-candidate.json"
$markdownPath = Join-Path $outputRoot "$stem.softpc-candidate.md"
$record | ConvertTo-Json -Depth 7 | Set-Content -LiteralPath $jsonPath -Encoding utf8

$lines = [System.Collections.Generic.List[string]]::new()
$lines.Add('# SoftPC Composition Candidate Inspection')
$lines.Add('')
$lines.Add('This is an evidence record only. It does not authorize use in any build or runtime path.')
$lines.Add('')
$lines.Add("- Candidate: ``$($candidate.FullName)``")
$lines.Add("- SHA-256: ``$hash``")
$lines.Add("- Size: $($candidate.Length) bytes")
$lines.Add("- Classification: $($record.Classification)")
if ($ProvenanceNote) { $lines.Add("- Provenance note: $ProvenanceNote") }
if ($machine.Count) {
    $lines.Add('')
    $lines.Add('## Machine Evidence')
    foreach ($entry in $machine) { $lines.Add("- ``$entry``") }
}
if ($members.Count) {
    $lines.Add('')
    $lines.Add("## Archive Members ($($members.Count))")
    foreach ($member in $members) { $lines.Add("- ``$member``") }
}
$lines.Add('')
$lines.Add('## Watched Symbols')
$lines.Add('')
$lines.Add('| Symbol | Defined | Undefined | Text mentions |')
$lines.Add('| --- | --- | --- | --- |')
foreach ($entry in $watchSummary) {
    $lines.Add("| ``$($entry.Symbol)`` | $($entry.Defined) | $($entry.Undefined) | $($entry.TextMentions) |")
}
if ($textHits.Count) {
    $lines.Add('')
    $lines.Add('## Relevant Text Lines')
    foreach ($hit in $textHits) { $lines.Add("- line $($hit.Line): ``$($hit.Text)``") }
}
$lines.Add('')
$lines.Add('Next step: perform the documented source-policy review; do not copy the candidate into an OpenNT component before that review.')
$lines | Set-Content -LiteralPath $markdownPath -Encoding utf8

[pscustomobject]@{
    Json = $jsonPath
    Markdown = $markdownPath
    Classification = $record.Classification
    Sha256 = $hash
    ArchiveMemberCount = $members.Count
} | ConvertTo-Json
