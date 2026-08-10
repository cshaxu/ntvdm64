[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}

$ErrorActionPreference = 'Stop'
$docs = Join-Path $RepositoryRoot 'docs'
$requiredFiles = @('README.md', 'STATUS.md', 'QUEUE.md', 'TODO.md')
$requiredDirectories = @('rules', 'design', 'history', 'etc')
$requiredRules = @('DOCUMENT.md', 'EXECUTION.md', 'ARCHITECTURE.md', 'CODING.md')
$requiredDesign = @('GOAL.md', 'ARCHITECTURE.md', 'CODING.md', 'UI.md', 'ROADMAP.md')

if (-not (Test-Path -LiteralPath $docs -PathType Container)) {
    throw "Missing documentation root: $docs"
}

foreach ($name in $requiredFiles) {
    $path = Join-Path $docs $name
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing authority document: docs/$name" }
    if ((Get-Content -LiteralPath $path -Raw).Length -eq 0) { throw "Empty authority document: docs/$name" }
}
foreach ($name in $requiredDirectories) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs $name) -PathType Container)) { throw "Missing authority directory: docs/$name" }
}
foreach ($name in $requiredRules) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs "rules/$name") -PathType Leaf)) { throw "Missing rule: docs/rules/$name" }
}
foreach ($name in $requiredDesign) {
    if (-not (Test-Path -LiteralPath (Join-Path $docs "design/$name") -PathType Leaf)) { throw "Missing design authority: docs/design/$name" }
}

$status = Get-Content -LiteralPath (Join-Path $docs 'STATUS.md') -Raw
foreach ($heading in @('## Current Work', '## Active Packet', '## Current Technical Baseline')) {
    if ($status -notmatch [regex]::Escape($heading)) { throw "STATUS.md is missing '$heading'" }
}

$documentRules = Get-Content -LiteralPath (Join-Path $docs 'rules/DOCUMENT.md') -Raw
if ($documentRules -notmatch 'Migration Exception') { throw 'DOCUMENT.md must define the controlled migration exception.' }
if (-not (Test-Path -LiteralPath (Join-Path $docs 'etc/operations/governance-migration.md') -PathType Leaf)) {
    throw 'Missing governance migration inventory.'
}

Write-Host 'Documentation governance verification passed.'
