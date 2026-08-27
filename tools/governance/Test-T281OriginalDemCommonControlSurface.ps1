[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture,
    [string]$RepositoryRoot = ''
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$ninjaPath = Join-Path $root ("build/M0-T281/S2/{0}/build.ninja" -f $Architecture)
if (-not (Test-Path -LiteralPath $ninjaPath -PathType Leaf)) {
    throw "Missing formal T281 Ninja graph: $ninjaPath"
}

$text = Get-Content -Raw -LiteralPath $ninjaPath
$required = @(
    'build obj/dem.obj: cc',
    'build obj/demdata.obj: cc',
    'build obj/demmsg.obj: cc',
    'build obj/demdisp.obj: cc',
    'build original-dem-common-control.lib: lib',
    'default original-dem-common-control.lib'
)
foreach ($marker in $required) {
    if ($text -notlike "*$marker*") {
        throw "T281 graph is missing required DEM common-control marker: $marker"
    }
}

$forbidden = @('rule link', 'rule run', '.exe', 'adapter-mvdm-host-in/', 'bochs-core/', 'demfile.c', 'demsrch.c', 'demhndl.c')
foreach ($marker in $forbidden) {
    if ($text -like "*$marker*") {
        throw "T281 graph enables a forbidden runtime/provider surface: $marker"
    }
}

$libraryPath = Join-Path (Split-Path -Parent $ninjaPath) 'original-dem-common-control.lib'
if (-not (Test-Path -LiteralPath $libraryPath -PathType Leaf)) {
    throw "Missing formal T281 static library: $libraryPath"
}

Write-Output "T281 $Architecture selector-disabled DEM common-control surface passed."
