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
    'tools/build/New-T279SupportLibraryNinja.ps1',
    'src/mvdm-support-overlay/README.md',
    'src/mvdm-support-overlay/source/oemuni_pointer_width.c',
    'tests/mvdm-support/oemuni_pointer_width_fixture.c',
    'docs/etc/operations/m0-t279-s3-support-library-build-plan-001.md',
    'docs/etc/operations/m0-t279-s3-support-library-build-evidence-001.md'
)
foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) {
        throw "Missing T279 S3 build artifact: $relative"
    }
}

$overlay = Get-Content -LiteralPath (Join-Path $root 'src/mvdm-support-overlay/source/oemuni_pointer_width.c') -Raw
$exports = @('FindFirstFileOem', 'FindNextFileOem', 'GetFullPathNameOem', 'SearchPathOem')
foreach ($name in $exports) {
    if ($overlay -notmatch ('\b' + [regex]::Escape($name) + '\b')) {
        throw "Missing x64 overlay export: $name"
    }
}
if ($overlay -match '\(ULONG\).*\+' -or $overlay -match '\(DWORD\).*\+') {
    throw 'Overlay contains a narrowed pointer reconstruction.'
}

$generator = Get-Content -LiteralPath (Join-Path $root 'tools/build/New-T279SupportLibraryNinja.ps1') -Raw
foreach ($token in @('deps = msvc', '/MT', 'mvdm-support-overlay', 'opennt_original_SearchPathOem')) {
    if ($generator -notmatch [regex]::Escape($token)) { throw "Ninja generator misses required token: $token" }
}

if ($RequireArtifacts) {
    foreach ($relative in @(
        'build/M0-T279/x86/oemuni.lib',
        'build/M0-T279/x86/suballoc.lib',
        'build/M0-T279/x64/oemuni.lib',
        'build/M0-T279/x64/suballoc.lib',
        'build/M0-T279/x64/oemuni_pointer_width_fixture.exe'
    )) {
        if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) {
            throw "Missing formal T279 S3 build artifact: $relative"
        }
    }
}
Write-Host 'PASS: T279 S3 source/build graph records x86 original and x64 four-export overlay forms.'
