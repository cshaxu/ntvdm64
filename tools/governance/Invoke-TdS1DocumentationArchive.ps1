[CmdletBinding()]
param(
    [string]$RepositoryRoot,
    [switch]$Apply
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$docs = Join-Path $root 'docs'
$archive = Join-Path $root 'artifacts/documentation-archive/20260910'
$moves = [System.Collections.Generic.List[object]]::new()

function Add-TreeMoves([string]$Source, [string]$Destination) {
    $sourceHasFiles = (Test-Path -LiteralPath $Source) -and (Get-ChildItem -LiteralPath $Source -Recurse -File | Select-Object -First 1)
    $enumerationRoot = if ($sourceHasFiles) { $Source } elseif (Test-Path -LiteralPath $Destination) { $Destination } else { return }
    Get-ChildItem -LiteralPath $enumerationRoot -Recurse -File | ForEach-Object {
        $relative = $_.FullName.Substring($enumerationRoot.Length + 1)
        $moves.Add([PSCustomObject]@{ Source = (Join-Path $Source $relative); Destination = (Join-Path $Destination $relative); Bytes = $_.Length; Sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant() })
    }
}

Add-TreeMoves (Join-Path $docs 'etc/evidence') (Join-Path $archive 'etc/evidence')
Add-TreeMoves (Join-Path $docs 'etc/research') (Join-Path $archive 'etc/research')
Add-TreeMoves (Join-Path $docs 'etc/archive') (Join-Path $archive 'etc/archive')
$operations = Join-Path $docs 'etc/operations'
Get-ChildItem -LiteralPath $operations -File -Filter '*.tsv' | ForEach-Object {
    $moves.Add([PSCustomObject]@{ Source = $_.FullName; Destination = (Join-Path $archive ('etc/operations/ledgers/' + $_.Name)); Bytes = $_.Length; Sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant() })
}
if (-not (Get-ChildItem -LiteralPath $operations -File -Filter '*.tsv')) {
    $ledgerRoot = Join-Path $archive 'etc/operations/ledgers'
    if (Test-Path -LiteralPath $ledgerRoot) {
        Get-ChildItem -LiteralPath $ledgerRoot -File -Filter '*.tsv' | ForEach-Object {
            $moves.Add([PSCustomObject]@{ Source = (Join-Path $operations $_.Name); Destination = $_.FullName; Bytes = $_.Length; Sha256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash.ToLowerInvariant() })
        }
    }
}

if (-not $Apply) {
    [PSCustomObject]@{ Files = $moves.Count; Bytes = ($moves | Measure-Object Bytes -Sum).Sum; Archive = $archive } | Format-List
    return
}

$map = @{}
foreach ($move in $moves) {
    if ((Test-Path -LiteralPath $move.Destination) -and (Test-Path -LiteralPath $move.Source)) { throw "Archive target already exists: $($move.Destination)" }
    $map[$move.Source] = $move.Destination
}

foreach ($move in $moves) {
    if (Test-Path -LiteralPath $move.Source) {
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $move.Destination) | Out-Null
        Move-Item -LiteralPath $move.Source -Destination $move.Destination
    }
}

$linkPattern = '(?<!\!)\[(?<label>[^\]]+)\]\((?<target>[^)\s]+)(?<suffix>\s+"[^"]*")?\)'
Get-ChildItem -LiteralPath $docs -Recurse -File -Filter '*.md' | ForEach-Object {
    $path = $_.FullName
    $text = [IO.File]::ReadAllText($path)
    $updated = [regex]::Replace($text, $linkPattern, {
        param($match)
        $target = $match.Groups['target'].Value
        if ($target -match '^(#|[a-z][a-z0-9+.-]*:|//)') { return $match.Value }
        $baseTarget = ($target -split '[#?]', 2)[0]
        $fragment = $target.Substring($baseTarget.Length)
        $candidate = [IO.Path]::GetFullPath((Join-Path (Split-Path -Parent $path) $baseTarget))
        if (-not $map.ContainsKey($candidate)) { return $match.Value }
        $baseUri = [Uri]((Split-Path -Parent $path).TrimEnd('\') + '\')
        $targetUri = [Uri]$map[$candidate]
        $relative = [Uri]::UnescapeDataString($baseUri.MakeRelativeUri($targetUri).ToString())
        return "[$($match.Groups['label'].Value)]($relative$fragment$($match.Groups['suffix'].Value))"
    })
    if ($updated -ne $text) { [IO.File]::WriteAllText($path, $updated, [Text.UTF8Encoding]::new($false)) }
}

$manifest = Join-Path $archive 'manifest.tsv'
$header = 'original_path' + [char]9 + 'archive_path' + [char]9 + 'bytes' + [char]9 + 'sha256'
$rows = $moves | Sort-Object Source | ForEach-Object {
    $source = $_.Source.Substring($root.Length + 1).Replace('\', '/')
    $destination = $_.Destination.Substring($root.Length + 1).Replace('\', '/')
    "$source$([char]9)$destination$([char]9)$($_.Bytes)$([char]9)$($_.Sha256)"
}
[IO.File]::WriteAllLines($manifest, [string[]](@($header) + @($rows)), [Text.UTF8Encoding]::new($false))
Write-Output "Archived $($moves.Count) documentation files to $archive"
