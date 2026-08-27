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
$ninjaPath = Join-Path $root ("build/M0-T281/S1/{0}/build.ninja" -f $Architecture)
if (-not (Test-Path -LiteralPath $ninjaPath -PathType Leaf)) {
    throw "Missing formal T281 Ninja graph: $ninjaPath"
}

$text = Get-Content -Raw -LiteralPath $ninjaPath
$required = @(
    'build obj/nt_bop.obj: cc',
    'build original-bop-dispatch.lib: lib obj/nt_bop.obj',
    'default original-bop-dispatch.lib'
)
foreach ($marker in $required) {
    if ($text -notlike "*$marker*") {
        throw "T281 graph is missing required selector-disabled marker: $marker"
    }
}

$forbidden = @('rule link', 'rule run', '.exe', 'adapter-bop/')
foreach ($marker in $forbidden) {
    if ($text -like "*$marker*") {
        throw "T281 graph enables a forbidden runtime/provider surface: $marker"
    }
}

$libraryPath = Join-Path (Split-Path -Parent $ninjaPath) 'original-bop-dispatch.lib'
if (-not (Test-Path -LiteralPath $libraryPath -PathType Leaf)) {
    throw "Missing formal T281 static library: $libraryPath"
}

Write-Output "T281 $Architecture selector-disabled dispatcher surface passed."
