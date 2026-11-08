[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$RetirementDriver,
 [string]$InstanceDriver,
 [switch]$ExplicitVmd,
 [switch]$RetireCommand,
 [switch]$ServiceLoss,
 [string]$WorkerCloseMonitor,
 [string]$RegistrySnapshotMonitor,
 [string]$RegistrySnapshotMarker,
 [switch]$ConsoleClose,
 [string]$InterruptObserver,
 [switch]$Keyboard,
 [switch]$BlockedKeyboard,
 [switch]$ConsumeKeyboard,
 [switch]$Reuse,
 [switch]$InitializationFailure,
 [switch]$Continuation,
 [switch]$RealCommand,
 [switch]$NestedCommand,
 [switch]$ReentrantCommand,
 [switch]$NestedRedirect,
 [switch]$FillApplication,
 [string]$OrderedCommandImage,
 [switch]$ResizeCommand,
 [switch]$GuestGridControl,
 [ValidateSet('386-dosonly0','386-dosonly1')][string]$RuntimeCase='386-dosonly0',
 [string]$UnreadInputImage,
 [ValidateSet('sequence','repeat','function','extended','oem')][string]$UnreadInputMode='sequence',
 [ValidateSet('A','B')][string]$FirstCommand='A',
 [switch]$TraceContinuation,
 [switch]$VideoApplication,
 [switch]$VideoFontControl,
 [switch]$VideoFontDirectControl,
 [switch]$FullscreenVmControl,
 [switch]$DeferredContinuation,
 [switch]$DirectoryYield,
 [switch]$LocalDirectoryControl,
 [string]$DosWitnessImage
)
$ErrorActionPreference='Stop'
if($GuestGridControl -and (!$RealCommand -or !$OrderedCommandImage -or $ResizeCommand -or $NestedCommand)){throw 'Guest grid control requires independent real ordered commands'}
if([bool]$RegistrySnapshotMonitor -ne [bool]$RegistrySnapshotMarker){throw 'Registry snapshot requires monitor and marker'}
if($ResizeCommand -and (!$OrderedCommandImage -or !$RealCommand -or $ServiceLoss -or $WorkerCloseMonitor)){throw 'Resize requires independent real ordered commands'}
if($ConsoleClose -and !$WorkerCloseMonitor){throw 'Console close requires management monitor'}
if($ServiceLoss -and $WorkerCloseMonitor){throw 'Select service loss or managed worker close'}
if(($ServiceLoss -or $WorkerCloseMonitor) -and (!$OrderedCommandImage -or !$RealCommand)){throw 'Termination requires real ordered native tasks'}
if($UnreadInputImage -and (!$RealCommand -or $OrderedCommandImage -or $NestedCommand)){throw 'Unread input requires separate RealCommand witness'}
if($FillApplication -and (!$Continuation -or $RealCommand)){throw 'Fill application requires diagnostic Continuation'}
if($NestedRedirect -and !$ReentrantCommand){throw 'Nested redirection requires reentrant commands'}
if($ReentrantCommand -and !$NestedCommand){throw 'Reentrant command witness requires NestedCommand'}
if($NestedCommand -and !$RealCommand){throw 'Nested command witness requires RealCommand'}
if($OrderedCommandImage -and !$RealCommand){throw 'Ordered native commands require RealCommand'}
if($RetireCommand -and (!$RetirementDriver -or $Continuation -or $DosWitnessImage)){throw 'Command retirement requires the retirement driver and production witness'}
if($RealCommand -and (!$Continuation -or $VideoApplication -or $Keyboard -or $DeferredContinuation)){throw 'Real command witness requires non-deferred Continuation without keyboard/video diagnostics'}
if($Reuse -and $RetirementDriver -and !$RetireCommand){throw 'Combined reuse requires real command retirement'}
if($InitializationFailure -and ($Reuse -or $RetirementDriver)){throw 'Initialization failure is a separate witness'}
if($Continuation -and ($InitializationFailure -or $Reuse -or $RetirementDriver)){throw 'Continuation is a separate witness'}
if($VideoApplication -and !$Continuation){throw 'VIDEO application requires continuation witness'}
if($VideoFontControl -and !$VideoApplication){throw 'Font control requires VIDEO application; it is not driver font acceptance'}
if($VideoFontDirectControl -and (!$VideoApplication -or $VideoFontControl)){throw 'Direct font control requires VIDEO application and excludes BIOS control'}
if($FullscreenVmControl -and !$VideoApplication){throw 'Fullscreen control requires the VIDEO witness; it is not windowed acceptance'}
if($TraceContinuation -and !$Continuation){throw 'Continuation tracing requires its witness'}
if($DeferredContinuation -and !$Continuation){throw 'Deferred completion requires its witness'}
if($DirectoryYield -and (!$Continuation -or $DeferredContinuation)){throw 'Directory yield control requires immediate continuation'}
if($Keyboard -and ($InitializationFailure -or $Reuse -or $RetirementDriver)){throw 'Keyboard calibration requires ordinary identity witnesses'}
if($Keyboard -and !$InterruptObserver){throw 'Keyboard calibration requires the diagnostic interrupt observer and host package'}
if($BlockedKeyboard -and (!$Keyboard -or !$DeferredContinuation -or !$TraceContinuation)){throw 'Blocked keyboard requires calibrated input, deferred continuations and tracing'}
if($ConsumeKeyboard -and !$BlockedKeyboard){throw 'Consumption witness requires blocked keyboard calibration'}
if($LocalDirectoryControl -and !$Continuation){throw 'Local directory control requires continuation'}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$copy=& "$PSScriptRoot/New-Win31ProbeCopy.ps1" -DriverImage $DriverImage -InstallSource $InstallSource -InstallRoot $InstallRoot -OutputRoot $OutputRoot -InterruptObserver $InterruptObserver -RetirementDriver $RetirementDriver -InstanceDriver $InstanceDriver
$task=$copy.Task; $source=$copy.Source; $dest=$copy.Destination; $original=$copy.Original
$enc=[Text.Encoding]::GetEncoding(28591)
if($ExplicitVmd){
 $system="$dest/SYSTEM.INI"
 $iniText=$enc.GetString([IO.File]::ReadAllBytes($system))
 if($iniText -notmatch '(?im)^[ \t]*device[ \t]*=[ \t]*\*vmd[ \t]*(?:;[^\r\n]*)?$'){
  $iniText=[regex]::Replace($iniText,'(?im)^\[386Enh\]','[386Enh]'+"`r`ndevice=*vmd")
  [IO.File]::WriteAllBytes($system,$enc.GetBytes($iniText))
 }
}

