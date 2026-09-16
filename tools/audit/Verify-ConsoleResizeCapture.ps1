[CmdletBinding()]
param([Parameter(Mandatory)][string]$Capture)
$ErrorActionPreference = 'Stop'
$text = Get-Content -LiteralPath ($Capture + '.cells.txt') -Raw
if ($text -notmatch 'Welcome to the MS-DOS Editor') { throw 'EDIT was not observed' }
$frames = @([regex]::Split($text, '(?m)(?=^t=)') | Where-Object { $_ -match '^t=' })
if (!$frames.Count) { throw 'No native Console frames' }
$last = $frames[-1]
if ($last -notmatch '(?m)^t=.* mode=7\r?$') { throw 'Original output mode was not restored' }
$rows = @{}
foreach ($match in [regex]::Matches($last, '(?m)^(\d+)\|([^\r\n]*)')) {
    $rows[[int]$match.Groups[1].Value] = $match.Groups[2].Value.TrimEnd()
}
$memoryRows = @($rows.Keys | Where-Object { $rows[$_] -match 'MS-DOS resident in High Memory' } | Sort-Object)
if (!$memoryRows.Count) { throw 'MEM completion not visible in final Console state' }
$gaps = @()
foreach ($row in $memoryRows) {
    $next = $row + 1
    while ($rows.ContainsKey($next) -and !$rows[$next]) { $next++ }
    if (!$rows.ContainsKey($next) -or $rows[$next] -notmatch '^O:\\WINNT>') {
        throw "No next COMMAND prompt after MEM row $row"
    }
    $gap = $next - $row - 1
    if ($gap -ne 1) { throw "Unexpected MEM-to-prompt blank gap: $gap" }
    $gaps += $gap
}
$exitRows = @($rows.Keys | Where-Object { $rows[$_] -match '^O:\\WINNT>exit$' })
if ($exitRows.Count -ne 1) { throw "Missing or duplicated final EXIT echo: $($exitRows.Count)" }
[pscustomobject]@{ Capture=$Capture; Frames=$frames.Count; MemoryBlocks=$memoryRows.Count; BlankGaps=$gaps; OutputMode=7; ExitEchoes=$exitRows.Count }
