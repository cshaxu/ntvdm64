param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')))
$ErrorActionPreference = 'Stop'
$tracker = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'docs\etc\bop-list.md') -Raw
$register = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'docs\etc\research\adapter-external-intrusion-exceptions.md') -Raw
$begin = $tracker.IndexOf('## Td S2 P2 Interface-Conformance Overlay')
$end = $tracker.IndexOf('## 1. DEM', $begin)
if ($begin -lt 0 -or $end -le $begin) { throw 'Td S2 overlay bounds missing' }
$rows = @($tracker.Substring($begin, $end - $begin) -split "`n" | Where-Object { $_ -match '^\| `BOP-' } | ForEach-Object {
  $p=$_ -split '\|'; [PSCustomObject]@{Id=$p[1].Trim(' ','`');D=$p[2].Trim(' ','`');E=$p[5].Trim();M=$p[6].Trim()}
})
$expected=@((0..72|%{'BOP-DEM-50-{0:X2}'-f $_})+(0..16|%{'BOP-COMMAND-54-{0:X2}'-f $_})+(0..11|%{'BOP-XMS-52-{0:X2}'-f $_})+(0..24|%{'BOP-DPMI-53-{0:X2}'-f $_})+(0..49|%{'BOP-REDIR-57-{0:X2}'-f $_})+'BOP-WOW-51'+(0..15|%{'BOP-DEBUGGER-56-{0:X2}'-f $_})+@('BOP-TOP-5A','BOP-TOP-5F','BOP-TOP-FE','BOP-TOP-5E','BOP-TOP-FD','BOP-TOP-59','BOP-TOP-5C','BOP-TOP-5B','BOP-TOP-5D')+(1..118|%{'BOP-DEPENDENCY-{0:D3}'-f $_}))
$actual=@($rows.Id|Sort-Object -Unique)
if($rows.Count -ne 321 -or $actual.Count -ne 321 -or (Compare-Object $actual $expected)){throw "Td S2 P7 requires 321 unique rows; rows=$($rows.Count), ids=$($actual.Count)"}
foreach($row in $rows){
 if($row.D -eq 'registered-exception'){@([regex]::Matches($row.E,'BX-(?:VDM|ABI|MANTLE)-\d{3}')|%{$_.Value})|%{if(-not $register.Contains($_)){throw "Missing exception: $_"}}}
 if($row.D -eq 'migration-debt' -and -not $row.M.Contains('migrate-facade')){throw "Missing migration: $($row.Id)"}
 if($row.D -eq 'deferred' -and -not $row.M.Contains('deferred-owner-package')){throw "Missing owner: $($row.Id)"}
}
Write-Output 'Td S2 P7 full interface audit: PASS (321 unique rows; exceptions reconciled).'
