[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [switch]$SelfTest
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

$ErrorActionPreference = 'Stop'
$docs = (Resolve-Path -LiteralPath (Join-Path $RepositoryRoot 'docs')).Path
$requiredFiles = @('README.md', 'states/CURRENT.md', 'states/QUEUE.md', 'states/TODO.md')
$requiredDirectories = @('rules', 'design', 'proposals', 'history', 'states', 'etc')
$requiredRules = @('DOCUMENT.md', 'EXECUTION.md', 'ARCHITECTURE.md', 'CODING.md')
$requiredDesign = @('GOAL.md', 'ARCHITECTURE.md', 'CODING.md', 'UI.md', 'ROADMAP.md')

function Assert-ExactNames([string[]]$Actual, [string[]]$Expected, [string]$Label) {
    if (@(Compare-Object ($Expected | Sort-Object) ($Actual | Sort-Object)).Count -ne 0) {
        throw "$Label has an invalid path set."
    }
}

function Get-MarkdownHeadings([string]$Text) {
    $inFence = $false
    foreach ($line in [regex]::Split($Text, "`r?`n")) {
        if ($line -match '^\s*```') { $inFence = -not $inFence; continue }
        if ($inFence) { continue }
        $match = [regex]::Match($line, '^(?<marks>#{1,6})[ \t]+(?<text>.+?)\s*$')
        if ($match.Success) { [pscustomobject]@{ Level = $match.Groups['marks'].Value.Length; Text = $match.Groups['text'].Value } }
    }
}

function Get-MarkdownAnchor([string]$Heading) {
    $anchor = [regex]::Replace($Heading.ToLowerInvariant(), '[^a-z0-9 _-]', '')
    return ([regex]::Replace([regex]::Replace($anchor, '\s+', '-'), '-+', '-')).Trim('-')
}

function Get-MarkdownLinks([string]$Text) {
    $inFence = $false
    foreach ($line in [regex]::Split($Text, "`r?`n")) {
        if ($line -match '^\s*```') { $inFence = -not $inFence; continue }
        if ($inFence) { continue }
        foreach ($match in [regex]::Matches($line, '\[[^\]]*\]\((?<destination><[^>]+>|[^)\s]+)(?:\s+[^)]*)?\)')) {
            $value = $match.Groups['destination'].Value.Trim('<', '>')
            if (-not [string]::IsNullOrWhiteSpace($value)) { $value }
        }
    }
}

function Get-LinkTarget([string]$Source, [string]$Destination) {
    $parts = $Destination.Split('#', 2)
    $path = if ([string]::IsNullOrWhiteSpace($parts[0])) { $Source } else { [System.IO.Path]::GetFullPath((Join-Path (Split-Path -Parent $Source) $parts[0])) }
    [pscustomobject]@{ Path = $path; Fragment = if ($parts.Count -eq 2) { $parts[1] } else { '' } }
}

function Assert-RelativeLinks([string[]]$MarkdownPaths) {
    foreach ($path in $MarkdownPaths) {
        foreach ($destination in @(Get-MarkdownLinks (Get-Content -Raw -LiteralPath $path))) {
            if ($destination -match '^[a-z][a-z0-9+.-]*:' -or $destination.StartsWith('//')) { continue }
            $target = Get-LinkTarget $path $destination
            if (-not (Test-Path -LiteralPath $target.Path)) { throw "Markdown link in $path targets a missing path: $destination" }
            if (-not [string]::IsNullOrWhiteSpace($target.Fragment)) {
                if ($target.Path -notmatch '\.md$') { throw "Markdown link in $path uses an anchor on a non-Markdown target: $destination" }
                $anchors = @(Get-MarkdownHeadings (Get-Content -Raw -LiteralPath $target.Path) | ForEach-Object { Get-MarkdownAnchor $_.Text })
                if ($anchors -notcontains $target.Fragment.ToLowerInvariant()) { throw "Markdown link in $path targets a missing anchor: $destination" }
            }
        }
    }
}

if ($SelfTest) {
    $sample = "# Title`n`n## Kept"
    if (@(Get-MarkdownHeadings $sample).Count -ne 2 -or (Get-MarkdownAnchor 'A / B') -ne 'a-b') {
        throw 'Documentation governance self-test failed: Markdown parser contract.'
    }
    $rejected = $false
    try { Assert-ExactNames @('unexpected.md') @('README.md') 'self-test' } catch { $rejected = $true }
    if (-not $rejected) { throw 'Documentation governance self-test failed: invalid topology was accepted.' }
    Write-Host 'Documentation governance self-test passed.'
}

if (-not (Test-Path -LiteralPath $docs -PathType Container)) {
    throw "Missing documentation root: $docs"
}

foreach ($name in $requiredFiles) {
    $path = Join-Path $docs $name
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Missing authority document: docs/$name"
    }
    if ((Get-Content -LiteralPath $path -Raw).Length -eq 0) {
        throw "Empty authority document: docs/$name"
    }
}
foreach ($name in $requiredDirectories) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs $name) -PathType Container)) {
        throw "Missing authority directory: docs/$name"
    }
}
Assert-ExactNames @(Get-ChildItem -LiteralPath $docs -File | ForEach-Object Name) @('README.md') 'docs/'
Assert-ExactNames @(Get-ChildItem -LiteralPath $docs -Directory | ForEach-Object Name) @('design', 'etc', 'history', 'proposals', 'rules', 'states') 'docs/'
Assert-ExactNames @(Get-ChildItem -LiteralPath (Join-Path $docs 'rules') -File | ForEach-Object Name) $requiredRules 'docs/rules/'
Assert-ExactNames @(Get-ChildItem -LiteralPath (Join-Path $docs 'design') -File | ForEach-Object Name) $requiredDesign 'docs/design/'
Assert-ExactNames @(Get-ChildItem -LiteralPath (Join-Path $docs 'states') -File | ForEach-Object Name) @('CURRENT.md', 'QUEUE.md', 'TODO.md') 'docs/states/'
foreach ($directory in @('rules', 'design', 'states', 'history')) {
    if (@(Get-ChildItem -LiteralPath (Join-Path $docs $directory) -Directory).Count -ne 0) {
        throw "docs/$directory/ must not contain subdirectories."
    }
}
foreach ($name in $requiredRules) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs "rules/$name") -PathType Leaf)) {
        throw "Missing rule: docs/rules/$name"
    }
}
foreach ($name in $requiredDesign) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs "design/$name") -PathType Leaf)) {
        throw "Missing design authority: docs/design/$name"
    }
}

