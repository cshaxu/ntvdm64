[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Path
)

$ErrorActionPreference = 'Stop'

function Require([bool]$condition, [string]$message) {
    if (-not $condition) { throw $message }
}

function Read-LittleEndianWord([string]$hex, [int]$offset) {
    return [Convert]::ToInt32($hex.Substring($offset + 2, 2) + $hex.Substring($offset, 2), 16)
}

$record = Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
Require ($record.schema -eq 'runner-startup-snapshot-evidence-v1') 'unexpected snapshot evidence schema'
Require ($record.boundary.vector -eq 6 -and $record.boundary.fault_rip -eq 0x7c00) 'unexpected neutral-boundary identity'
Require ($record.cpu.mode -eq 1 -and $record.cpu.cs -eq 0 -and $record.cpu.eip -eq 0x7c00) 'unexpected CPU snapshot identity'
Require ($record.ranges.Count -eq 3) 'expected exactly three declared ranges'

$byId = @{}
foreach ($range in $record.ranges) {
    Require (-not $byId.ContainsKey([int]$range.id)) 'duplicate range id'
    Require ($range.bytes_hex -match '\A[0-9a-f]*\z' -and (($range.bytes_hex.Length % 2) -eq 0)) 'invalid lower-hex range payload'
    $byId[[int]$range.id] = $range
}

Require ($byId.ContainsKey(11) -and $byId[11].address -eq 0x40 -and $byId[11].bytes_hex.Length -eq 96) 'invalid IVT snapshot range'
Require ($byId.ContainsKey(12) -and $byId[12].address -eq 0x496 -and $byId[12].bytes_hex -eq '10') 'invalid BDA snapshot range'
Require ($byId.ContainsKey(13) -and $byId[13].address -eq 0x714 -and $byId[13].bytes_hex.Length -eq 8) 'invalid virtual-interrupt snapshot range'
Require ($record.digest -match '\A[0-9a-f]{16}\z') 'invalid snapshot digest'

$ivtHex = [string]$byId[11].bytes_hex
$vectors = @(
    [pscustomobject]@{ Vector = '10h'; Offset = 0x00 },
    [pscustomobject]@{ Vector = '15h'; Offset = 0x14 },
    [pscustomobject]@{ Vector = '19h'; Offset = 0x24 },
    [pscustomobject]@{ Vector = '1Bh'; Offset = 0x2c }
)

$decoded = foreach ($vector in $vectors) {
    $byteOffset = $vector.Offset * 2
    $offset = Read-LittleEndianWord $ivtHex $byteOffset
    $segment = Read-LittleEndianWord $ivtHex ($byteOffset + 4)
    [pscustomobject]@{
        vector = $vector.Vector
        physical_address = ('0x{0:x}' -f (0x40 + $vector.Offset))
        far_pointer = ('{0:x4}:{1:x4}' -f $segment, $offset)
    }
}

[pscustomobject]@{
    schema = $record.schema
    boundary = ('#UD vector {0} at {1:x4}:{2:x4}' -f $record.boundary.vector, $record.cpu.cs, $record.cpu.eip)
    ivt = $decoded
    bda_0040_0096 = ('0x{0}' -f $byId[12].bytes_hex)
    ntvdmstate_0714 = ('0x{0}' -f $byId[13].bytes_hex)
    digest = $record.digest
}
