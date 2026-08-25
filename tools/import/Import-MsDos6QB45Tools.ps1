param(
    [string]$SourceRoot = 'D:\home\repos.hobby\ms-dos-6\45\tl',
    [string]$DestinationRoot = (Join-Path (Split-Path -Parent $PSScriptRoot) 'historical\msdos6-qb45')
)

$ErrorActionPreference = 'Stop'
$sourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
$destinationRoot = [System.IO.Path]::GetFullPath($DestinationRoot)
if (Test-Path -LiteralPath $destinationRoot) {
    throw "Destination already exists: $destinationRoot"
}

$sets = @('bin', 'binb', 'lib', 'inc')
New-Item -ItemType Directory -Path $destinationRoot | Out-Null
$records = New-Object System.Collections.Generic.List[object]
foreach ($set in $sets) {
    $sourceSet = Join-Path $sourceRoot $set
    if (-not (Test-Path -LiteralPath $sourceSet -PathType Container)) {
        throw "Missing expected MS-DOS 6 tool set: $sourceSet"
    }
    Get-ChildItem -LiteralPath $sourceSet -File | ForEach-Object {
        $destinationSet = Join-Path $destinationRoot $set
        New-Item -ItemType Directory -Path $destinationSet -Force | Out-Null
        $destination = Join-Path $destinationSet $_.Name
        Copy-Item -LiteralPath $_.FullName -Destination $destination
        $records.Add([ordered]@{
            path = "$set/$($_.Name)"
            source = $_.FullName
            bytes = (Get-Item -LiteralPath $destination).Length
            sha256 = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
        })
    }
}

[ordered]@{
    schema = 'runner-historical-tool-manifest-v1'
    component = 'MS-DOS 6 QuickBASIC 4.5 TL historical toolchain'
    source_root = $sourceRoot
    license_status = 'Historical Microsoft tool input; research-only. No redistribution authorization is asserted.'
    canonical_use = 'Exact historical toolchain for MS-DOS 6 QBASIC, EDIT, and MSHERC reconstruction probes.'
    files = $records
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $destinationRoot 'MANIFEST.json') -Encoding ascii

[pscustomobject]@{ DestinationRoot = $destinationRoot; ImportedFiles = $records.Count; Manifest = Join-Path $destinationRoot 'MANIFEST.json' } | ConvertTo-Json
