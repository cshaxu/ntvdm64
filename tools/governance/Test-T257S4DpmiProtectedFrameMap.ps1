param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..'))
)

$ErrorActionPreference = 'Stop'
$evidence = Join-Path $RepositoryRoot 'docs\etc\evidence\m0-t257-s4-dosx-protected-frame-map-001.md'
$sourceFiles = @(
    'src\opennt\base\mvdm\dpmi32\dpmiint.c',
    'src\opennt\base\mvdm\dpmi32\dpmiint.h',
    'src\opennt\base\mvdm\dpmi32\stack.c',
    'src\opennt\base\mvdm\dpmi32\dpmimscr.c',
    'src\opennt\base\mvdm\dpmi\486\dxintr.asm',
    'src\opennt\base\mvdm\dpmi\486\dxint31.asm'
)

foreach ($relative in $sourceFiles) {
    if (-not (Test-Path (Join-Path $RepositoryRoot $relative))) {
        throw "T257 S4 missing original source: $relative"
    }
}
if (-not (Test-Path $evidence)) { throw "T257 S4 missing evidence: $evidence" }

$text = Get-Content -Raw $evidence
foreach ($marker in @(
    '53:02', '53:0A', '53:0C/0D', '53:11', '53:14/15', '53:16/17', '53:18',
    'NtVdmControl', 'VdmInstall*', 'EnableEmulatorIretHooks',
    'atomic', 'Native Bochs remains the exclusive architectural', 'exception and IRET owner'
)) {
    if (-not $text.Contains($marker)) { throw "T257 S4 evidence missing marker: $marker" }
}

Write-Output 'T257 S4 DPMI protected frame map: PASS (complete source family and refused legacy-hook/frame seam recorded).'
