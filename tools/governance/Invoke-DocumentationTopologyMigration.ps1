[CmdletBinding()]
param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$repositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$docsRoot = Join-Path $repositoryRoot 'docs'
$map = @{}

function Add-Mapping([string]$source, [string]$destination) {
    $sourceFull = [IO.Path]::GetFullPath((Join-Path $repositoryRoot $source))
    $destinationFull = [IO.Path]::GetFullPath((Join-Path $repositoryRoot $destination))
    if (-not (Test-Path -LiteralPath $sourceFull -PathType Leaf) -and -not (Test-Path -LiteralPath $destinationFull -PathType Leaf)) { throw "Missing migration source and destination: $source" }
    if ($map.ContainsKey($sourceFull)) { throw "Duplicate migration source: $source" }
    if ($map.Values -contains $destinationFull) { throw "Duplicate migration destination: $destination" }
    if ((Test-Path -LiteralPath $destinationFull) -and (Test-Path -LiteralPath $sourceFull)) { throw "Migration destination already exists: $destination" }
    $map[$sourceFull] = $destinationFull
}

function Add-TreeMapping([string]$sourceDirectory, [string]$destinationDirectory) {
    $sourceRoot = Join-Path $repositoryRoot $sourceDirectory
    $destinationRoot = Join-Path $repositoryRoot $destinationDirectory
    $files = if (Test-Path -LiteralPath $sourceRoot) { @(Get-ChildItem -LiteralPath $sourceRoot -Recurse -File) } else { @() }
    if ($files.Count -eq 0 -and (Test-Path -LiteralPath $destinationRoot)) {
        foreach ($file in Get-ChildItem -LiteralPath $destinationRoot -Recurse -File) {
            $relative = $file.FullName.Substring($destinationRoot.Length).TrimStart('\')
            Add-Mapping (Join-Path $sourceDirectory $relative) (Join-Path $destinationDirectory $relative)
        }
    } else {
        foreach ($file in $files) {
            $relative = $file.FullName.Substring($sourceRoot.Length).TrimStart('\')
            Add-Mapping (Join-Path $sourceDirectory $relative) (Join-Path $destinationDirectory $relative)
        }
    }
}

function Get-RelativePath([string]$fromDirectory, [string]$toPath) {
    $fromUri = [Uri]::new($fromDirectory.TrimEnd('\') + '\')
    $toUri = [Uri]::new($toPath)
    return [Uri]::UnescapeDataString($fromUri.MakeRelativeUri($toUri).ToString())
}

Add-Mapping 'docs/STATUS.md' 'docs/states/CURRENT.md'
Add-Mapping 'docs/QUEUE.md' 'docs/states/QUEUE.md'
Add-Mapping 'docs/TODO.md' 'docs/states/TODO.md'
Add-Mapping 'docs/history/status-ledger-snapshot-20260910.md' 'docs/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md'
Add-Mapping 'docs/history/task-index.md' 'docs/etc/archive/task-index-legacy.md'

foreach ($file in Get-ChildItem -LiteralPath $docsRoot -File) {
    if ($file.Name -notin @('README.md', 'STATUS.md', 'QUEUE.md', 'TODO.md')) {
        Add-Mapping (Join-Path 'docs' $file.Name) (Join-Path 'docs/etc/archive/pre-governance-root' $file.Name)
    }
}
foreach ($file in Get-ChildItem -LiteralPath (Join-Path $docsRoot 'etc/archive/pre-governance-root') -File -ErrorAction SilentlyContinue) {
    Add-Mapping (Join-Path 'docs' $file.Name) (Join-Path 'docs/etc/archive/pre-governance-root' $file.Name)
}

Add-TreeMapping 'docs/architecture' 'docs/etc/architecture/pre-governance'
Add-TreeMapping 'docs/governance' 'artifacts/documentation-archive/20260910/etc/operations/ledgers/pre-governance'
Add-TreeMapping 'docs/planning' 'docs/etc/archive/pre-governance-planning'
Add-TreeMapping 'docs/research' 'docs/etc/research/pre-governance'

foreach ($proposal in Get-ChildItem -LiteralPath (Join-Path $docsRoot 'etc/operations') -File -Filter 'proposal-*.md') {
    Add-Mapping (Join-Path 'docs/etc/operations' $proposal.Name) (Join-Path 'docs/proposals' $proposal.Name)
}
foreach ($proposal in Get-ChildItem -LiteralPath (Join-Path $docsRoot 'proposals') -File -Filter 'proposal-*.md' -ErrorAction SilentlyContinue) {
    Add-Mapping (Join-Path 'docs/etc/operations' $proposal.Name) (Join-Path 'docs/proposals' $proposal.Name)
}

$markdown = @{}
foreach ($file in Get-ChildItem -LiteralPath $docsRoot -Recurse -File -Filter '*.md') {
    $markdown[$file.FullName] = [IO.File]::ReadAllText($file.FullName)
}

foreach ($entry in $map.GetEnumerator()) {
    if (-not $markdown.ContainsKey($entry.Key) -and (Test-Path -LiteralPath $entry.Value -PathType Leaf) -and ([IO.Path]::GetExtension($entry.Value) -eq '.md')) {
        $markdown[$entry.Key] = [IO.File]::ReadAllText($entry.Value)
    }
}

if (-not $Apply) {
    "Planned file moves: $($map.Count)"
    $map.GetEnumerator() | Sort-Object Key | ForEach-Object {
        "$($_.Key.Substring($repositoryRoot.Length + 1).Replace('\','/')) -> $($_.Value.Substring($repositoryRoot.Length + 1).Replace('\','/'))"
    }
    exit 0
}

foreach ($directory in @(
    'docs/states', 'docs/proposals', 'docs/etc/evidence/status-ledgers',
    'docs/etc/archive/pre-governance-root', 'docs/etc/archive/pre-governance-planning',
    'docs/etc/architecture/pre-governance', 'artifacts/documentation-archive/20260910/etc/operations/ledgers/pre-governance',
    'docs/etc/research/pre-governance'
)) {
    New-Item -ItemType Directory -Force -Path (Join-Path $repositoryRoot $directory) | Out-Null
}

foreach ($entry in $map.GetEnumerator() | Sort-Object Key) {
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $entry.Value) | Out-Null
    if (Test-Path -LiteralPath $entry.Key -PathType Leaf) { Move-Item -LiteralPath $entry.Key -Destination $entry.Value }
}

foreach ($oldSource in $markdown.Keys) {
    $newSource = if ($map.ContainsKey($oldSource)) { $map[$oldSource] } else { $oldSource }
    $sourceBase = if ($oldSource -like '*history\status-ledger-snapshot-20260910.md') { $docsRoot } else { Split-Path -Parent $oldSource }
    $rewritten = [regex]::Replace($markdown[$oldSource], '\[(?<label>[^\]]*)\]\((?<target>[^\s\)]+)\)', {
        param($match)
        $target = $match.Groups['target'].Value
        if ($target -match '^(https?:|mailto:|#|/|[A-Za-z][A-Za-z0-9+.-]*:)') { return $match.Value }
        $parts = $target -split '(?=[#?])', 2
        $path = $parts[0]
        if ([string]::IsNullOrWhiteSpace($path)) { return $match.Value }
        try { $oldTarget = [IO.Path]::GetFullPath((Join-Path $sourceBase $path)) } catch { return $match.Value }
        if ($map.ContainsKey($oldTarget)) { $newTarget = $map[$oldTarget] }
        elseif (Test-Path -LiteralPath $oldTarget) { $newTarget = $oldTarget }
        else { return $match.Value }
        $relative = Get-RelativePath (Split-Path -Parent $newSource) $newTarget
        if ($parts.Count -gt 1) { $relative += $parts[1] }
        return "[$($match.Groups['label'].Value)]($relative)"
    })
    if ($rewritten -ne $markdown[$oldSource]) { [IO.File]::WriteAllText($newSource, $rewritten, [Text.UTF8Encoding]::new($false)) }
}

foreach ($directory in @('docs/architecture', 'docs/governance', 'docs/planning', 'docs/research')) {
    $full = Join-Path $repositoryRoot $directory
    if ((Test-Path -LiteralPath $full) -and -not (Get-ChildItem -LiteralPath $full -Force | Select-Object -First 1)) { Remove-Item -LiteralPath $full }
}

"Completed file moves: $($map.Count)"
