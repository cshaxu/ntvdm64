[CmdletBinding()]
param([string]$RepositoryRoot = '')

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path

$required = @(
    'src/adapter-win32/include/nt.h',
    'src/adapter-win32/include/ntrtl.h',
    'src/adapter-win32/include/nturtl.h',
    'src/adapter-win32/include/vdmapi.h',
    'src/adapter-win32/source/opennt_support_rtl.c',
    'docs/etc/operations/m0-t279-s2-adapter-win32-binding-plan-001.md',
    'docs/etc/operations/m0-t279-s2-adapter-win32-binding-audit-001.md'
)
foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $root $relative) -PathType Leaf)) {
        throw "Missing T279 S2 binding artifact: $relative"
    }
}

$mirrorPairs = @(
    @('src/opennt-mvdm-support/oemuni/file.c', 'O:/repos.external/OpenNT/base/mvdm/oemuni/file.c'),
    @('src/opennt-mvdm-support/oemuni/process.c', 'O:/repos.external/OpenNT/base/mvdm/oemuni/process.c'),
    @('src/opennt-mvdm-support/suballoc/suballoc.c', 'O:/repos.external/OpenNT/base/mvdm/suballoc/suballoc.c')
)
foreach ($pair in $mirrorPairs) {
    $actual = (Get-FileHash -LiteralPath (Join-Path $root $pair[0]) -Algorithm SHA256).Hash
    $baseline = (Get-FileHash -LiteralPath $pair[1] -Algorithm SHA256).Hash
    if ($actual -ne $baseline) { throw "Original support mirror changed: $($pair[0])" }
}

$audit = Get-Content -LiteralPath (Join-Path $root 'docs/etc/operations/m0-t279-s2-adapter-win32-binding-audit-001.md') -Raw
foreach ($name in @('FindFirstFileOem', 'FindNextFileOem', 'GetFullPathNameOem', 'SearchPathOem')) {
    if ($audit -notmatch [regex]::Escape($name)) { throw "Missing x64 overlay decision: $name" }
}
Write-Host 'PASS: T279 S2 keeps three original support bodies byte-exact and records four x64 pointer-overlay requirements.'
