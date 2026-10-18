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
 [switch]$TimeoutPayloadLifetime
)
# Entire actual-guest message group. Invoke as an owned hidden background run.
# No original guest is changed or rebuilt; only independently authored probes.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if($ReplyPayloadLifetime -and !$StructuredPayloads){throw 'ReplyPayloadLifetime requires the complete StructuredPayloads group'}
if($TimeoutPayloadLifetime -and !$ReplyPayloadLifetime){throw 'TimeoutPayloadLifetime requires the complete ReplyPayloadLifetime group'}
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
 if(!$bytes.Length -or $bytes.Length%$recordBytes){throw "Incomplete $name message records"}
 $fields=@('role','phase','errors','task','peerTask','basic','nested','early','posted','same','filtered','threadpost','text','native','destroyed')
 if($ThreadClientView){$fields+=@('clientVersion','clientExpected','clientChecks')}
 if($StructuredPayloads){$fields+=@('structures','copyNested')}
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
  & $assembler @arguments $assembly *> "$root/$role-assemble.txt"
  if($LASTEXITCODE){throw "Independent $role assembly failed"}
 }
 Push-Location $root
 try{
  $nativeDefines=@();if($StructuredPayloads){$nativeDefines+='/DM4_STRUCTURES'}
  if($ReplyPayloadLifetime){$nativeDefines+='/DM4_REPLY_PAYLOAD'}
  if($TimeoutPayloadLifetime){$nativeDefines+='/DM4_TIMEOUT_PAYLOAD'}
  & $CompilerDriver cl.exe /nologo /LD /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS @nativeDefines "/Fo$root/peer.obj" $native /link "/OUT:$root/MSGHOST.DLL" "/IMPLIB:$root/MSGHOST.lib" /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12 user32.lib *> "$root/native-build.txt"
  if($LASTEXITCODE){throw 'Native peer build failed'}
 }finally{Pop-Location}
 Get-FileHash $assembly,$native,$PSCommandPath,"$root/MSGSEND.EXE","$root/MSGRECV.EXE","$root/MSGHOST.DLL"|Export-Csv -NoTypeInformation "$root/probe-inputs.csv"
 if($ReplyPayloadLifetime){Get-FileHash "$PSScriptRoot/wow_reply_payload_lifetime.h","$PSScriptRoot/wow_reply_payload_report.ps1"|Export-Csv -NoTypeInformation "$root/reply-source-inputs.csv"}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/MSGSEND.EXE","$root/MSGRECV.EXE","$root/MSGHOST.DLL" $tests
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
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 $arguments=@("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait')
 if($Win16Style -eq 'Win16'){$arguments+='--win16-style'}elseif($Win16Style -eq 'All'){$arguments+='--win16-style-all'}
 $arguments+=@("$tests/MSGSEND.EXE",'--observation-timeout-ms','60000')
 foreach($argument in $arguments){$info.ArgumentList.Add($argument)}
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=$arguments;timeoutSeconds=70}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 if(!$process.WaitForExit(70000)){throw 'Actual message group deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer-exit.txt"
 # Preserve short/failed reports before asserting success; no failed-row replay.
 foreach($name in @('MSGSEND','MSGRECV','MSGHOST','MSGEXT')){
  if(Test-Path "$tests/$name.BIN"){Copy-Item "$tests/$name.BIN" "$root/$name.bin"}
 }
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
 if(!$last.task -or !$peer.task -or $last.task -eq $peer.task -or $last.peerTask -ne $peer.task -or
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
 if($bytes.Length -ne $nativeBytes -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'NMSG'){throw 'Missing native peer report'}
 $names=@('signature','errors','pid','senderThread','receiverThread','nativeThread','received','nested','replied','text','exit','windowAlive');$nativeReport=[ordered]@{}
 if($StructuredPayloads){$names+='structures'}
 for($i=0;$i -lt $names.Count;$i++){$nativeReport[$names[$i]]=[BitConverter]::ToUInt32($bytes,4*$i)}
 $nativeReport|ConvertTo-Json|Set-Content "$root/native.json"
 if($nativeReport.errors -or !$nativeReport.pid -or $nativeReport.exit -or $nativeReport.windowAlive -or
    $nativeReport.received -ne 4 -or $nativeReport.nested -ne 1 -or $nativeReport.replied -ne 1 -or $nativeReport.text -ne 1 -or
    @(@($nativeReport.senderThread,$nativeReport.receiverThread,$nativeReport.nativeThread)|Where-Object {$_}|Sort-Object -Unique).Count -ne 3){throw 'Native peer identity/receive/result/lifetime failure'}
 if($StructuredPayloads -and $nativeReport.structures -ne 6){throw 'Both native directions did not complete all three structure families'}
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
 foreach($task in @($last.task,$peer.task)){
  $identity='{0:X8}' -f [int]$task
  if([regex]::Matches($trace,'(?m)^'+$nativeReport.pid+' TaskRetirement .*task='+$identity+' stage=complete status=0\r?$').Count -ne 1){throw 'Each actual task must retire exactly once in the verified worker'}
 }
 'PASS actual USER16 two-task same-worker send/nesting/reply/post/filter/quit/string-copyback, native peer both directions and normal task retirement'|Set-Content "$root/result.txt"
 if($NativeExtension){'PASS actual original generic thunk -> public WOWCallback16/Ex -> guest -> sibling send; WOWYield16/DirectedYield16; invalid Ex arguments; normal task retirement'|Set-Content "$root/extension-result.txt"}
 if($StructuredPayloads){
  @{families=@('MINMAXINFO','WINDOWPOS','COPYDATA');directions=@('Win16-to-Win16','Win16-to-native','native-to-Win16');guestCallbacks=@($last.structures,$peer.structures);nativeChecks=$nativeReport.structures;nestedMinmaxStackCopyback=$true;nestedCopydataFarIdentity=@($last.copyNested,$peer.copyNested);originalConvertersUnchanged=$true;originalGuestChanged=$false;limits='Synchronous and named original CallWindowProc paths only; not physical memory movement, all native procedure-thunk provenance branches, interrupted/exceptional allocation retirement or all M4'}|ConvertTo-Json|Set-Content "$root/structures-result.json"
 }
 if($ReplyPayloadLifetime){'PASS actual Win16 consumed COPYDATA before early reply; sender-owned header/payload protected and unreadable before receiver continuation; original unthunk/normal retirement completed; native captured-buffer lifetime not inferred'|Set-Content "$root/reply-payload-result.txt"}
 if($TimeoutPayloadLifetime){'PASS actual native SendMessageTimeout status0/error1460 after live Win16 receiver entered; owned input protected/unreadable before continuation; original unthunk and normal task retirement; not native API success or all M4'|Set-Content "$root/timeout-payload-result.txt"}
 if($ReplyPayloadLifetime){foreach($input in Import-Csv "$root/reply-source-inputs.csv"){if((Get-FileHash $input.Path).Hash -ne $input.Hash){throw 'Reply payload test source changed'}}}
 @{unproved='Cross-worker, external native process/bitness, implicit focus receive, full converted structures, SMTO_BLOCK and receiver death during pending send remain S4 obligations';largeEnvironment='Known original guest limitation not repaired by logon-block acceptance'}|ConvertTo-Json|Set-Content "$root/limits.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 if($scope){Stop-IsolatedPackageScope $scope}
 if($runtime){Remove-PhysicalRuntimeStage $runtime}
 if($process){$null=$process.Handle;if(!$process.HasExited){$process.Kill()};$process.Dispose()}
}
