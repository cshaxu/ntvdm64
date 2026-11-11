[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$InstanceDriver,
 [string]$InterruptObserver,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [switch]$MouseMenu,
 [switch]$GuestUserInput,
 [switch]$SaveWitness,
 [switch]$ReadWriteWitness,
 [string]$FrontendMouseProbeRoot,
 [switch]$FullscreenTransitions,
 [switch]$FrontendKeyboard,
 [switch]$Windows31,
 [switch]$BlockingDosFirst,
 [string]$ThreadSnapshot,
 [ValidateSet(0,1)][int]$DosOnly=0,
 [Alias('Drivers')][ValidateSet(0,1)][int]$Multitask=1,
 [string]$DismissWelcomeTitleHex
)
$ErrorActionPreference='Stop'
if(($DosOnly -or !$Multitask) -and !$GuestUserInput){throw 'Profile variations require the production controller'}
$blockingEditor=(!$DosOnly -and !$Multitask)
if($blockingEditor -and (!$FrontendMouseProbeRoot -or !$ReadWriteWitness)){throw 'Blocking editor acceptance requires frontend input and read/write witnesses'}
if($FrontendKeyboard -and !$FrontendMouseProbeRoot){throw 'Frontend keyboard requires frontend probe tools'}
if($FullscreenTransitions -and !$FrontendMouseProbeRoot){throw 'Fullscreen transitions require frontend input/frame probes'}
if($FrontendMouseProbeRoot -and !$GuestUserInput){throw 'Frontend mouse requires the production guest controller'}
if($FrontendMouseProbeRoot){$FrontendMouseProbeRoot=(Resolve-Path $FrontendMouseProbeRoot).Path}
if($GuestUserInput -and ($MouseMenu -or !$SaveWitness)){throw 'GuestUserInput requires SaveWitness and does not support MouseMenu'}
if($ReadWriteWitness -and (!$GuestUserInput -or !$SaveWitness)){throw 'ReadWriteWitness requires GuestUserInput and SaveWitness'}
if(!$GuestUserInput -and !$InterruptObserver){throw 'Diagnostic editor witness requires InterruptObserver'}
if($DismissWelcomeTitleHex){
 if($DismissWelcomeTitleHex -notmatch '^(?:[0-9a-fA-F]{2}){1,63}$'){throw 'Welcome title must be guest ANSI bytes in hexadecimal'}
 for($i=0;$i -lt $DismissWelcomeTitleHex.Length;$i+=2){if($DismissWelcomeTitleHex.Substring($i,2) -eq '00'){throw 'Welcome title cannot contain a zero byte'}}
}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
# This fixture sends diagnostic-only BOP markers for input and observations.
# Reject an ordinary host before staging a guest rather than timing out.
$hostImage=[IO.File]::ReadAllBytes((Join-Path $PackageRoot 'system32/ntvdm.exe'))
if(!$GuestUserInput -and (![Text.Encoding]::ASCII.GetString($hostImage).Contains('MVDM_SCREEN_TRACE') -or
   ![Text.Encoding]::Unicode.GetString($hostImage).Contains('MVDM_REAL_COMMAND'))){
 throw 'Editor fixture requires the diagnostic real-command NTVDM; use a production-only witness for the released package'
}
$copyArgs=@{DriverImage=$DriverImage;InstanceDriver=$InstanceDriver;InstallSource=$InstallSource;InstallRoot=$InstallRoot;OutputRoot=$OutputRoot;MouseDriver="$repo/assets/release/MOUSE31.DRV"}
if($InterruptObserver){$copyArgs.InterruptObserver=$InterruptObserver}
$family=if($Windows31){"Win31"}else{"Win95"}
$copy=& "$PSScriptRoot/New-${family}ProbeCopy.ps1" @copyArgs
$task=$copy.Task; $source=$copy.Source; $dest=$copy.Destination; $original=$copy.Original
try {
$enc=[Text.Encoding]::GetEncoding(28591)
# Exercise the shipped dynamic launcher; the shim only suppresses launch while
# preparing the profile. The observer below runs its real generated PIF.
$launcher=if($Windows31){"win31-setup"}else{"win95-launch"}
Copy-Item "$repo/tools/$launcher/WIN.CMD" "$dest/PATCH/WIN.CMD"
Copy-Item "$repo/assets/release/PIF.EXE" "$dest/PATCH/PIF.EXE"
& cmd.exe /d /c "`"$dest/PATCH/WIN.CMD`" --install"
if($LASTEXITCODE){throw 'Editor launcher setup failed'}
$selector=Join-Path $task 'selector';New-Item -ItemType Directory $selector|Out-Null
[IO.File]::WriteAllText("$selector/run16.cmd","@echo off`r`nexit /b 0`r`n")
$choices=if($Windows31){"2`r`n$DosOnly`r`n$Multitask`r`n"}else{"$DosOnly`r`n$Multitask`r`n"}
[IO.File]::WriteAllText("$task/select-input.txt",$choices)
$selectPath=$env:PATH;$env:PATH="$selector;$selectPath"
try {
 & cmd.exe /d /c "`"$dest/PATCH/WIN.CMD`" < `"$task/select-input.txt`"" *> "$task/select.log"
 if($LASTEXITCODE){throw 'Editor launcher profile failed'}
} finally {$env:PATH=$selectPath}
if(((Get-Content "$dest/PATCH/CONFIG.NT" -Raw) -match '(?im)^dosonly\s*$') -ne [bool]$DosOnly){throw 'Selected DOSONLY mismatch'}
$selectedIni=Get-Content "$dest/SYSTEM.INI" -Raw
foreach($driver in 'INSIGNIA.386','NTVDM64.386'){
 $enabled=[regex]::IsMatch($selectedIni,'(?im)^device='+[regex]::Escape("$dest\PATCH\$driver")+'\s*$')
 if($enabled -ne [bool]($driver -eq 'INSIGNIA.386' -or $Multitask)){throw "Selected driver mismatch: $driver"}
}
Copy-Item -LiteralPath "$dest/SYSTEM.INI" -Destination "$task/selected-system.ini"