Get-ChildItem -LiteralPath (Join-Path $docs 'history') -File -Filter '*.md' | ForEach-Object {
    if ($_.Name -notmatch '^m\d+-(?:t\d+|td)-.+\.md$') {
        throw "History record must use task-id and introduction naming: docs/history/$($_.Name)"
    }
}

$requiredTitles = @{
    'README.md' = 'Documentation Guide'; 'rules/ARCHITECTURE.md' = 'Architecture Rules';
    'rules/CODING.md' = 'Coding Rules'; 'rules/DOCUMENT.md' = 'Documentation Rules';
    'rules/EXECUTION.md' = 'Execution Rules'; 'design/ARCHITECTURE.md' = 'System Architecture';
    'design/CODING.md' = 'Source Layout'; 'design/GOAL.md' = 'Project Goal';
    'design/ROADMAP.md' = 'Roadmap'; 'design/UI.md' = 'Product UX';
    'states/CURRENT.md' = 'Project Status'; 'states/QUEUE.md' = 'Queue'; 'states/TODO.md' = 'Debt Ledger'
}
foreach ($relativePath in $requiredTitles.Keys) {
    $headings = @(Get-MarkdownHeadings (Get-Content -Raw -LiteralPath (Join-Path $docs $relativePath)))
    if ($headings.Count -eq 0 -or $headings[0].Level -ne 1 -or $headings[0].Text -ne $requiredTitles[$relativePath]) {
        throw "docs/$relativePath must begin with '# $($requiredTitles[$relativePath])'."
    }
    if ((@($headings | Where-Object Level -eq 1)).Count -ne 1) {
        throw "docs/$relativePath must contain exactly one level-one title."
    }
}

$statusPath = Join-Path $docs 'states/CURRENT.md'
if ((Get-Item -LiteralPath $statusPath).Length -gt 32768) {
    throw 'docs/states/CURRENT.md exceeds its 32 KiB control-plane limit.'
}
$status = Get-Content -LiteralPath $statusPath -Raw
foreach ($heading in @('## Current Work', '## Current Technical Baseline')) {
    if ($status -notmatch [regex]::Escape($heading)) {
        throw "states/CURRENT.md is missing '$heading'"
    }
}
$hasActivePacket = $status -match '\*\*Active:\s+(M\d+\s+T\d+\s+S\d+|Td\s+S\d+\s+P\d+)\b'
$hasExplicitIntermission = $status -match '\*\*No active (?:numeric )?M/T/S packet\.'
if (-not $hasActivePacket -and -not $hasExplicitIntermission) {
    throw 'states/CURRENT.md must identify an active M<milestone> T<task> S<subtask> packet, an active Td S<subtask> P<part> packet, or explicitly state that no M/T/S packet is active.'
}
if ($hasActivePacket -and $hasExplicitIntermission) {
    throw 'states/CURRENT.md cannot identify an active packet and a no-active-packet intermission simultaneously.'
}
if ($hasActivePacket) {
    $activePacketCount = [regex]::Matches($status, '(?m)^## Active Packet\s*$').Count
    if ($activePacketCount -ne 1) {
        throw "states/CURRENT.md must contain exactly one active packet; found $activePacketCount."
    }
    foreach ($field in @(
        'Identifier Mode', 'Admission And Approval', 'Objective', 'Non-goals',
        'Reference Baseline', 'Files And ABI Surface', 'Applicable Rules',
        'Verification', 'Expected Markers', 'Asset Needs',
        'Reporting Requirements', 'Stop Conditions', 'Exit Criteria',
        'Original Owner Request', 'Similar-Issue Sweep'
    )) {
        if ($status -notmatch ('(?m)^\|\s*' + [regex]::Escape($field) + '\s*\|\s*\S.+?\|\s*$')) {
            throw "states/CURRENT.md active packet is missing a non-empty '$field' row."
        }
    }
} elseif ([regex]::Matches($status, '(?m)^## Active Packet\s*$').Count -ne 0) {
    throw 'states/CURRENT.md must not retain an Active Packet section during an explicit intermission.'
}

