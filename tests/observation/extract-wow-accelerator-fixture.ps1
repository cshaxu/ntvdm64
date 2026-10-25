param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Fixture output must be under repository build.'}
$source=Join-Path $repo 'src/mvdm/wow32/waccel.c'
$text=[IO.File]::ReadAllText($source)
$signature='HACCEL CreateAccel32(VPVOID vpAccel16, DWORD cbAccel16)'
$start=$text.IndexOf($signature)
if($start -lt 0){throw 'Original CreateAccel32 not found.'}
$brace=$text.IndexOf('{',$start);$depth=0;$finish=-1
for($i=$brace;$i -lt $text.Length;++$i){
    if($text[$i] -eq '{'){++$depth}elseif($text[$i] -eq '}'){--$depth;if(!$depth){$finish=$i+1;break}}
}
if($finish -le $start){throw 'Original CreateAccel32 boundary changed.'}
New-Item -ItemType Directory -Force $out|Out-Null
[IO.File]::WriteAllText((Join-Path $out 'accel-create32.inc'),$text.Substring($start,$finish-$start),[Text.Encoding]::ASCII)
Get-FileHash $source -Algorithm SHA256
