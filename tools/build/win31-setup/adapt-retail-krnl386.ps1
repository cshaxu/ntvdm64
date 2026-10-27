param(
    [Parameter(Mandatory)][string]$OriginalKrnl386,
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference = 'Stop'
$retailHash = 'FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980'
$candidateHash = '75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A'
$offset = 0xCFED
[byte[]]$before = 0x33,0xDB,0x3C,0x0A,0x73,0x22,0x3C,0x03,0x77,0x12,0xBB,0x38,0x00,0x80,0xFC,0x00,0x74,0x16,0xBB,0x35,0x00,0x80,0xFC,0x1F,0x76,0x0E,0xEB,0x00,0x50,0xE8,0x23,0x00,0x8B,0xD8,0x58,0x83,0xFB,0xFF,0x74,0x0B
[byte[]]$after = 0xBB,0x21,0x00,0xE9,0x22,0x00 + (1..34 | ForEach-Object {[byte]0x90})
$source = (Resolve-Path -LiteralPath $OriginalKrnl386).Path
if((Get-FileHash -LiteralPath $source).Hash -ne $retailHash) {throw 'Unsupported retail KRNL386 identity'}
if(Test-Path -LiteralPath $OutputRoot) {throw 'Use a fresh build-owned KRNL386 output'}
New-Item -ItemType Directory -Path $OutputRoot | Out-Null
$target = Join-Path $OutputRoot 'KRNL386.EXE'
[byte[]]$image = [IO.File]::ReadAllBytes($source)
if($offset + $before.Length -gt $image.Length) {throw 'KRNL386 patch range is outside file'}
for($i=0; $i -lt $before.Length; $i++) {if($image[$offset+$i] -ne $before[$i]) {throw "Unexpected retail KRNL386 byte at +0x$('{0:X}' -f ($offset+$i))"}}
[Array]::Copy($after,0,$image,$offset,$after.Length)
if($image.Length -lt 0x402 -or [BitConverter]::ToUInt32($image,0x3c) -ne 0x400 -or $image[0x400] -ne 0x4e -or $image[0x401] -ne 0x45) {throw 'KRNL386 header contract mismatch'}
$image[0x3d]=0
[IO.File]::WriteAllBytes($target,$image)
if((Get-FileHash -LiteralPath $target).Hash -ne $candidateHash) {throw 'KRNL386 candidate identity mismatch'}
[ordered]@{
    role='owner-approved-recoverable-standard-mode-SFT-probe-adaptation'
    originalSha256=$retailHash
    outputSha256=$candidateHash
    offset=('0x{0:X}' -f $offset)
    length=$before.Length
    before=([BitConverter]::ToString($before)).Replace('-','')
    after=([BitConverter]::ToString($after)).Replace('-','')
    headerOffset='0x3D'
    headerBefore='04'
    headerAfter='00'
    allOtherBytesUnchanged=$true
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $OutputRoot 'adaptation.json') -Encoding UTF8
Write-Host 'PASS exact KRNL386 SFT-probe adaptation; output identity verified'
