[CmdletBinding()]
param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$mirror = Join-Path $RepositoryRoot 'src\mvdm-host\dpmi32'
$canonicalOpenNt = 'O:\repos.external\OpenNT\base\mvdm\dpmi32'
$canonicalOpenNt45 = 'O:\repos.external\OpenNT-4.5\nt\private\mvdm\dpmi32'
$readme = Join-Path $RepositoryRoot 'src\mvdm-host\README.md'
$overlay = Join-Path $RepositoryRoot 'src\mvdm-host-overlay\dpmi32'
$identityFacade = Join-Path $RepositoryRoot 'src\adapter-mvdm-host-out\softpc\mvdm_host_identity.c'
$matrix = Join-Path $RepositoryRoot 'docs\etc\operations\m0-t289-s6-dpmi-family-disposition-matrix.tsv'

$paths = @(
    'buffer.c', 'data.c', 'debug.c', 'dpmi32.c', 'dpmi32p.h', 'dpmidata.h',
    'dpmiint.c', 'dpmiint.h', 'dpmimemr.c', 'dpmimscr.c', 'dpmiselr.c',
    'i386\dpmi386.c', 'i386\dpmimem.c', 'i386\dpmimisc.c', 'int21map.c',
    'makefile', 'modesw.c', 'precomp.h', 'register.c', 'savestat.c', 'sources',
    'stack.c', 'vxd.c', 'xlathlp.h', 'xmem.c'
)
$modified = @('dpmi32.c', 'dpmiint.c', 'dpmimemr.c', 'dpmiselr.c', 'stack.c', 'xmem.c')

foreach ($path in $paths) {
    $openNtPath = Join-Path $canonicalOpenNt $path
    $openNt45Path = Join-Path $canonicalOpenNt45 $path
    $mirrorPath = Join-Path $mirror $path
    foreach ($candidate in @($openNtPath, $openNt45Path, $mirrorPath)) {
        if (-not (Test-Path -LiteralPath $candidate)) { throw "Missing audited DPMI path: $candidate" }
    }
    $openNtHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $openNtPath).Hash
    $openNt45Hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $openNt45Path).Hash
    if ($openNtHash -ne $openNt45Hash) { throw "Canonical edition mismatch: $path" }
    $mirrorHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $mirrorPath).Hash
    if (($modified -notcontains $path) -and ($mirrorHash -ne $openNtHash)) {
        throw "Unexpected non-mirror DPMI file: $path"
    }
}

$readmeText = Get-Content -Raw -LiteralPath $readme
foreach ($number in 12..19) {
    $id = 'MVDM-HOST-DIV-{0:D3}' -f $number
    if ($readmeText -notmatch [regex]::Escape($id)) { throw "Missing divergence register entry: $id" }
}

foreach ($path in $modified) {
    $text = Get-Content -Raw -LiteralPath (Join-Path $mirror $path)
    if ($text -notmatch 'DIVERGENCE\(') { throw "Modified DPMI mirror lacks DIVERGENCE marker: $path" }
}

$mirrorSources = Get-ChildItem -LiteralPath $mirror -Recurse -File
if (@($mirrorSources | Select-String -Pattern 'adapter-bochs|bochs-core|src\.old|machine_facade').Count -ne 0) {
    throw 'DPMI mirror directly names a prohibited Bochs or historical product boundary.'
}
$overlaySources = Get-ChildItem -LiteralPath $overlay -Recurse -File
if (@($overlaySources | Select-String -Pattern '\bmapping_manager\b').Count -ne 0) {
    throw 'DPMI overlay bypasses the session-owned identity facade.'
}
if (-not (Select-String -LiteralPath $identityFacade -Pattern 'session_host_resource_mappings' -Quiet)) {
    throw 'DPMI host-resource identity facade is not session-owned.'
}

$matrixText = Get-Content -Raw -LiteralPath $matrix
foreach ($selector in @('53:01', '53:03', '53:06', '53:0E', '53:14', '53:15', '53:16', '53:17', '53:18')) {
    if ($matrixText -notmatch [regex]::Escape($selector)) { throw "Unavailable selector missing from transfer matrix: $selector" }
}

Write-Output 'T289 S7 DPMI mirror audit passed: 19 exact, 6 registered modified, 8 registered divergence IDs.'
