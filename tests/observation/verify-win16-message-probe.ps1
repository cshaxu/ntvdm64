[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('Off','Win16','All')][string]$Win16Style='Off',
 [string]$Nasm='nasm.exe',
 [switch]$NativeExtension
)
# Entire actual-guest message group. Invoke as an owned hidden background run.
# No original guest is changed or rebuilt; only independently authored probes.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
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
 if(!$bytes.Length -or $bytes.Length%34){throw "Incomplete $name message records"}
 $fields=@('role','phase','errors','task','peerTask','basic','nested','early','posted','same','filtered','threadpost','text','native','destroyed')
 $rows=@()
 for($offset=0;$offset -lt $bytes.Length;$offset+=34){
  if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'MSG1'){throw "Invalid $name signature"}
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
  & $assembler @arguments $assembly *> "$root/$role-assemble.txt"
  if($LASTEXITCODE){throw "Independent $role assembly failed"}
 }
 Push-Location $root
 try{
  & $CompilerDriver cl.exe /nologo /LD /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root/peer.obj" $native /link "/OUT:$root/MSGHOST.DLL" "/IMPLIB:$root/MSGHOST.lib" /EXPORT:MessagePeer=_MessagePeer@4 /EXPORT:MessageExtension=_MessageExtension@12 user32.lib *> "$root/native-build.txt"
  if($LASTEXITCODE){throw 'Native peer build failed'}
 }finally{Pop-Location}
 Get-FileHash $assembly,$native,$PSCommandPath,"$root/MSGSEND.EXE","$root/MSGRECV.EXE","$root/MSGHOST.DLL"|Export-Csv -NoTypeInformation "$root/probe-inputs.csv"
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
 $entries=[WowMessageLogonEnvironment]::Read([Security.Principal.WindowsIdentity]::GetCurrent().Token)
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
 $send=Read-GuestReport "$root/MSGSEND.bin" sender
 $receive=Read-GuestReport "$root/MSGRECV.bin" receiver
 $report=Get-Content "$root/observer.txt" -Raw
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher failed normal completion'}
 if(($send.phase -join ',') -ne '0,1,2,3,4,6' -or ($receive.phase -join ',') -ne '0,5,6' -or
    @(@($send)+@($receive)|Where-Object errors -ne 0).Count){throw 'Guest message phase/assertion failure'}
 $last=$send[-1];$peer=$receive[-1]
 $expectedNative=if($NativeExtension){5}else{4}
 if(!$last.task -or !$peer.task -or $last.task -eq $peer.task -or $last.peerTask -ne $peer.task -or
    @($send|Where-Object task -ne $last.task).Count -or @($receive|Where-Object task -ne $peer.task).Count -or
    $last.role -ne 0 -or $peer.role -ne 1 -or $last.nested -ne 2 -or $last.same -ne 2 -or
    $last.early -ne 1 -or $last.posted -ne 1 -or $last.filtered -ne 1 -or $last.threadpost -ne 1 -or
    $last.text -ne 1 -or $last.native -ne $expectedNative -or $last.destroyed -ne 1 -or
    $peer.basic -ne 1 -or $peer.nested -ne 1 -or $peer.early -ne 1 -or $peer.posted -ne 1){throw 'Incomplete actual Win16 message contract'}
 $bytes=[IO.File]::ReadAllBytes("$root/MSGHOST.bin")
 if($bytes.Length -ne 48 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'NMSG'){throw 'Missing native peer report'}
 $names=@('signature','errors','pid','senderThread','receiverThread','nativeThread','received','nested','replied','text','exit','windowAlive');$nativeReport=[ordered]@{}
 for($i=0;$i -lt 12;$i++){$nativeReport[$names[$i]]=[BitConverter]::ToUInt32($bytes,4*$i)}
 $nativeReport|ConvertTo-Json|Set-Content "$root/native.json"
 if($nativeReport.errors -or !$nativeReport.pid -or $nativeReport.exit -or $nativeReport.windowAlive -or
    $nativeReport.received -ne 4 -or $nativeReport.nested -ne 1 -or $nativeReport.replied -ne 1 -or $nativeReport.text -ne 1 -or
    @(@($nativeReport.senderThread,$nativeReport.receiverThread,$nativeReport.nativeThread)|Where-Object {$_}|Sort-Object -Unique).Count -ne 3){throw 'Native peer identity/receive/result/lifetime failure'}
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
 @{unproved='Cross-worker, external native process/bitness, implicit focus receive, full converted structures, SMTO_BLOCK and receiver death during pending send remain S4 obligations';largeEnvironment='Known original guest limitation not repaired by logon-block acceptance'}|ConvertTo-Json|Set-Content "$root/limits.json"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 if($scope){Stop-IsolatedPackageScope $scope}
 if($runtime){Remove-PhysicalRuntimeStage $runtime}
 if($process){$null=$process.Handle;if(!$process.HasExited){$process.Kill()};$process.Dispose()}
}
