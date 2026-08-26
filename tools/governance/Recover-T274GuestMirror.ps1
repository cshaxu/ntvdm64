param(
    [Parameter(Mandatory = $true)]
    [string]$SourceRoot,
    [Parameter(Mandatory = $true)]
    [string]$TargetRoot,
    [Parameter(Mandatory = $true)]
    [string]$ManifestPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-RelativePath([string]$Root, [string]$Path) {
    return $Path.Substring($Root.Length).TrimStart([char]92).Replace([char]92, [char]'/')
}

$source = (Resolve-Path -LiteralPath $SourceRoot).Path.TrimEnd([char]92)
$target = (Resolve-Path -LiteralPath $TargetRoot).Path.TrimEnd([char]92)
$files = @(Get-ChildItem -LiteralPath $source -Recurse -File | Sort-Object FullName)
if ($files.Count -eq 0) {
    throw "Source guest mirror is empty: $source"
}

$manifest = foreach ($file in $files) {
    $relative = Get-RelativePath $source $file.FullName
    $destination = Join-Path $target $relative.Replace([char]'/', [char]92)
    $destinationDirectory = Split-Path -Parent $destination
    if (-not (Test-Path -LiteralPath $destinationDirectory -PathType Container)) {
        New-Item -ItemType Directory -Path $destinationDirectory | Out-Null
    }
    $sourceHash = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    if (Test-Path -LiteralPath $destination -PathType Leaf) {
        $destinationHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($destinationHash -ne $sourceHash) {
            throw "Refusing to overwrite non-identical guest file: $destination"
        }
        $action = 'already-identical'
    } else {
        Copy-Item -LiteralPath $file.FullName -Destination $destination
        $action = 'copied-byte-identical'
    }
    [pscustomobject][ordered]@{
        target_path = $relative
        source_path = $file.FullName
        source_sha256 = $sourceHash
        size_bytes = $file.Length
        action = $action
    }
}

$manifestDirectory = Split-Path -Parent $ManifestPath
if (-not (Test-Path -LiteralPath $manifestDirectory -PathType Container)) {
    throw "Manifest directory does not exist: $manifestDirectory"
}
$manifest | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $ManifestPath -Encoding utf8
Write-Output "Recovered $($manifest.Count) byte-identical guest files."
