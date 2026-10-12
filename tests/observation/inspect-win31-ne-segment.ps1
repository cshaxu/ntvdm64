param(
 [Parameter(Mandatory)][string]$Image,
 [Parameter(Mandatory)][int]$Segment,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned research output required'}
$null=New-Item -ItemType Directory $root
try {
 $bytes=[IO.File]::ReadAllBytes((Resolve-Path $Image))
 function U16([int]$p){[BitConverter]::ToUInt16($bytes,$p)}
 function U32([int]$p){[BitConverter]::ToUInt32($bytes,$p)}
 $ne=U32 60
 if((U16 0) -ne 0x5A4D -or (U16 $ne) -ne 0x454E){throw 'Expected MZ/NE'}
 if($Segment -lt 1 -or $Segment -gt (U16 ($ne+28))){throw 'Invalid segment'}
 $record=$ne+(U16 ($ne+34))+8*($Segment-1)
 # Widen before shifting: PowerShell preserves UInt16 and otherwise wraps
 # segment offsets above 64 KiB back into an earlier segment's relocations.
 $offset=([long](U16 $record)) -shl (U16 ($ne+50));$length=[int](U16 ($record+2));if(!$length){$length=65536}
 $flags=U16 ($record+4)
 if(($flags -band 8) -or $offset+$length -gt $bytes.Length){throw 'Iterated or truncated segment unsupported'}
 $ndisasm=(Get-Command ndisasm.exe -ErrorAction Stop).Source
 & $ndisasm -b 16 -e $offset -k "$length,$($bytes.Length-$offset-$length)" $Image *> "$root/segment.asm"
 if($LASTEXITCODE){throw 'Disassembler failed'}
 $modules=@{}
 $modref=$ne+(U16 ($ne+40));$import=$ne+(U16 ($ne+42))
 for($i=1;$i -le (U16 ($ne+30));$i++){$p=$import+(U16 ($modref+2*($i-1)));$modules[$i]=[Text.Encoding]::ASCII.GetString($bytes,$p+1,$bytes[$p])}
 $relocs=@()
 if($flags -band 0x100){
  $p=$offset+$length;$count=U16 $p;$p+=2
  for($i=0;$i -lt $count;$i++){
   $kind=$bytes[$p];$rel=$bytes[$p+1];$source=U16 ($p+2);$a=U16 ($p+4);$b=U16 ($p+6);$p+=8
   $target=switch($rel -band 3){0{"segment:$($a -band 255):$('0x{0:X4}' -f $b)"};1{"$($modules[[int]$a])!ordinal:$b"};2{$q=$import+$b;"$($modules[[int]$a])!$([Text.Encoding]::ASCII.GetString($bytes,$q+1,$bytes[$q]))"};3{"osfixup:$a/$b"}}
   if(($rel -band 3) -in @(1,2) -and !$modules.ContainsKey([int]$a)){throw 'Unknown imported module'}
   $sites=@();$seen=@{}
   while($source -ne 65535){
    if($source+2 -gt $length -or $seen.ContainsKey($source)){throw 'Invalid relocation chain'}
    $seen[$source]=$true;$sites+=('0x{0:X4}' -f $source)
    if($rel -band 4){break};$source=U16 ($offset+$source)
   }
   $relocs+=@{kind=$kind;flags=$rel;target=$target;sites=$sites}
  }
 }
 @{source=(Resolve-Path $Image).Path;sha256=(Get-FileHash $Image).Hash;segment=$Segment;offset=$offset;length=$length;flags=$flags;relocations=$relocs;role='Static read-only research; unrelocated 16-bit disassembly'}|ConvertTo-Json -Depth 7|Set-Content "$root/segment.json"
 '0'|Set-Content "$root/exit-code.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
