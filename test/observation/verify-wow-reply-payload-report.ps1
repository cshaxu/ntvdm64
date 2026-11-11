[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$TimeoutReport)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned report controls required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/wow_reply_payload_report.ps1"
try{
 Get-FileHash $PSCommandPath,"$PSScriptRoot/wow_reply_payload_report.ps1",$TimeoutReport|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $good=Read-WowReplyPayloadReport $TimeoutReport
 Assert-WowReplyPayloadReport $good -Timeout
 $negative=@(@('native-success','sendStatus',1),@('generic-error','sendError',0),@('wrong-error','sendError',1400),@('dead-receiver','receiverAlive',0),@('not-entered','accepted',0),@('no-full-return','fullReturn',0),@('wrong-mode','mode',0),@('unprotected','protected',0),@('readable','unreadable',0))
 $results=foreach($case in $negative){
  $bad=$good.PSObject.Copy();$bad.($case[1])=$case[2];$rejected=$false
  try{Assert-WowReplyPayloadReport $bad -Timeout}catch{$rejected=$true}
  if(!$rejected){throw "Invalid timeout classified as pass: $($case[0])"}
  [pscustomobject]@{case=$case[0];rejected=$true;synthetic=$true}
 }
 foreach($field in @('replyResult','afterFlags')){
  $copy=$good.PSObject.Copy();$copy.$field=0
  if($field -eq 'replyResult'){$copy.$field=[uint32]::MaxValue}
  Assert-WowReplyPayloadReport $copy -Timeout
 }
 $modeRejected=$false;try{Assert-WowReplyPayloadReport $good}catch{$modeRejected=$true}
 if(!$modeRejected){throw 'Timeout accepted as successful early reply'}
 foreach($kind in @('short','wrong-signature')){
  $bytes=[IO.File]::ReadAllBytes($TimeoutReport)
  if($kind -eq 'short'){$bytes=$bytes[0..99]}else{$bytes[0]=0}
  $path="$root/$kind.bin";[IO.File]::WriteAllBytes($path,$bytes);$rejected=$false
  try{Read-WowReplyPayloadReport $path|Out-Null}catch{$rejected=$true}
  if(!$rejected){throw 'Malformed timeout bytes accepted'}
 }
 $results|ConvertTo-Json|Set-Content "$root/rejections.json"
 @{validActualTimeoutDecoded=$true;fieldNegatives=9;modeNegative=1;byteNegatives=2;unconstrainedFailureOutputControls=2;scope='Synthetic parser/assertion controls, not actual receiver death or native API failure injection'}|ConvertTo-Json|Set-Content "$root/result.json"
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Report control input changed'}}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
