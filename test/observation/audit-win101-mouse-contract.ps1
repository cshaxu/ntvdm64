param(
    [string]$Driver = 'O:\Windows\MOUSE.DRV',
    [string]$Snapshot = 'O:\Windows\_update\logs\ega-ports-memory.bin'
)
$ErrorActionPreference = 'Stop'
# Read-only original-media/retained-evidence audit, not a runtime mouse test.
$bytes = [IO.File]::ReadAllBytes($Driver)
function U16([int]$Offset) {
    if ($Offset -lt 0 -or $Offset + 2 -gt $bytes.Length) { throw 'Truncated NE word' }
    [BitConverter]::ToUInt16($bytes, $Offset)
}
if ((U16 0) -ne 0x5a4d -or $bytes.Length -lt 0x40) { throw 'Not MZ' }
$ne = [BitConverter]::ToUInt32($bytes, 0x3c)
if ((U16 $ne) -ne 0x454e) { throw 'Not NE' }
$segmentCount = U16 ($ne + 0x1c)
$table = $ne + (U16 ($ne + 0x22))
$shift = U16 ($ne + 0x32)
if ($shift -gt 16) { throw 'Invalid segment shift' }
$segments = @()
for ($index = 0; $index -lt $segmentCount; $index++) {
    $at = $table + $index * 8
    $offset = [int64](U16 $at) -shl $shift
    $length = U16 ($at + 2)
    if (!$length) { $length = 65536 }
    if ($offset + $length -gt $bytes.Length) { throw 'Truncated segment' }
    $segments += [ordered]@{ number=$index+1; offset=$offset; length=$length; flags=(U16 ($at+4)) }
}
$position = $ne + (U16 ($ne + 4))
$end = $position + (U16 ($ne + 6))
$ordinal = 1
$entries = @()
while ($position -lt $end) {
    $count = $bytes[$position++]
    if (!$count) { break }
    $segment = $bytes[$position++]
    for ($index=0; $index -lt $count; $index++) {
        if (!$segment) { $ordinal++; continue }
        if ($segment -eq 255) { throw 'Movable entry bundle needs separate audit' }
        if ($position+3 -gt $end) { throw 'Truncated entry' }
        $entries += [ordered]@{ ordinal=$ordinal; segment=$segment; flags=$bytes[$position]; offset=(U16 ($position+1)) }
        $position += 3
        $ordinal++
    }
}
$vectors = @()
if ($Snapshot) {
    $memory = [IO.File]::ReadAllBytes($Snapshot)
    foreach ($vector in @(0x0a,0x33,0x71)) {
        if ($memory.Length -lt $vector*4+4) { throw 'Truncated IVT snapshot' }
        $vectors += [ordered]@{
            interrupt=('INT{0:X2}' -f $vector)
            address=('{0:X4}:{1:X4}' -f [BitConverter]::ToUInt16($memory,$vector*4+2),[BitConverter]::ToUInt16($memory,$vector*4))
        }
    }
}
[ordered]@{
    kind='readonly-contract-audit-not-runtime-validation'
    driver=(Resolve-Path -LiteralPath $Driver).Path
    sha256=(Get-FileHash -LiteralPath $Driver -Algorithm SHA256).Hash
    neFlags=(U16 ($ne+0xc)); automaticDataSegment=(U16 ($ne+0xe))
    segments=$segments; entries=$entries
    snapshot=$Snapshot; vectors=$vectors
} | ConvertTo-Json -Depth 6
