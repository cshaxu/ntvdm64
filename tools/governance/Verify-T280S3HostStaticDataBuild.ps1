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
    'src/opennt-mvdm-host/softpc.new/host/src/nt_ertbl.c',
    'src/opennt-mvdm-host/softpc.new/host/src/nt_mess.c',
    'src/adapter-softpc/include/error_abi.h',
    'tools/build/New-T280HostStaticDataNinja.ps1',
    'docs/etc/operations/m0-t280-s3-host-static-data-build-plan-001.md',
    'docs/etc/operations/m0-t280-s3-host-static-data-build-evidence-001.md'
)
foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) {
        throw "Missing T280 S3 artifact: $relative"
    }
}
$bridge = Get-Content -LiteralPath (Join-Path $root 'src/adapter-softpc/include/error_abi.h') -Raw
foreach ($token in @('ERROR_STRUCT', 'EH_ERROR', 'EV_EXTRA_CHAR', 'DIVERGENCE:')) {
    if ($bridge -notmatch [regex]::Escape($token)) { throw "Missing bridge token: $token" }
}
if ($bridge -match 'adapter-bochs|adapter-bop|session|mapping_manager') {
    throw 'Static-data declaration bridge must not carry runtime, machine or mapping behavior.'
}
$generator = Get-Content -LiteralPath (Join-Path $root 'tools/build/New-T280HostStaticDataNinja.ps1') -Raw
foreach ($token in @('deps = msvc', '/MT', '/W4', 'nt_ertbl.c', 'nt_mess.c', 'error_abi.h')) {
    if ($generator -notmatch [regex]::Escape($token)) { throw "Ninja generator misses required token: $token" }
}
if ($generator -match '/WX|/wd|/we') { throw 'Static-data graph suppresses or promotes warnings.' }
if ($RequireArtifacts) {
    foreach ($relative in @('build/M0-T280/x86/host-static-data.lib', 'build/M0-T280/x64/host-static-data.lib')) {
        if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) {
            throw "Missing formal T280 S3 build artifact: $relative"
        }
    }
}
Write-Host 'PASS: T280 S3 static-data island retains original sources and a behavior-free SoftPC declaration bridge.'
