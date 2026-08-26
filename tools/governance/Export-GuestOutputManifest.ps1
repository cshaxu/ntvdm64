param(
    [Parameter(Mandatory = $true)]
    [string]$OutputRoot,
    [Parameter(Mandatory = $true)]
    [string]$ManifestPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $OutputRoot).Path.TrimEnd([char]92)
$files = @(Get-ChildItem -LiteralPath $root -Recurse -File | Sort-Object FullName)
if ($files.Count -eq 0) {
    throw "Guest output root is empty: $root"
}
$rows = foreach ($file in $files) {
    [pscustomobject][ordered]@{
        output_path = $file.FullName.Substring($root.Length).TrimStart([char]92).Replace([char]92, [char]'/')
        sha256 = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
        size_bytes = $file.Length
    }
}
$directory = Split-Path -Parent $ManifestPath
if (-not (Test-Path -LiteralPath $directory -PathType Container)) {
    throw "Manifest directory does not exist: $directory"
}
$rows | ConvertTo-Csv -Delimiter "`t" -NoTypeInformation |
    Set-Content -LiteralPath $ManifestPath -Encoding utf8
Write-Output "Manifested $($rows.Count) immutable guest output files."
