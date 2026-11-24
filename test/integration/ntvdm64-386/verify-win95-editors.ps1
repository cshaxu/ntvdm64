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
$familyNumber=if($Windows31){31}else{95}
& "$repo/tools/select-guest-launch.cmd" $dest $task $familyNumber $DosOnly $Multitask install *> "$task/select.log"
if($LASTEXITCODE){throw 'Editor launcher profile failed'}
if(((Get-Content "$dest/PATCH/CONFIG.NT" -Raw) -match '(?im)^dosonly\s*$') -ne [bool]$DosOnly){throw 'Selected DOSONLY mismatch'}
$selectedIni=Get-Content "$dest/SYSTEM.INI" -Raw
foreach($driver in 'INSIGNIA.386','NTVDM64.386'){
 $enabled=[regex]::IsMatch($selectedIni,'(?im)^device='+[regex]::Escape("$dest\PATCH\$driver")+'\s*$')
 if($enabled -ne [bool]($driver -eq 'INSIGNIA.386' -or $Multitask)){throw "Selected driver mismatch: $driver"}
}
Copy-Item -LiteralPath "$dest/SYSTEM.INI" -Destination "$task/selected-system.ini"

$witness=if($ReadWriteWitness){'seed'}elseif($SaveWitness){'empty'}else{'none'}
& "$repo/tools/prepare-editor-inputs.cmd" $dest $task $PackageRoot prompts $witness
if($LASTEXITCODE){throw 'Editor prompt preparation failed'}
if((Get-FileHash "$PackageRoot/system32/COMMAND.COM").Hash -ne (Get-FileHash "$dest/PATCH/COMMAND.COM").Hash){throw 'COMMAND copy mismatch'}
$features=@()
if($MouseMenu){$features+='MOUSE_MENU'}
if($SaveWitness){$features+='SAVE_WITNESS'}
if($GuestUserInput){$features+='GUEST_INPUT'}
if($DosOnly){$features+='GUEST_ONLY'}
if($FullscreenTransitions){$features+='FULLSCREEN'}
if($blockingEditor){$features+='BLOCKING'}
if($BlockingDosFirst){$features+='DOS_FIRST'}
if($FrontendMouseProbeRoot){$features+='FRONTEND'}
if($FrontendKeyboard){$features+='FRONTEND_KEYS'}
if(($MouseMenu -or $GuestUserInput) -and !$Windows31){$features+='CHILD_GEOMETRY'}
$buildLine='/d /s /c ""'+$repo+'/tools/build-win95-editor.cmd" "'+$task+'/controller.exe" "'+$dest+'" "'+($features -join ';')+'" "'+$DismissWelcomeTitleHex+'""'
$producer=Start-Process $env:ComSpec -ArgumentList $buildLine -WindowStyle Hidden -PassThru -RedirectStandardOutput "$task/controller-build.stdout.txt" -RedirectStandardError "$task/controller-build.stderr.txt"
$null=$producer.Handle
@{pid=$producer.Id;start=$producer.StartTime.ToUniversalTime().ToString('o');command=$buildLine}|ConvertTo-Json|Set-Content "$task/controller-build.process.json"
try {
 if(!$producer.WaitForExit(300000)){taskkill.exe /PID $producer.Id /T /F;throw 'Controller build deadline'}
 $producer.WaitForExit();$producer.ExitCode|Set-Content "$task/controller-build.exit.txt"
 if($producer.ExitCode){throw 'Controller build failed'}
}finally{$producer.Dispose()}

& "$repo/tools/build-fixtures.cmd" x64 win31-probe-paths *> "$task/controller-paths-build.log"
if($LASTEXITCODE){throw 'Controller path binder build failed'}
& "$repo/build/.ninja/fixtures-x64/win31-probe-paths.exe" --fields "$task/controller.exe" '@W31_BOOT_PATH@' "$dest\PATCH\BOOT.BIN" '@W31_READY_PATH@' "$dest\PATCH\READY.BIN"
if($LASTEXITCODE){throw 'Controller path binding failed'}
& "$repo/tools/prepare-editor-inputs.cmd" $dest $task "$task/controller.exe" controller
if($LASTEXITCODE){throw 'Editor controller preparation failed'}
$keep=@('SystemRoot','WINDIR','COMSPEC','PATHEXT','SYSTEMDRIVE','NUMBER_OF_PROCESSORS','PROCESSOR_ARCHITECTURE','PROCESSOR_IDENTIFIER','OS','PSExecutionPolicyPreference')
Get-ChildItem Env:|Where-Object {$_.Name -notin $keep}|ForEach-Object{Remove-Item -LiteralPath ('Env:'+ $_.Name)}
$env:PATH='C:\Windows\System32;C:\Windows';$env:TEMP='O:\tmp';$env:TMP='O:\tmp'
$env:MVDM_COMPLETION_COUNT="$dest/PATCH/DONE.BIN"
$env:MVDM_PROMPT_READERS="$dest/PATCH/READERS.BIN"
$env:MVDM_SCREEN_TRACE="$task/screens.txt"
$env:MVDM_EDITORS_READY="$dest/PATCH/EDITORS.BIN"
$env:MVDM_REAL_COMMAND='1'
$env:MVDM_CONTINUATION_TRACE="$task/trace.txt"

try {

 . "$repo/test/observation/isolated_package_cleanup.ps1"
 $stage=$null;$scope=$null;$process=$null
 New-Item -ItemType Directory "$task/runtime"|Out-Null
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
  . "$repo/test/observation/isolated_package_cleanup.ps1"
  Remove-OwnedGuestProbeCopy $dest $task $source
 }
}
