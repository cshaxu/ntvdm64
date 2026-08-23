param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..'))
)

$ErrorActionPreference = 'Stop'
$path = Join-Path $RepositoryRoot 'docs\etc\bop-list.md'
$text = Get-Content -LiteralPath $path -Raw
$begin = $text.IndexOf('### Td S2 P6 — remaining owner-package dispositions')
$end = $text.IndexOf('## 1. DEM', $begin)
if ($begin -lt 0 -or $end -le $begin) { throw 'Td S2 P6 overlay missing' }
$audit = $text.Substring($begin, $end - $begin)
$actual = @([regex]::Matches($audit,
  '`(BOP-(?:REDIR-57|DEBUGGER-56)-[0-9A-F]{2}|BOP-WOW-51|BOP-TOP-(?:59|5[A-F]|F[D-E])|BOP-DEPENDENCY-(?:0(?:4[3-9]|5[0-8]|7[8-9]|[89][0-9])|1(?:0[0-9]|1[0-5])))`') |
  ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$expected = @(
  (0..49 | ForEach-Object { 'BOP-REDIR-57-{0:X2}' -f $_ }) +
  (0..15 | ForEach-Object { 'BOP-DEBUGGER-56-{0:X2}' -f $_ }) +
  @('BOP-WOW-51', 'BOP-TOP-5A', 'BOP-TOP-5F', 'BOP-TOP-FE', 'BOP-TOP-5E',
    'BOP-TOP-FD', 'BOP-TOP-59', 'BOP-TOP-5C', 'BOP-TOP-5B', 'BOP-TOP-5D') +
  (@(43) + (45..58) + (78..115) | ForEach-Object { 'BOP-DEPENDENCY-{0:D3}' -f $_ })
)
if ($actual.Count -ne $expected.Count -or (Compare-Object $actual $expected)) {
  throw "Td S2 P6 requires exactly $($expected.Count) remaining rows, found $($actual.Count)"
}
foreach ($value in @('opennt-shaped-facade', 'registered-exception',
    'migration-debt', 'deferred', 'not-applicable-guest')) {
  if (-not $audit.Contains(('`' + $value + '`'))) { throw "Td S2 P6 lacks: $value" }
}
Write-Output 'Td S2 P6 remaining interface audit: PASS (76 BOP + 53 dependency rows).'
