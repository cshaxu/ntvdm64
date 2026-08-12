[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}

$ErrorActionPreference = 'Stop'
$docs = (Resolve-Path -LiteralPath (Join-Path $RepositoryRoot 'docs')).Path
$requiredFiles = @('README.md', 'STATUS.md', 'QUEUE.md', 'TODO.md')
$requiredDirectories = @('rules', 'design', 'history', 'etc')
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

$status = Get-Content -LiteralPath (Join-Path $docs 'STATUS.md') -Raw
foreach ($heading in @('## Current Work', '## Active Packet', '## Current Technical Baseline')) {
    if ($status -notmatch [regex]::Escape($heading)) {
        throw "STATUS.md is missing '$heading'"
    }
}
if ($status -notmatch '\*\*Active:\s+M\d+\s+T\d+\s+S\d+\b') {
    throw 'STATUS.md must identify the active packet as M<milestone> T<task> S<subtask>.'
}
$activePacketCount = [regex]::Matches($status, '(?m)^## Active Packet\s*$').Count
if ($activePacketCount -ne 1) {
    throw "STATUS.md must contain exactly one active packet; found $activePacketCount."
}
foreach ($field in @(
    'Identifier Mode', 'Admission And Approval', 'Objective', 'Non-goals',
    'Reference Baseline', 'Files And ABI Surface', 'Applicable Rules',
    'Verification', 'Expected Markers', 'Asset Needs',
    'Reporting Requirements', 'Stop Conditions', 'Exit Criteria',
    'Original Owner Request', 'Similar-Issue Sweep'
)) {
    if ($status -notmatch [regex]::Escape("| $field |")) {
        throw "STATUS.md active packet is missing '$field'."
    }
}

$queue = Get-Content -LiteralPath (Join-Path $docs 'QUEUE.md') -Raw
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
foreach ($record in @(
    'etc/operations/governance-migration.md',
    'etc/operations/task-identifier-governance.md',
    'etc/operations/m0-t95-subtask-plan.md',
    'etc/evidence/m0-t95-status-ledger-20260811.md'
)) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs $record) -PathType Leaf)) {
        throw "Missing governance record: docs/$record"
    }
}

$inventoryPath = Join-Path $docs 'etc/operations/document-inventory.md'
if (-not (Test-Path -LiteralPath $inventoryPath -PathType Leaf)) {
    throw 'Missing exact-file documentation inventory.'
}
$inventory = Get-Content -LiteralPath $inventoryPath -Raw
$inventoryEntries = @{}
foreach ($match in [regex]::Matches(
    $inventory,
    '(?m)^\| (?<path>[^|]+) \| (?<classification>[^|]+) \| (?<hash>[a-f0-9]{64}) \|\s*$'
)) {
    $path = $match.Groups['path'].Value.Trim().Replace('/', '\')
    if ($inventoryEntries.ContainsKey($path)) {
        throw "Documentation inventory has a duplicate path: $path"
    }
    $inventoryEntries[$path] = $match.Groups['hash'].Value
}

$inventoryRelativePath = 'etc\operations\document-inventory.md'
$actualDocuments = @{}
foreach ($document in Get-ChildItem -LiteralPath $docs -Recurse -File |
    Where-Object { $_.Extension -in @('.md', '.json', '.tsv') }) {
    $relativePath = $document.FullName.Substring($docs.Length).TrimStart('\')
    if ($relativePath -eq $inventoryRelativePath) {
        continue
    }
    if ($actualDocuments.ContainsKey($relativePath)) {
        throw "Duplicate documentation path after normalization: $relativePath"
    }
    $actualDocuments[$relativePath] = (Get-FileHash -LiteralPath $document.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
}

$missingInventoryEntries = [System.Collections.Generic.List[string]]::new()
foreach ($entryPath in $actualDocuments.Keys) {
    if (-not $inventoryEntries.ContainsKey([string]$entryPath)) {
        $missingInventoryEntries.Add([string]$entryPath)
    }
}
$staleInventoryEntries = [System.Collections.Generic.List[string]]::new()
foreach ($entryPath in $inventoryEntries.Keys) {
    if (-not $actualDocuments.ContainsKey([string]$entryPath)) {
        $staleInventoryEntries.Add([string]$entryPath)
    }
}
if ($missingInventoryEntries.Count -gt 0 -or $staleInventoryEntries.Count -gt 0) {
    throw 'Documentation inventory does not exactly match the documentation file set. Regenerate it with tools/Export-DocumentationInventory.ps1.'
}
foreach ($path in $actualDocuments.Keys) {
    if ($inventoryEntries[$path] -ne $actualDocuments[$path]) {
        throw "Documentation inventory hash is stale for: $path. Regenerate it with tools/Export-DocumentationInventory.ps1."
    }
}

Write-Host 'Documentation governance verification passed.'
