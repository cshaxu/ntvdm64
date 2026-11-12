[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('shell','hook','word','caret','clipreject','keyboard','media','vdd','optical')][string]$Case='hook',
 [switch]$LeaveHookInstalled,
 [string]$CompilerEnvironment,
 [ValidateSet(15,3,131,49153,49154)][int]$ClipboardFormat=15,
 [string]$DebugObserver,
 [switch]$OleRoundtrip,
 [string]$ProviderBuildRoot,
 [ValidateRange(0,25)][int]$OpticalDrive=5,
 [uint32]$ExpectedOpticalFrames=0,
 [switch]$OpticalNoMedia)
# Original USER/Shell boundary probes: shell, hooks, window words and caret.
# No Win16 Style modes or original guest build. Owned background invocation only.
$ErrorActionPreference='Stop'
if($LeaveHookInstalled -and $Case -ne 'hook'){throw 'LeaveHookInstalled requires hook case'}
if($OleRoundtrip -and ($Case -ne 'clipreject' -or $ClipboardFormat -notin @(49153,49154))){throw 'OLE roundtrip requires descriptor clipreject case'}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit";if($code){throw "$name failed"}
}
$runtime=$null;$scope=$null;$process=$null
try {
 Owned token whoami.exe '/groups /fo csv /nh' 15
 $label=[regex]::Match((Get-Content "$root/token.out" -Raw),'S-1-16-\d+').Value
 if($label -ne 'S-1-16-8192'){throw 'Require desktop-capable medium token'}
 @{pid=$PID;host=(Get-Process -Id $PID).Path;integrity=$label;script_sha256=(Get-FileHash $PSCommandPath).Hash}|ConvertTo-Json|Set-Content "$root/runner-context.json"
 $extension=if($Case -in @('vdd','optical')){'COM'}else{'EXE'}
 $source="$PSScriptRoot/../observation/win16_${Case}_probe.asm"
 if($Case -eq 'vdd'){$source="$PSScriptRoot/../observation/s44_vdd_bop.asm"}
 if($Case -eq 'word'){$source="$PSScriptRoot/../observation/win16_window_word_probe.asm"}
 $probe=if($Case -eq 'shell'){'SHLPROBE'}elseif($Case -eq 'word'){'WORDPROB'}elseif($Case -eq 'optical'){'CDPROBE'}elseif($Case -eq 'vdd'){'S44BOP'}elseif($Case -eq 'media'){'MEDPROBE'}elseif($Case -eq 'keyboard'){'KEYPROBE'}elseif($Case -eq 'caret'){'CARETPRB'}elseif($Case -eq 'clipreject'){'CLIPREJ1'}else{'CBTPROB'}
 $assembler=(Get-Command nasm.exe).Source
 $defines=if($Case -eq 'optical'){'-DOPTICAL_DRIVE='+$OpticalDrive+' '}elseif($Case -eq 'vdd'){'-DS41_ISOLATED '}elseif($LeaveHookInstalled){'-DHOOK_LEAVE_INSTALLED '}elseif($Case -eq 'clipreject'){('-DFORMAT='+$ClipboardFormat+' ')}else{''}
 if($OleRoundtrip){$defines+='-DOLE_ROUNDTRIP '}
 Owned assemble $assembler ($defines+'-f bin "'+$source+'" -o "'+$root+'/'+$probe+'.'+$extension+'"') 30
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 Save-IsolatedPackageManifest $runtime "$root/manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/$probe.$extension" "$tests/$probe.$extension"
 if($Case -eq 'vdd'){
  if(!$ProviderBuildRoot){throw 'Controlled VDD requires current provider build root'}
  Copy-Item "$ProviderBuildRoot/s44-bop-vdd.dll" "$tests/s44-bop-vdd.dll"
  Get-FileHash "$tests/s44-bop-vdd.dll","$repo/test/mvdm-host/s44_test_vdd.c"|Export-Csv -NoTypeInformation "$root/vdd-inputs.csv"
 }
 if($Case -eq 'media'){
  Copy-Item "$runtime/system/MMSYSTEM.DLL" "$tests/MMSYSTEM.DLL"
  if((Get-FileHash "$tests/MMSYSTEM.DLL").Hash -ne (Get-FileHash "$runtime/system/MMSYSTEM.DLL").Hash){throw 'Immutable MMSYSTEM copy differs'}
  $wave=[IO.MemoryStream]::new();$writer=[IO.BinaryWriter]::new($wave)
  try {
   $writer.Write([Text.Encoding]::ASCII.GetBytes('RIFF'));$writer.Write([uint32]836)
   $writer.Write([Text.Encoding]::ASCII.GetBytes('WAVEfmt '));$writer.Write([uint32]16)
   $writer.Write([uint16]1);$writer.Write([uint16]1);$writer.Write([uint32]8000);$writer.Write([uint32]8000)
   $writer.Write([uint16]1);$writer.Write([uint16]8);$writer.Write([Text.Encoding]::ASCII.GetBytes('data'));$writer.Write([uint32]800)
   for($i=0;$i -lt 800;$i++){$writer.Write([byte]128)}
   [IO.File]::WriteAllBytes("$tests/T478.WAV",$wave.ToArray())
  }finally{$writer.Dispose();$wave.Dispose()}
  Get-FileHash "$tests/MMSYSTEM.DLL"|Export-Csv -NoTypeInformation "$root/guest-library.csv"
 }
 if($OleRoundtrip){
  if(!$CompilerEnvironment -or !$ProviderBuildRoot){throw 'Require compiler environment and exact provider build root'}
  $map=(Resolve-Path "$ProviderBuildRoot/wow32.dll.map").Path
  $image=(Resolve-Path "$ProviderBuildRoot/wow32.dll").Path
  $manifest=Get-Content "$PackageRoot/product-system32-manifest.json" -Raw|ConvertFrom-Json
  $entry=@($manifest|Where-Object Name -eq 'WOW32.DLL')
  if($entry.Count -ne 1 -or $entry[0].InputSha256 -ne (Get-FileHash $image).Hash){throw 'Map build image differs from staged package input'}
  $mapText=Get-Content $map -Raw
  $symbol=[regex]::Match($mapText,'_wow_ole_convert_descriptor@8\s+([0-9A-Fa-f]{8})\s+f\b')
  $base=[regex]::Match($mapText,'Preferred load address is ([0-9A-Fa-f]+)')
  if(!$symbol.Success -or !$base.Success){throw 'Missing product conversion symbol/base'}
  $rva=[Convert]::ToUInt32($symbol.Groups[1].Value,16)-[Convert]::ToUInt32($base.Groups[1].Value,16)
  $nativeSource="$PSScriptRoot/../observation/wow_ole_descriptor_roundtrip.c"
  @('@echo off',('call "'+$CompilerEnvironment+'" -arch=x86 -host_arch=x64 >nul'),
   'if errorlevel 1 exit /b %errorlevel%',
   ('cl /nologo /LD /MT /O2 /DOLE_PROVIDER_RVA='+$rva+' "'+$nativeSource+'" /Fo"'+$root+'/ole.obj" /link /OUT:"'+$root+'/OLEHOST.DLL" /IMPLIB:"'+$root+'/ole.lib" /EXPORT:OleRoundtrip=_OleRoundtrip@12'),
   'exit /b %errorlevel%')|Set-Content "$root/compiler.cmd"
  Owned native-compile cmd.exe ('/d /c "'+$root+'/compiler.cmd"') 60
  Copy-Item "$root/OLEHOST.DLL" "$runtime/system32/OLEHOST.DLL"
  Copy-Item $map "$root/provider.map"
  Get-FileHash $nativeSource,"$root/OLEHOST.DLL",$map,$image|Export-Csv -NoTypeInformation "$root/native-inputs.csv"
  @{rva=$rva;input_sha256=$entry[0].InputSha256;package_sha256=$entry[0].Sha256}|ConvertTo-Json|Set-Content "$root/provider-binding.json"
 }

 if($Case -eq 'caret'){
  if(!$CompilerEnvironment -or !(Test-Path -LiteralPath $CompilerEnvironment)){throw 'Caret observer requires VS compiler environment'}
  $nativeSource="$PSScriptRoot/../observation/wow_caret_native_observer.c"
  @('@echo off',('call "'+$CompilerEnvironment+'" -arch=x86 -host_arch=x64 >nul'),
   'if errorlevel 1 exit /b %errorlevel%',
   ('cl /nologo /LD /O2 "'+$nativeSource+'" /Fo"'+$root+'/caret.obj" /link user32.lib /OUT:"'+$root+'/CARHOST.DLL" /IMPLIB:"'+$root+'/caret.lib" /EXPORT:CaretObserve=_CaretObserve@12'),
   'exit /b %errorlevel%')|Set-Content "$root/compiler.cmd"
  Owned native-compile cmd.exe ('/d /c "'+$root+'/compiler.cmd"') 60
  Copy-Item "$root/CARHOST.DLL" "$runtime/system32/CARHOST.DLL"
  Get-FileHash $nativeSource,"$root/CARHOST.DLL"|Export-Csv -NoTypeInformation "$root/native-inputs.csv"
 }

 if($Case -eq 'shell'){
  Owned child-assemble $assembler ('-DCHILD -f bin "'+$source+'" -o "'+$root+'/SHLCHILD.EXE"') 30
  Copy-Item "$root/SHLCHILD.EXE" "$tests/SHLCHILD.EXE"
 }
 Get-FileHash $source,$PSCommandPath,$Observer,$LogonHelper,$assembler,"$root/$probe.$extension"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Get-ChildItem "$runtime/system32" -File|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 Add-Type -Path (Resolve-Path $LogonHelper).Path
 $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($identity.Token)}finally{$identity.Dispose()}
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
 $info.EnvironmentVariables['S44_VDD_TRACE_PATH']="$root/vdd.txt"
 $info.EnvironmentVariables['TEMP']=$runtime;$info.EnvironmentVariables['TMP']=$runtime
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root/windows.txt"
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/$probe.$extension",'--observation-timeout-ms','45000')){$info.ArgumentList.Add($arg)}
 if($DebugObserver){
  # Observe the existing private-desktop runner's owned process tree. This is
  # diagnostic only; preserving exceptions is not a capability acceptance.
  $line=(@($info.ArgumentList)|ForEach-Object {'"'+$_+'"'}) -join ' '
  $observed=$info.FileName
  $info.FileName=(Resolve-Path $DebugObserver).Path
  $info.ArgumentList.Clear()
  foreach($arg in @('--exe',$observed,$line,"$root/exceptions.txt")){$info.ArgumentList.Add($arg)}
  Get-FileHash $DebugObserver,"$PSScriptRoot/../observation/wow_exception_observer.c"|Export-Csv -NoTypeInformation "$root/debugger-inputs.csv"
 }
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=$(if($DebugObserver){135}else{55})}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$process.WaitForExit($(if($DebugObserver){135000}else{55000}))){throw 'Owned guest observer deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer.exit"
 $report=Get-Content "$root/observer.txt" -Raw
 if($DebugObserver){throw "Diagnostic debugger run; inspect exceptions, not acceptance"}
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher did not complete normally'}
 if($Case -eq 'vdd'){
  $text=Get-Content "$root/observer.txt.console.txt" -Raw
  $trace=@(Get-Content "$root/vdd.txt")
  if($text -notmatch 'S44BOP OK' -or $text -match 'S44BOP FAIL'){throw 'Original BOP registration/dispatch failed'}
  foreach($marker in @('BOP_INIT','BOP_DISPATCH','HOOK_INSTALL_OK','HOOK_INVALID_OK','IO_INSTALL_OK','IO_INB','IO_OUTB','DETACH','HOOK_REMOVE_OK')){
   if($trace -notcontains $marker){throw "Missing controlled VDD marker: $marker"}
  }
  if($trace -match 'FAIL|UNEXPECTED'){throw 'Controlled VDD reported failure'}
  'PASS original BOP register/dispatch/deregister, hook install/remove and owned IO callbacks'|Set-Content "$root/result.txt"
  '0'|Set-Content "$root/exit.txt"
  return
 }
 Copy-Item "$tests/$probe.BIN" "$root/guest-report.bin"
 $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 if($Case -eq 'optical'){
  if($bytes.Length -ne 65 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'CDP1'){throw 'Bad optical report'}
  $names=@('count','first','version','isCd','signature','absoluteReadCarry','volumeStatus','diskStatus','readLongStatus','repeatStatus')
  $record=[ordered]@{}
  for($i=0;$i -lt 10;$i++){$record[$names[$i]]=[BitConverter]::ToUInt16($bytes,4+2*$i)}
  $record.volume=[BitConverter]::ToUInt32($bytes,24)
  $record.repeatVolume=[BitConverter]::ToUInt32($bytes,28)
  $record.diskInfo=@($bytes[32..38]);$record.letters=@($bytes[39..64])
  $record|ConvertTo-Json -Depth 4|Set-Content "$root/guest-report.json"
  if(!$record.count -or $record.version -ne 0x215 -or $record.isCd -ne 1 -or $record.signature -ne 0xadad -or $record.letters -notcontains $OpticalDrive){throw 'MSCDEX registration/enumeration mismatch'}
  if($record.absoluteReadCarry -ne 0 -or $record.readLongStatus -ne 0x8103){throw 'Original unsupported read contract mismatch'}
  $expectedStatus=if($OpticalNoMedia){0x810c}else{0x100}
  if($record.volumeStatus -ne $expectedStatus -or $record.diskStatus -ne $expectedStatus -or $record.repeatStatus -ne $expectedStatus -or $record.volume -ne $record.repeatVolume){throw 'Original optical TOC/volume/repeat failed'}
  if($OpticalNoMedia){
   if($record.volume -or @($record.diskInfo[1..6]|Where-Object {$_ -ne 0}).Count){throw 'No-media original result mismatch'}
  }elseif(!$record.volume){throw 'Mounted medium has no TOC volume'}
  if($ExpectedOpticalFrames -and $record.volume -ne $ExpectedOpticalFrames){throw 'Optical frame count mismatch'}
  $(if($OpticalNoMedia){'PASS original empty-drive enumeration, failed TOC with zero result and unsupported-read status; original error mapping retained'}else{'PASS original MSCDEX registration, enumeration, TOC/volume and repeated query; unsupported data reads retain original failure'})|Set-Content "$root/result.txt"
 }elseif($Case -eq 'shell'){
        if($bytes.Length -ne 42){throw 'Expected three 14-byte Shell records'}
        $records=@()
        for($i=0;$i -lt 3;$i++){
            $o=14*$i
            if([Text.Encoding]::ASCII.GetString($bytes,$o,4) -ne 'SHL1'){throw 'Bad Shell record'}
            $records += [pscustomobject]@{phase=[BitConverter]::ToUInt16($bytes,$o+4);
                errors=[BitConverter]::ToUInt16($bytes,$o+6);success=[BitConverter]::ToUInt16($bytes,$o+8);
                failure=[BitConverter]::ToUInt16($bytes,$o+10);task=[BitConverter]::ToUInt16($bytes,$o+12)}
        }
        $records | ConvertTo-Json | Set-Content "$root/guest-report.json"
        if(($records.phase -join ',') -ne '0,1,2' -or @($records|Where-Object errors -ne 0).Count -or
                $records[-1].success -le 32 -or $records[-1].failure -gt 32){throw 'Shell result/continuation failed'}
        if(!(Test-Path "$tests/SHLCHILD.BIN")){throw 'Shell child did not run'}
        Copy-Item "$tests/SHLCHILD.BIN" "$root/child-report.bin"
        $child=[IO.File]::ReadAllBytes("$root/child-report.bin")
        if($child.Length -ne 14 -or [Text.Encoding]::ASCII.GetString($child,0,4) -ne 'SHL1' -or
                [BitConverter]::ToUInt16($child,4) -ne 3){throw 'Shell child did not finish report'}
        $parentTask=$records[0].task
        $childTask=[BitConverter]::ToUInt16($child,12)
        if(!$parentTask -or @($records|Where-Object task -ne $parentTask).Count -or
                !$childTask -or $childTask -eq $parentTask){
            throw 'Shell parent task changed across success/failure or child identity is not distinct'
        }
        'PASS actual guest ShellExecute success/failure, child execution and parent continuation'|Set-Content "$root/result.txt"
 }elseif($Case -eq 'media'){
  if($bytes.Length -ne 124 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'MED1'){throw 'Bad media report'}
  $values=@(0..10|ForEach-Object {[BitConverter]::ToUInt16($bytes,4+2*$_)})
  @{values=$values;errorText=@($bytes[26..91]);wave=@(0..7|ForEach-Object {[BitConverter]::ToUInt16($bytes,92+2*$_)});midi=@(0..2|ForEach-Object {[BitConverter]::ToUInt16($bytes,108+2*$_)});mciOpen=[BitConverter]::ToUInt32($bytes,114);mciClose=[BitConverter]::ToUInt32($bytes,118);mciDevice=[BitConverter]::ToUInt16($bytes,122)}|ConvertTo-Json -Depth 4|Set-Content "$root/guest-report.json"
  if($values[5] -ne 2 -or $values[6] -ne 2 -or $values[7] -ne 257 -or !$values[8]){throw 'Original multimedia failure contract mismatch'}
  if($values[9] -or $values[10] -or $bytes[26] -eq 165 -or $bytes[26] -eq 0 -or $bytes[90] -ne 165 -or $bytes[91] -ne 165){throw 'Query/error buffer or handle ownership mismatch'}
  if($values[4] -eq 0){
   foreach($offset in @(92,94,96,100,102,104)){if([BitConverter]::ToUInt16($bytes,$offset) -ne 0){throw 'Silent wave operation failed'}}
   $flags=[BitConverter]::ToUInt16($bytes,98)
   if(($flags -band 3) -ne 3 -or ($flags -band 16)){throw 'Silent wave header not completed/prepared or still queued'}
  }
  if($values[2] -gt 0 -and ([BitConverter]::ToUInt16($bytes,108) -ne 0 -or [BitConverter]::ToUInt16($bytes,110) -ne 0)){throw 'Available MIDI open/close failed'}
  if([BitConverter]::ToUInt32($bytes,114) -ne 0 -or [BitConverter]::ToUInt32($bytes,118) -ne 0 -or ![BitConverter]::ToUInt16($bytes,122)){throw 'MCI waveaudio open/close unavailable or failed'}
  'PASS real guest multimedia queries/errors, available silent-wave completion/retirement, MIDI open/close and MCI waveaudio open/close; no input capture claim' |Set-Content "$root/result.txt"
 }elseif($Case -eq 'keyboard'){
  if($bytes.Length -ne 144 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'KBP1'){throw 'Bad keyboard report'}
  $values=@(0..11|ForEach-Object {[BitConverter]::ToUInt16($bytes,4+2*$_)})
  @{returns=$values;payload=@($bytes[28..107]);edgeReturns=@(0..4|ForEach-Object {[BitConverter]::ToUInt16($bytes,108+2*$_)});edgePayload=@($bytes[118..143])}|ConvertTo-Json -Depth 4|Set-Content "$root/guest-report.json"
  foreach($offset in @(28,36)){
   if(($bytes[$offset..($offset+7)] -join ',') -ne '65,98,57,0,165,165,165,165'){throw 'String conversion/terminator/guard mismatch'}
  }
  foreach($offset in @(44,52)){
   if(($bytes[$offset..($offset+7)] -join ',') -ne '65,0,90,57,165,165,165,165'){throw 'Binary conversion count or guard mismatch'}
  }
  if(@($bytes[60..67]|Where-Object {$_ -ne 165}).Count){throw 'Zero-count conversion wrote output'}
  if($values[6] -ne 32 -or !$values[7] -or !$values[8] -or !$values[9] -or $values[10] -lt 1 -or $values[10] -gt 31 -or $values[11] -ne 1){throw 'Keyboard scalar/name/ToAscii mismatch'}
  if($bytes[68+$values[10]] -ne 0 -or $bytes[100] -ne 165 -or $bytes[101] -ne 165 -or $bytes[102] -ne 32 -or $bytes[106] -ne 165 -or $bytes[107] -ne 165){throw 'Keyboard name/ToAscii buffer mismatch'}
  if($values[5] -ne $values[8]){throw 'Space OEM scan disagrees with virtual-key scan'}
  foreach($offset in @(108,110,112,114,116)){if([BitConverter]::ToUInt16($bytes,$offset) -ne 0){throw 'Invalid query/short name/noncharacter return mismatch'}}
  foreach($offset in @(118,122,130,134)){
   if(($bytes[$offset..($offset+3)] -join ',') -ne '0,165,165,165'){throw 'Empty string or one-byte name terminator/guard mismatch'}
  }
  if(@($bytes[126..129]|Where-Object {$_ -ne 165}).Count -or @($bytes[142..143]|Where-Object {$_ -ne 165}).Count){throw 'Zero-count or noncharacter result guard overwritten'}
  'PASS real Win16 keyboard conversions, empty strings, invalid queries, short names, zero count, buffer guards, scalar queries and space ToAscii; layout-specific behavior not inferred'|Set-Content "$root/result.txt"
 }elseif($Case -eq 'clipreject'){
  if($bytes.Length -ne 18 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'CRJ1'){throw 'Bad Clipboard rejection report'}
  $values=@(0..6|ForEach-Object {[BitConverter]::ToUInt16($bytes,4+2*$_)})
  $values|ConvertTo-Json|Set-Content "$root/guest-report.json"
  if($values[1] -ne $(if($OleRoundtrip){15}else{0}) -or $values[2] -ne 1 -or $values[3] -ne 0 -or !$values[4] -or !$values[5] -or !$values[6]){throw 'Rejected structured format lost guest ownership or cleanup'}
  $(if($OleRoundtrip){'PASS actual product OLE forward/reverse payload conversion, invalid input/direction, guest input and output ownership'}else{'PASS original Win16 rejected structured-format request, guest bytes retained and caller cleanup'})|Set-Content "$root/result.txt"
 }elseif($Case -eq 'caret'){
  if($bytes.Length -ne 24 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'CAR1'){throw 'Bad caret report'}
  $values=@(0..9|ForEach-Object {[BitConverter]::ToUInt16($bytes,4+2*$_)})
  $values|ConvertTo-Json|Set-Content "$root/guest-report.json"
  if(@($values[0..5]|Where-Object {$_ -ne 1}).Count -or $values[6] -ne 7 -or $values[7] -ne 9 -or !$values[8] -or !$values[9]){throw 'Original caret/native owner, visibility, position or cleanup mismatch'}
  'PASS original Win16 solid/gray caret owner, position, show/hide, replacement and explicit destruction'|Set-Content "$root/result.txt"
 }elseif($Case -eq 'word'){
  if($bytes.Length -ne 22 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'WWP1'){throw 'Bad window-word report'}
  $record=[ordered]@{};$names=@('created_value','previous','inside_read','readback','destroyed','unregistered','class_extra','second_previous','second_read')
  for($i=0;$i -lt $names.Count;$i++){$record[$names[$i]]=[BitConverter]::ToUInt16($bytes,4+2*$i)}
  $record|ConvertTo-Json|Set-Content "$root/guest-report.json"
  if($record.created_value -ne 0x5a39 -or $record.previous -ne 0 -or $record.inside_read -ne 0x5a39 -or $record.readback -ne 0x5a39 -or !$record.destroyed -or !$record.unregistered -or $record.class_extra -ne 2 -or $record.second_previous -ne 0x5a39 -or $record.second_read -ne 0x6b4a){throw 'Create parameter/window-word contract failed'}
  'PASS original USER16 creation parameter and two-byte window-word persistence'|Set-Content "$root/result.txt"
 }else{
    if($bytes.Length -ne 600){throw "Expected four22-byte phase records and512-byte class history; got $($bytes.Length)"}
    @(0..15|ForEach-Object {[Text.Encoding]::ASCII.GetString($bytes,88+32*$_,32).Split([char]0)[0]})|
        ConvertTo-Json|Set-Content "$root/observed-classes.json"
    $records=@()
    $names=@('phase','errors','created','queried','nested','vetoed','destroyed','unhooked','freed')
    for($record=0;$record -lt 4;$record++){
        $offset=22*$record
        if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'CBT1'){throw 'Bad guest record signature'}
        $row=[ordered]@{}
        for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,$offset+4+2*$i)}
        $records+=[pscustomobject]$row
    }
    $records|ConvertTo-Json|Set-Content "$root/guest-report.json"
    if(($records.phase -join ',') -ne '0,1,2,4' -or @($records|Where-Object errors -ne 0).Count){throw 'Guest phase/failure mismatch'}
    $last=$records[-1]
    if($last.created -ne 4 -or $last.queried -ne 4 -or $last.nested -ne 1 -or
            $last.vetoed -ne 1 -or $last.destroyed -ne 3 -or $last.unhooked -ne [int](!$LeaveHookInstalled) -or $last.freed -ne [int](!$LeaveHookInstalled)){
        throw 'Guest callback/nesting/veto/recovery/cleanup assertions failed'
    }
    if($LeaveHookInstalled){'PASS real Win16 CBT left installed until task exit; normal launcher completion (slot state requires separate owner proof)'|Set-Content "$root/result.txt"}
    else{'PASS real Win16 CBT query/nested creation/veto/recovery/unhook and launcher completion'|Set-Content "$root/result.txt"}

 }
 '0'|Set-Content "$root/exit.txt"
} catch { $_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw }
finally {
 if($process){if(!$process.HasExited){taskkill.exe /PID $process.Id /T /F | Out-Null};$process.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally {
  if($tests -and $probe -and (Test-Path "$tests/$probe.BIN")){Copy-Item "$tests/$probe.BIN" "$root/guest-report.bin" -Force}
  if($tests -and (Test-Path "$tests/SHLCHILD.BIN")){Copy-Item "$tests/SHLCHILD.BIN" "$root/child-report.bin" -Force}
  if($runtime){Remove-PhysicalRuntimeStage $runtime}
 }
}
