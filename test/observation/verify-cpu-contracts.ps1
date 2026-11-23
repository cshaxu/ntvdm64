[CmdletBinding()]
param([Parameter(Mandatory)][string]$EvidenceRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($EvidenceRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path -LiteralPath $root)){
 throw 'Fresh build-owned evidence root required'
}
$null=New-Item -ItemType Directory $root
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$results=[Collections.Generic.List[object]]::new()
function RunOwned([string]$Name,[string]$Image,[string]$Arguments,[int]$Expected,[int]$Seconds=60){
 $start=@{FilePath=$Image;WorkingDirectory=$repo;WindowStyle='Hidden';PassThru=$true;
  RedirectStandardOutput="$root/$Name.stdout.txt";RedirectStandardError="$root/$Name.stderr.txt"}
 if($Arguments){$start.ArgumentList=$Arguments}
 $p=Start-Process @start
 try{
  if(!$p.WaitForExit($Seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $Name deadline"}
  $p.WaitForExit();$code=$p.ExitCode
  $results.Add(@{case=$Name;pid=$p.Id;exit=$code;expected=$Expected;
   started=$p.StartTime.ToUniversalTime().ToString('o');finished=$p.ExitTime.ToUniversalTime().ToString('o')})
  if(($Expected -eq 0 -and $code -ne 0) -or ($Expected -ne 0 -and $code -eq 0)){
   throw "$Name unexpected exit $code"
  }
 }finally{$p.Dispose()}
}
function Build([string]$Name,[string]$Target,[int]$Expected){
 RunOwned $Name $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 '+$Target+'"') $Expected 300
}
$result=@{result='failed';cases=$results;product_changed=$false;guest_changed=$false}
try{
 Build build cpu-contracts 0
 $before=@(Get-FileHash "$worker/obj/ccpu/c_neg64.obj")
 foreach($name in 'original-ccpu386.lib','ntvdm.exe'){
  if(Test-Path "$worker/$name"){$before+=Get-FileHash "$worker/$name"}
 }
 foreach($case in 'ccpu-neg64','ccpu-leaf'){
  Build "$case-negative" "$case-negative" 1
  $text=(Get-Content "$root/$case-negative.stdout.txt" -Raw)+(Get-Content "$root/$case-negative.stderr.txt" -Raw)
  if($text -notmatch 'C4113.*differs in parameter lists'){throw "$case wrong signature was not specifically rejected"}
 }
 RunOwned neg64 "$worker/ccpu-neg64.exe" '' 0
 if((Get-Content "$root/neg64.stdout.txt" -Raw) -notmatch 'PASS CCPU NEG64 cases=631410 pointer_orders=2 unsigned_mod64=1 guards=1 involution=1'){
  throw 'NEG64 behavior witness missing'
 }
 RunOwned wrong-oracle "$worker/ccpu-neg64.exe" '--wrong-oracle' 1
 if((Get-Content "$root/wrong-oracle.stdout.txt" -Raw) -notmatch 'FAIL NEG64 case=0'){throw 'Wrong oracle was not rejected'}
 RunOwned notification "$worker/ccpu-notification.exe" '' 0
 RunOwned native-prefix "$worker/native-int-prefix.exe" '' 0
 foreach($record in $before){if((Get-FileHash $record.Path).Hash -ne $record.Hash){throw 'Normal cached input changed'}}
 $before|Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/cache-inputs.json"
 Get-FileHash "$worker/ccpu-neg64.exe","$worker/ccpu-notification.exe","$worker/native-int-prefix.exe",
  "$worker/notification_body.inc","$repo/tools/build/worker/cpu-tests.cmake",$PSCommandPath |
  Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
 $result.result='pass'
 $result.scope='Native NEG64 helper and declarations; projected CPU event functions; host x86 INT-prefix reference. Not guest decoder or IRQ delivery acceptance.'
}catch{$result.error="$_";Write-Output $_}
$result|ConvertTo-Json -Depth 6|Set-Content "$root/result.json"
if($result.result -ne 'pass'){exit 1}
