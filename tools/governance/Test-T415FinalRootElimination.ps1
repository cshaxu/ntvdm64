[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BaselineRef = '622b71cda'
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$RepositoryRoot = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$openNt = 'O:\repos.external\OpenNT'
$fontRoot = Join-Path $RepositoryRoot 'src/opennt-host/base/win32/winnls/fontsup/system'
$utilRoot = Join-Path $RepositoryRoot 'src/mvdm/v86/util'
$carrier = 'src/ntvdm/monitor/source/monitor_printer.c'
$formerCarrier = 'src/mvdm-host/kernel-vdm/v86/monitor/i386/monitor_printer.c'

function Get-Blob([string]$Path) {
    $blob = @(& git -C $RepositoryRoot hash-object -- $Path)
    if ($LASTEXITCODE -ne 0 -or $blob.Count -ne 1) { throw "Cannot hash $Path" }
    return $blob[0].Trim()
}

foreach ($retired in @('src/mvdm-host', 'src/mvdm-guest', 'src/mvdm-tools')) {
    if (Test-Path -LiteralPath (Join-Path $RepositoryRoot $retired)) {
        throw "Retired root remains: $retired"
    }
}

$fonts = @(Get-ChildItem -LiteralPath $fontRoot -File -Filter '*.fon' | Sort-Object Name)
if ($fonts.Count -ne 42) { throw "Expected 42 OpenNT font files, found $($fonts.Count)" }
foreach ($font in $fonts) {
    $upstream = Join-Path $openNt ('base/win32/winnls/fontsup/system/' + $font.Name)
    if (!(Test-Path -LiteralPath $upstream) -or (Get-Blob $font.FullName) -ne (Get-Blob $upstream)) {
        throw "Font blob mismatch: $($font.Name)"
    }
}

foreach ($name in @('makefile', 'quit.asm')) {
    $current = Join-Path $utilRoot $name
    $upstream = Join-Path $openNt ('base/mvdm/v86/util/' + $name)
    if (!(Test-Path -LiteralPath $current) -or !(Test-Path -LiteralPath $upstream) -or
        (Get-Blob $current) -ne (Get-Blob $upstream)) {
        throw "MVDM v86/util blob mismatch: $name"
    }
}

$expectedCarrier = @(& git -C $RepositoryRoot rev-parse ($BaselineRef + ':' + $formerCarrier))
if ($LASTEXITCODE -ne 0 -or $expectedCarrier.Count -ne 1) { throw 'Cannot read baseline printer carrier blob' }
if ((Get-Blob (Join-Path $RepositoryRoot $carrier)) -ne $expectedCarrier[0].Trim()) {
    throw 'Printer carrier content changed during overlay relocation'
}

Write-Output 'T415 final-root elimination passed: fonts=42 v86-util=2 carrier=1 retired-roots=0'
