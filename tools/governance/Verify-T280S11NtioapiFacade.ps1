[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
foreach ($relative in @(
    'src/adapter-mvdm-host-out/win32/include/ntioapi.h',
    'src/adapter-mvdm-host-out/win32/source/ntioapi_facade.c',
    'tests/adapter-mvdm-host-out/win32/t280_s11_ntioapi_facade_fixture.c',
    'tools/build/New-T280S11NtioapiFacadeNinja.ps1',
    'docs/etc/operations/m0-t280-s11-reached-ntioapi-facade-plan-001.md',
    'docs/etc/operations/m0-t280-s11-ntioapi-facade-evidence-001.md'
)) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) { throw "Missing T280 S11 artifact: $relative" }
}
$header = Get-Content -Raw -LiteralPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/ntioapi.h')
$source = Get-Content -Raw -LiteralPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/source/ntioapi_facade.c')
foreach ($token in @('OPENNT_IO_STATUS_BLOCK', 'opennt_NtOpenFile', 'opennt_NtQueryDirectoryFile', 'opennt_NtQueryVolumeInformationFile', 'opennt_NtQueryObject', 'opennt_NtOpenSymbolicLinkObject', 'opennt_NtQuerySymbolicLinkObject')) {
    if ($header -notmatch [regex]::Escape($token) -or $source -notmatch [regex]::Escape($token)) { throw "T280 S11 source-shaped facade misses $token." }
}
if ($source -match 'GetVDMAddr|Sim32') { throw 'T280 S11 façade incorrectly contains a guest-memory mapping path.' }
if ($source -notmatch 'STATUS_NOT_IMPLEMENTED') { throw 'T280 S11 façade lacks an explicit unavailable outcome.' }
Write-Host 'PASS: T280 S11 reached NT I/O facade retains source shapes and host-local mapping isolation.'
