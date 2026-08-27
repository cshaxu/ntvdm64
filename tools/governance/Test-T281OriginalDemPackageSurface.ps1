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
$build = Join-Path $root ("build/M0-T281/S4/{0}" -f $Architecture)
$ninjaPath = Join-Path $build 'build.ninja'
if (-not (Test-Path -LiteralPath $ninjaPath -PathType Leaf)) { throw "Missing formal graph: $ninjaPath" }
$text = Get-Content -Raw -LiteralPath $ninjaPath
$units = @('dem', 'demdata', 'demmsg', 'demdisp', 'demdasd', 'demdir', 'demerror', 'demfcb', 'demfile', 'demgset', 'demhndl', 'demioctl', 'demlabel', 'demlock', 'demmisc', 'demsrch')
foreach ($unit in $units) { if ($text -notlike "*build obj/$unit.obj: cc*") { throw "Missing DEM package member: $unit" } }
foreach ($marker in @('rule link', 'rule run', '.exe', 'adapter-bop/', 'bochs-core/', 'vdm_control.c', 'nt_bop.c')) { if ($text -like "*$marker*") { throw "Forbidden runtime surface: $marker" } }
$libraryPath = Join-Path $build 'original-dem-package.lib'
if (-not (Test-Path -LiteralPath $libraryPath -PathType Leaf)) { throw "Missing static package archive: $libraryPath" }
$members = & lib /nologo /list $libraryPath
$memberText = $members -join [Environment]::NewLine
foreach ($unit in $units) {
    if ($memberText -notmatch "(?m)^\s*obj[\\/]$unit\.obj\s*$") {
        throw "Archive lacks member: $unit.obj"
    }
}
Write-Output "T281 $Architecture complete selector-disabled DEM package surface passed."
