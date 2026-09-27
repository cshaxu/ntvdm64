param([Parameter(Mandatory)][string]$Path)
$ErrorActionPreference='Stop'
# Read after worker exit. A published kind is the last field written.
$bytes=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Path).Path)
if($bytes.Length -lt 16){throw 'Truncated keyboard observation header'}
$magic=[BitConverter]::ToUInt32($bytes,0)
$count=[BitConverter]::ToInt32($bytes,4)
$dropped=[BitConverter]::ToInt32($bytes,8)
$capacity=[BitConverter]::ToUInt32($bytes,12)
if($magic -ne 0x3144424b -or $capacity -ne 4096 -or $bytes.Length -ne 16+64*$capacity){throw 'Invalid keyboard observation format'}
if($dropped -or $count -lt 0 -or $count -gt $capacity){throw "Incomplete keyboard observation: count=$count dropped=$dropped"}
$names=@('', 'KEY', 'COUNT_BEFORE', 'MARKER', 'COUNT_AFTER', 'PORT60', 'HISTORY', 'PREPEND', 'RETURNED_KEY')
$lengths=@(0,9,7,2,2,7,7,6,9)
for($i=0;$i -lt $count;++$i){
    $offset=16+64*$i
    $kind=[BitConverter]::ToInt32($bytes,$offset)
    if($kind -lt 1 -or $kind -gt 8){throw "Unpublished/invalid event $i"}
    $data=for($j=0;$j -lt $lengths[$kind];++$j){[BitConverter]::ToInt32($bytes,$offset+12+4*$j)}
    [pscustomobject]@{Index=$i;Kind=$names[$kind];Tick=[BitConverter]::ToUInt32($bytes,$offset+4);Thread=[BitConverter]::ToUInt32($bytes,$offset+8);Data=($data -join ',')}
}
