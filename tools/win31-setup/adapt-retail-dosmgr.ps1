param([Parameter(Mandatory)][string]$OriginalWin386,
      [Parameter(Mandatory)][string]$NtDos,
      [Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)){throw 'Fresh build-owned output required'}
if((Get-FileHash $OriginalWin386).Hash -ne '6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5'){throw 'Unsupported retail WIN386 identity'}
if((Get-FileHash $NtDos).Hash -ne '957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84'){throw 'Unsupported NT DOS ABI identity'}
$original=[IO.File]::ReadAllBytes((Resolve-Path $OriginalWin386).Path)
$image=[byte[]]$original.Clone()
function U16($o){[BitConverter]::ToUInt16($image,$o)}
function U32($o){[BitConverter]::ToUInt32($image,$o)}
$w3=U32 60
if([Text.Encoding]::ASCII.GetString($image,$w3,2) -ne 'W3'){throw 'Not W3'}
$module=$null
for($i=0;$i -lt (U16 ($w3+4));$i++){
    $entry=$w3+16+16*$i
    if([Text.Encoding]::ASCII.GetString($image,$entry,8).Trim() -eq 'DOSMGR'){$module=U32 ($entry+8)}
}
if(!$module -or [Text.Encoding]::ASCII.GetString($image,$module,2) -ne 'LE'){throw 'No LE DOSMGR'}
$objects=$module+(U32 ($module+64));$object=$objects+24
if((U32 ($module+68)) -ne 3 -or (U32 ($object+8)) -ne 0x2015){throw 'Unexpected DOSMGR object contract'}
$pageSize=U32 ($module+40);$pageStart=U32 ($object+12)
$pageTable=$module+(U32 ($module+72));$dataBase=U32 ($module+128)
function FileOffset([int]$offset){
    $page=[int][Math]::Floor($offset/$pageSize)
    if($page -ge (U32 ($object+16))){throw 'Object range overflow'}
    $record=$pageTable+4*($pageStart-1+$page)
    if($image[$record+3] -ne 0){throw 'Unsupported page type'}
    $number=$image[$record]*65536+$image[$record+1]*256+$image[$record+2]
    $dataBase+($number-1)*$pageSize+($offset%$pageSize)
}
function ExpectBytes($offset,[byte[]]$expected){
    for($i=0;$i -lt $expected.Length;$i++){if($image[$offset+$i] -ne $expected[$i]){throw 'Before-byte contract mismatch'}}
}
# Replace only the unsupported SFT-size discovery. System-VM/client context
# has already been established at 186D. No temporary buffer or CON handles
# have been acquired, so bypassing that complete probe leaves nothing to close.
# The caller consumes carry only, then overwrites EAX (12CA/12D0).
if((U32 $object) -ne 0x3580 -or $pageSize -ne 4096 -or (U32 ($object+16)) -ne 4){throw 'Unexpected padding/layout'}
ExpectBytes (FileOffset 0x186a) ([byte[]]@(0x8b,0x6b,8))
ExpectBytes (FileOffset 0x186d) ([byte[]]@(0x6a,0,0x6a,4,0xcd,0x20,0xa9,0,1,0))
ExpectBytes (FileOffset 0x28e8) ([byte[]]::new(4)) # temporary allocation initially absent
ExpectBytes (FileOffset 0x199a) ([byte[]]@(0x89,0x15,0x9c,0x3a,0,0,0xf8))
$patchOffset=FileOffset 0x186d
$patch=[byte[]]@(0xba,33,0,0,0,0xe9,0,0,0,0)
[Array]::Copy([BitConverter]::GetBytes([int32](0x199a-0x1877)),0,$patch,6,4)
[Array]::Copy($patch,0,$image,$patchOffset,10)
$regions=@(,@($patchOffset,10))
# No LE relocation source may target the changed instruction span.
$fixupPage=$module+(U32 ($module+104));$fixupBase=$module+(U32 ($module+108))
for($pageIndex=0;$pageIndex -lt (U32 ($module+20));$pageIndex++){
    $pos=$fixupBase+(U32 ($fixupPage+4*$pageIndex));$end=$fixupBase+(U32 ($fixupPage+4*($pageIndex+1)))
    while($pos -lt $end){
        $sourceType=$image[$pos];$flags=$image[$pos+1];$pos+=2
        if($flags -band 3){throw 'Unsupported external fixup'}
        if($sourceType -band 32){$count=$image[$pos];$pos++;$sources=$null}else{$sources=@([BitConverter]::ToInt16($image,$pos));$pos+=2}
        $pos+= $(if($flags -band 64){2}else{1})
        $pos+= $(if($flags -band 16){4}else{2})
        if($flags -band 4){$pos+= $(if($flags -band 32){4}else{2})}
        if($null -eq $sources){$sources=@();for($i=0;$i -lt $count;$i++){$sources+=[BitConverter]::ToInt16($image,$pos);$pos+=2}}
        foreach($sourceOffset in $sources){
            $local=($pageIndex-($pageStart-1))*4096+$sourceOffset
            foreach($span in @(,@(0x186d,10))){
                if($local -ge $span[0]-3 -and $local -lt $span[0]+$span[1]){throw 'Relocation overlaps new code'}
            }
        }
    }
    if($pos -ne $end){throw 'Fixup parser boundary mismatch'}
}
$sha=[Security.Cryptography.SHA256]::Create();$position=0
try{
    foreach($region in $regions){
        $length=$region[0]-$position
        if($length -lt 0){throw 'Overlapping patch regions'}
        if([BitConverter]::ToString($sha.ComputeHash($image,$position,$length)) -ne [BitConverter]::ToString($sha.ComputeHash($original,$position,$length))){throw 'Unexpected outside-region change'}
        $position=$region[0]+$region[1]
    }
    if([BitConverter]::ToString($sha.ComputeHash($image,$position,$image.Length-$position)) -ne [BitConverter]::ToString($sha.ComputeHash($original,$position,$image.Length-$position))){throw 'Unexpected tail change'}
}finally{$sha.Dispose()}
New-Item -ItemType Directory -Path $output|Out-Null
[IO.File]::WriteAllBytes("$output/WIN386.EXE",$image)
[ordered]@{role='owner-approved-retail-copy-experiment-not-runtime-acceptance';originalSha256=(Get-FileHash $OriginalWin386).Hash;ntDosSha256=(Get-FileHash $NtDos).Hash;outputSha256=(Get-FileHash "$output/WIN386.EXE").Hash;regions=@($regions|ForEach-Object {@{offset=$_[0];length=$_[1]}});entrySize=33;discoveryBypassedBeforeResourceAcquisition=$true;clientStatePairPreserved=$true;existingRelocatedCommitPreserved=$true;allOtherBytesUnchanged=$true}|ConvertTo-Json -Depth 4|Set-Content "$output/adaptation.json"
'PASS exact-ABI discovery substitution, no acquired probe resources, relocation/range checks; execution unverified'