Copy-Item -LiteralPath "$PackageRoot/system32/COMMAND.COM" -Destination "$dest/PATCH/COMMAND.COM"
if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'COMMAND copy mismatch'}
if($SaveWitness){foreach($name in 'A.TXT','B.TXT'){
 $seed=if($ReadWriteWitness){if($name -eq 'A.TXT'){'seedA'}else{'seedB'}}else{''}
 [IO.File]::WriteAllBytes("$dest/PATCH/$name",[Text.Encoding]::ASCII.GetBytes($seed))
}}
foreach($id in 'A','B'){
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "Prompt probe $id" --program "$dest\PATCH\COMMAND.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'PIF failed'}
 & "$PSScriptRoot/Set-Win95ProbePifMemory.ps1" -Path "$dest/PATCH/$id.PIF"
}
$ini="$dest/WIN.INI";$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))
$asm="%define MOUSE_MENU $([int][bool]($MouseMenu -or $GuestUserInput))`n%define DOSONLY 0`n%define SAVE_WITNESS $([int][bool]$SaveWitness)`n%define FIRST_DOS 0`n%define FIRST_DOS_BEFORE_SECOND 0`n"+(Get-Content "$repo/tests/observation/win31_startup_contract.asm" -Raw)
$actions=Get-Content "$repo/test/integration/ntvdm64-386/editor-actions.inc" -Raw
$routines=Get-Content "$repo/test/integration/ntvdm64-386/editor-routines.inc" -Raw
if($GuestUserInput){
 $actions=$actions.Replace('call wait_readers','call production_wait').Replace('call wait_editors','call production_wait').Replace('call wait_complete','call production_wait')
 $saveSequence="mov si,events_text_b-data`ncall type_events`ncall alt_file`nmov si,events_s-data`ncall type_events"
 if(!$actions.Contains($saveSequence)){throw 'DOS save sequence changed'}
 $actions=$actions.Replace($saveSequence,"mov si,events_text_b-data`ncall type_events`ncall mouse_file`nmov word [mouse_half_row-data],9`ncall mouse_cell")
 $inject=@'
inject_key:
pushad
mov cx,ax
and ax,0xff
cmp ch,2
jne .make
or ah,0x80
.make:
xor si,si
xor di,di
api 2,289                       ; USER keybd_event in focused guest VM
popad
ret
production_wait:
push cx
mov cx,20
.again:
call settle
loop .again
pop cx
ret
wait_complete:
'@
 $routines=[regex]::Replace($routines,'(?s)inject_key:\s*.*?wait_complete:',[System.Text.RegularExpressions.MatchEvaluator]{param($m) $inject},1)
 $mouse=@'
mouse_input:
pushad
mov dx,ax
and dx,1
mov [production_pending_button-data],dx
xor bx,bx
xor cx,cx
xor ax,ax
cmp word [mouse_delta-data],0
jnz .move
cmp word [mouse_delta-data+2],0
jz .buttons
.move:
movzx eax,word [mouse_point-data]
imul eax,eax,65535
movzx edi,word [mouse_extent-data]
dec edi
jz fatal
xor edx,edx
div edi
mov bx,ax
movzx eax,word [mouse_point-data+2]
imul eax,eax,65535
movzx edi,word [mouse_extent-data+2]
dec edi
jz fatal
xor edx,edx
div edi
mov cx,ax
mov ax,0x8001                  ; absolute guest desktop coordinates
mov dx,[production_pending_button-data]
.buttons:
cmp dx,[production_buttons-data]
je .dispatch
mov [production_buttons-data],dx
test dx,dx
jz .up
or ax,2
jmp .dispatch
.up:
or ax,4
.dispatch:
test ax,ax
jz .done
mov dx,2
xor si,si
xor di,di
api 2,299                       ; guest USER mouse_event
.done:
popad
ret
'@
 $routines=[regex]::Replace($routines,'(?s)mouse_input:\s*.*\z',[System.Text.RegularExpressions.MatchEvaluator]{param($m) $mouse})
}
if($DosOnly){
 # The common action template exercises two working editors. For a guest-only
 # profile both are DOS EDIT; native rejection is a separate contract witness.
 $actions=$actions.Replace('mov word [mouse_half_row-data],15','mov word [mouse_half_row-data],19')
 $actions=$actions.Replace("mov si,events_text_a-data","mov si,events_esc-data`ncall type_events`nmov si,events_text_a-data")
 $saveNative="mov ax,0x111`nmov bx,0x1d`ncall inject_key`nmov si,events_s-data`ncall type_events`nmov ax,0x211`nmov bx,0x1d`ncall inject_key"
 if(!$actions.Contains($saveNative)){throw 'Native save action changed'}
 $actions=$actions.Replace($saveNative,"call alt_file`nmov word [mouse_half_row-data],9`ncall mouse_cell")
}
if($FullscreenTransitions){
 foreach($marker in 'mov si,events_text_a-data','mov si,events_text_b-data'){
  $actions=$actions.Replace($marker,"mov ax,0x8000`ncall mouse_input`ncall target_prompt`n"+$marker)
 }
}
if($blockingEditor){
 $actions=@'
mov word [which-data],title_a-data
call target_prompt
call production_wait
call target_prompt
mov si,events_a-data
call type_events
mov ax,0x2000
call mouse_input
call target_prompt
mov si,events_exit-data
call type_events
call wait_gone
mov word [which-data],title_b-data
call target_prompt
call production_wait
call target_prompt
mov si,events_b-data
call type_events
call production_wait
call target_prompt
mov ax,0x2001
call mouse_input
call target_prompt
mov si,events_exit-data
call type_events
call wait_gone
'@
 if($BlockingDosFirst){
  $split=$actions.IndexOf('mov word [which-data],title_b-data')
  if($split -lt 0){throw 'Missing DOS editor action block'}
  $actions=$actions.Substring($split)+"`n"+$actions.Substring(0,$split)
 }
}
$asm=$asm.Replace('push word 0                      ; DWORD reserved',$actions+"`n"+'push word 0                      ; DWORD reserved')
if($routines -notmatch '(?s)api 2,59\s+\.wait_active:\s+call settle\s+api 2,60\s+[^\r\n]*[\r\n]+cmp ax,\[active-data\]\s+jne \.wait_active'){
 throw 'Shared editor routine must wait for the selected prompt to become active'
}
if($DismissWelcomeTitleHex){
 $routines=$routines.Replace("target_prompt:`n","target_prompt:`ncall dismiss_welcome`n")
 $routines=$routines.Replace(".wait_active:`ncall settle`n", ".wait_active:`ncall settle`ncall dismiss_welcome`n")
 $routines=$routines.Replace("mouse_cell:`n","mouse_cell:`ncall dismiss_welcome`n")
 $routines+=@'

; Close only the explicitly named guest welcome window in this test copy.
dismiss_welcome:
pushad
push word 0
push word 0
push ds
push word welcome_title-data
api 2,50                         ; FindWindow by exact title
or ax,ax
jz .done
push ax
push word 0x10                  ; WM_CLOSE
push word 0
push word 0
push word 0
api 2,110                       ; PostMessage
call settle
.done:
popad
ret
'@
}
if(($MouseMenu -or $GuestUserInput) -and !$Windows31){
 $rect="push word [active-data]`npush ds`npush word mouse_rect-data"
 $point="push word [active-data]`npush ds`npush word mouse_point-data"
 if(!$routines.Contains($rect) -or !$routines.Contains($point)){throw 'Mouse geometry insertion point missing'}
 $routines=$routines.Replace($rect,"call mouse_find_text`npush word [mouse_hwnd-data]`npush ds`npush word mouse_rect-data")
 $routines=$routines.Replace($point,"push word [mouse_hwnd-data]`npush ds`npush word mouse_point-data")
 $routines+=@'

; Win95 puts a toolbar in the DOS window's parent client area. Locate the
; actual text child rather than assuming the Win3.1 parent geometry.
mouse_find_text:
push word [active-data]
push word 5                       ; GW_CHILD
api 2,262
.next:
test ax,ax
jz fatal
mov [mouse_hwnd-data],ax
push ax
push ds
push word mouse_class-data
push word 64
api 2,58                        ; GetClassName
cmp dword [mouse_class-data],0x47797474 ; ttyG
jne .other
cmp word [mouse_class-data+4],0x6172   ; ra
jne .other
cmp word [mouse_class-data+6],0x0062   ; b and terminator
je .found
.other:
push word [mouse_hwnd-data]
push word 2                       ; GW_HWNDNEXT
api 2,262
jmp .next
.found:
ret
'@
}
if($FrontendMouseProbeRoot){
 if($FrontendKeyboard){
  $routines=[regex]::Replace($routines,'(?s)inject_key:.*?production_wait:',"inject_key:`njmp frontend_key`nproduction_wait:")
 }
 $mouseRequest=Get-Content "$PSScriptRoot/frontend-mouse-request.inc" -Raw
 # Replace only the existing mouse_input body; later geometry helpers remain.
 $routines=[regex]::Replace($routines,'(?s)mouse_input:.*?(?=\r?\n; Close only|\r?\n; Win95 puts|\z)',[System.Text.RegularExpressions.MatchEvaluator]{param($m) $mouseRequest})
 $asm=$asm.Replace('data_end:',"frontend_request: times 4 dw 0`nfrontend_ack: dw 0`nfrontend_request_name: db '$dest\PATCH\MREQ.BIN',0`nfrontend_ack_name: db '$dest\PATCH\MACK.BIN',0`ndata_end:")
}
$asm=$asm.Replace('code_end:',$routines+"`ncode_end:")
$asm=$asm.Replace('data_end:',"mouse_half_row: dw 0`nmouse_rect: times 4 dw 0`nmouse_point: times 2 dw 0`nmouse_actual: times 2 dw 0`nvmd_api: dd 0`nvmd_version: dw 0`nvmd_reply: times 50 db 0`nvmd_query_error: dw 0`nmouse_scroll: times 2 dw 0`nmouse_delta: times 2 dw 0`nmouse_extent: times 2 dw 0`nmouse_receipt_name: db '$dest\PATCH\MOUSE'`nmouse_receipt_digit: db '0'`ndb '.BIN',0`ntimer_id: dw 0`ntimer_msg: times 18 db 0`ncompleted_count: dd 0`nwanted_count: dd 0`nread_count: dw 0`neditors_name: db '$dest\PATCH\EDITORS.BIN',0`nreaders_name: db '$dest\PATCH\READERS.BIN',0`ncompletion_name: db '$dest\PATCH\DONE.BIN',0`ninput_api: dd 0`nstamp: dw 0`nactive: dw 0`nwhich: dw 0`ntitle_a: db 'Prompt probe A',0`ntitle_b: db 'Prompt probe B',0`n"+(Get-Content "$repo/test/integration/ntvdm64-386/editor-events.inc" -Raw)+"`ndata_end:")
if($DosOnly){
 $dosEvents=[regex]::Match($asm,'(?m)^events_b:.*$').Value.Replace('events_b:','events_a:').Replace('SAVE_ARG_B','SAVE_ARG_A')
 if(!$dosEvents){throw 'Missing DOS editor event sequence'}
 $asm=[regex]::Replace($asm,'(?m)^events_a:.*$',[System.Text.RegularExpressions.MatchEvaluator]{param($m) $dosEvents})
}
if($blockingEditor){
 # Native shell-out freezes the guest controller; DOS reentry does not.
 # Keep DOS Enter in the guest sequence and hand off after guest progress.
 $asm=$asm.Replace('SAVE_ARG_A 0x1c0d,0','SAVE_ARG_A 0')
}
if($GuestUserInput){$asm=$asm.Replace('data_end:',"production_buttons: dw 0`nproduction_pending_button: dw 0`ndata_end:")}
if($MouseMenu -or $GuestUserInput){$asm=$asm.Replace('data_end:',"mouse_hwnd: dw 0`nmouse_class: times 64 db 0`ndata_end:")}
if($DismissWelcomeTitleHex){
 $titleBytes=@(for($i=0;$i -lt $DismissWelcomeTitleHex.Length;$i+=2){'0x'+$DismissWelcomeTitleHex.Substring($i,2)}) -join ','
 $asm=$asm.Replace('data_end:',"welcome_title: db $titleBytes,0`ndata_end:")
}
[IO.File]::WriteAllText("$task/controller.asm",$asm,[Text.UTF8Encoding]::new($false))
& nasm -f bin "$task/controller.asm" -o "$task/controller.exe"
if($LASTEXITCODE){throw 'Controller assembly failed'}

