$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot '../../tools/audit/Merge-ConsoleTextSnapshots.ps1')
$first = "# viewport`r`n[0] prompt`r`n[2] MEM result`r`n[3] depth one`r`n[5] exit"
$second = "[0] MEM result`r`n[1] depth one`r`n[3] exit`r`n[5] MEM result`r`n[6] depth two"
$third = "[0] exit`r`n[2] MEM result`r`n[3] depth two`r`n[5] Bad command or filename"
$merged = Merge-ConsoleTextSnapshots @($first,$first,$second,$second,$third)
if ([regex]::Matches($merged,'MEM result').Count -ne 2) { throw 'Repeated/scrolling snapshots changed execution count' }
if ($merged -notmatch 'Bad command or filename') { throw 'Guest failure was lost' }
$rejected = $false
try { Merge-ConsoleTextSnapshots @($first,'[0] unrelated repaint') | Out-Null } catch { $rejected = $true }
if (!$rejected) { throw 'Disjoint frames were accepted as contiguous history' }
$rejected = $false
try { Merge-ConsoleTextSnapshots @($first,'# capture-error=1237',$first) | Out-Null } catch { $rejected = $true }
if (!$rejected) { throw 'Failed capture was silently omitted from evidence' }
$grown=Merge-ConsoleTextSnapshots @("[0] banner`n[1] R:\>","[0] banner`n[1] R:\>mem`n[2] result")
if ([regex]::Matches($grown,'R:\\>').Count -ne 1 -or $grown -notmatch 'R:\\>mem') {
    throw 'Live prompt growth was duplicated or lost'
}
Write-Output 'PASS overlapping snapshots retain output/error exactly once and reject unproved continuity'
