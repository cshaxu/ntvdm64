[CmdletBinding()]
param([string]$OutputRoot = (Join-Path $PSScriptRoot '../../build/broker-protocol'))
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$OutputRoot=[IO.Path]::GetFullPath($OutputRoot)
New-Item -ItemType Directory -Force $OutputRoot | Out-Null
& "$repo/tools/build-fixtures.cmd" x86 broker-protocol-fixtures *> "$OutputRoot/build.log"
if($LASTEXITCODE){throw "Build failed; see $OutputRoot/build.log"}
$root="$repo/build/.ninja/msvc-x86-release/worker"
$cases=@{
 'delivery-journal-test'=@('broker_vdm_delivery_rollback|broker-transport:vdm_delivery.obj',
   'broker_vdm_receipt_accept|broker-transport:vdm_receipt.obj',
   'broker_vdm_receipt_take|broker-transport:vdm_receipt.obj')
 'vdm-protocol-test'=@('broker_vdm_payload_encode|common-base-srv-transport:vdm_payload.obj',
   'broker_vdm_payload_validate|common-base-srv-transport:vdm_payload.obj',
   'broker_vdm_message_read|common-base-srv-transport:vdm_message.obj')
}
foreach($name in ($cases.Keys|Sort-Object)) {
 $map=Get-Content "$root/$name.exe.map"
 foreach($provider in $cases[$name]){
  $parts=$provider.Split('|')
  if(!($map|Where-Object {$_ -match ('\s_'+[regex]::Escape($parts[0])+'\s') -and $_.Contains($parts[1])})){
   throw "Missing actual provider: $provider"
  }
 }
 $bytes=[IO.File]::ReadAllBytes("$root/$name.exe")
 if([BitConverter]::ToUInt16($bytes,[BitConverter]::ToInt32($bytes,0x3c)+4) -ne 0x14c){throw "Expected x86: $name"}
 $process=Start-Process "$root/$name.exe" -WindowStyle Hidden -PassThru `
  -RedirectStandardOutput "$OutputRoot/$name.log" -RedirectStandardError "$OutputRoot/$name.err"
 if(!$process.WaitForExit(10000)){$process.Kill();$process.WaitForExit();throw "Timed out: $name"}
 if($process.ExitCode -ne 0){throw "Failed $name with $($process.ExitCode)"}
 Get-Content "$OutputRoot/$name.log"
}
