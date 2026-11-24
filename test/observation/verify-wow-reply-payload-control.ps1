[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
# Whole pure-native reference, including a deliberately wrong-reply control.
# Invoke in an owned hidden background process. No product DLL is loaded.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned native control required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/wow_reply_payload_report.ps1"
try{
 Get-FileHash $PSCommandPath,"$PSScriptRoot/wow_reply_payload_control.c","$PSScriptRoot/wow_reply_payload_lifetime.h","$PSScriptRoot/wow_reply_payload_report.ps1"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 & "$repo/tools/build-fixtures.cmd" x86 wow-reply-payload *> "$root/build.log"
 if($LASTEXITCODE){throw 'Native reply payload build failed'}
 Copy-Item "$repo/build/.ninja/fixtures-x86/wow-reply-payload.exe" "$root/native-control.exe"
 $observed=foreach($case in @('normal','wrong-reply','timeout')){
  $report="$root/$case.bin";$line='"'+$report+'"';if($case -eq 'wrong-reply'){$line+=' --bad-reply'}
  if($case -eq 'timeout'){$line+=' --timeout'}
  $p=Start-Process "$root/native-control.exe" -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$case.stdout.txt" -RedirectStandardError "$root/$case.stderr.txt";$null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image="$root/native-control.exe";command=$line;deadlineSeconds=30}|ConvertTo-Json|Set-Content "$root/$case.process.json"
  if(!$p.WaitForExit(30000)){taskkill.exe /PID $p.Id /T /F;throw 'Owned pure-native payload deadline'}
  $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$case.exit.txt"
  $row=Read-WowReplyPayloadReport $report;$row|ConvertTo-Json|Set-Content "$root/$case.json"
  if($case -ne 'wrong-reply'){
   if($code){throw 'Pure native lifetime reference failed; do not attribute to WOW'}
   Assert-WowReplyPayloadReport $row -NativeControl -Timeout:($case -eq 'timeout')
  }else{
   if($code -ne 1 -or !$row.errors -or $row.replyResult -ne 0x447a0052 -or $row.protected -ne 0 -or $row.continued -ne 1){throw 'Wrong native reply did not reject before protecting input'}
   $rejected=$false
   try{Assert-WowReplyPayloadReport $row -NativeControl}catch{if($_.Exception.Message -eq 'Early-reply payload assertions failed'){$rejected=$true}else{throw}}
   if(!$rejected){throw 'Negative native report accepted as a normal lifetime pass'}
  }
  [pscustomobject]@{case=$case;exit=$code;report=$row;expectedFailure=($case -eq 'wrong-reply')}
 }
 $observed|ConvertTo-Json -Depth 5|Set-Content "$root/results.json"
 Get-FileHash "$root/native-control.exe"|Export-Csv -NoTypeInformation "$root/artifacts.csv"
 foreach($input in Import-Csv "$root/inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Native control input changed'}}
 @{nativeReferencePassed=$true;timeoutReferencePassed=$true;negativeRejected=$true;productLoaded=$false;limits='Only owned sender page protected. Actual timeout preserves native status0/error1460 and an entered/live receiver; this is not native API success. No assertion about undocumented USER capture allocation lifetime.'}|ConvertTo-Json|Set-Content "$root/result.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
