[CmdletBinding()]
param(
    [string]$RepositoryRoot = ''
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$dispatch = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/dos/dem/demdisp.c') -Raw
$header = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-host/dos/dem/dem.h') -Raw
$guestRead = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-guest/dos/v86/doskrnl/dos/handle.asm') -Raw
$graph = Get-Content -LiteralPath (Join-Path $root 'tools/build/New-T310OriginalSoftpcNinja.ps1') -Raw

if ($dispatch -notmatch 'demFastIoUnavailable,\s*//SVC_DEMFASTREAD' -or
    $dispatch -notmatch 'demFastIoUnavailable,\s*//SVC_DEMFASTWRITE' -or
    $dispatch -notmatch '(?s)static VOID demFastIoUnavailable \(VOID\).*?setCF\(1\)') {
    throw 'Fast-I/O slots do not decline through the one CF fallback seam.'
}
if ($dispatch -match '\bdemFast(Read|Write)\s*(?:\(|,)' -or
    $header -match 'demFastRead|demFastWrite' -or
    $graph -match "\$demOverlayNames = @\('demfastio\.c'\)") {
    throw 'A selected autonomous fast-I/O implementation remains.'
}
if ($guestRead -notmatch '(?s)SVC\s+SVC_DEMFASTREAD\s*\r?\n\s*jnc.*?HRDSVC\s+SVC_DEMREAD' -or
    $guestRead -notmatch '(?s)SVC\s+SVC_DEMFASTWRITE\s*\r?\n\s*jnc.*?HRDSVC\s+SVC_DEMWRITE') {
    throw 'The original DOS CF fallback sequence is not present.'
}

Write-Output 'T409 fast-I/O original fallback boundary passed.'
