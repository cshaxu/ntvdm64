[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$output=[IO.Path]::GetFullPath($OutputRoot,$repo)
if(!$output.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)){
 throw 'Require a fresh build-owned evidence directory'
}
$null=New-Item -ItemType Directory $output
$results=@()
foreach($index in 1,2){
 foreach($negative in $false,$true){
  $target="modesw-guard-$index"
  if($negative){$target+='-negative'}
  & "$repo/tools/build-fixtures.cmd" x86 $target *> "$output/$target.log"
  $code=$LASTEXITCODE
  $log=Get-Content "$output/$target.log" -Raw
  $line=@(50,98)[$index-1]
  if($negative){
   $diagnostic="($line): fatal error C1189: #error:  DIVERGENCE MVDM-HOST-DIV-619"
   if(!$code -or !$log.Contains($diagnostic)){throw "Missing original rejection: $target at $line"}
  }elseif($code){throw "Selected non-i386 branch failed: $target"}
  $results+=@{target=$target;exit=$code;expectedRejection=$negative;originalLine=$line}
 }
}
$results|ConvertTo-Json|Set-Content "$output/results.json"
'PASS modesw: two selected branches and two original-line i386 rejections'