$queue = Get-Content -LiteralPath (Join-Path $docs 'states/QUEUE.md') -Raw
if ($queue -match '\bT\d+\b') {
    throw 'QUEUE.md must contain only unnumbered candidate T packages; numeric T identifiers belong in STATUS.md at admission.'
}
if ($queue -match '\bS\d+\b|\bP\d+\b') {
    throw 'QUEUE.md must not contain S or P identifiers.'
}
$queueRows = @([regex]::Matches($queue, '(?m)^\|\s*\d+\s*\|.+\|\s*\[[^\]]+\]\(\.\./proposals/[^)]+\.md\)\s*\|\s*$'))
if ($queueRows.Count -ne 3) { throw 'Every Queue candidate row must link one proposal file.' }
foreach ($proposal in @(Get-ChildItem -LiteralPath (Join-Path $docs 'proposals') -File -Filter '*.md')) {
    if ($proposal.Name -match 'proposal-(?:wow16-single-process-lifecycle-recovery|cross-process-broker-closure|multiprocess-release-matrix)-001\.md' -and $queue -notmatch [regex]::Escape($proposal.Name)) {
        throw "Current proposal is not linked by Queue: $($proposal.Name)"
    }
}
$todo = Get-Content -Raw -LiteralPath (Join-Path $docs 'states/TODO.md')
if ($todo -notmatch '(?m)^\|\s*Priority\s*\|\s*Debt\s*\|\s*Admission path\s*\|\s*$') { throw 'TODO.md must retain the priority, debt, and admission-path table.' }

$documentRules = Get-Content -LiteralPath (Join-Path $docs 'rules/DOCUMENT.md') -Raw
if ($documentRules -notmatch 'Migration Exception') {
    throw 'DOCUMENT.md must define the controlled migration exception.'
}
foreach ($record in @('etc/operations/policy/source-policy.md')) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs $record) -PathType Leaf)) {
        throw "Missing governance record: docs/$record"
    }
}

$etc = Join-Path $docs 'etc'
$etcIndex = Join-Path $etc 'README.md'
if (-not (Test-Path -LiteralPath $etcIndex -PathType Leaf)) {
    throw 'docs/etc/ must contain README.md index.'
}
$indexedSupportingPaths = @(
    (Get-MarkdownLinks (Get-Content -Raw -LiteralPath $etcIndex)) |
    Where-Object { $_ -notmatch '^[a-z][a-z0-9+.-]*:' } |
    ForEach-Object { (Get-LinkTarget $etcIndex $_).Path }
)
Get-ChildItem -LiteralPath $etc -Recurse -File -Filter '*.md' |
    Where-Object { $_.FullName -ne (Resolve-Path -LiteralPath $etcIndex).Path } |
    ForEach-Object {
        if ($indexedSupportingPaths -notcontains $_.FullName) {
            throw "Supporting record is not indexed by docs/etc/README.md: $($_.FullName)"
        }
    }

$markdownPaths = @(
    Get-Item -LiteralPath (Join-Path $RepositoryRoot 'README.md') -ErrorAction SilentlyContinue
    Get-Item -LiteralPath (Join-Path $RepositoryRoot 'AGENTS.md') -ErrorAction SilentlyContinue
    Get-Item -LiteralPath (Join-Path $RepositoryRoot 'CONTRIBUTING.md') -ErrorAction SilentlyContinue
    Get-ChildItem -LiteralPath $docs -Recurse -File -Filter '*.md'
) | Where-Object { $null -ne $_ } | Sort-Object FullName -Unique
foreach ($markdownPath in $markdownPaths) {
    try {
        $utf8Text = [System.IO.File]::ReadAllText($markdownPath.FullName, [System.Text.UTF8Encoding]::new($false, $true))
    } catch {
        throw "Documentation is not valid UTF-8: $($markdownPath.FullName)"
    }
    if ($utf8Text -match '\u00E2|\u00C3|\uFFFD') {
        throw "Documentation contains encoding corruption: $($markdownPath.FullName)"
    }
}
Assert-RelativeLinks @($markdownPaths | ForEach-Object FullName)

$archivedStatusLedger = Join-Path $RepositoryRoot 'artifacts/documentation-archive/20260910/etc/evidence/m0-t95-status-ledger-20260811.md'
if (-not (Test-Path -LiteralPath $archivedStatusLedger -PathType Leaf)) {
    throw 'Missing archived M0 T95 status ledger.'
}

Write-Host 'Documentation governance verification passed.'
