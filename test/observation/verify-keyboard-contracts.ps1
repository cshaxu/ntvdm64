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
function Owned([string]$Name,[string]$Image,[string]$Arguments,[int]$Seconds){
 $options=@{FilePath=$Image;WorkingDirectory=$repo;WindowStyle='Hidden';PassThru=$true;
  RedirectStandardOutput="$root/$Name.stdout.txt";RedirectStandardError="$root/$Name.stderr.txt"}
 if($Arguments){$options.ArgumentList=$Arguments}
 $p=Start-Process @options
 try{
  if(!$p.WaitForExit($Seconds*1000)){& taskkill.exe /PID $p.Id /T /F;throw "Owned $Name deadline"}
  $p.WaitForExit();$results.Add(@{case=$Name;pid=$p.Id;exit=$p.ExitCode;
   started=$p.StartTime.ToUniversalTime().ToString('o');finished=$p.ExitTime.ToUniversalTime().ToString('o')})
  if($p.ExitCode){throw "$Name failed $($p.ExitCode)"}
 }finally{$p.Dispose()}
}
$result=@{result='failed';cases=$results;product_changed=$false;guest_changed=$false}
try{
 Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 keyboard-contracts"') 300
 $build=Join-Path $repo 'build/.ninja/fixtures-x86'
 $images=@()
 foreach($name in 'keyboard_origin_return','keyboard_bios_join','keyboard_origin_device','unused_key_return'){
  $exe=Join-Path $build "$name.exe";$images+=$exe
  Owned $name $exe '' 60
 }
 Get-FileHash $images | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
 $result.result='pass'
 $result.scope='Current projected keyboard/host routines and actual history carrier; bounded event/device providers, not guest input acceptance.'
}catch{$result.error="$_";Write-Output $_}
$result|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
if($result.result -ne 'pass'){exit 1}
