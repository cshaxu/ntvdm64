param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..'))
)

$ErrorActionPreference = 'Stop'
$trackerPath = Join-Path $RepositoryRoot 'docs\etc\bop-list.md'
if (-not (Test-Path -LiteralPath $trackerPath -PathType Leaf)) {
    throw "Td S2 P4 missing tracker: $trackerPath"
}

$text = Get-Content -LiteralPath $trackerPath -Raw
$begin = $text.IndexOf('### Td S2 P4 — DEM and COMMAND interface dispositions')
$end = $text.IndexOf('## 1. DEM', $begin)
if ($begin -lt 0 -or $end -le $begin) {
    throw 'Td S2 P4 missing bounded DEM/COMMAND overlay section'
}
$audit = $text.Substring($begin, $end - $begin)
$actual = @([regex]::Matches($audit,
    '`(BOP-(?:DEM-50|COMMAND-54)-[0-9A-F]{2}|BOP-DEPENDENCY-0(?:0[1-9]|[12][0-9]|3[0-3]))`') |
    ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$expected = @(
    (0..72 | ForEach-Object { 'BOP-DEM-50-{0:X2}' -f $_ }) +
    (0..16 | ForEach-Object { 'BOP-COMMAND-54-{0:X2}' -f $_ }) +
    (1..33 | ForEach-Object { 'BOP-DEPENDENCY-{0:D3}' -f $_ })
)
if ($actual.Count -ne $expected.Count -or (Compare-Object $actual $expected)) {
    throw "Td S2 P4 requires exactly $($expected.Count) DEM/COMMAND rows, found $($actual.Count)"
}

foreach ($disposition in @('opennt-shaped-facade', 'registered-exception',
        'migration-debt', 'deferred')) {
    if (-not $audit.Contains(('`' + $disposition + '`'))) {
        throw "Td S2 P4 lacks disposition evidence: $disposition"
    }
}

Write-Output 'Td S2 P4 DEM/COMMAND interface audit: PASS (90 BOP + 33 dependency rows).'
