param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
$tool="$repo/build/.ninja/build-tools/ntvdm-package-id.exe"
$id=(& $tool --new).Trim();if($LASTEXITCODE -or ([guid]$id) -eq [guid]::Empty){throw 'Invalid generated identity'}
foreach($case in @(@('x86','ntvdm.exe'),@('x64','run16.exe'))){
 $input="$repo/assets/release/$($case[1])";$before=(Get-FileHash $input).Hash
 & $tool --machine $case[0] $input;if($LASTEXITCODE){throw 'Valid machine rejected'}
 $wrong=if($case[0] -eq 'x86'){'x64'}else{'x86'}
 & $tool --machine $wrong $input;if(!$LASTEXITCODE){throw 'Wrong machine accepted'}
 $copy=Join-Path $out $case[1];Copy-Item $input $copy
 & $tool $id $copy;if($LASTEXITCODE){throw 'Stamp failed'}
 $read=(& $tool --read $copy).Trim()
 if($LASTEXITCODE -or $read -ne [Convert]::ToHexString(([guid]$id).ToByteArray())){throw 'Stamp/read mismatch'}
 & $tool --machine $case[0] $copy;if($LASTEXITCODE){throw 'Stamped machine rejected'}
 if((Get-FileHash $input).Hash -ne $before){throw 'Source changed'}
}
$file=Join-Path $out 'invalid.exe'
foreach($kind in 'empty','signature','offset','magic'){
 $b=[IO.File]::ReadAllBytes("$repo/assets/release/ntvdm.exe")
 switch($kind){
  empty {$b=[byte[]]@()}
  signature {$b[0]=0}
  offset {[BitConverter]::GetBytes([int]::MaxValue).CopyTo($b,60)}
  magic {$pe=[BitConverter]::ToInt32($b,60);$b[$pe+24]=0;$b[$pe+25]=0}
 }
 [IO.File]::WriteAllBytes($file,$b)
 & $tool --machine x86 $file;if(!$LASTEXITCODE){throw "Malformed PE accepted: $kind"}
}
& $tool --machine arm64 $file;if(!$LASTEXITCODE){throw 'Unknown architecture accepted'}
& $tool --machine x86 "$out/missing.exe";if(!$LASTEXITCODE){throw 'Absent image accepted'}
& $tool '00000000-0000-0000-0000-000000000000' "$out/run16.exe";if(!$LASTEXITCODE){throw 'Empty identity accepted'}
'PASS UUID, x86/x64, stamp/read, source preservation, malformed/missing/machine rejection'
$global:LASTEXITCODE=0