$dosWitness=if($RetirementDriver){'dos-retirement.asm'}else{'dos-identity.asm'}
$dosDefines=@()
if($RetireCommand){$dosDefines+='-DRETIRE_COMMAND=1'}
if($Continuation){$dosDefines=@('-DCONTINUATION=1')}
if($RealCommand){$dosDefines+='-DREAL_COMMAND=1'}
if($GuestGridControl){$dosDefines+='-DGUEST_GRID_CONTROL=1'}
if($NestedCommand){$dosDefines+='-DNESTED_COMMAND=1'}
if($UnreadInputImage){
 $dosDefines+='-DUNREAD_INPUT=1'
 $dosDefines+=('-DUNREAD_MODE='+$UnreadInputMode)
 Copy-Item -LiteralPath $UnreadInputImage -Destination "$dest/PATCH/UNREAD.EXE"
}
if($OrderedCommandImage){
 $dosDefines+='-DORDERED_COMMAND=1'
 if($ResizeCommand){$dosDefines+='-DRESIZED_COMMAND=1'}
 Copy-Item -LiteralPath $OrderedCommandImage -Destination "$dest/PATCH/ORDER.EXE"
}
if($DirectoryYield){$dosDefines+='-DDIRECTORY_YIELD=1'}
if($Keyboard){$dosDefines+='-DKEYBOARD=1'}
if($BlockedKeyboard){$dosDefines+='-DBLOCKED_KEYBOARD=1'}
if($ConsumeKeyboard){$dosDefines+='-DCONSUME_KEYBOARD=1'}
if($VideoApplication){$dosDefines+='-DVIDEO_APPLICATION=1'}
if($VideoFontControl){$dosDefines+='-DVIDEO_FONT_CONTROL=1'}
if($VideoFontDirectControl){$dosDefines+='-DVIDEO_FONT_DIRECT_CONTROL=1'}
if($DosWitnessImage){
 Copy-Item -LiteralPath (Resolve-Path -LiteralPath $DosWitnessImage).Path -Destination "$task/identity.com"
} else {
 & nasm -f bin @dosDefines "$PSScriptRoot/$dosWitness" -o "$task/identity.com"
 if($LASTEXITCODE){throw 'DOS identity witness build failed'}
}
$directoryRoot="$dest\PATCH"
if($LocalDirectoryControl){
 $directoryRoot=Join-Path ([IO.Path]::GetTempPath()) ([IO.Path]::GetFileName($dest))
 if($directoryRoot -notmatch '^C:\\' -or (Test-Path $directoryRoot) -or $directoryRoot.Length -gt 55){throw 'Fresh short C: temporary directory required'}
 New-Item -ItemType Directory $directoryRoot|Out-Null
 [IO.File]::WriteAllText("$directoryRoot/.t479-owner",$task)
}
foreach($id in 'A','B'){
 $directory="$dest\PATCH"
 if($Continuation){$directory="$directoryRoot\V$id";New-Item -ItemType Directory -Path $directory|Out-Null}
 $bytes=[IO.File]::ReadAllBytes("$task/identity.com")
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_IDENTITY_RESULT@')
 if($marker -lt 0){throw 'No output marker'}
 [Array]::Clear($bytes,$marker,128)
 $path=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\$id.BIN")
 [Array]::Copy($path,0,$bytes,$marker,$path.Length)
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_PAIR_READY@')
 if($marker -lt 0){throw 'No pair rendezvous marker'}
 [Array]::Clear($bytes,$marker,128)
 $path=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\PAIR.KEY")
 [Array]::Copy($path,0,$bytes,$marker,$path.Length)
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_ROLE@')
 if($marker -lt 0){throw 'No VM role marker'}
 $bytes[$marker]=[byte][char]$id
 if($NestedCommand){
  $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_NESTED_OUTPUT@')
  if($marker -lt 0){throw 'Missing nested output marker'}
  $value=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\$id.TXT")
  if($value.Length -ge 128){throw 'Nested output path too long'}
  [Array]::Clear($bytes,$marker,128)
  [Array]::Copy($value,0,$bytes,$marker,$value.Length)
 }
 if($RetireCommand){
  $fields=@{
   '@VM_NATIVE_READY@'=@(128,"$dest\PATCH\NATIVE.OK")
   '@VM_NATIVE_COMMAND@'=@(256,"C:\Windows\System32\cmd.exe /c `"echo READY>$dest\PATCH\NATIVE.OK & pause`"")
  }
  foreach($name in $fields.Keys){
   $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf($name)
   if($marker -lt 0){throw "Missing retirement marker $name"}
   $value=[Text.Encoding]::ASCII.GetBytes($fields[$name][1])
   $capacity=[int]$fields[$name][0]
   if($value.Length -ge $capacity){throw 'Retirement fixture field too long'}
   [Array]::Clear($bytes,$marker,$capacity)
   [Array]::Copy($value,0,$bytes,$marker,$value.Length)
  }
 }
 if($RealCommand -or $RetireCommand){
  $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_ENVIRONMENT@')
  if($marker -lt 0){throw 'No native command environment marker'}
  # The matrix later creates its own short physical runtime. Its path is not
  # known here; the repository package path is not necessarily DOS-addressable.
  # Give the nested DOS witness an immutable MEM copy in the short guest fixture.
  $commandPath="$PackageRoot\system32;C:\Windows\System32;C:\Windows"
  if($NestedCommand){
   Copy-Item -LiteralPath "$PackageRoot/system32/MEM.EXE" -Destination "$dest/PATCH/MEM.EXE" -Force
   if((Get-FileHash "$PackageRoot/system32/MEM.EXE").Hash -ne (Get-FileHash "$dest/PATCH/MEM.EXE").Hash){throw 'Nested MEM copy differs'}
   if($ReentrantCommand){
    Copy-Item -LiteralPath "$PackageRoot/system32/COMMAND.COM" -Destination "$dest/PATCH/COMMAND.COM" -Force
    if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'Nested COMMAND copy differs'}
   }
   $commandPath="$dest\PATCH;$commandPath"
  }
  $environment=[Text.Encoding]::ASCII.GetBytes("ROLE=$id`0ROUND=0`0PATH=$commandPath`0`0")
  if($environment.Length -gt 512){throw 'Native command fixture environment too large'}
  [Array]::Clear($bytes,$marker,512)
  [Array]::Copy($environment,0,$bytes,$marker,$environment.Length)
 }
 if($OrderedCommandImage -or $UnreadInputImage){
  $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_ORDER_COMMAND@')
  if($marker -lt 0){throw 'Missing ordered command marker'}
  $value=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\ORDER.EXE T479-$([IO.Path]::GetFileName($dest)) $FirstCommand"+[char]13)
  if($ResizeCommand){$value=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\ORDER.EXE T481-$([IO.Path]::GetFileName($dest)) $FirstCommand resize $dest\PATCH\RESIZE"+[char]13)}
  if($ServiceLoss -or $WorkerCloseMonitor){$value=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\ORDER.EXE T481 $FirstCommand hold $dest\PATCH\HOLD"+[char]13)}
  if($UnreadInputImage){$value=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\UNREAD.EXE $dest\PATCH\INPUT $UnreadInputMode --vm"+[char]13)}
  if($value.Length -ge 256){throw 'Ordered command too long'}
  [Array]::Clear($bytes,$marker,256)
  [Array]::Copy($value,0,$bytes,$marker,$value.Length)
 }
 if($Continuation){
  $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@VM_DIRECTORY@')
  if($marker -lt 0){throw 'No VM directory marker'}
  [Array]::Clear($bytes,$marker,128)
  $path=[Text.Encoding]::ASCII.GetBytes($directory)
  [Array]::Copy($path,0,$bytes,$marker,$path.Length)
 }
 [IO.File]::WriteAllBytes("$dest/PATCH/$id.COM",$bytes)
 $vmFlags=if($FullscreenVmControl){'0x2100a'}else{'0x21002'}
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "VM identity $id" --program "$dest\PATCH\$id.COM" --directory $directory --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags $vmFlags
 if($LASTEXITCODE){throw 'DOS VM PIF build failed'}
 # Generated fixture only; STDPIF and W386PIF30 from original pif.h.
 # Zero conventional-memory fields leave too little COM load space. Reserve
 # 128 KiB and permit 640 KiB independently of the witness's current size.
 & {
  $pif=[IO.File]::ReadAllBytes("$dest/PATCH/$id.PIF")
  $text=[Text.Encoding]::ASCII.GetString($pif)
  $std=$text.IndexOf('MICROSOFT PIFEX')
  $hdr=$text.IndexOf('WINDOWS 386 3.0')
  if($std -lt 36 -or $hdr -lt $std){throw 'Missing generated PIF headers'}
  if($hdr+22 -gt $pif.Length){throw 'Truncated generated PIF header'}
  $body=[BitConverter]::ToUInt16($pif,$hdr+18)
  if($body -lt $hdr+22 -or $body+4 -gt $pif.Length){throw 'Invalid generated PIF memory fields'}
  foreach($offset in @(32,$body)){[Array]::Copy([BitConverter]::GetBytes([uint16]640),0,$pif,$offset,2)}
  foreach($offset in @(34,($body+2))){[Array]::Copy([BitConverter]::GetBytes([uint16]128),0,$pif,$offset,2)}
  $sum=0;for($at=2;$at -lt $std;++$at){$sum=($sum+$pif[$at]) -band 255};$pif[1]=[byte]$sum
  [IO.File]::WriteAllBytes("$dest/PATCH/$id.PIF",$pif)
 }
}
$ini="$dest/WIN.INI"
$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
if($InitializationFailure){$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*','load=')}
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))

$defines=@()
if($InitializationFailure){$defines=@('-DINIT_FAILURE=1')}
if($Reuse){
 $reuseDefines=@()
 if($RetireCommand){$reuseDefines+='-DRETIRE_COMMAND=1'}
 & nasm -f bin @reuseDefines "$PSScriptRoot/dos-reuse.asm" -o "$dest/PATCH/REUSE.COM"
 if($LASTEXITCODE){throw 'Reuse witness assembly failed'}
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/REUSE.PIF" --title 'VM reuse' --program "$dest\PATCH\REUSE.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'Reuse PIF build failed'}
 $defines=@('-DREUSE=1')
}
if($Keyboard){$defines+='-DKEYBOARD=1'}
if($BlockedKeyboard){$defines+='-DBLOCKED_KEYBOARD=1'}
& nasm -f bin @defines "$PSScriptRoot/win31-identity.asm" -o "$task/controller.exe"
if($LASTEXITCODE){throw 'Win16 controller build failed'}
$bytes=[IO.File]::ReadAllBytes("$task/controller.exe")
$fields=@(@('@VM_API_PATH@',"$dest\PATCH\VMLOAD.BIN"),@('@VM_A_PATH@',"$dest\PATCH\A.BIN"),@('@VM_B_PATH@',"$dest\PATCH\B.BIN"))
if($Reuse){$fields+=@(@('@VM_REUSE_PROGRAM@',"$dest\PATCH\REUSE.PIF"),@('@VM_REUSE_RESULT@',"$dest\PATCH\REUSE.BIN"))}
foreach($field in $fields){
 $image=[Text.Encoding]::ASCII.GetString($bytes)
 $offset=$image.IndexOf($field[0],[StringComparison]::Ordinal)
 $value=[Text.Encoding]::ASCII.GetBytes($field[1])
 if($offset -lt 0 -or $image.LastIndexOf($field[0],[StringComparison]::Ordinal) -ne $offset -or $value.Length -ge 128){throw 'Invalid controller path field'}
 [Array]::Clear($bytes,$offset,128)
 [Array]::Copy($value,0,$bytes,$offset,$value.Length)
}
[IO.File]::WriteAllBytes("$task/controller.exe",$bytes)
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS')
$nasm=(Get-Command nasm.exe -CommandType Application | Select-Object -First 1).Source
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH=(Split-Path $nasm)+';C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
if($InitializationFailure){$env:MVDM_INIT_FAILURE_REPORT="$task/initialization-rollback.bin"}
if($RealCommand){$env:MVDM_REAL_COMMAND='1'}
if($NestedCommand){$env:MVDM_NESTED_COMMAND='1'}
if($ReentrantCommand){$env:MVDM_REENTRANT_COMMAND='1'}
if($NestedRedirect){$env:MVDM_NESTED_REDIRECT='1'}
if($FillApplication){$env:MVDM_FILL_APPLICATION='1'}
if($TraceContinuation){$env:MVDM_CONTINUATION_TRACE="$task/continuation-trace.txt"}
if($InterruptObserver){$env:MVDM_INTERRUPT_OBSERVATION='1'}
if($BlockedKeyboard){$env:MVDM_BLOCKED_KEYBOARD='1'}
if($ConsumeKeyboard){$env:MVDM_CONSUME_KEYBOARD='1'}
if($VideoApplication){$env:MVDM_VIDEO_APPLICATION='1'}
if($DeferredContinuation){$env:MVDM_CONTINUATION_DEFER='1'}
if($LocalDirectoryControl){$env:MVDM_CONTINUATION_LOCAL='1'}
if($DosWitnessImage){$env:MVDM_SCREEN_UNSEEDED='1'}
function Confirm-Reuse($rows){
  $b=[IO.File]::ReadAllBytes("$dest/PATCH/REUSE.BIN")
  Copy-Item "$dest/PATCH/REUSE.BIN" "$task/runtime/REUSE.BIN"
  if($b.Length -ne 36 -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'VRUS'){throw 'Invalid reuse report'}
  $reuseResult=[ordered]@{vm=[BitConverter]::ToUInt32($b,4);generation=[BitConverter]::ToUInt64($b,8);oldGeneration=[BitConverter]::ToUInt64($b,16);error=[BitConverter]::ToUInt32($b,24);rejected=[BitConverter]::ToUInt32($b,28);consumed=[BitConverter]::ToUInt32($b,32)}
  $reuseResult|ConvertTo-Json|Set-Content "$task/runtime/vm-reuse.json"
  $previous=@($rows|Where-Object{$_.vm -eq $reuseResult.vm -and $_.generation -eq $reuseResult.oldGeneration})
  if($reuseResult.error -or $previous.Count -ne 1 -or $reuseResult.generation -le $reuseResult.oldGeneration -or $reuseResult.rejected -ne 2 -or $reuseResult.consumed -ne 1){throw 'Actual VMM handle reuse or stale generation rejection not proved'}
  'PASS actual VMM handle reuse, old direct/queued wake rejection and new counted completion'
  if($RetireCommand){'PASS reused VM executes a new native command and receives exit code37'}

}
try {
 $retirementOptions=@{}
 if($RegistrySnapshotMonitor){$retirementOptions.RegistrySnapshotMonitor=$RegistrySnapshotMonitor;$retirementOptions.RegistrySnapshotMarker=$RegistrySnapshotMarker}
 if($ServiceLoss -or $WorkerCloseMonitor){$retirementOptions.ServiceLossMarkers=@("$dest\PATCH\HOLD-A.txt","$dest\PATCH\HOLD-B.txt")}
 if($WorkerCloseMonitor){$retirementOptions.WorkerCloseMonitor=$WorkerCloseMonitor;$retirementOptions.ConsoleClose=$ConsoleClose}
 if($RetireCommand){$retirementOptions.RetiredNativeCommandMarker="$dest\PATCH\NATIVE.OK"}
 & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/runtime" -WindowsProbeImage "$task/controller.exe" -TimeoutMs 90000 -Cases $RuntimeCase @retirementOptions
 if($LASTEXITCODE){throw 'Guest matrix runner failed'}
 if($ServiceLoss -or $WorkerCloseMonitor){
  'PASS actual Win3.1 VM native chains retire after selected lifecycle action; no normal-exit claim'
  return
 }
 if(!(Test-Path "$dest/PATCH/VMLOAD.BIN")){throw 'No guest API result'}
 $b=[IO.File]::ReadAllBytes("$dest/PATCH/VMLOAD.BIN")
 Copy-Item "$dest/PATCH/VMLOAD.BIN" "$task/runtime/VMLOAD.BIN"
 $result=[ordered]@{api=[BitConverter]::ToUInt32($b,0);signature=[BitConverter]::ToUInt32($b,4);vm=[BitConverter]::ToUInt32($b,8);driver=(Get-FileHash "$dest/PATCH/NTVDM64.386").Hash}
 $result|ConvertTo-Json|Set-Content "$task/runtime/vxd-result.json"
 if($InitializationFailure){
  $rollback=[IO.File]::ReadAllBytes("$task/initialization-rollback.bin")
  if($rollback.Length -ne 4 -or [BitConverter]::ToUInt32($rollback,0) -ne 1 -or $result.signature -eq 0x4e544d56){throw 'Initialization failure and actual IRQ rollback not proved'}
  'PASS injected arm failure, real guest rollback, released host IRQ/hook and normal Windows exit'
  return
 }
 if($result.signature -ne 0x4e544d56 -or !$result.vm){throw 'VxD API identity did not pass'}

 if($RetirementDriver){
  $rows=@(foreach($id in 'A','B'){
   $data=[IO.File]::ReadAllBytes("$dest/PATCH/$id.BIN")
   Copy-Item "$dest/PATCH/$id.BIN" "$task/runtime/$id.BIN"
   if($data.Length -ne 28 -or [Text.Encoding]::ASCII.GetString($data,0,4) -ne 'VRET'){throw 'Invalid retirement witness output'}
   [pscustomobject]@{name=$id;vm=[BitConverter]::ToUInt32($data,4);generation=[BitConverter]::ToUInt64($data,8);error=[BitConverter]::ToUInt32($data,16);blocked=[BitConverter]::ToUInt32($data,20);rejected=[BitConverter]::ToUInt32($data,24)}
  })
  $rows|ConvertTo-Json|Set-Content "$task/runtime/vm-retirement.json"
  if($rows[0].error -or $rows[1].error -or !$rows[0].vm -or !$rows[1].vm -or $rows[0].vm -eq $rows[1].vm -or $rows[1].blocked -ne 0x400 -or $rows[1].rejected -ne 1){throw 'Forced retirement/stale wake failed'}
  if($RetireCommand){
   if(!(Test-Path "$dest/PATCH/NATIVE.OK") -or (Get-Content "$dest/PATCH/NATIVE.OK" -Raw).Trim() -ne 'READY'){throw 'Native command execution before VM retirement not proved'}
   Copy-Item "$dest/PATCH/NATIVE.OK" "$task/runtime/NATIVE.OK"
   'PASS native command started before forced VM retirement through production ingress'
  }
  'PASS forced retirement of blocked VM, rejected late wake and normal Windows exit'
  if($Reuse){Confirm-Reuse $rows}
  return
 }

 $rows=@(foreach($id in 'A','B'){
  $data=[IO.File]::ReadAllBytes("$dest/PATCH/$id.BIN")
  Copy-Item "$dest/PATCH/$id.BIN" "$task/runtime/$id.BIN"
  $expectedBytes=if($Continuation){76}else{72}
  if($GuestGridControl){$expectedBytes+=16}
  if($UnreadInputImage){$expectedBytes+=8}
  if($VideoApplication){$expectedBytes+=10}
  if($Keyboard){$expectedBytes+=2}
  if($data.Length -ne $expectedBytes -or [Text.Encoding]::ASCII.GetString($data,0,4) -ne 'VMID'){throw 'Invalid witness output'}
  $row=[ordered]@{name=$id;signature=[BitConverter]::ToUInt32($data,4);vm=[BitConverter]::ToUInt32($data,8);generation=[BitConverter]::ToUInt64($data,12);status=[BitConverter]::ToUInt16($data,20);negativeStatus=[BitConverter]::ToUInt16($data,22);negativeResult=[BitConverter]::ToUInt32($data,24);start=[BitConverter]::ToUInt32($data,28);finish=[BitConverter]::ToUInt32($data,32)}
  if($row.signature -ne 0x4e544d56 -or !$row.vm -or !$row.generation -or ($row.status -band 1) -or !($row.negativeStatus -band 1) -or $row.negativeResult -ne 1){throw "Identity/negative API failed: $id"}
  $row.callback=[BitConverter]::ToUInt32($data,36)
  $row.callbackCount=[BitConverter]::ToUInt16($data,40)
  $row.callbackErrors=[BitConverter]::ToUInt16($data,42)
  if(!$row.callback -or $row.callbackCount -ne 160 -or $row.callbackErrors){throw "Callback identity/stack/register/flags failed: $id"}
  $row.waitCount=[BitConverter]::ToUInt16($data,44)
  $row.waitErrors=[BitConverter]::ToUInt16($data,46)
  if($row.waitCount -ne 3 -or $row.waitErrors){throw "Counted precompletion/stale generation failed: $id"}
  $row.pairCount=[BitConverter]::ToUInt16($data,48)
  $row.pairErrors=[BitConverter]::ToUInt16($data,50)
  if($row.pairCount -ne 1 -or $row.pairErrors){throw "Blocked cross-VM wait/wake failed: $id"}
  $row.notificationPublished=[BitConverter]::ToUInt64($data,52)
  $row.notificationSeen=[BitConverter]::ToUInt64($data,60)
  $row.notificationIrq=[BitConverter]::ToUInt16($data,68)
  $row.notificationErrors=[BitConverter]::ToUInt16($data,70)
  if(!$row.notificationPublished -or $row.notificationSeen -lt $row.notificationPublished -or $row.notificationIrq -gt 15 -or $row.notificationErrors){throw "Actual PIC/VPICD notification failed: $id"}
  if($Continuation){
   if($VideoApplication){
    $video=@(0..4|ForEach-Object{[BitConverter]::ToUInt16($data,76+2*$_)})
    Write-Host "$id VIDEO stage/samples/height/offset=$($video -join ',')"
   }
   if($UnreadInputImage){
    $inputState=@(0..3|ForEach-Object{[BitConverter]::ToUInt16($data,76+2*$_)})
    Write-Host "$id unread stage/index/actual/expected=$($inputState -join ',')"
   }
   $row.continuationCount=[BitConverter]::ToUInt16($data,72)
   $row.continuationErrors=[BitConverter]::ToUInt16($data,74)
   if($GuestGridControl){Write-Host ("{0} grid stage={1} return={2:X8} row={3} height={4}" -f $id,[BitConverter]::ToUInt16($data,76),[BitConverter]::ToUInt32($data,78),[BitConverter]::ToUInt16($data,82),[BitConverter]::ToUInt16($data,84))}
   if($GuestGridControl){
    $row.fontNonzeroBytes=[BitConverter]::ToUInt16($data,86)
    $row.palettePreservationChecks=[BitConverter]::ToUInt16($data,90)
    if($row.palettePreservationChecks -ne 3){throw "Guest palette preservation not proved: $id"}
    $row.fontPreservationChecks=[BitConverter]::ToUInt16($data,88)
    Write-Host "$id actual font nonzero=$($row.fontNonzeroBytes) preservation-checks=$($row.fontPreservationChecks) palette-checks=$($row.palettePreservationChecks)"
    if(!$row.fontNonzeroBytes -or $row.fontPreservationChecks -ne 3){throw "Actual nonempty guest glyph preservation not proved: $id"}
   }
   if($row.continuationCount -ne 3 -or $row.continuationErrors){throw "Detached BOP continuation state failed: $id"}
  }
  if($Keyboard){
   $row.keyboard=[BitConverter]::ToUInt16($data,$expectedBytes-2)
   $expectedKey=if($id -eq 'A'){if($ConsumeKeyboard){0}else{0x1e61}}else{0x3062}
   if($row.keyboard -ne $expectedKey){throw "Guest keyboard calibration failed: $id got $($row.keyboard)"}
  }
  [pscustomobject]$row
 })
 $rows|ConvertTo-Json|Set-Content "$task/runtime/vm-pair.json"
 if($BlockedKeyboard){
  $trace=@(Get-Content "$task/continuation-trace.txt")
  $blocked=-1;$sent=-1;$wake=-1;$target=''
  for($i=0;$i -lt $trace.Count;++$i){
   if($trace[$i] -match '^18 ([0-9a-f]{8}) ([0-9a-f]{8})$' -and $blocked -lt 0){
    if(!([Convert]::ToUInt32($Matches[2],16) -band 0x400)){throw 'Stimulus target was not blocked'}
    $blocked=$i;$target=$Matches[1]
   }
   if($blocked -ge 0 -and $sent -lt 0 -and $trace[$i] -match '^17 \w+ 00000041$'){$sent=$i}
   if($sent -ge 0 -and $trace[$i] -match '^19 '){$wake=$i;break}
  }
  if($blocked -lt 0 -or $sent -le $blocked -or $wake -le $sent){throw 'Missing blocked/stimulus/wake ordering'}
  $before=@()
  if($wake -gt $sent+1){$before=@($trace[($sent+1)..($wake-1)]|Where-Object{$_ -match "^1033 $target "})}
  if($ConsumeKeyboard){
   $accepted=@($trace|Where-Object{$_ -match '^22 '})
   if($accepted.Count -ne 2 -or $accepted[0] -ne "22 $target 0000001e" -or
      $accepted[1] -ne "22 $target 0000009e"){throw 'Exact target make/break consumption not proved'}
  }
  [ordered]@{target=$target;blocked=$blocked;stimulus=$sent;firstHostWake=$wake;
   irq1BeforeHostWake=$before.Count;traceSha256=(Get-FileHash "$task/continuation-trace.txt").Hash}|
   ConvertTo-Json|Set-Content "$task/runtime/blocked-keyboard.json"
 }
 # These INT1A intervals start at .timing AFTER commands and peer wake;
 # they are not VM lifetime intervals. Requiring their overlap rejects valid
 # scheduling. The blocked-peer signal requires both generations to be live.
 if($rows[0].vm -eq $rows[1].vm -or $rows[0].generation -eq $rows[1].generation -or
    @($rows|Where-Object {$_.pairCount -ne 1 -or $_.pairErrors}).Count){throw 'Distinct live VM identities and blocked-peer wake not proved'}
 'PASS two concurrent DOS VMs, distinct identity/generation, negative API and 320 callback identity/stack/register/flags checks'
 if($NestedCommand){
  foreach($id in 'A','B'){
   $cells=[IO.File]::ReadAllBytes("$dest/PATCH/$id.TXT")
   Copy-Item "$dest/PATCH/$id.TXT" "$task/runtime/$id.TXT"
   if($cells.Length -ne 4000){throw 'Nested text extent mismatch'}
   $chars=[byte[]]::new(2000)
   for($i=0;$i -lt 2000;$i++){$chars[$i]=$cells[2*$i]}
   $screen=[Text.Encoding]::ASCII.GetString($chars)
   [IO.File]::WriteAllText("$task/runtime/$id.text.txt",$screen)
   if($ReentrantCommand -and !$screen.Contains('command /c cmd /c mem')){throw "Reentrant command line absent in VM $id"}
   if($NestedRedirect){
    $redirect=Join-Path "$directoryRoot/V$id" 'result.txt'
    $text=[IO.File]::ReadAllText($redirect)
    Copy-Item -LiteralPath $redirect -Destination "$task/runtime/$id.redirect.txt"
    if($text -notmatch 'bytes total conventional memory' -or $text -notmatch 'largest executable program size' -or
       $text -notmatch "(?m)^$id\s*$"){throw "Nested redirect/append content invalid in VM $id"}
    if($screen -notmatch 'bytes total conventional memory'){throw "Nested pipe output absent in VM $id"}
   } elseif($screen -notmatch 'bytes total conventional memory' -or $screen -notmatch 'largest executable program size'){throw "Actual MEM output not returned to VM $id"}
  }
  'PASS nested MEM output actually returned to both guest VMs before EXIT'
 }
 if($UnreadInputImage){
  foreach($id in 'A','B'){foreach($round in '0','1'){
   $record="$dest/PATCH/INPUT-$id-$round.txt"
   $first=Get-Content $record -TotalCount 1
   $count=if($UnreadInputMode -eq 'sequence'){40}else{2}
   if($first -notmatch "synthetic-unread=$UnreadInputMode .*requested=$count written=$count .*error=0$"){throw "Native unread records not proved: $id/$round"}
   Copy-Item $record "$task/runtime/INPUT-$id-$round.txt"
  }}
  "PASS real native unread-input $UnreadInputMode returned through guest BIOS peek/read in both VMs and both command rounds"
 }
 if($RealCommand){'PASS real native 54:08 AH=0/AH=1 and 54:0A in both DOS VMs; 0A input is provider stimulus, full guest I/O untested'}
 elseif($Continuation){'PASS 54:08 AH=0/AH=1 and 54:0A in both actual DOS VMs; diagnostic results only'}
 if($Reuse){Confirm-Reuse $rows}

} finally {
 if(@(Get-ChildItem $source -Force -Recurse -File).Count -ne $original.Count){throw 'Original installation file inventory changed'}
 foreach($row in $original){if((Get-FileHash ($source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
 'Original Windows installation bytes verified unchanged'
}
