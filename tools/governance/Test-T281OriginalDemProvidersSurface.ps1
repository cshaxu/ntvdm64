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
$ninjaPath = Join-Path $root ("build/M0-T281/S3/{0}/build.ninja" -f $Architecture)
if (-not (Test-Path -LiteralPath $ninjaPath -PathType Leaf)) {
    throw "Missing formal T281 Ninja graph: $ninjaPath"
}

$text = Get-Content -Raw -LiteralPath $ninjaPath
$required = @('build obj/demdasd.obj: cc', 'build obj/demsrch.obj: cc',
    'build original-dem-providers.lib: lib', 'default original-dem-providers.lib',
    'adapter-mvdm-host-out/monitor/include/vdm.h')
foreach ($marker in $required) {
    if ($text -notlike "*$marker*") {
        throw "T281 graph is missing required DEM provider marker: $marker"
    }
}

$forbidden = @('rule link', 'rule run', '.exe', 'adapter-mvdm-host-in/', 'bochs-core/',
    'vdm_control.c', 'nt_bop.c')
foreach ($marker in $forbidden) {
    if ($text -like "*$marker*") {
        throw "T281 graph enables a forbidden runtime surface: $marker"
    }
}

$libraryPath = Join-Path (Split-Path -Parent $ninjaPath) 'original-dem-providers.lib'
if (-not (Test-Path -LiteralPath $libraryPath -PathType Leaf)) {
    throw "Missing formal T281 static library: $libraryPath"
}

Write-Output "T281 $Architecture selector-disabled DEM provider surface passed."
