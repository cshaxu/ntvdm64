[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
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

$status = Get-Content -LiteralPath (Join-Path $docs 'states/CURRENT.md') -Raw
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
        if ($status -notmatch [regex]::Escape("| $field |")) {
            throw "states/CURRENT.md active packet is missing '$field'."
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

$documentRules = Get-Content -LiteralPath (Join-Path $docs 'rules/DOCUMENT.md') -Raw
if ($documentRules -notmatch 'Migration Exception') {
    throw 'DOCUMENT.md must define the controlled migration exception.'
}
foreach ($record in @('etc/operations/policy/source-policy.md')) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs $record) -PathType Leaf)) {
        throw "Missing governance record: docs/$record"
    }
}

$archivedStatusLedger = Join-Path $RepositoryRoot 'artifacts/documentation-archive/20260910/etc/evidence/m0-t95-status-ledger-20260811.md'
if (-not (Test-Path -LiteralPath $archivedStatusLedger -PathType Leaf)) {
    throw 'Missing archived M0 T95 status ledger.'
}

Write-Host 'Documentation governance verification passed.'
