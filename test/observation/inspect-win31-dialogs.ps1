param([Parameter(Mandatory)][string]$Image,[Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
try {
 $b=[IO.File]::ReadAllBytes((Resolve-Path $Image));function U16($p){[BitConverter]::ToUInt16($b,$p)};function U32($p){[BitConverter]::ToUInt32($b,$p)}
 $ne=U32 60;if((U16 $ne) -ne 0x454e){throw 'NE required'}
 $p=$ne+(U16 ($ne+36));$shift=U16 $p;$p+=2;$records=@()
 while(($type=U16 $p)){$count=U16 ($p+2);$p+=8
  for($i=0;$i -lt $count;$i++) {
   $offset=([long](U16 $p)) -shl $shift;$length=([long](U16 ($p+2))) -shl $shift;$id=U16 ($p+6);$p+=12
   if($type -ne 0x8005){continue};$end=$offset+$length;if($end -gt $b.Length){throw 'Resource outside file'}
   $script:q=$offset
   function Field {
    if($script:q -ge $end){throw 'Truncated field'}
    if($b[$script:q] -eq 255){$script:q++;$v=U16 $script:q;$script:q+=2;return "#$v"}
    $start=$script:q;while($script:q -lt $end -and $b[$script:q]){$script:q++};if($script:q -ge $end){throw 'Unterminated field'}
    $v=[Text.Encoding]::ASCII.GetString($b,$start,$script:q-$start);$script:q++;return $v
   }
   $style=U32 $q;$controls=$b[$q+4];$script:q+=13
   $menu=Field;$class=Field;$title=Field
   if($style -band 64){$point=U16 $q;$script:q+=2;$font=Field}
   $items=@()
   for($c=0;$c -lt $controls;$c++) {
    if($q+15 -gt $end){throw 'Truncated control'}
    $controlId=U16 ($q+8);$cs=U32 ($q+10);$script:q+=14
    if($b[$q] -ge 128){$cls='#{0}' -f $b[$q];$script:q++}else{$cls=Field}
    $text=Field;$extra=$b[$q];$script:q+=1+$extra;if($q -gt $end){throw 'Truncated extra data'}
    $items+=@{id=$controlId;class=$cls;style=('0x{0:X8}' -f $cs);text=$text}
   }
   $records+=@{id=$id;offset=$offset;length=$length;title=$title;style=('0x{0:X8}' -f $style);controls=$items}
  }
 }
 @{source=(Resolve-Path $Image).Path;sha256=(Get-FileHash $Image).Hash;dialogs=$records}|ConvertTo-Json -Depth 8|Set-Content "$root/dialogs.json"
 '0'|Set-Content "$root/exit.txt"
}catch {$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";exit 1}
