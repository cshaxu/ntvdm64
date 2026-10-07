param(
    [Parameter(Mandatory)][string]$OriginalImage,
    [Parameter(Mandatory)][string]$Driver,
    [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
   (Test-Path -LiteralPath $output)) { throw 'Fresh build-owned output required' }
$image=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $OriginalImage).Path)
$addon=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $Driver).Path)
function Word($b,[int]$p) {
    if($p -lt 0 -or $p+2 -gt $b.Length){throw 'Truncated word'}
    [BitConverter]::ToUInt16($b,$p)
}
function PutWord($b,[int]$p,[int]$v) {
    if($v -lt 0 -or $v -gt 65535 -or $p+2 -gt $b.Length){throw 'Word overflow'}
    [Array]::Copy([BitConverter]::GetBytes([uint16]$v),0,$b,$p,2)
}
function ModuleName($b,[int]$ne) {
    if((Word $b $ne) -ne 0x454e){throw 'Not NE'}
    $p=$ne+(Word $b ($ne+0x26));$n=$b[$p]
    if(!$n -or $p+1+$n+2 -gt $b.Length){throw 'Invalid module name'}
    [Text.Encoding]::ASCII.GetString($b,$p+1,$n)
}
if((Word $image 0) -ne 0x5a4d -or (Word $addon 0) -ne 0x5a4d){throw 'Not MZ'}
$current=[BitConverter]::ToUInt32($image,0x3c)
$seen=@{};$modules=@();$mouse=$null;$next=0
while($current) {
    if($seen.ContainsKey($current) -or $current+64 -gt $image.Length){throw 'Invalid module chain'}
    if((Word $image $current) -eq 0 -and (Word $image ($current+2)) -eq 0){break}
    $seen[$current]=$true
    $name=ModuleName $image $current
    $after=(Word $image ($current+8))*16
    if($after -and $after -le $current){throw 'Nonascending module chain'}
    $modules+=@{name=$name;offset=$current;next=$after}
    if($name -eq 'MOUSE'){$mouse=$current;$next=$after}
    $current=$after
}
if(!$mouse -or !$next -or $next -gt $image.Length){throw 'No bounded MOUSE slot'}
$ne=[BitConverter]::ToUInt32($addon,0x3c)
if((ModuleName $addon $ne) -ne 'MOUSE' -or (Word $addon ($ne+0x1e)) -ne 0 -or
   (Word $addon ($ne+0x20)) -ne 0 -or (Word $addon ($ne+0x32)) -ne 4){throw 'Unsupported addon NE shape'}
$count=Word $addon ($ne+0x1c)
if($count -ne 2 -or (Word $addon ($ne+0xe)) -ne 2){throw 'Unexpected addon segments'}
$table=$ne+(Word $addon ($ne+0x22))
$first=(Word $addon $table)*16
$headerLength=$first-$ne
if($headerLength -lt 64 -or $headerLength%16){throw 'Unaligned addon header'}
$patched=[byte[]]$image.Clone()
[Array]::Clear($patched,$mouse,$next-$mouse)
[Array]::Copy($addon,$ne,$patched,$mouse,$headerLength)
# Setup repurposes NE checksum as the next-module paragraph; preserve it.
[Array]::Copy($image,$mouse+8,$patched,$mouse+8,4)
$position=$mouse+$headerLength
$segments=@()
for($index=0;$index -lt $count;$index++) {
    $record=$table+$index*8
    $source=(Word $addon $record)*16
    $length=Word $addon ($record+2)
    $flags=Word $addon ($record+4)
    if(!$length -or ($flags -band 0x100) -or $source+$length -gt $addon.Length){throw 'Unsupported segment/relocation'}
    if($position+$length -gt $next){throw 'Addon exceeds existing mouse slot; no output written'}
    [Array]::Copy($addon,$source,$patched,$position,$length)
    PutWord $patched ($mouse+(Word $addon ($ne+0x22))+$index*8) ($position/16)
    $segments+=@{number=$index+1;offset=$position;length=$length;flags=$flags}
    $position=($position+$length+15) -band -16
}
# Exact invariants: file length, module-chain locations and all other bytes.
if($patched.Length -ne $image.Length){throw 'Image length changed'}
for($i=0;$i -lt $image.Length;$i++) {
    if(($i -lt $mouse -or $i -ge $next) -and $patched[$i] -ne $image[$i]){throw 'Nonmouse byte changed'}
}
foreach($m in $modules) {
    if((ModuleName $patched $m.offset) -ne $m.name -or (Word $patched ($m.offset+8))*16 -ne $m.next){throw 'Module chain changed'}
}
New-Item -ItemType Directory -Path $output | Out-Null
[IO.File]::WriteAllBytes((Join-Path $output 'WIN100.BIN'),$patched)
[ordered]@{
    kind='owner-authorized-mouse-module-installation-not-runtime-validation'
    original=(Resolve-Path $OriginalImage).Path
    originalSha256=(Get-FileHash $OriginalImage).Hash
    driverSha256=(Get-FileHash $Driver).Hash
    outputSha256=(Get-FileHash (Join-Path $output 'WIN100.BIN')).Hash
    imageLength=$image.Length
    changedRange=@{start=$mouse;endExclusive=$next}
    allOtherBytesUnchanged=$true
    modules=$modules;segments=$segments
} | ConvertTo-Json -Depth 6 | Set-Content (Join-Path $output 'installation.json')
'PASS structural mouse-slot replacement; Windows loading still unverified'
