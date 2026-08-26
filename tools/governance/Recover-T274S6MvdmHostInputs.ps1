param(
    [Parameter(Mandatory = $true)]
    [string]$S5ManifestPath,
    [Parameter(Mandatory = $true)]
    [string]$TargetRoot,
    [Parameter(Mandatory = $true)]
    [string]$ManifestPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$target = (Resolve-Path -LiteralPath $TargetRoot).Path.TrimEnd([char]92)
$s5 = Import-Csv -LiteralPath $S5ManifestPath -Delimiter "`t"
$deferred = @($s5 | Where-Object { $_.s5_disposition -ne 'import-topology-or-resource-s5' } |
    Sort-Object target_path)
if ($deferred.Count -eq 0) {
    throw 'No S5-deferred host inputs are available for S6 recovery.'
}

$manifest = foreach ($row in $deferred) {
    $source = (Resolve-Path -LiteralPath $row.selected_source_path).Path
    $destination = Join-Path $target $row.target_path.Replace([char]'/', [char]92)
    $directory = Split-Path -Parent $destination
    if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
        New-Item -ItemType Directory -Path $directory | Out-Null
    }
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($sourceHash -ne $row.selected_source_sha256) {
        throw "Selected-source hash changed since S5: $source"
    }
    if (Test-Path -LiteralPath $destination -PathType Leaf) {
        $destinationHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($destinationHash -ne $sourceHash) {
            throw "Refusing to overwrite non-identical host input: $destination"
        }
        $action = 'already-identical'
    } else {
        Copy-Item -LiteralPath $source -Destination $destination
        $action = 'copied-byte-identical'
    }
    [pscustomobject][ordered]@{
        target_path = $row.target_path
        package_root = $row.package_root
        s6_input_class = $row.s5_disposition
        selected_source_path = $row.selected_source_path
        selected_source_sha256 = $row.selected_source_sha256
        action = $action
        source_selection = $row.source_selection
    }
}

$manifestDirectory = Split-Path -Parent $ManifestPath
if (-not (Test-Path -LiteralPath $manifestDirectory -PathType Container)) {
    throw "Manifest directory does not exist: $manifestDirectory"
}
$manifest | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $ManifestPath -Encoding utf8
Write-Output "Recovered $($manifest.Count) selected MVDM host inputs."
