[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [switch]$RequireArchitectureProbes
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$expected = '26B1B3CFF877EE2802AEFF0EEAB04F1DAF7127E63279BCAA990C8B4F97384F2D'
$source = (Get-FileHash -Algorithm SHA256 -LiteralPath 'O:\repos.external\OpenNT\public\internal\windows\inc\winconp.h').Hash
$target = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $root 'src\opennt-platform-abi\source\public\internal\windows\inc\winconp.h')).Hash
if ($source -ne $expected -or $target -ne $expected) {
    throw 'winconp.h is not byte-identical to its pinned original declaration carrier.'
}
$forwarder = Get-Content -Raw -LiteralPath (Join-Path $root 'src\adapter-win32\include\winconp.h')
if ($forwarder -notmatch 'opennt-platform-abi/source/public/internal/windows/inc/winconp\.h') {
    throw 'The adapter winconp declaration forwarder does not select the exact platform-ABI carrier.'
}
if ($RequireArchitectureProbes) {
    foreach ($architecture in @('x86', 'x64')) {
        $graph = Join-Path $root "build\M0-T282\S1\$architecture\build.ninja"
        $object = Join-Path $root "build\M0-T282\S1\$architecture\obj\cmdkeyb.obj"
        if (-not (Test-Path -LiteralPath $graph) -or -not (Test-Path -LiteralPath $object)) {
            throw "Missing $architecture COMMAND console declaration probe."
        }
        if ((Get-Content -Raw -LiteralPath $graph) -notmatch 'adapter-win32/include') {
            throw "$architecture graph does not select the adapter declaration carrier."
        }
    }
}
Write-Output 'T282 S3 COMMAND console declaration-carrier verification passed.'
