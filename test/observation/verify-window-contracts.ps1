[CmdletBinding()]
param([Parameter(Mandatory)][string]$EvidenceRoot, [ValidateSet('x86','x64')][string]$Architecture='x64', [string[]]$Only)
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
 $names=@('native-input-geometry','native-window-sizing','ntvwm-carrier-guard','ntvwm-carrier-order','ntvwm-close-priority','ntvwm-console-state','ntvwm-palette-geometry','native-console-capture')
 if($Only){foreach($name in $Only){if($name -notin $names){throw "Unknown window contract: $name"}};$names=@($names|Where-Object {$_ -in $Only})}
 Owned build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" '+$Architecture+' window-contracts"') 300
 $build=Join-Path $repo "build/.ninja/fixtures-$Architecture"
 $images=@()
 foreach($name in $names){
  $exe=Join-Path $build "$name.exe";$images+=$exe
  $arguments=if($name -eq 'ntvwm-carrier-guard'){'"'+$root+'/guard-result.txt"'}else{''}
  Owned $name $exe $arguments 60
  $log=Get-Content "$root/$name.stdout.txt" -Raw
  if($name -eq 'ntvwm-carrier-guard' -and (Get-Content "$root/guard-result.txt" -Raw) -notmatch 'PASS exact HWND'){throw 'Missing guard witness'}
  if($name -eq 'ntvwm-console-state' -and $log -notmatch 'NTVWM-STATE PASS'){throw 'Missing Console state witness'}
  if($name -eq 'native-window-sizing'){
   foreach($style in 'fixed-caption','sizeable-caption','popup-border','minimized-caption'){
    if($log -notmatch ([regex]::Escape($style)+' request=154,240 actual=\d+,\d+ style=')){throw "Missing $style observation"}
   }
   if($log -notmatch 'minimized-caption after-move154-and-restore=\d+,\d+ iconic=0'){throw 'Minimized window did not restore'}
  }
 }
 Get-FileHash $images | Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
 $result.result='pass'
 $result.scope='Native Console/window contracts and bounded production function fixtures; not guest acceptance. Pixel observations are not universal metrics.'
}catch{$result.error="$_";Write-Output $_}
$result|ConvertTo-Json -Depth 5|Set-Content "$root/result.json"
if($result.result -ne 'pass'){exit 1}
exit 0
