[CmdletBinding()]
param(
    [string]$SourceRoot = 'O:\repos.external\bochs-2.6-compat\bochs-2.6',
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot),
    [string]$ArchiveName = 'bochs-3.0-unadmitted-f5dad7eff8d5',
    [switch]$ManifestOnly
)

$ErrorActionPreference = 'Stop'
$source = [IO.Path]::GetFullPath($SourceRoot)
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
$target = Join-Path $repository 'src\bochs'
$archive = Join-Path $repository ("src\archived\$ArchiveName")
$manifestPath = Join-Path $repository 'artifacts\research-inputs\bochs-2.6-source-manifest.json'

if (-not (Test-Path -LiteralPath $source -PathType Container)) {
    throw "Missing authoritative Bochs 2.6 source: $source"
}
foreach ($required in @('CHANGES', 'README', 'config.h.in', 'configure')) {
    if (-not (Test-Path -LiteralPath (Join-Path $source $required) -PathType Leaf)) {
        throw "Authoritative source is missing ${required}: $source"
    }
}

if (-not $ManifestOnly) {
    if (Test-Path -LiteralPath $archive) {
        throw "Refusing to overwrite existing archive: $archive"
    }
    if (Test-Path -LiteralPath $target) {
        Rename-Item -LiteralPath $target -NewName $ArchiveName
    }
    New-Item -ItemType Directory -Path $target -Force | Out-Null
    & robocopy $source $target /E /COPY:DAT /DCOPY:DAT /R:1 /W:1 /XJ | Out-Host
    if ($LASTEXITCODE -gt 7) {
        throw "robocopy failed with exit code $LASTEXITCODE"
    }
}
elseif (-not (Test-Path -LiteralPath $target -PathType Container)) {
    throw "Manifest-only verification requires an imported target: $target"
}

function Get-FileEntries([string]$Root) {
    return @(Get-ChildItem -LiteralPath $Root -Recurse -File -Force |
        Sort-Object FullName |
        ForEach-Object {
            [PSCustomObject][ordered]@{
                path = $_.FullName.Substring($Root.Length).TrimStart('\').Replace('\', '/')
                bytes = $_.Length
                sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
            }
        })
}

$sourceEntries = Get-FileEntries $source
$entries = Get-FileEntries $target
$sourceFingerprints = @($sourceEntries | ForEach-Object { "$($_.path)|$($_.bytes)|$($_.sha256)" })
$targetFingerprints = @($entries | ForEach-Object { "$($_.path)|$($_.bytes)|$($_.sha256)" })
$difference = Compare-Object -ReferenceObject $sourceFingerprints -DifferenceObject $targetFingerprints
if ($difference) {
    throw "Imported Bochs tree does not match authoritative source; differences: $($difference.Count)"
}

$manifest = [ordered]@{
    schema = 'ntdos64.bochs-source-manifest.v1'
    purpose = 'T95 internal-research source identity record; not a runtime input'
    sourceRoot = $source
    targetRoot = $target
    sourceChangesFirstLine = (Get-Content -LiteralPath (Join-Path $source 'CHANGES') -TotalCount 1)
    importedAtUtc = [DateTime]::UtcNow.ToString('o')
    fileCount = $entries.Count
    totalBytes = [Int64](($entries | Measure-Object -Property bytes -Sum).Sum)
    files = $entries
}

New-Item -ItemType Directory -Path (Split-Path -Parent $manifestPath) -Force | Out-Null
[IO.File]::WriteAllText($manifestPath, ($manifest | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
Write-Host "Verified and recorded $($entries.Count) Bochs files at $target"
if (-not $ManifestOnly) { Write-Host "Archived prior tree at $archive" }
Write-Host "Wrote source manifest: $manifestPath"
