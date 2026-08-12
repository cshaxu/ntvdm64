[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$OutputPath = 'docs/etc/operations/document-inventory.md'
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$docsRoot = Join-Path $repositoryRoot 'docs'
$outputFullPath = [System.IO.Path]::GetFullPath((Join-Path $repositoryRoot $OutputPath))

function Get-Classification([string]$relativePath) {
    if ($relativePath -in @('README.md', 'STATUS.md', 'QUEUE.md', 'TODO.md')) {
        return 'principal control document'
    }
    if ($relativePath -match '^rules/') {
        return 'current rule authority'
    }
    if ($relativePath -match '^design/(GOAL|ARCHITECTURE|CODING|UI|ROADMAP)\.md$') {
        return 'current design authority'
    }
    if ($relativePath -match '^history/') {
        return 'closed task history'
    }
    if ($relativePath -match '^etc/') {
        return 'indexed supporting material'
    }
    return 'retained pre-governance evidence'
}

$entries = Get-ChildItem -LiteralPath $docsRoot -Recurse -File |
    Where-Object { $_.Extension -in @('.md', '.json', '.tsv') } |
    Where-Object { $_.FullName -ne $outputFullPath } |
    Sort-Object FullName |
    ForEach-Object {
        $relativePath = $_.FullName.Substring($docsRoot.Length).TrimStart('\').Replace('\', '/')
        [pscustomobject]@{
            Path = $relativePath
            Classification = Get-Classification $relativePath
            Sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        }
    }

$lines = @(
    '# Documentation Inventory',
    '',
    '## Purpose',
    '',
    'This generated inventory is the migration-control record for every Markdown,',
    'JSON, and TSV documentation record',
    'document under docs/. It applies the ntvdm64 fixed-topology rule without',
    'silently moving or deleting retained evidence. Regenerate it with',
    'tools/Export-DocumentationInventory.ps1 whenever any governed document',
    'changes; the documentation gate rejects missing, added, renamed, or',
    'content-divergent entries.',
    '',
    '## Snapshot',
    '',
    "| Documentation records | $($entries.Count) |",
    "| --- | --- |",
    "| Generated UTC | $([DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ')) |",
    '',
    '## Entries',
    '',
    '| Path | Classification | SHA-256 |',
    '| --- | --- | --- |'
)

foreach ($entry in $entries) {
    $lines += "| $($entry.Path) | $($entry.Classification) | $($entry.Sha256) |"
}

Set-Content -LiteralPath $outputFullPath -Value $lines -Encoding utf8
Write-Host "Wrote documentation inventory for $($entries.Count) documentation files: $OutputPath"