$bytes=[IO.File]::ReadAllBytes("$task/controller.exe")
foreach($field in @(@('@W31_BOOT_PATH@',"$dest\PATCH\BOOT.BIN"),@('@W31_READY_PATH@',"$dest\PATCH\READY.BIN"))){
 $marker=[Text.Encoding]::ASCII.GetString($bytes).IndexOf($field[0]);if($marker -lt 0){throw "Missing controller marker $($field[0])"}
 $value=[Text.Encoding]::ASCII.GetBytes($field[1]);[Array]::Clear($bytes,$marker,128);[Array]::Copy($value,0,$bytes,$marker,$value.Length)
}
[IO.File]::WriteAllBytes("$task/controller.exe",$bytes)
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS','PSExecutionPolicyPreference')
$nasm=(Get-Command nasm.exe -CommandType Application | Select-Object -First 1).Source
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH=(Split-Path $nasm)+';C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$env:MVDM_COMPLETION_COUNT="$dest/PATCH/DONE.BIN"
$env:MVDM_PROMPT_READERS="$dest/PATCH/READERS.BIN"
$env:MVDM_SCREEN_TRACE="$task/screens.txt"
$env:MVDM_EDITORS_READY="$dest/PATCH/EDITORS.BIN"
$env:MVDM_REAL_COMMAND='1'
$env:MVDM_TERMINATE_VDM_REPORT_PATH="$task/termination.txt"
$env:MVDM_CONTINUATION_TRACE="$task/trace.txt"

