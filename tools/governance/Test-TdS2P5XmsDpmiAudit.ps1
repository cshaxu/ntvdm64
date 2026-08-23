param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..'))
)

$ErrorActionPreference = 'Stop'
$trackerPath = Join-Path $RepositoryRoot 'docs\etc\bop-list.md'
$text = Get-Content -LiteralPath $trackerPath -Raw
$begin = $text.IndexOf('### Td S2 P5 — XMS and DPMI interface dispositions')
$end = $text.IndexOf('## 1. DEM', $begin)
if ($begin -lt 0 -or $end -le $begin) {
    throw 'Td S2 P5 missing bounded XMS/DPMI overlay section'
}
$audit = $text.Substring($begin, $end - $begin)
$actual = @([regex]::Matches($audit,
    '`(BOP-(?:XMS-52|DPMI-53)-[0-9A-F]{2}|BOP-DEPENDENCY-0(?:3[4-9]|4[01]|5[9]|6[0-9]|7[0-7]))`') |
    ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$expected = @(
    (0..11 | ForEach-Object { 'BOP-XMS-52-{0:X2}' -f $_ }) +
    (0..24 | ForEach-Object { 'BOP-DPMI-53-{0:X2}' -f $_ }) +
    ((34..41) + (59..77) | ForEach-Object { 'BOP-DEPENDENCY-{0:D3}' -f $_ })
)
if ($actual.Count -ne $expected.Count -or (Compare-Object $actual $expected)) {
    throw "Td S2 P5 requires exactly $($expected.Count) XMS/DPMI rows, found $($actual.Count)"
}
foreach ($disposition in @('opennt-shaped-facade', 'registered-exception',
        'migration-debt', 'deferred')) {
    if (-not $audit.Contains(('`' + $disposition + '`'))) {
        throw "Td S2 P5 lacks disposition evidence: $disposition"
    }
}
Write-Output 'Td S2 P5 XMS/DPMI interface audit: PASS (37 BOP + 27 dependency rows).'
