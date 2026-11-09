[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$DriverImage,
 [Parameter(Mandatory)][string]$InstanceDriver,
 [Parameter(Mandatory)][string]$InterruptObserver,
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$InstallRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [switch]$MouseMenu,
 [switch]$SaveWitness,
 [string]$DismissWelcomeTitleHex
)
$ErrorActionPreference='Stop'
if($DismissWelcomeTitleHex){
 if($DismissWelcomeTitleHex -notmatch '^(?:[0-9a-fA-F]{2}){1,63}$'){throw 'Welcome title must be guest ANSI bytes in hexadecimal'}
 for($i=0;$i -lt $DismissWelcomeTitleHex.Length;$i+=2){if($DismissWelcomeTitleHex.Substring($i,2) -eq '00'){throw 'Welcome title cannot contain a zero byte'}}
}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$copy=& "$PSScriptRoot/New-Win95ProbeCopy.ps1" -DriverImage $DriverImage -InstanceDriver $InstanceDriver -InterruptObserver $InterruptObserver -InstallSource $InstallSource -InstallRoot $InstallRoot -OutputRoot $OutputRoot
$task=$copy.Task; $source=$copy.Source; $dest=$copy.Destination; $original=$copy.Original
try {
$enc=[Text.Encoding]::GetEncoding(28591)

Copy-Item -LiteralPath "$PackageRoot/system32/COMMAND.COM" -Destination "$dest/PATCH/COMMAND.COM"
if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'COMMAND copy mismatch'}
if($SaveWitness){foreach($name in 'A.TXT','B.TXT'){[IO.File]::WriteAllBytes("$dest/PATCH/$name",[byte[]]@())}}
foreach($id in 'A','B'){
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/$id.PIF" --title "Prompt probe $id" --program "$dest\PATCH\COMMAND.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'PIF failed'}
 & "$PSScriptRoot/Set-Win95ProbePifMemory.ps1" -Path "$dest/PATCH/$id.PIF"
}
$ini="$dest/WIN.INI";$s=$enc.GetString([IO.File]::ReadAllBytes($ini))
$s=[regex]::Replace($s,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\A.PIF $dest\PATCH\B.PIF")
[IO.File]::WriteAllBytes($ini,$enc.GetBytes($s))
$asm="%define MOUSE_MENU $([int][bool]$MouseMenu)`n%define DOSONLY 0`n%define SAVE_WITNESS $([int][bool]$SaveWitness)`n%define FIRST_DOS 0`n"+(Get-Content "$repo/tests/observation/win31_startup_contract.asm" -Raw)
$actions=Get-Content "$repo/test/integration/ntvdm64-386/editor-actions.inc" -Raw
$asm=$asm.Replace('push word 0                      ; DWORD reserved',$actions+"`n"+'push word 0                      ; DWORD reserved')
$routines=Get-Content "$repo/test/integration/ntvdm64-386/editor-routines.inc" -Raw
if($routines -notmatch '(?s)api 2,59\s+\.wait_active:\s+call settle\s+api 2,60\s+[^\r\n]*[\r\n]+cmp ax,\[active-data\]\s+jne \.wait_active'){
 throw 'Shared editor routine must wait for the selected prompt to become active'
}
if($DismissWelcomeTitleHex){
 $routines=$routines.Replace("target_prompt:`n","target_prompt:`ncall dismiss_welcome`n")
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
if($MouseMenu){
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
$asm=$asm.Replace('code_end:',$routines+"`ncode_end:")
$asm=$asm.Replace('data_end:',"mouse_half_row: dw 0`nmouse_rect: times 4 dw 0`nmouse_point: times 2 dw 0`nmouse_actual: times 2 dw 0`nvmd_api: dd 0`nvmd_version: dw 0`nvmd_reply: times 50 db 0`nvmd_query_error: dw 0`nmouse_scroll: times 2 dw 0`nmouse_delta: times 2 dw 0`nmouse_extent: times 2 dw 0`nmouse_receipt_name: db '$dest\PATCH\MOUSE'`nmouse_receipt_digit: db '0'`ndb '.BIN',0`ntimer_id: dw 0`ntimer_msg: times 18 db 0`ncompleted_count: dd 0`nwanted_count: dd 0`nread_count: dw 0`neditors_name: db '$dest\PATCH\EDITORS.BIN',0`nreaders_name: db '$dest\PATCH\READERS.BIN',0`ncompletion_name: db '$dest\PATCH\DONE.BIN',0`ninput_api: dd 0`nstamp: dw 0`nactive: dw 0`nwhich: dw 0`ntitle_a: db 'Prompt probe A',0`ntitle_b: db 'Prompt probe B',0`n"+(Get-Content "$repo/test/integration/ntvdm64-386/editor-events.inc" -Raw)+"`ndata_end:")
if($MouseMenu){$asm=$asm.Replace('data_end:',"mouse_hwnd: dw 0`nmouse_class: times 64 db 0`ndata_end:")}
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
  $scope=New-IsolatedPackageScope $stage
  $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
  $env:MVDM_OBSERVER_OWN_JOB='1'
  $env:PATH="$stage\system32;C:\Windows\System32;C:\Windows"
  $process=Start-Process $Observer -ArgumentList @("$stage/system32/run16.exe",$dest,"$task/runtime/observer.txt","$dest/PATCH/WIN.PIF",'--observation-timeout-ms','90000') -WindowStyle Hidden -PassThru
  @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');stage=$stage}|ConvertTo-Json|Set-Content "$task/runtime/process.json"
  if(!$process.WaitForExit(105000)){throw 'Owned Win95 observer deadline'}
  $process.WaitForExit()
  $process.ExitCode|Set-Content "$task/runtime/observer.exit"
 } finally {
  if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
  if($scope){Stop-IsolatedPackageScope $scope}
  if($stage){Remove-PhysicalRuntimeStage $stage}
 }

 $observation=Get-Content "$task/runtime/observer.txt" -Raw
 if($observation -notmatch '(?m)^result=exited\r?$'){throw 'Win95 observer did not see exit'}
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
 if($SaveWitness){foreach($pair in @(@('A.TXT','aaa'),@('B.TXT','bbb'))){
  $path="$dest/PATCH/$($pair[0])";$saved=Get-Content -LiteralPath $path -Raw
  if([string]::IsNullOrEmpty($saved) -or $saved.Trim() -cne $pair[1]){throw "Wrong saved contents: $path"}
  Copy-Item -LiteralPath $path -Destination $task
 }}
 if($MouseMenu){
  $receipts=@(Get-ChildItem "$dest/PATCH/MOUSE*.BIN")
  $expectedReceipts=if($SaveWitness){5}else{4}
  if($receipts.Count -ne $expectedReceipts){throw 'Mouse receipt count mismatch'}
  foreach($receipt in $receipts){
   $data=[IO.File]::ReadAllBytes($receipt.FullName)
   if($data.Length -ne 78){throw 'Invalid mouse receipt'}
   $dx=[Math]::Abs([int][BitConverter]::ToInt16($data,8)-[int][BitConverter]::ToInt16($data,12))
   $dy=[Math]::Abs([int][BitConverter]::ToInt16($data,10)-[int][BitConverter]::ToInt16($data,14))
   if($dx -gt 3 -or $dy -gt 4){throw "Mouse missed target: $($receipt.Name)"}
  }
 }
 [ordered]@{nativeVm=$nativeVm;dosVm=$dosVm;completed=2;menus=$true;mouseMenu=[bool]$MouseMenu;saveVerified=[bool]$SaveWitness;observerExit=([regex]::Match($observation,'(?m)^exit=(\S+)').Groups[1].Value);normalShutdownProved=$false}|ConvertTo-Json|Set-Content "$task/editor-result.json"
 'PASS Win95 two prompt VMs opened and closed native and DOS editors; normal shutdown not proven'

} finally {
 if(@(Get-ChildItem $source -Force -Recurse -File).Count -ne $original.Count){throw 'Original installation file inventory changed'}
 foreach($row in $original){if((Get-FileHash ($source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
 'Original Windows installation bytes verified unchanged'
}
} finally {
 . "$repo/tests/observation/isolated_package_cleanup.ps1"
 Remove-OwnedGuestProbeCopy $dest $task $source
}
