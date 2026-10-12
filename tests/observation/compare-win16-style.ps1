param(
 [Parameter(Mandatory)][string]$Reference,
 [Parameter(Mandatory)][string]$Capture,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($OutputRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-only output'}
$null=New-Item -ItemType Directory $root -Force
Add-Type -AssemblyName System.Drawing
$a=[Drawing.Bitmap]::FromFile($Reference);$b=[Drawing.Bitmap]::FromFile($Capture)
# Coordinate fixtures identify the supplied Control Panel image and the
# directly captured minimize-only trial. This is not a whole-screen metric.
$pairs=@(
 @{name='Control box';a=@(4,8,30,30);b=@(524,34,30,30)},
 @{name='Minimize';a=@(611,8,30,30);b=@(903,34,30,30)},
 @{name='Title';a=@(252,10,170,27);b=@(659,36,170,27)}
)
$canvas=[Drawing.Bitmap]::new(1000,530);$g=[Drawing.Graphics]::FromImage($canvas)
$font=[Drawing.Font]::new('Arial',12)
$g.Clear([Drawing.Color]::White)
$g.InterpolationMode=[Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
$g.PixelOffsetMode=[Drawing.Drawing2D.PixelOffsetMode]::Half
$g.DrawString('Reference / probe — buttons 4x, title 2x; nearest-neighbour, not a similarity pass',
 $font,[Drawing.Brushes]::Black,10,5)
$results=@();$y=40
try {
 foreach($p in $pairs){
    $g.DrawString($p.name,$font,[Drawing.Brushes]::Black,10,$y)
    $y+=25
    for($n=0;$n -lt 2;$n++){
       $src=if($n -eq 0){$a}else{$b}
       $q=if($n -eq 0){$p.a}else{$p.b}
       $rectangle=[Drawing.Rectangle]::new($q[0],$q[1],$q[2],$q[3])
       # Title at 2x to fit; buttons at 4x. No anisotropic stretching.
       $scale=if($p.name -eq 'Title'){2}else{4}
       $dest=[Drawing.Rectangle]::new(10+500*$n,$y,$q[2]*$scale,$q[3]*$scale)
       $g.DrawImage($src,$dest,$rectangle,[Drawing.GraphicsUnit]::Pixel)
       $colors=@{}
       $minX=$q[2];$minY=$q[3];$maxX=-1;$maxY=-1;$ink=0
       for($cy=$q[1];$cy -lt $q[1]+$q[3];$cy++){
          for($cx=$q[0];$cx -lt $q[0]+$q[2];$cx++){
             $pixel=$src.GetPixel($cx,$cy)
             $key=$pixel.ToArgb().ToString('X8')
             $colors[$key]=1+$colors[$key]
             if($pixel.R -lt 80 -and $pixel.G -lt 80 -and $pixel.B -lt 80){
                $ink++;$minX=[Math]::Min($minX,$cx-$q[0]);$maxX=[Math]::Max($maxX,$cx-$q[0])
                $minY=[Math]::Min($minY,$cy-$q[1]);$maxY=[Math]::Max($maxY,$cy-$q[1])
             }
          }
       }
       $results+=@{part=$p.name;source=$n;rectangle=$q;darkWidth=$maxX-$minX+1;darkHeight=$maxY-$minY+1;darkPixels=$ink;dominant=@($colors.GetEnumerator()|Sort-Object Value -Descending|Select-Object -First 6)}
    }
    $y+=125
 }
 $canvas.Save("$root/detail.png",[Drawing.Imaging.ImageFormat]::Png)
 $results|ConvertTo-Json -Depth 5|Set-Content "$root/pixels.json"
 # Raw edge samples retain interpolation colours instead of treating the
 # supplied screenshot as an unscaled palette-indexed bitmap.
 $lines=@()
 foreach($s in @(@{name='reference-left-frame';bmp=$a;x=0;y=100;n=10},
                 @{name='probe-left-frame';bmp=$b;x=520;y=100;n=10})){
    $samples=@()
    for($i=0;$i -lt $s.n;$i++){
       $pixel=$s.bmp.GetPixel($s.x+$i,$s.y)
       $samples+=@{offset=$i;rgb=('{0:X2}{1:X2}{2:X2}' -f $pixel.R,$pixel.G,$pixel.B)}
    }
    $lines+=@{name=$s.name;x=$s.x;y=$s.y;samples=$samples}
 }
 $lines|ConvertTo-Json -Depth 5|Set-Content "$root/edge-samples.json"
 Get-FileHash $Reference,$Capture|ConvertTo-Json|Set-Content "$root/inputs.json"
}finally{$g.Dispose();$font.Dispose();$canvas.Dispose();$a.Dispose();$b.Dispose()}