try {

 . "$repo/tests/observation/isolated_package_cleanup.ps1"
 $stage=$null;$scope=$null;$process=$null
 New-Item -ItemType Directory "$task/runtime"|Out-Null
 Copy-Item "$task/controller.exe" "$dest/PATCH/VMTEST.EXE"
 $s=$enc.GetString([IO.File]::ReadAllBytes("$dest/WIN.INI"))
 $s=[regex]::Replace($s,'(?im)^run=[^\r\n]*',"run=$dest\PATCH\VMTEST.EXE")
 [IO.File]::WriteAllBytes("$dest/WIN.INI",$enc.GetBytes($s))
 try {
  $stage=New-PhysicalRuntimeStage $PackageRoot -FreshPackageIdentity
  $stage=[IO.Path]::GetFullPath($stage)
  $scope=New-IsolatedPackageScope $stage
  $env:MVDM_OBSERVER_SERIAL_NTVDM="$stage\system32\ntvdm.exe"
  $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
  $env:MVDM_OBSERVER_OWN_JOB='1'
  if($GuestUserInput){$env:MVDM_OBSERVER_CAPTURE_WINDOW='1'}
  $env:PATH="$stage\system32;C:\Windows\System32;C:\Windows"
  # Per-key frontend round trips also need the long interaction budget.
  $timeout=if($FullscreenTransitions -or $FrontendKeyboard){180000}else{90000}
  $process=Start-Process $Observer -ArgumentList @("$stage/system32/run16.exe",$dest,"$task/runtime/observer.txt","$dest/PATCH/WIN.PIF",'--observation-timeout-ms',"$timeout") -WindowStyle Hidden -PassThru
  @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');stage=$stage}|ConvertTo-Json|Set-Content "$task/runtime/process.json"
  if($FrontendMouseProbeRoot){
   . "$PSScriptRoot/Invoke-EditorFullscreenProbe.ps1"
   $sequence=0;$keyModifiers=0;$deadline=[DateTime]::UtcNow.AddMilliseconds($timeout+15000)
   while(!$process.HasExited -and [DateTime]::UtcNow -lt $deadline){
    $request=$null
    try {if(Test-Path "$dest/PATCH/MREQ.BIN"){$request=[IO.File]::ReadAllBytes("$dest/PATCH/MREQ.BIN")}} catch [IO.IOException] {}
    if($request -and $request.Length -eq 8){
     $next=[BitConverter]::ToUInt16($request,0)
     if($next -eq $sequence+1){
      $frontend=@(Get-CimInstance Win32_Process -Filter "Name='ntcon.exe'"|Where-Object {$_.ExecutablePath -ieq "$stage\system32\ntcon.exe"})
      if($frontend.Count -ne 1){
       $process.Refresh()
       if($process.HasExited){throw 'Owned observer ended before frontend input completed; inspect observer.txt'}
       throw 'Expected one owned NTCON for mouse input'
      }
      $dx=[BitConverter]::ToInt16($request,2);$dy=[BitConverter]::ToInt16($request,4);$buttons=[BitConverter]::ToUInt16($request,6)
      if(($buttons -band 0x4000) -ne 0){
       $down=$buttons -band 1
       $keys=@{13=131073;8=131074;27=196609;9=196610;16=196611;17=196612;18=196613;186=59;190=46;220=92}
       $key=if($keys.ContainsKey([int]$dy)){$keys[[int]$dy]}elseif(($dy -ge 48 -and $dy -le 57) -or ($dy -ge 65 -and $dy -le 90) -or $dy -eq 32){$dy}else{throw "Unsupported fixture key $dy"}
       $bit=switch($dy){16 {4} 17 {1} 18 {2} default {0}}
       if($down){$keyModifiers=$keyModifiers -bor $bit}else{$keyModifiers=$keyModifiers -band (-bnot $bit)}
       & "$FrontendMouseProbeRoot/probe.exe" $frontend[0].ProcessId "NTVDMConsoleTest-$($process.Id)" "$FrontendMouseProbeRoot/mouse-hook.dll" --key $dx $key $down $keyModifiers "$task/runtime/key-$next.txt"
       if($LASTEXITCODE){throw "Frontend keyboard delivery failed: $LASTEXITCODE"}
      } elseif($buttons -eq 0x2000 -or $buttons -eq 0x2001){
       $editor=if($buttons -eq 0x2000){'Native'}else{'Dos'}
       Invoke-EditorFullscreenProbe -ProbeRoot $FrontendMouseProbeRoot -FrontendPid $frontend[0].ProcessId -Desktop "NTVDMConsoleTest-$($process.Id)" -OutputPrefix "$task/runtime/block-$next" -BlockingEditor $editor -ThreadSnapshot $ThreadSnapshot -WindowedDos
      } elseif($buttons -eq 0x8000){
       Invoke-EditorFullscreenProbe -ProbeRoot $FrontendMouseProbeRoot -FrontendPid $frontend[0].ProcessId -Desktop "NTVDMConsoleTest-$($process.Id)" -OutputPrefix "$task/runtime/full-$next"
      } else {
       & "$FrontendMouseProbeRoot/probe.exe" $frontend[0].ProcessId "NTVDMConsoleTest-$($process.Id)" "$FrontendMouseProbeRoot/mouse-hook.dll" $dx $dy $buttons "$task/runtime/mouse-$next.txt"
       if($LASTEXITCODE){throw "Frontend mouse delivery failed: $LASTEXITCODE"}
      }
      [IO.File]::WriteAllBytes("$dest/PATCH/MACK.NEW",[BitConverter]::GetBytes([uint16]$next))
      Move-Item -LiteralPath "$dest/PATCH/MACK.NEW" -Destination "$dest/PATCH/MACK.BIN" -Force
      $sequence=$next
     }
    }
    Start-Sleep -Milliseconds 20;$process.Refresh()
   }
   if(!$process.HasExited){throw 'Owned Win95 observer deadline'}
  } elseif(!$process.WaitForExit(105000)){throw 'Owned Win95 observer deadline'}
  $process.WaitForExit()
  $process.ExitCode|Set-Content "$task/runtime/observer.exit"
 } finally {
  if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
  if($scope){Stop-IsolatedPackageScope $scope}
  if($stage){Remove-PhysicalRuntimeStage $stage}
 }

 $observation=Get-Content "$task/runtime/observer.txt" -Raw
 if($observation -notmatch '(?m)^result=exited\r?$'){throw 'Win95 observer did not see exit'}
 if(!$GuestUserInput){
 if(!(Test-Path "$dest/PATCH/DONE.BIN")){throw 'No editor completion witness'}
 if([BitConverter]::ToUInt32([IO.File]::ReadAllBytes("$dest/PATCH/DONE.BIN"),0) -ne 2){throw 'Expected two editor completions'}
 if(!(Test-Path "$task/screens.txt")){throw 'No editor screen witness'}
 $frames=@((Get-Content "$task/screens.txt" -Raw) -split '(?m)(?=^VM=)' | Where-Object {$_ -match '^VM='})
 $native=@($frames|Where-Object {$_ -match 'New File' -and $_ -match 'Exit'})
 $dos=@($frames|Where-Object {$_ -match 'Save As' -and $_ -match 'Exit' -and $_ -notmatch 'New File'})
 if(!$native.Count -or !$dos.Count){throw 'Both editor menus must be observed'}
 $nativeVm=[regex]::Match($native[0],'^VM=(\w+)').Groups[1].Value
 $dosVm=[regex]::Match($dos[0],'^VM=(\w+)').Groups[1].Value
 if($nativeVm -eq $dosVm){throw 'Editor menus must belong to distinct guest VMs'}
 $trace=Get-Content "$task/trace.txt" -Raw
 if(@([regex]::Matches($trace,'(?m)^11 ')).Count -lt 2 -or @([regex]::Matches($trace,'(?m)^12 ')).Count -lt 2){throw 'Both real command admissions and completions are required'}
 }
 if($SaveWitness){$expected=if($ReadWriteWitness){@(@('A.TXT','aaaseedA'),@('B.TXT','bbbseedB'))}else{@(@('A.TXT','aaa'),@('B.TXT','bbb'))};foreach($pair in $expected){
  $path="$dest/PATCH/$($pair[0])";$saved=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
  if($saved -cne $pair[1] -and $saved -cne ($pair[1]+"`r`n")){throw "Wrong saved contents: $path"}
  Copy-Item -LiteralPath $path -Destination $task
 }}
 if(($MouseMenu -or $GuestUserInput) -and !$blockingEditor){
  $receipts=@(Get-ChildItem "$dest/PATCH/MOUSE*.BIN")
  $expectedReceipts=if($GuestUserInput){6+2*$DosOnly}elseif($SaveWitness){5}else{4}
  if($receipts.Count -ne $expectedReceipts){throw 'Mouse receipt count mismatch'}
  foreach($receipt in $receipts){
   $data=[IO.File]::ReadAllBytes($receipt.FullName)
   if($data.Length -ne 78){throw 'Invalid mouse receipt'}
   $dx=[Math]::Abs([int][BitConverter]::ToInt16($data,8)-[int][BitConverter]::ToInt16($data,12))
   $dy=[Math]::Abs([int][BitConverter]::ToInt16($data,10)-[int][BitConverter]::ToInt16($data,14))
   if($dx -gt 3 -or $dy -gt 4){throw "Mouse missed target: $($receipt.Name)"}
  }
 }
 $result=[ordered]@{family=$family;nativeVm=$nativeVm;dosVm=$dosVm;dosonly=$DosOnly;instanceDriver=$true;multitask=$Multitask;editors=@($(if($DosOnly){'EDIT.COM'}else{'EDIT.EXE'}),'EDIT.COM');guestUserInput=[bool]$GuestUserInput;frontendMouse=[bool]$FrontendMouseProbeRoot;fullscreenTransitions=[bool]$FullscreenTransitions;frontendKeyboard=[bool]$FrontendKeyboard;mouseMenu=[bool]($MouseMenu -or $GuestUserInput);saveVerified=[bool]$SaveWitness;readWriteVerified=[bool]$ReadWriteWitness;observerExit=([regex]::Match($observation,'(?m)^exit=(\S+)').Groups[1].Value);normalShutdownProved=$false}
 if(!$GuestUserInput){$result.completed=2;$result.menus=$true}
 $result|ConvertTo-Json|Set-Content "$task/editor-result.json"
 if($GuestUserInput){"PASS $family two prompt editors saved distinct text and closed; mouse=$(if($FrontendMouseProbeRoot){'private frontend sink'}else{'guest USER'}); physical Raw Input and normal shutdown not proven"}
 else {"PASS $family two prompt VMs opened and closed selected editors; normal shutdown not proven"}

} finally {
 if(@(Get-ChildItem $source -Force -Recurse -File).Count -ne $original.Count){throw 'Original installation file inventory changed'}
 foreach($row in $original){if((Get-FileHash ($source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
 'Original Windows installation bytes verified unchanged'
}
} finally {
 try {
  if($GuestUserInput){
   foreach($name in 'A.TXT','B.TXT'){
    if(Test-Path -LiteralPath "$dest/PATCH/$name"){Copy-Item -LiteralPath "$dest/PATCH/$name" -Destination "$task/observed-$name"}
   }
   Get-ChildItem -LiteralPath "$dest/PATCH" -Filter 'MOUSE*.BIN' -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination "$task/observed-$($_.Name)"
   }
  }
 } finally {
  . "$repo/tests/observation/isolated_package_cleanup.ps1"
  Remove-OwnedGuestProbeCopy $dest $task $source
 }
}
