param(
 [Parameter(Mandatory)][string]$Driver,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$VerifyHeader,
 [switch]$MenuGlyphs,
 [switch]$ControlGlyphs,
 [switch]$ComboGlyphs,
 [switch]$ScrollGlyphs,
 [switch]$DisabledScrollGlyphs
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
 throw 'Fresh build-owned research output required'
}
$null=New-Item -ItemType Directory $root
try {
 Add-Type -AssemblyName System.Drawing
 $bytes=[IO.File]::ReadAllBytes((Resolve-Path $Driver))
 function U16([int]$p){[BitConverter]::ToUInt16($bytes,$p)}
 function U32([int]$p){[BitConverter]::ToUInt32($bytes,$p)}
 $ne=U32 60
 if((U16 0) -ne 0x5A4D -or (U16 $ne) -ne 0x454E){throw 'Expected MZ/NE driver'}
 $p=$ne+(U16 ($ne+36));$shift=U16 $p;$p+=2
 if($shift -gt 16){throw 'Unsupported resource alignment'}
 $names=@{32754='CLOSE';32749='REDUCE';32748='ZOOM';32747='RESTORE';32746='REDUCED';32745='ZOOMD';32744='RESTORED'}
 if(([int]$MenuGlyphs.IsPresent+[int]$ControlGlyphs.IsPresent+[int]$ComboGlyphs.IsPresent+[int]$ScrollGlyphs.IsPresent+[int]$DisabledScrollGlyphs.IsPresent) -gt 1){throw 'Select one resource family'}
 if($MenuGlyphs){$names=@{32760='CHECK';32739='MNARROW'}}
 if($ControlGlyphs){
  if($MenuGlyphs){throw 'Select one resource family'}
  $names=@{32759='CHECKBOXES'}
 }
 if($ComboGlyphs){$names=@{32738='COMBO'}}
 if($ScrollGlyphs){$names=@{32753='UPARROW';32752='DNARROW';32751='RGARROW';32750='LFARROW';32743='UPARROWD';32742='DNARROWD';32741='RGARROWD';32740='LFARROWD'}}
 if($DisabledScrollGlyphs){$names=@{32737='UPARROWI';32736='DNARROWI';32735='RGARROWI';32734='LFARROWI'}}
 $items=@();$verified=0
 while(($type=U16 $p) -ne 0){
  $count=U16 ($p+2);$p+=8
  for($i=0;$i -lt $count;$i++){
   $offset=([long](U16 $p)) -shl $shift;$length=([long](U16 ($p+2))) -shl $shift;$id=U16 ($p+6);$p+=12
   $number=$id -band 32767
   if($type -ne 0x8002 -or !($id -band 32768) -or !$names.ContainsKey($number)){continue}
   if($offset+$length -gt $bytes.Length){throw 'Resource outside image'}
   $header=U32 $offset
   if($header -eq 12){$w=U16 ($offset+4);$h=U16 ($offset+6);$planes=U16 ($offset+8);$bits=U16 ($offset+10);$entry=3}
   elseif($header -eq 40){$w=U32 ($offset+4);$h=U32 ($offset+8);$planes=U16 ($offset+12);$bits=U16 ($offset+14);$entry=4;if((U32 ($offset+16)) -ne 0){throw 'Compressed bitmap'}}
   else{throw "Unsupported bitmap header $header"}
   if($bits -notin @(1,4) -or $planes -ne 1 -or $w -lt 1 -or $h -lt 1 -or $w -gt 256 -or $h -gt 256){throw 'Unsupported bitmap shape'}
   $stride=[int]([Math]::Floor(($w*$bits+31)/32)*4)
   $colors=1 -shl $bits
   $pixels=$offset+$header+$colors*$entry
   if($pixels+$stride*$h -gt $offset+$length){throw 'Bitmap pixels outside resource'}
   $palette=@();for($c=0;$c -lt $colors;$c++){$q=$offset+$header+$c*$entry;$palette+=[Drawing.Color]::FromArgb($bytes[$q+2],$bytes[$q+1],$bytes[$q])}
   $bitmap=[Drawing.Bitmap]::new([int]$w,[int]$h)
   try {
    $rows=@()
    for($y=0;$y -lt $h;$y++){$row='';for($x=0;$x -lt $w;$x++){
     $v=$bytes[$pixels+($h-1-$y)*$stride+[int][Math]::Floor($x*$bits/8)]
     $index=if($bits -eq 1){($v -shr (7-($x%8))) -band 1}elseif(($x%2) -eq 0){$v -shr 4}else{$v -band 15}
     $bitmap.SetPixel($x,$y,$palette[$index])
     if($VerifyHeader){
      $color=$palette[$index]
      if($color.R -ne $color.G -or $color.R -ne $color.B){throw 'Unexpected non-grey OEM pixel'}
      $row+=switch($color.R){0{'0'}255{'1'}128{'2'}192{'3'}default{throw 'Unexpected OEM shade'}}
     }
    };$rows+=$row}
    if($VerifyHeader){
     $source=Get-Content -Raw $VerifyHeader
     $pattern='(?s)/\* '+$names[$number]+' \*/(.*?)\}'
     $match=[regex]::Match($source,$pattern)
     if(!$match.Success){throw "Missing embedded $($names[$number])"}
     $embedded=@([regex]::Matches($match.Groups[1].Value,('"([0-3]{'+$w+'})"'))|ForEach-Object {$_.Groups[1].Value})
     if(($embedded -join "`n") -cne ($rows -join "`n")){throw "Embedded pixels differ: $($names[$number])"}
     $verified++
    }
    $bitmap.Save("$root/$($names[$number]).png",[Drawing.Imaging.ImageFormat]::Png)
   }finally{$bitmap.Dispose()}
   $items+=@{id=$number;name=$names[$number];offset=$offset;length=$length;width=$w;height=$h;header=$header}
  }
 }
 if($items.Count -ne $names.Count){throw 'Missing expected OEM resources'}
 if($VerifyHeader -and $verified -ne $names.Count){throw 'Expected all embedded resources verified'}
 $canvas=[Drawing.Bitmap]::new(680,[int](16+98*$items.Count));$g=[Drawing.Graphics]::FromImage($canvas);$font=[Drawing.Font]::new('Arial',11)
 try {
  $g.Clear([Drawing.Color]::White)
  $g.InterpolationMode=[Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
  $g.PixelOffsetMode=[Drawing.Drawing2D.PixelOffsetMode]::Half
  $row=0
  foreach($item in ($items|Sort-Object id -Descending)){
   $g.DrawString("$($item.name)  #$($item.id)  $($item.width)x$($item.height)",$font,[Drawing.Brushes]::Black,8,8+$row*98)
   $bitmap=[Drawing.Bitmap]::FromFile("$root/$($item.name).png")
   try{$g.DrawImage($bitmap,[Drawing.Rectangle]::new(280,8+$row*98,$bitmap.Width*4,$bitmap.Height*4),0,0,$bitmap.Width,$bitmap.Height,[Drawing.GraphicsUnit]::Pixel)}finally{$bitmap.Dispose()}
   $row++
  }
  $canvas.Save("$root/oem-buttons.png",[Drawing.Imaging.ImageFormat]::Png)
 }finally{$font.Dispose();$g.Dispose();$canvas.Dispose()}
 @{source=(Resolve-Path $Driver).Path;sha256=(Get-FileHash $Driver).Hash;resources=$items;verifiedEmbedded=$verified;role='Read-only original visual resource verification'}|ConvertTo-Json -Depth 5|Set-Content "$root/resources.json"
 '0'|Set-Content "$root/exit-code.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
