[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('Off','Win16','All')][string]$Win16Style='Off',
 [string]$Nasm='nasm.exe',
 [switch]$NativeExtension,
 [switch]$ThreadClientView,
 [switch]$StructuredPayloads,
 [switch]$ReplyPayloadLifetime,
 [switch]$TimeoutPayloadLifetime,
 [switch]$CopyDataProc32,
 [string]$ExternalNativePeer='',
 [string]$NativeHost='',
 [string]$CrossWorkerCoordinator='',
 [switch]$SharedWorkerControl,
 [switch]$PendingSend,
 [switch]$Construction,
 [switch]$ClassicDde,
 [string]$TransactionDiagnosticManifest,
 [ValidateSet('BeforeCallback','AfterCallback')][string]$TransactionPhase='BeforeCallback'
)
# Entire actual-guest message group. Invoke as an owned hidden background run.
# No original guest is changed or rebuilt; only independently authored probes.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if($TransactionDiagnosticManifest){
 if(!$Construction -or $NativeHost){throw 'Transaction fault requires the whole normal construction group and freshly built peer'}
 $transaction=Get-Content $TransactionDiagnosticManifest -Raw|ConvertFrom-Json
 if($transaction.kind -ne 'diagnostic-only' -or $transaction.publishable -ne $false -or
  $transaction.target -ne 'MSGSEND' -or $transaction.fault -ne 'original message callback BeforeCallback/AfterCallback' -or
  (Get-FileHash "$PackageRoot/system32/WOW32.DLL").Hash -ne $transaction.diagnosticProvider){throw 'Exact transaction-only provider required'}
}
if($Construction -and (!$StructuredPayloads -or !$ThreadClientView -or $CrossWorkerCoordinator -or
 $ExternalNativePeer -or $NativeExtension -or $CopyDataProc32 -or $ReplyPayloadLifetime -or
 $TimeoutPayloadLifetime -or $PendingSend)){
 throw 'Construction requires the normal structured/client-view message group'
}
if($ClassicDde -and (!$StructuredPayloads -or !$ThreadClientView -or $CrossWorkerCoordinator -or
 $ExternalNativePeer -or $NativeExtension -or $CopyDataProc32 -or $ReplyPayloadLifetime -or
 $TimeoutPayloadLifetime -or $PendingSend -or $Construction)){
 throw 'ClassicDde requires the normal structured/client-view same-worker message group'
}
if($PendingSend -and (!$NativeHost -or !$StructuredPayloads -or !$ThreadClientView -or
 $CrossWorkerCoordinator -or $ExternalNativePeer -or $NativeExtension -or
 $CopyDataProc32 -or $ReplyPayloadLifetime -or $TimeoutPayloadLifetime)){
 throw 'PendingSend requires normal structured/client-view probes and prebuilt pending-send host'
}
if($SharedWorkerControl -and !$CrossWorkerCoordinator){throw 'SharedWorkerControl requires the cross-worker negative setup'}
if($CrossWorkerCoordinator -and (!$NativeHost -or !$StructuredPayloads -or !$ThreadClientView -or
    $ExternalNativePeer -or $NativeExtension -or $CopyDataProc32 -or $ReplyPayloadLifetime -or $TimeoutPayloadLifetime)){
 throw 'Cross-worker group requires normal structured/client-view probes and prebuilt native host/coordinator'
}
if($ReplyPayloadLifetime -and !$StructuredPayloads){throw 'ReplyPayloadLifetime requires the complete StructuredPayloads group'}
if($TimeoutPayloadLifetime -and !$ReplyPayloadLifetime){throw 'TimeoutPayloadLifetime requires the complete ReplyPayloadLifetime group'}
if($CopyDataProc32 -and (!$StructuredPayloads -or $ReplyPayloadLifetime -or $TimeoutPayloadLifetime)){throw 'Proc32 group requires normal structured mode without reply/timeout variants'}
if($NativeHost -and (!$StructuredPayloads -or $CopyDataProc32 -or $ReplyPayloadLifetime -or $TimeoutPayloadLifetime)){
 throw 'Prebuilt native host requires normal structured mode'
}
if($ExternalNativePeer -and
    (!$NativeHost -or !$StructuredPayloads -or
     $CopyDataProc32 -or $ReplyPayloadLifetime -or $TimeoutPayloadLifetime)){
 throw 'External native group requires both built peer/host images and normal structured mode'
}
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
 throw 'Fresh build-owned evidence required'
}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$assembly="$PSScriptRoot/win16_message_probe.asm"
$native="$PSScriptRoot/wow_message_peer.c"
function Read-GuestReport([string]$path,[string]$name){
 $bytes=[IO.File]::ReadAllBytes($path)
 $recordBytes=if($ThreadClientView){40}else{34}
 $signature=if($ThreadClientView){'MSG2'}else{'MSG1'}
 if($StructuredPayloads){$recordBytes+=4;$signature=if($ThreadClientView){'MSG6'}else{'MSG5'}}
 if($CrossWorkerCoordinator){$recordBytes+=6;$signature='MSG7'}
 if($PendingSend){$recordBytes+=4;$signature='MSG8'}
 if($Construction){$recordBytes+=14;$signature='MSG9'}
 if($ClassicDde){$recordBytes+=8}
 if(!$bytes.Length -or $bytes.Length%$recordBytes){throw "Incomplete $name message records"}
 $fields=@('role','phase','errors','task','peerTask','basic','nested','early','posted','same','filtered','threadpost','text','native','destroyed')
 if($ThreadClientView){$fields+=@('clientVersion','clientExpected','clientChecks')}
 if($StructuredPayloads){$fields+=@('structures','copyNested')}
 if($CrossWorkerCoordinator){$fields+=@('remoteTask','foreignExpected','launchReady')}
 if($PendingSend){$fields+=@('waitBlocked','waitDeath')}
 if($Construction){$fields+=@('mdiCreated','mdiDestroyed','mdiTitleMutation','mdiParamMutation','mdiTextChecks','helpCalls','helpFreed')}
 if($ClassicDde){$fields+=@('ddeReceived','ddeReleaseAttempt','ddeRetired','ddeStage')}
 $rows=@()
 for($offset=0;$offset -lt $bytes.Length;$offset+=$recordBytes){
  if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne $signature){throw "Invalid $name signature"}
  $row=[ordered]@{}
  for($i=0;$i -lt $fields.Count;$i++){$row[$fields[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
  $rows+=[pscustomobject]$row
 }
 $rows|ConvertTo-Json|Set-Content "$root/$name.json"
 return $rows
}
try{
 $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
 foreach($role in @('SEND','RECV')){
  $arguments=@('-f','bin','-o',"$root/MSG$role.EXE",'-l',"$root/$role.lst")
  if($role -eq 'RECV'){$arguments+='-DRECEIVER'}
  if($NativeExtension){$arguments+='-DEXTENSION_WITNESS'}
  if($ThreadClientView){$arguments+='-DTHREAD_CLIENT_VIEW'}
  if($StructuredPayloads){$arguments+='-DM4_STRUCTURES'}
  if($ReplyPayloadLifetime){$arguments+='-DM4_REPLY_PAYLOAD'}
  if($TimeoutPayloadLifetime){$arguments+='-DM4_TIMEOUT_PAYLOAD'}
  if($CopyDataProc32){$arguments+='-DM4_COPYDATA_PROC32'}
  if($CrossWorkerCoordinator){$arguments+='-DCROSS_WORKER'}
  if($PendingSend){$arguments+='-DPENDING_SEND'}
  if($Construction){$arguments+='-DCONSTRUCTION'}
  if($ClassicDde){$arguments+='-DCLASSIC_DDE'}
  if($TransactionDiagnosticManifest -and $role -eq 'SEND'){$arguments+='-DMESSAGE_TRANSACTION'}
  & $assembler @arguments $assembly *> "$root/$role-assemble.txt"
  if($LASTEXITCODE){throw "Independent $role assembly failed"}
 }
 if($NativeHost){
  foreach($image in @($ExternalNativePeer,$NativeHost,$CrossWorkerCoordinator)|Where-Object {$_}){
   $path=(Resolve-Path $image).Path
   if(!$path.StartsWith((Join-Path $repo 'build/.ninja')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'External peer/host must come from stable Ninja fixture targets'}
  }
  Copy-Item $NativeHost "$root/MSGHOST.DLL"
  if($CrossWorkerCoordinator){
   Copy-Item $CrossWorkerCoordinator "$root/MSGXHOST.EXE"
   Get-FileHash $CrossWorkerCoordinator,"$PSScriptRoot/wow_cross_worker_probe.c"|Export-Csv -NoTypeInformation "$root/cross-worker-inputs.csv"
  }
  if($ExternalNativePeer){
  Copy-Item $ExternalNativePeer "$root/MSGPEER.EXE"
  $externalBytes=[IO.File]::ReadAllBytes("$root/MSGPEER.EXE")
  $externalMachine=[BitConverter]::ToUInt16($externalBytes,[BitConverter]::ToInt32($externalBytes,0x3c)+4)
  if($externalMachine -notin @(0x14c,0x8664)){throw 'Wrong external peer PE machine'}
  }
 }else{
 Push-Location $root
 try{
  $nativeDefines=@();if($StructuredPayloads){$nativeDefines+='/DM4_STRUCTURES'}
  if($ReplyPayloadLifetime){$nativeDefines+='/DM4_REPLY_PAYLOAD'}
  if($TimeoutPayloadLifetime){$nativeDefines+='/DM4_TIMEOUT_PAYLOAD'}
  if($CopyDataProc32){$nativeDefines+='/DM4_COPYDATA_PROC32'}
  if($Construction){$nativeDefines+='/DMESSAGE_CONSTRUCTION'}
  if($ClassicDde){$nativeDefines+='/DCLASSIC_DDE'}
  if($TransactionDiagnosticManifest){$nativeDefines+='/DMESSAGE_TRANSACTION'}
  & $CompilerDriver cl.exe /nologo /LD /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS @nativeDefines "/Fo$root/peer.obj" $native /link "/OUT:$root/MSGHOST.DLL" "/IMPLIB:$root/MSGHOST.lib" /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12 user32.lib *> "$root/native-build.txt"
  if($LASTEXITCODE){throw 'Native peer build failed'}
 }finally{Pop-Location}
 }
 Get-FileHash $assembly,$native,$PSCommandPath,"$root/MSGSEND.EXE","$root/MSGRECV.EXE","$root/MSGHOST.DLL"|Export-Csv -NoTypeInformation "$root/probe-inputs.csv"
 if($ReplyPayloadLifetime){Get-FileHash "$PSScriptRoot/wow_reply_payload_lifetime.h","$PSScriptRoot/wow_reply_payload_report.ps1"|Export-Csv -NoTypeInformation "$root/reply-source-inputs.csv"}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 if($ExternalNativePeer){$scope.Paths+=Join-Path $tests 'MSGPEER.EXE'}
 if($CrossWorkerCoordinator){$scope.Paths+=Join-Path $tests 'MSGXHOST.EXE'}
 Copy-Item "$root/MSGSEND.EXE","$root/MSGRECV.EXE","$root/MSGHOST.DLL" $tests
 if($CrossWorkerCoordinator){Copy-Item "$root/MSGXHOST.EXE" $tests}
 if($ExternalNativePeer){Copy-Item "$root/MSGPEER.EXE" $tests;Get-FileHash "$root/MSGPEER.EXE"|Export-Csv -NoTypeInformation "$root/external-peer-input.csv"}
 Get-ChildItem "$runtime/system32" -File|Where-Object Name -in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','ntw16s32.dll','ntw16s64.dll','W16Style.exe','KRNL386.EXE','USER.EXE','GDI.EXE')|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 @{machine='x86';cpu='original SoftPC CCPU40';guest='unchanged original package plus independent NE probes';style=$Win16Style}|ConvertTo-Json|Set-Content "$root/profile.json"
 # Complete supported logon environment, not a hand-picked variable allowlist.
 # Known inherited large-environment COMMAND failure is retained separately.
 Add-Type -OutputAssembly "$root/logon-env.dll" -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public static class WowMessageLogonEnvironment {
 [DllImport("userenv.dll",SetLastError=true)] static extern bool CreateEnvironmentBlock(out IntPtr block,IntPtr token,bool inherit);
 [DllImport("userenv.dll",SetLastError=true)] static extern bool DestroyEnvironmentBlock(IntPtr block);
 public static string[] Read(IntPtr token) {
  IntPtr block;
  if(!CreateEnvironmentBlock(out block,token,false))throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
  try { var values=new List<string>();IntPtr at=block;
   while(Marshal.ReadInt16(at)!=0){string value=Marshal.PtrToStringUni(at);values.Add(value);at=IntPtr.Add(at,(value.Length+1)*2);}
   return values.ToArray();
  }finally{DestroyEnvironmentBlock(block);}
 }
}
'@
 Add-Type -Path "$root/logon-env.dll"
 $logonIdentity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($logonIdentity.Token)}
 finally{$logonIdentity.Dispose()}
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$split=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$split)]=$entry.Substring($split+1)}
 $info.EnvironmentVariables['TEMP']=$runtime;$info.EnvironmentVariables['TMP']=$runtime
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root/lifetime.txt"
 $info.EnvironmentVariables['MVDM_WOW_CALLBACK_FRAME_TRACE_PATH']="$root/lifetime.txt"
 if($TransactionDiagnosticManifest){
  $info.EnvironmentVariables['MVDM_TEST_MESSAGE_TRANSACTION']="$root/transaction"
  $info.EnvironmentVariables['MVDM_TEST_MESSAGE_PHASE']=$TransactionPhase
  Get-FileHash $TransactionDiagnosticManifest|Export-Csv -NoTypeInformation "$root/transaction-provider-input.csv"
 }
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 $arguments=@("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait')
 if($Win16Style -eq 'Win16'){$arguments+='--win16-style'}elseif($Win16Style -eq 'All'){$arguments+='--win16-style-all'}
 if($CrossWorkerCoordinator){
  $arguments+=@("$tests/MSGXHOST.EXE","$tests/MSGSEND.EXE","$tests/MSGRECV.EXE","$tests/MSGXHOST.TXT","$runtime\system32\ntvdm.exe",$(if($SharedWorkerControl){'1'}else{'0'}))
 }else{$arguments+="$tests/MSGSEND.EXE"}
 $arguments+=@('--observation-timeout-ms','60000')
 foreach($argument in $arguments){$info.ArgumentList.Add($argument)}
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=$arguments;timeoutSeconds=70}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 if(!$process.WaitForExit(70000)){throw 'Actual message group deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer-exit.txt"
 # Preserve short/failed reports before asserting success; no failed-row replay.
 foreach($name in @('MSGSEND','MSGRECV','MSGHOST','MSGEXT')){
  if(Test-Path "$tests/$name.BIN"){Copy-Item "$tests/$name.BIN" "$root/$name.bin"}
 }
 if(Test-Path "$tests/MSGPEER-FAIL.txt"){Copy-Item "$tests/MSGPEER-FAIL.txt" "$root/native-failures.txt"}
 if($CrossWorkerCoordinator -and (Test-Path "$tests/MSGXHOST.TXT")){Copy-Item "$tests/MSGXHOST.TXT" "$root/cross-worker.txt"}
 if($PendingSend -and (Test-Path "$tests/MSGWAIT.BIN")){Copy-Item "$tests/MSGWAIT.BIN" "$root/MSGWAIT.bin"}
 if($ReplyPayloadLifetime -and (Test-Path "$tests/MSGREPLY.BIN")){Copy-Item "$tests/MSGREPLY.BIN" "$root/MSGREPLY.bin"}
 if($TimeoutPayloadLifetime -and (Test-Path "$tests/MSGTIME.BIN")){Copy-Item "$tests/MSGTIME.BIN" "$root/MSGTIME.bin"}
 $send=Read-GuestReport "$root/MSGSEND.bin" sender
 $receive=Read-GuestReport "$root/MSGRECV.bin" receiver
 if($ThreadClientView){
  @{sender=@($send|Select-Object phase,errors,task,clientVersion,clientExpected,clientChecks);
    receiver=@($receive|Select-Object phase,errors,task,clientVersion,clientExpected,clientChecks);
    source='Original USER16 FLATFS 003B, client version at50h; guest reads before any host query';
    originalGuestChanged=$false}|ConvertTo-Json -Depth 5|Set-Content "$root/thread-client.json"
 }
 $report=Get-Content "$root/observer.txt" -Raw
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher failed normal completion'}
 if(($send.phase -join ',') -ne '0,1,2,3,4,6' -or ($receive.phase -join ',') -ne '0,5,6' -or
    @(@($send)+@($receive)|Where-Object errors -ne 0).Count){throw 'Guest message phase/assertion failure'}
 $last=$send[-1];$peer=$receive[-1]
 if($ThreadClientView -and
    ($last.clientChecks -lt 12 -or $peer.clientChecks -lt 12 -or
     @($send|Where-Object {$_.clientVersion -ne 0x030a -or $_.clientExpected -ne 0x030a}).Count -or
     @($receive|Where-Object {$_.clientVersion -ne 0x0300 -or $_.clientExpected -ne 0x0300}).Count)){
  throw 'Actual distinct-version task client views were not retained across message/native/callback returns'
 }
 $expectedNative=if($NativeExtension){5}else{4}
 if($StructuredPayloads){$expectedNative+=3}
 if($ClassicDde){$expectedNative++}
 $identityOk=if($CrossWorkerCoordinator){
  $last.remoteTask -eq $peer.task -and $peer.remoteTask -eq $last.task -and
  $last.foreignExpected -ne 0 -and $peer.foreignExpected -ne 0 -and
  $last.peerTask -eq $last.foreignExpected -and $peer.peerTask -eq $peer.foreignExpected -and
  $last.launchReady -eq 1 -and $peer.launchReady -eq 0
 }else{$last.task -ne $peer.task -and $last.peerTask -eq $peer.task}
 if(!$last.task -or !$peer.task -or !$identityOk -or
    @($send|Where-Object task -ne $last.task).Count -or @($receive|Where-Object task -ne $peer.task).Count -or
    $last.role -ne 0 -or $peer.role -ne 1 -or $last.nested -ne 2 -or $last.same -ne 2 -or
    $last.early -ne 1 -or $last.posted -ne 1 -or $last.filtered -ne 1 -or $last.threadpost -ne 1 -or
    $last.text -ne 1 -or $last.native -ne $expectedNative -or $last.destroyed -ne 1 -or
    $peer.basic -ne 1 -or $peer.nested -ne 1 -or $peer.early -ne 1 -or $peer.posted -ne 1){throw 'Incomplete actual Win16 message contract'}
 if($StructuredPayloads -and ($last.structures -ne 4 -or $peer.structures -ne 4)){
  throw 'MINMAXINFO nested stack copyback, WINDOWPOS or COPYDATA guest path incomplete'
 }
 if($StructuredPayloads){
  $expectedCopyNested=if($TimeoutPayloadLifetime){3}elseif($ReplyPayloadLifetime){2}else{1}
  if($last.copyNested -ne $expectedCopyNested -or $peer.copyNested -ne 1){throw 'Original CallWindowProc nested COPYDATA lost its exact far-buffer identity'}
 }
 $bytes=[IO.File]::ReadAllBytes("$root/MSGHOST.bin")
 $nativeBytes=if($StructuredPayloads){52}else{48}
 if($ClassicDde){$nativeBytes+=12}
 if($CopyDataProc32){$nativeBytes+=8}
 if($ExternalNativePeer){$nativeBytes+=8}
 if($CrossWorkerCoordinator){$nativeBytes+=4}
 if($bytes.Length -ne $nativeBytes -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'NMSG'){throw 'Missing native peer report'}
 $names=@('signature','errors','pid','senderThread','receiverThread','nativeThread','received','nested','replied','text','exit','windowAlive');$nativeReport=[ordered]@{}
 if($StructuredPayloads){$names+='structures'}
 if($ClassicDde){$names+=@('ddeAck','ddeReleased','ddeRetired')}
 if($CopyDataProc32){$names+=@('proc32Fresh','proc32Borrowed')}
 if($ExternalNativePeer){$names+=@('externalPid','externalBits')}
 if($CrossWorkerCoordinator){$names+='receiverPid'}
 for($i=0;$i -lt $names.Count;$i++){$nativeReport[$names[$i]]=[BitConverter]::ToUInt32($bytes,4*$i)}
 $nativeReport|ConvertTo-Json|Set-Content "$root/native.json"
 if($Construction){
  if($last.mdiCreated -ne 3 -or $last.mdiDestroyed -ne 3 -or $last.mdiTitleMutation -ne 1 -or
   $last.mdiParamMutation -ne 1 -or $last.mdiTextChecks -ne 3 -or $last.helpCalls -ne 1 -or
   $last.helpFreed -ne 1 -or $peer.mdiCreated -ne 0){throw 'Actual MDI/text/WinHelp construction ownership paths incomplete'}
  @{created=$last.mdiCreated;destroyed=$last.mdiDestroyed;titleMutation=$last.mdiTitleMutation;paramsMutation=$last.mdiParamMutation;textChecks=$last.mdiTextChecks;helpCalls=$last.helpCalls;helpFreed=$last.helpFreed;scope='Actual USER16 MDI Create/MDICreate/DefFrameProc/DefMDIChildProc, text and WinHelp private-buffer ownership; fault injection remains exact-body only'}|ConvertTo-Json|Set-Content "$root/construction.json"
 }
 if($PendingSend){
  if($last.waitBlocked -ne 1 -or $last.waitDeath -ne 0 -or $peer.waitBlocked -ne 0 -or $peer.waitDeath -ne 1){throw 'Guest blocked-send/death callback counts incomplete'}
  $waitBytes=[IO.File]::ReadAllBytes("$root/MSGWAIT.bin")
  if($waitBytes.Length -ne 60 -or [Text.Encoding]::ASCII.GetString($waitBytes,0,4) -ne 'WAIT'){throw 'Missing pending-send report'}
  $waitNames=@('signature','errors','pid','senderThread','receiverThread','nativeThread','blockedStatus','blockedError','nestedAfterTimeout','deathStatus','deathError','deathWindowAlive','deathOwnerBefore','guestStaleResult','deathResult')
  $wait=[ordered]@{};for($i=0;$i -lt $waitNames.Count;$i++){$wait[$waitNames[$i]]=[BitConverter]::ToUInt32($waitBytes,4*$i)}
  $wait|ConvertTo-Json|Set-Content "$root/pending-send.json"
  if($wait.errors -or $wait.pid -ne $nativeReport.pid -or
   $wait.senderThread -ne $nativeReport.senderThread -or $wait.receiverThread -ne $nativeReport.receiverThread -or
   $wait.nativeThread -ne $nativeReport.nativeThread -or $wait.blockedStatus -ne 0 -or
   $wait.nestedAfterTimeout -ne 1 -or $wait.deathStatus -eq 0 -or $wait.deathError -ne 0 -or
   $wait.deathResult -ne 0x447a007c -or $wait.deathWindowAlive -ne 0 -or
   $wait.deathOwnerBefore -ne $wait.pid -or $wait.guestStaleResult -ne 0x447a007e){throw 'Actual blocked-send/destruction/result/identity contract failed'}
 }
 if($ExternalNativePeer -and
    (!$nativeReport.externalPid -or $nativeReport.externalPid -eq $nativeReport.pid -or
     $nativeReport.externalBits -ne $(if($externalMachine -eq 0x14c){32}else{64}))){
  throw 'Native peer did not execute in a distinct process of the requested bitness'
 }
 if($nativeReport.errors -or !$nativeReport.pid -or $nativeReport.exit -or $nativeReport.windowAlive -or
    $nativeReport.received -ne 4 -or $nativeReport.nested -ne 1 -or $nativeReport.replied -ne 1 -or $nativeReport.text -ne 1 -or
    @(@($nativeReport.senderThread,$nativeReport.receiverThread,$nativeReport.nativeThread)|Where-Object {$_}|Sort-Object -Unique).Count -ne 3){throw 'Native peer identity/receive/result/lifetime failure'}
 if($StructuredPayloads -and $nativeReport.structures -ne 6){throw 'Both native directions did not complete all three structure families'}
 if($ClassicDde){
  if($last.ddeReceived -ne 1 -or $last.ddeReleaseAttempt -ne 1 -or $last.ddeRetired -ne 1 -or
     $peer.ddeReceived -ne 0 -or $nativeReport.ddeAck -ne 1 -or $nativeReport.ddeReleased -ne 1 -or
     $nativeReport.ddeRetired -ne 1){throw 'Actual classic DDE execute/ack/free lifecycle incomplete'}
  @{execute='native HGLOBAL -> original WM32DDEExecute -> Win16 alias';ack='Win16 PostMessage WM_DDE_ACK -> original reverse thunk -> native packed lParam';
    release='guest early GlobalFree request is deferred; native GlobalFree retires both aliases';guest=@{received=$last.ddeReceived;releaseAttempt=$last.ddeReleaseAttempt;retired=$last.ddeRetired};
    native=@{ack=$nativeReport.ddeAck;released=$nativeReport.ddeReleased;retired=$nativeReport.ddeRetired};
    originalGuestChanged=$false;scope='Classic WM_DDE_EXECUTE/ACK only; no DDEML endpoint, atoms, DATA/POKE/ADVISE or allocation-failure injection'}|ConvertTo-Json|Set-Content "$root/classic-dde-result.json"
 }
 if($TransactionDiagnosticManifest){
  $trace=@(Get-Content "$root/transaction.trace")
  $pattern='^pid=(\d+) tid=(\d+) task=([0-9A-F]{4}) phase=(BeforeCallback|AfterCallback) key=([0-9A-F]{8}) slot=(\d+) before=(\d+) after=(\d+) cursor=([0-9A-F]{8})/([0-9A-F]{8}) init=(\d+)/(\d+) injected=(\d+) escaped=(\d+)$'
  if($trace.Count -ne 1){throw 'One actual original transaction boundary required'}
  $match=[regex]::Match($trace[0],$pattern)
  $after=if($transaction.control){'1'}else{'0'}
  if(!$match.Success -or [uint32]$match.Groups[1].Value -ne $nativeReport.pid -or
   [uint32]$match.Groups[2].Value -ne $nativeReport.senderThread -or
   [Convert]::ToUInt16($match.Groups[3].Value,16) -ne $last.task -or $match.Groups[4].Value -ne $TransactionPhase -or
   [int]$match.Groups[6].Value -ge 32 -or $match.Groups[7].Value -ne '1' -or
   $match.Groups[8].Value -ne $after -or $match.Groups[13].Value -ne '1' -or $match.Groups[14].Value -ne '1'){
   throw 'Actual original heap/callback/identity outcome does not match the candidate or immutable causal control'
  }
  if(!$transaction.control -and ($match.Groups[9].Value -ne $match.Groups[10].Value -or
   $match.Groups[11].Value -ne $match.Groups[12].Value)){throw 'Actual failed-call callback stack was not restored'}
  @{passed=$true;control=[bool]$transaction.control;originalHeapBitBefore=1;originalHeapBitAfter=[int]$after;
   repaired=(!$transaction.control);phase=$TransactionPhase;task=$last.task;normalGuestRetirement=$true;
   scope='Real original window entry/allocator/callback and unchanged guest; test-owned caught SEH, not original hard-error UI policy'}|
   ConvertTo-Json|Set-Content "$root/transaction-result.json"
 }
 if($CopyDataProc32 -and ($nativeReport.proc32Fresh -ne 1 -or $nativeReport.proc32Borrowed -ne 1)){throw 'Actual original Proc32 fresh/active-borrow paths incomplete'}
 if($ReplyPayloadLifetime){
  . "$PSScriptRoot/wow_reply_payload_report.ps1"
  $replyPayload=Read-WowReplyPayloadReport "$root/MSGREPLY.bin"
  $replyPayload|ConvertTo-Json|Set-Content "$root/reply-payload.json"
  Assert-WowReplyPayloadReport $replyPayload
  if($replyPayload.senderThread -ne $nativeReport.nativeThread -or $replyPayload.receiverThread -ne $nativeReport.senderThread){throw 'Actual early-reply sender/receiver identity mismatch'}
 }
 if($TimeoutPayloadLifetime){
  $timeoutPayload=Read-WowReplyPayloadReport "$root/MSGTIME.bin"
  $timeoutPayload|ConvertTo-Json|Set-Content "$root/timeout-payload.json"
  Assert-WowReplyPayloadReport $timeoutPayload -Timeout
  if($timeoutPayload.senderThread -ne $nativeReport.nativeThread -or $timeoutPayload.receiverThread -ne $nativeReport.senderThread){throw 'Actual timed-out native send/Win16 receiver identity mismatch'}
 }
 if($NativeExtension){
  $ext=[IO.File]::ReadAllBytes("$root/MSGEXT.bin")
  if($ext.Length -ne 40 -or [Text.Encoding]::ASCII.GetString($ext,0,4) -ne 'GEXT'){throw 'Missing original native-extension witness'}
  $names=@('signature','errors','pid','thread','receiverTask','extendedCallback','simpleCallback','yield','directedYield','invalidSizeRejected');$extension=[ordered]@{}
  for($i=0;$i -lt 10;$i++){$extension[$names[$i]]=[BitConverter]::ToUInt32($ext,4*$i)}
  $extension|ConvertTo-Json|Set-Content "$root/extension.json"
  if($extension.errors -or $extension.pid -ne $nativeReport.pid -or $extension.thread -ne $nativeReport.senderThread -or
     $extension.receiverTask -ne $peer.task -or $extension.extendedCallback -ne 0x447a0079 -or $extension.simpleCallback -ne 0x447a0079 -or
     $extension.yield -ne 1 -or $extension.directedYield -ne 1 -or $extension.invalidSizeRejected -ne 1){throw 'Native callback/yield/original-argument identity mismatch'}
 }
 $trace=Get-Content "$root/lifetime.txt" -Raw
 if($CrossWorkerCoordinator){
  $control=Get-Content "$root/cross-worker.txt" -Raw
  foreach($role in @('sender','receiver')){
   $rows=[regex]::Matches($control,'(?m)^worker role='+$role+' pid=(\d+) tid=(\d+) hwnd=([0-9A-F]+) created=([0-9A-F]{16}) image=(.+)\r?$')
   $pidValue=if($role -eq 'sender'){$nativeReport.pid}else{$nativeReport.receiverPid}
   $threadValue=if($role -eq 'sender'){$nativeReport.senderThread}else{$nativeReport.receiverThread}
   if($rows.Count -ne 1 -or [uint32]$rows[0].Groups[1].Value -ne $pidValue -or
      [uint32]$rows[0].Groups[2].Value -ne $threadValue -or
      $rows[0].Groups[5].Value.TrimEnd("`r") -ne "$runtime\system32\ntvdm.exe" -or
      [regex]::Matches($control,'(?m)^worker-exited role='+$role+' pid='+$pidValue+' exit=0 windowAlive=0\r?$').Count -ne 1){
    throw 'Actual cross-worker HWND/process/thread/normal-exit identity mismatch'
   }
  }
  if(!$nativeReport.receiverPid -or $nativeReport.receiverPid -eq $nativeReport.pid -or
     $control -notmatch 'PASS cross-worker normal process retirement' -or
     [regex]::Matches($control,'(?m)^launch role=(?:sender|receiver) returned_pid=\d+ flags=00000800\r?$').Count -ne 2){throw 'Two genuine separate-WOW launches were not verified'}
  @(@{worker=$nativeReport.pid;task=$last.task;foreignGetWindowTask=$last.peerTask;expectedShell=$last.foreignExpected;remoteTask=$last.remoteTask},
    @{worker=$nativeReport.receiverPid;task=$peer.task;foreignGetWindowTask=$peer.peerTask;expectedShell=$peer.foreignExpected;remoteTask=$peer.remoteTask})|ConvertTo-Json|Set-Content "$root/worker-task-pairs.json"
 }else{foreach($task in @($last.task,$peer.task)){
  $identity='{0:X8}' -f [int]$task
  if([regex]::Matches($trace,'(?m)^'+$nativeReport.pid+' TaskRetirement .*task='+$identity+' stage=complete status=0\r?$').Count -ne 1){throw 'Each actual task must retire exactly once in the verified worker'}
 }}
 $(if($CrossWorkerCoordinator){'PASS actual USER16 cross-worker messages, local shell fallback, exact PID/task pairs and normal separate-process retirement'}else{'PASS actual USER16 two-task same-worker send/nesting/reply/post/filter/quit/string-copyback, native peer both directions and normal task retirement'})|Set-Content "$root/result.txt"
 if($NativeExtension){'PASS actual original generic thunk -> public WOWCallback16/Ex -> guest -> sibling send; WOWYield16/DirectedYield16; invalid Ex arguments; normal task retirement'|Set-Content "$root/extension-result.txt"}
 if($StructuredPayloads){
  @{families=@('MINMAXINFO','WINDOWPOS','COPYDATA');directions=@('Win16-to-Win16','Win16-to-native','native-to-Win16');guestCallbacks=@($last.structures,$peer.structures);nativeChecks=$nativeReport.structures;nestedMinmaxStackCopyback=$true;nestedCopydataFarIdentity=@($last.copyNested,$peer.copyNested);converterIdentity='Package/source manifests, not inferred by this probe';originalGuestChanged=$false;limits='Named synchronous paths only; not physical memory movement, all procedure branches, allocation-failure retirement or all M4'}|ConvertTo-Json|Set-Content "$root/structures-result.json"
 }
 if($ClassicDde){'PASS actual classic native-to-Win16 WM_DDE_EXECUTE, reverse ACK packing, deferred guest release and native retirement; no DDEML admitted'|Set-Content "$root/classic-dde-result.txt"}
 if($CopyDataProc32){@{fresh=$nativeReport.proc32Fresh;activeBorrow=$nativeReport.proc32Borrowed;nativeCallerThread=$nativeReport.senderThread;ordinaryStructures=$nativeReport.structures;guestNormalRetirement=$true;limits='Original GetWindowLong/CallWindowProc native thunk; exact pointer identity additionally checked by source-body fixture, not inferred from native sender capture'}|ConvertTo-Json|Set-Content "$root/copydata-proc32-result.json"}
 if($ReplyPayloadLifetime){'PASS actual Win16 consumed COPYDATA before early reply; sender-owned header/payload protected and unreadable before receiver continuation; original unthunk/normal retirement completed; native captured-buffer lifetime not inferred'|Set-Content "$root/reply-payload-result.txt"}
 if($TimeoutPayloadLifetime){'PASS actual native SendMessageTimeout status0/error1460 after live Win16 receiver entered; owned input protected/unreadable before continuation; original unthunk and normal task retirement; not native API success or all M4'|Set-Content "$root/timeout-payload-result.txt"}
 if($ReplyPayloadLifetime){foreach($input in Import-Csv "$root/reply-source-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Reply payload test source changed'}}}
 @{crossWorker=[bool]$CrossWorkerCoordinator;externalProcess=$(if($ExternalNativePeer){$nativeReport.externalPid}else{'not exercised'});unproved='Implicit focus receive, full converted structures, SMTO_BLOCK and receiver death during pending send remain S4 obligations; ordinary mode does not exercise cross-worker or an external native process';largeEnvironment='Known original guest limitation not repaired by logon-block acceptance'}|ConvertTo-Json|Set-Content "$root/limits.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 if($scope){Stop-IsolatedPackageScope $scope}
 if($runtime){Remove-PhysicalRuntimeStage $runtime}
 if($process){$null=$process.Handle;if(!$process.HasExited){$process.Kill()};$process.Dispose()}
}
