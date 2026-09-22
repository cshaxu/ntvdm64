[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$root = [IO.Path]::GetFullPath($BuildRoot)
if ($root -notmatch '\\build\\M0-T420\\S44\\') {
    throw 'S44 command fixture must remain below build/M0-T420/S44.'
}
New-Item -ItemType Directory -Force -Path $root | Out-Null
$fixture = Join-Path $root 'G7.COM'
[IO.File]::WriteAllBytes($fixture, [byte[]](
    0xba,0x0c,0x01,             # MOV DX,010Ch (COM message)
    0xb4,0x09,0xcd,0x21,        # MOV AH,09h; INT 21h
    0xb8,0x07,0x4c,0xcd,0x21,   # MOV AX,4C07h; INT 21h
    0x53,0x31,0x30,0x5f,0x47,0x55,0x45,0x53,0x54,0x5f,0x53,0x45,0x56,0x45,0x4e,0x24
))
Write-Output $fixture
