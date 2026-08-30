[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [switch]$RequireArtifacts
)

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$required = @(
    'src/adapter-mvdm-host-out/monitor/include/monitor_context.h',
    'src/adapter-mvdm-host-out/win32/include/winbasep.h',
    'src/adapter-mvdm-host-out/monitor/source/vdm_control.c',
    'tests/adapter-mvdm-host-out/monitor/vdm_control_fixture.c',
    'tools/build/New-T280VdmControlNinja.ps1',
    'docs/etc/operations/m0-t280-s5-vdm-control-plan-001.md',
    'docs/etc/operations/m0-t280-s5-vdm-control-evidence-001.md'
)
foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) { throw "Missing T280 S5 artifact: $relative" }
}
$source = Get-Content -LiteralPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/source/vdm_control.c') -Raw
foreach ($token in @('VdmQueryDir', 'adapter_vdm_monitor_bind_query_dir', 'STATUS_NOT_IMPLEMENTED', 'DIVERGENCE:')) {
    if ($source -notmatch [regex]::Escape($token)) { throw "VDM control source misses: $token" }
}
if ($source -match 'NtQueryDirectoryFile|CreateFile|bochs|mapping_manager') { throw 'VDM control adapter exceeds its declared session-dispatch seam.' }
$carrier = Get-Content -LiteralPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/winbasep.h') -Raw
if ($carrier -notmatch 'DIVERGENCE:' -or $carrier -match 'typedef|#include') { throw 'winbasep carrier gained declarations or behavior.' }
$generator = Get-Content -LiteralPath (Join-Path $root 'tools/build/New-T280VdmControlNinja.ps1') -Raw
foreach ($token in @('deps = msvc', '/MT', '/W4', 'vdm_control.c', 'session.c', 'vdm_control_fixture.c')) {
    if ($generator -notmatch [regex]::Escape($token)) { throw "VDM Ninja graph misses: $token" }
}
if ($RequireArtifacts) {
    foreach ($relative in @('build/M0-T280/x86-vdm-control/vdm_control_fixture.exe','build/M0-T280/x64-vdm-control/vdm_control_fixture.exe')) {
        if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) { throw "Missing formal T280 S5 artifact: $relative" }
    }
}
Write-Host 'PASS: T280 S5 VdmQueryDir adapter is session-bound and explicitly bounded.'
