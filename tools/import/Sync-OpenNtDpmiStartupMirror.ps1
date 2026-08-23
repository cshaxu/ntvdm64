[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$sourceRoot = Join-Path $root 'src/opennt/base/mvdm/dpmi32'
$destinationRoot = Join-Path $root 'src/bx-vdm/bop/opennt/dpmi'
$sources = @(
    'data.c', 'debug.c', 'dpmi32.c', 'dpmiselr.c', 'dpmidata.h',
    'dpmi32p.h', 'precomp.h', 'xlathlp.h'
)

if (-not (Test-Path -LiteralPath $sourceRoot -PathType Container)) {
    throw "Missing in-tree OpenNT DPMI source: $sourceRoot"
}
New-Item -ItemType Directory -Force -Path $destinationRoot | Out-Null
foreach ($name in $sources) {
    $source = Join-Path $sourceRoot $name
    $destination = Join-Path $destinationRoot $name
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Missing admitted OpenNT DPMI startup input: $source"
    }
    Copy-Item -LiteralPath $source -Destination $destination -Force
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    $destinationHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
    if ($sourceHash -ne $destinationHash) {
        throw "DPMI startup mirror hash mismatch: $name"
    }
    Write-Output "mirrored $name $sourceHash"
}
