param(
    [Parameter(Mandatory)][string]$UserImage,
    [Parameter(Mandatory)][string]$DisplayDriver
)
$ErrorActionPreference='Stop'
# Read-only corroboration of USER 3:0DE6..0E1C / 3:1794..179D.
# These offsets belong to the pinned owner-provided binaries, not arbitrary NE files.
$identities=@{
    User='98036967F8396B387DA431699A9D3BE95EB56FCD0118F529E1FC2F25105EFAC6'
    Driver='53312ED013BDA513C74E33659F4EE7D1B4F2B1CF1A06AED672432A8FBC4A820A'
}
if((Get-FileHash -LiteralPath $UserImage).Hash -ne $identities.User){throw 'Unexpected USER identity'}
if((Get-FileHash -LiteralPath $DisplayDriver).Hash -ne $identities.Driver){throw 'Unexpected display-driver identity'}
$u=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $UserImage))
$d=[IO.File]::ReadAllBytes((Resolve-Path -LiteralPath $DisplayDriver))
function Word([byte[]]$image,[int]$offset){[BitConverter]::ToUInt16($image,$offset)}
$ne=[BitConverter]::ToInt32($u,60)
$segment=Word $u ($ne+14)
$table=$ne+(Word $u ($ne+34))
$dataOffset=[int](Word $u ($table+($segment-1)*8)) -shl (Word $u ($ne+50))
$type=[Text.Encoding]::ASCII.GetString($u,$dataOffset+0x1e2,6)
if($type -cne 'OEMBIN' -or $u[$dataOffset+0x1e8] -ne 0){throw 'Unexpected USER resource type'}
$ne=[BitConverter]::ToInt32($d,60)
$table=$ne+(Word $d ($ne+36));$shift=Word $d $table;$cursor=$table+2
$found=$null
while(($kind=Word $d $cursor) -ne 0){
    $count=Word $d ($cursor+2);$cursor+=8
    $name=if($kind -band 0x8000){''}else{[Text.Encoding]::ASCII.GetString($d,$table+$kind+1,$d[$table+$kind])}
    for($i=0;$i -lt $count;++$i){
        $id=Word $d ($cursor+6)
        if($name -ceq $type -and $id -eq 0x8001){
            if($found){throw 'Duplicate metrics resource'}
            $offset=[int](Word $d $cursor) -shl $shift
            $length=[int](Word $d ($cursor+2)) -shl $shift
            if($length -lt 4 -or $offset+$length -gt $d.Length){throw 'Invalid metrics resource extent'}
            $found=@{
                userSha256=$identities.User;driverSha256=$identities.Driver
                userDataSegment=$segment;userTypeOffset='01E2';resourceType=$type;resourceId=1
                resourceOffset=$offset;resourceLength=$length
                verticalThumb= (Word $d $offset);horizontalThumb=(Word $d ($offset+2))
            }
        }
        $cursor+=12
    }
}
if(!$found){throw 'Metrics resource not found'}
if($found.verticalThumb -ne 17 -or $found.horizontalThumb -ne 17){throw 'Unexpected original VGA thumb dimensions'}
$found|ConvertTo-Json
