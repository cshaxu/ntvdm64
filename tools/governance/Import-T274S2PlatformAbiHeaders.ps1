param(
    [Parameter(Mandatory = $true)]
    [string]$LedgerPath,
    [Parameter(Mandatory = $true)]
    [string]$OpenNtRoot,
    [Parameter(Mandatory = $true)]
    [string]$OpenNt45PrivateRoot,
    [Parameter(Mandatory = $true)]
    [string]$TargetRoot,
    [Parameter(Mandatory = $true)]
    [string]$ManifestPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-TargetRelativePath([string]$SourcePath, [string]$OpenNt, [string]$OpenNt45Private) {
    if ($SourcePath.StartsWith($OpenNt45Private, [StringComparison]::OrdinalIgnoreCase)) {
        return Join-Path 'source\opennt-4.5-private' $SourcePath.Substring($OpenNt45Private.Length).TrimStart([char]92)
    }
    if ($SourcePath.StartsWith($OpenNt, [StringComparison]::OrdinalIgnoreCase)) {
        return Join-Path 'source\opennt' $SourcePath.Substring($OpenNt.Length).TrimStart([char]92)
    }
    throw "Candidate is not under an approved OpenNT root: $SourcePath"
}

$openNt = (Resolve-Path -LiteralPath $OpenNtRoot).Path.TrimEnd([char]92)
$openNt45Private = (Resolve-Path -LiteralPath $OpenNt45PrivateRoot).Path.TrimEnd([char]92)
$target = (Resolve-Path -LiteralPath $TargetRoot).Path.TrimEnd([char]92)
$ledger = Import-Csv -LiteralPath $LedgerPath -Delimiter "`t"
$candidates = @($ledger | Where-Object { $_.disposition -eq 'exact-opennt-platform-candidate' })
if ($candidates.Count -eq 0) {
    throw 'No exact OpenNT platform candidates are available for import.'
}

$manifest = foreach ($row in $candidates | Sort-Object include) {
    $source = (Resolve-Path -LiteralPath $row.selected_candidate).Path
    $relative = Get-TargetRelativePath $source $openNt $openNt45Private
    $destination = Join-Path $target $relative
    $destinationDirectory = Split-Path -Parent $destination
    if (-not (Test-Path -LiteralPath $destinationDirectory -PathType Container)) {
        New-Item -ItemType Directory -Path $destinationDirectory | Out-Null
    }
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
    if (Test-Path -LiteralPath $destination -PathType Leaf) {
        $destinationHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($destinationHash -ne $sourceHash) {
            throw "Refusing to overwrite non-identical imported declaration: $destination"
        }
        $action = 'already-identical'
    } else {
        Copy-Item -LiteralPath $source -Destination $destination
        $action = 'copied-byte-identical'
    }
    [pscustomobject][ordered]@{
        include = $row.include
        source_path = $source
        source_sha256 = $sourceHash
        target_path = $relative.Replace([char]92, [char]'/')
        action = $action
        direct_occurrences = $row.occurrence_count
        first_mvdm_source = $row.first_source
    }
}

$manifestDirectory = Split-Path -Parent $ManifestPath
if (-not (Test-Path -LiteralPath $manifestDirectory -PathType Container)) {
    throw "Manifest directory does not exist: $manifestDirectory"
}
$manifest | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $ManifestPath -Encoding utf8
Write-Output "Imported $($manifest.Count) exact platform declaration headers."
