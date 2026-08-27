[CmdletBinding()]
param([string]$RepositoryRoot = '', [switch]$RequireArtifacts)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
foreach ($relative in @(
    'src/adapter-mvdm-host-out/win32/include/nt.h',
    'src/adapter-mvdm-host-out/win32/README.md',
    'tests/adapter-mvdm-host-out/win32/t280_s9_dem_declaration_fixture.c',
    'tools/build/New-T280S9DemDeclarationNinja.ps1',
    'docs/etc/operations/m0-t280-s9-dem-declaration-binding-plan-001.md',
    'docs/etc/operations/m0-t280-s9-dem-declaration-binding-evidence-001.md'
)) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) { throw "Missing T280 S9 artifact: $relative" }
}
$facade = Get-Content -LiteralPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h') -Raw
foreach ($token in @('#include <devioctl.h>', '#undef GetDiskSpaceInformation', 'ntioapi.h awaits its own complete ABI binding', 'DIVERGENCE:')) {
    if ($facade -notmatch [regex]::Escape($token)) { throw "T280 S9 facade misses: $token" }
}
if ($facade -match '#include <ntioapi\.h>') { throw 'T280 S9 imported the unbound ntioapi surface into adapter-mvdm-host-out/win32.' }
$fixture = Get-Content -LiteralPath (Join-Path $root 'tests/adapter-mvdm-host-out/win32/t280_s9_dem_declaration_fixture.c') -Raw
foreach ($token in @('DISK_GEOMETRY','DEVICE_TYPE','GetDiskSpaceInformation')) {
    if ($fixture -notmatch [regex]::Escape($token)) { throw "T280 S9 fixture misses: $token" }
}
if ($fixture -match 'FILE_BOTH_DIR_INFORMATION|GetVDMAddr|Sim32') { throw 'T280 S9 fixture exceeds the storage declaration boundary.' }
if ($RequireArtifacts) {
    foreach ($arch in @('x86','x64')) {
        if (-not (Test-Path -LiteralPath (Join-Path $root "build/M0-T280/$arch-dem-storage-declarations/build.ninja") -PathType Leaf)) { throw "Missing T280 S9 Ninja graph for $arch." }
    }
}
Write-Host 'PASS: T280 S9 storage declarations use original carriers without binding the NT IO or mapping surfaces.'
