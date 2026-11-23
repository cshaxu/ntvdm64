[CmdletBinding()]
param([Parameter(Mandatory)][string]$EvidenceRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($EvidenceRoot,$repo)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){
 throw 'Fresh build-owned evidence root required'
}
$null=New-Item -ItemType Directory $root
$results=[Collections.Generic.List[object]]::new()
function Owned([string]$Name,[string]$Image,[string]$Arguments,[int]$Seconds,[int]$Expected=0){
 $options=@{FilePath=$Image;WorkingDirectory=$repo;WindowStyle='Hidden';PassThru=$true;
  RedirectStandardOutput="$root/$Name.stdout.txt";RedirectStandardError="$root/$Name.stderr.txt"}
 if($Arguments){$options.ArgumentList=$Arguments}
 $p=Start-Process @options
 try{
  if(!$p.WaitForExit($Seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $Name deadline"}
  $p.WaitForExit();$results.Add(@{case=$Name;pid=$p.Id;exit=$p.ExitCode;
   started=$p.StartTime.ToUniversalTime().ToString('o');finished=$p.ExitTime.ToUniversalTime().ToString('o')})
  if($p.ExitCode -ne $Expected){throw "$Name returned $($p.ExitCode), expected $Expected"}
 }finally{$p.Dispose()}
}
$result=@{result='failed';cases=$results;product_changed=$false;guest_changed=$false}
try{
 $images=@()
 foreach($arch in 'x86','x64'){
  Owned "build-$arch" $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" '+$arch+' native-payload-probes"') 300
  $build=Join-Path $repo "build/.ninja/fixtures-$arch"
  $exe=Join-Path $build 'native-observation-probe.exe';$images+=$exe
  Owned "observation-$arch" $exe '' 15 2
  foreach($kind in 'cui','gui'){
   $exe=Join-Path $build "mixed-batch-$kind.exe";$images+=$exe
   Owned "batch-$arch-$kind-empty" $exe '' 15 64
   $log="$root/batch-$arch-$kind.txt"
   Owned "batch-$arch-$kind" $exe ('"'+$log+'" probe 37 "space value"') 15 37
   $bits=if($arch -eq 'x64'){64}else{32}
   if((Get-Content $log -Raw) -cne "probe|bits=$bits|arg=space value|hook=0`r`n"){throw 'Batch argument/architecture witness mismatch'}
  }
 }
 $build=Join-Path $repo 'build/.ninja/fixtures-x86'
 foreach($kind in 'cui','gui'){
  $exe=Join-Path $build "unpaired-$kind.exe";$images+=$exe
  Owned "unpaired-$kind-empty" $exe '--invalid' 15 1
 }
 $build=Join-Path $repo 'build/.ninja/fixtures-x64'
 $exe=Join-Path $build 'mixed-batch-pif.exe';$images+=$exe
 Owned pif $exe ('"'+$root+'/probe.pif" "PROBE.EXE"') 15
 Owned pif-existing $exe ('"'+$root+'/probe.pif" "PROBE.EXE"') 15 65
 foreach($name in 'probe','console-snapshot'){
  $exe=Join-Path $build "mouse-tools/$name.exe";$images+=$exe
  Owned "mouse-$name-empty" $exe '' 15 64
 }
 $images+=Join-Path $build 'mouse-tools/mouse-hook.dll'
 Get-FileHash $images | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
 $result.result='pass'
 $result.scope='Authored probe argument, batch exit/encoding and PIF creation contracts only; not Run16 lifecycle or guest mouse acceptance.'
}catch{$result.error="$_";Write-Output $_}
$result|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
if($result.result -ne 'pass'){exit 1}
exit 0
