[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$sourceRoot = Join-Path $root 'src/opennt/base/mvdm/dos/command'
$serviceHeader = Join-Path $root 'src/opennt/base/mvdm/inc/cmdsvc.h'
$destinationRoot = Join-Path $root 'src/bx-vdm/bop/opennt/command'
$sources = @(
    'cmd.c', 'cmd.h', 'cmdconf.c', 'cmddata.c', 'cmddisp.c', 'cmdenv.c',
    'cmdexec.c', 'cmdexit.c', 'cmdkeyb.c', 'cmdkeyb.h', 'cmdmisc.c',
    'cmdpif.c', 'cmdpif.h', 'cmdredir.c'
)

if (-not (Test-Path -LiteralPath $sourceRoot -PathType Container)) {
    throw "Missing in-tree OpenNT COMMAND source: $sourceRoot"
}
New-Item -ItemType Directory -Force -Path $destinationRoot | Out-Null
foreach ($name in $sources) {
    $source = Join-Path $sourceRoot $name
    $destination = Join-Path $destinationRoot $name
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Missing admitted OpenNT COMMAND input: $source"
    }
    Copy-Item -LiteralPath $source -Destination $destination -Force
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    $destinationHash = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
    if ($sourceHash -ne $destinationHash) {
        throw "COMMAND source mirror hash mismatch: $name"
    }
    Write-Output "mirrored $name $sourceHash"
}
if (-not (Test-Path -LiteralPath $serviceHeader -PathType Leaf)) {
    throw "Missing admitted OpenNT COMMAND service input: $serviceHeader"
}
$serviceDestination = Join-Path $destinationRoot 'cmdsvc.h'
Copy-Item -LiteralPath $serviceHeader -Destination $serviceDestination -Force
$serviceSourceHash = (Get-FileHash -LiteralPath $serviceHeader -Algorithm SHA256).Hash
$serviceDestinationHash = (Get-FileHash -LiteralPath $serviceDestination -Algorithm SHA256).Hash
if ($serviceSourceHash -ne $serviceDestinationHash) {
    throw 'COMMAND source mirror hash mismatch: cmdsvc.h'
}
Write-Output "mirrored cmdsvc.h $serviceSourceHash"
