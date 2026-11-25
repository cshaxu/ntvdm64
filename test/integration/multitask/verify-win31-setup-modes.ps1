[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$MouseDriver,
 [switch]$DosReentry
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
if(!$MouseDriver){$MouseDriver=Join-Path $repo 'assets/release/MOUSE31.DRV'}
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $output|Out-Null
$mouseTools=Join-Path $repo 'build/.ninja/fixtures-x64/mouse-tools'
$build=Start-Process $env:ComSpec -ArgumentList ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x64 private-mouse-tools"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$output/mouse-build.log" -RedirectStandardError "$output/mouse-build.err"
try{
 if(!$build.WaitForExit(300000)){& taskkill.exe /PID $build.Id /T /F;throw 'Owned mouse build deadline'}
 $build.WaitForExit()
 @{pid=$build.Id;start=$build.StartTime;finish=$build.ExitTime;exit=$build.ExitCode}|ConvertTo-Json|Set-Content "$output/mouse-build.json"
 if($build.ExitCode){throw 'Mouse tools build failed'}
}finally{$build.Dispose()}
$failures=@()
foreach($mode in 0,1){
 $task=Join-Path $output "mode$mode"
 $copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" `
  -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
  -InstallSource $InstallSource -InstallRoot 'O:/tmp/q31' -OutputRoot $task -MouseDriver $MouseDriver
 $dest=$copy.Destination
 try {
 & "$repo/tools/build-fixtures.cmd" x86 win31-profile $dest *> "$task/profile-build.log"
 if($LASTEXITCODE){throw 'Matrix witness build failed'}
 $outputLine=Select-String -LiteralPath "$task/profile-build.log" -Pattern '^NTVDM_PROFILE_BIN=(.+)$'
 if(!$outputLine){throw 'Profile build did not report its output directory'}
 $profileCache=$outputLine.Matches[0].Groups[1].Value
  $guest=@{};foreach($name in 'WIN.COM','SYSTEM/DOSX.EXE','SYSTEM/KRNL386.EXE','SYSTEM/WIN386.EXE'){$guest[$name]=(Get-FileHash "$dest/$name").Hash}
  & "$repo/tools/prepare-win31-matrix.cmd" setup $dest $task *> "$task/setup.log"
  if($LASTEXITCODE){throw 'Setup finalization failed'}
  function Select-LaunchProfile([string]$case,[string]$label=$case) {
   $enhanced=$case.StartsWith('386-');$dos=[int]$case.EndsWith('dosonly1')
   $winMode=if($enhanced){2}else{1}
   $multitask=[int]($enhanced -and $mode -eq 0)
   & "$repo/tools/select-guest-launch.cmd" $dest $task 31 $dos $multitask select $winMode $label *> "$task/select-$label.log"
   if($LASTEXITCODE){throw "Launcher selection failed: $label"}
   foreach($name in 'INSIGNIA.386','NTVDM64.386'){
    if((Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Wrong driver $name"}
   }
  }
  foreach($name in $guest.Keys){if((Get-FileHash "$dest/$name").Hash -ne $guest[$name]){throw "Guest binary changed: $name"}}
  & "$repo/test/integration/win31_program_groups_test.ps1" -InstallRoot $dest
  # Each generated driver profile boots in both Windows modes with both
  # DOSONLY values. The runtime runner verifies GetWinFlags and the guest
  # DOSONLY query, then restores CONFIG.NT in this isolated installation.
  $profile=Join-Path $dest 'DOSCHECK.PIF'
  $receipt=Join-Path $dest 'PATCH\EXEC.TXT'
  if((Test-Path $profile) -or (Test-Path $receipt)){throw 'Prompt fixture path already in use'}
  & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $task "$dest/DOSPRMPT.PIF" DOSCHECK.PIF
  if($LASTEXITCODE){throw 'Cannot stage the scheduling prompt'}
  $nativeCmd=Join-Path ([Environment]::SystemDirectory) 'cmd.exe'
  $hostVersion=(& $nativeCmd /d /c ver | Out-String).Trim()
  if($LASTEXITCODE -or !$hostVersion){throw 'Cannot obtain native CMD version for comparison'}
  $nativeScript=Join-Path $dest 'S14.CMD'
  if(Test-Path $nativeScript){throw 'Native command fixture already exists'}
  $scheduleChild=Join-Path $dest 'SCHED.COM'
  & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $task "$PSScriptRoot/scheduling-witness.cmd" S14.CMD "$profileCache/SCHED.COM" SCHED.COM
  if($LASTEXITCODE){throw 'Cannot stage scheduling witnesses'}
  $controller=Join-Path $task 'prompt-check.exe'
  $controllerOptions=@()
  if($DosReentry){$controllerOptions+= '-DW31_SCHEDULING_INTERACTIVE'}
  $controllerName=if($controllerOptions.Count){'INTERACT.EXE'}else{'SCHEDULE.EXE'}
  & "$repo/tools/prepare-test-inputs.cmd" $task "$profileCache/$controllerName" prompt-check.exe
  if($LASTEXITCODE){throw 'Cannot stage Windows scheduling witness'}
  foreach($case in 'std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1'){
   if($DosReentry -and $case -ne '386-dosonly0'){continue}
   $out=Join-Path $task $case
   Select-LaunchProfile $case
   # Standard Windows ignores [386Enh]. Exercise interaction on the bare
   # profile; with the pair installed, verify startup, DOSONLY and exit.
   if($mode -eq 0 -and $case.StartsWith('std-')){
    try {
     & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot $out -Cases $case
    } catch {
     $failures+="setup=$mode $case smoke $($_.Exception.Message)"
    }
    continue
   }
   # DOSONLY=0 selects Run16 routing, not universal guest suspension: a DOS
   # child can reenter the existing worker while guest Windows keeps running.
   $nativeChild=$case.EndsWith('dosonly0') -and !$DosReentry
   $argument=if($DosReentry){''}elseif($nativeChild){"/c $nativeCmd /d /c $nativeScript"}else{"/c $scheduleChild"}
   $expected=if($nativeChild){$hostVersion}else{'S14_OK'}
   $scheduling=if($case.StartsWith('std-') -or ($nativeChild -and $mode -eq 1)){'blocking'}else{'nonblocking'}
   & "$repo/assets/release/PIF.EXE" update $profile --title 'Scheduling prompt' --directory $dest --arguments $argument
   if($LASTEXITCODE){throw "Cannot configure DOS prompt for $case"}
   if($DosReentry){
    $description=& "$repo/assets/release/PIF.EXE" show $profile
    $flagsLine=$description | Select-String '^W386\.FLAGS=0x([0-9a-f]+)$'
    if($LASTEXITCODE -or !$flagsLine){throw 'Cannot read interactive DOS prompt flags'}
    $flags=([Convert]::ToUInt32($flagsLine.Matches[0].Groups[1].Value,16) -band (-bnot 8)) -bor 2
    & "$repo/assets/release/PIF.EXE" update $profile --w386-flags $flags
    if($LASTEXITCODE){throw 'Cannot select windowed/background DOS prompt'}
   }
   $signals=@("$dest/PATCH/MREADY.BIN","$dest/PATCH/MDOWN.BIN")
   foreach($signal in $signals){if(Test-Path $signal){throw "Stale fixture signal: $signal"}}
   try {
    $route=if($DosReentry){'SCHED.COM'}else{''}
    & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot $out -WindowsProbeImage $controller -Cases $case -MouseProbeRoot $mouseTools -Scheduling $scheduling -ScheduledRun16Command $route
    if(!(Test-Path $receipt) -or (Get-Content -LiteralPath $receipt -Raw).Trim() -cne $expected){throw "Unexpected command output for $case"}
    $owner=if($nativeChild){$hostVersion}else{'DOS'}
    if((Get-Content "$dest/PATCH/SCHED.RUN" -Raw).Trim() -cne $owner){throw "Wrong execution owner: $case"}
   } catch {
    $failures+= "setup=$mode $case $($_.Exception.Message)"
   } finally {
    foreach($signal in $signals){if(Test-Path $signal){Move-Item -LiteralPath $signal -Destination $out}}
    if(Test-Path $receipt){Move-Item -LiteralPath $receipt -Destination (Join-Path $out 'command.txt')}
    foreach($name in 'SCHED.RUN','SCHED.GO','SCHED.END','SCHED.HB'){
     $path=Join-Path "$dest/PATCH" $name
     if(Test-Path $path){Move-Item -LiteralPath $path -Destination $out}
    }
   }
  }
  if($DosReentry){continue}
  # Enhanced DOSONLY=1 schedules guest prompts with either payload selection.
  $controller=Join-Path $task 'prompt-fullscreen.exe'
  & "$repo/tools/prepare-test-inputs.cmd" $task "$profileCache/PROMPT.EXE" prompt-fullscreen.exe
  if($LASTEXITCODE){throw 'Cannot stage fullscreen Windows witness'}
  # Preserve the single-prompt source flags except for windowed/background.
  $description=& "$repo/assets/release/PIF.EXE" show $profile
  $flagsLine=$description | Select-String '^W386\.FLAGS=0x([0-9a-f]+)$'
  if($LASTEXITCODE -or !$flagsLine){throw 'Cannot read DOS prompt display flags'}
  $flags=([Convert]::ToUInt32($flagsLine.Matches[0].Groups[1].Value,16) -band (-bnot 8)) -bor 2
  $program="$dest\FULLKEY.COM"
  if(Test-Path $program){throw 'Fullscreen fixture path already in use'}
  & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $task "$profileCache/FULLKEY.COM" FULLKEY.COM
  if($LASTEXITCODE){throw 'Cannot stage fullscreen DOS witness'}
  & "$repo/assets/release/PIF.EXE" update $profile --arguments "/c $program" --w386-flags $flags
  if($LASTEXITCODE){throw 'Cannot configure windowed DOS prompt for Alt+Enter'}
  foreach($case in '386-dosonly1','386-dosonly0'){
   Select-LaunchProfile $case "$case-fullscreen"
   $out=Join-Path $task "$case-fullscreen"
   try {
    & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot $out -WindowsProbeImage $controller -Cases $case -MouseProbeRoot $mouseTools -FullscreenKey
    $observed=Get-Content "$out/$case.observer.txt" -Raw
    if($observed -notmatch '(?m)^window-key-message-count=6 window-enter-count=1\r?$'){throw 'Observer did not deliver the four Alt+Enter events and the k down/up pair'}
    if(!(Test-Path $receipt) -or (Get-Content "$out/$case.observer.txt.caf.txt" -Raw) -notmatch 'caf-visible-window=1'){throw 'Missing fullscreen DOS keyboard or NTCON text window witness'}
   } catch {
    $failures+="setup=$mode $case-fullscreen $($_.Exception.Message)"
   } finally {
    foreach($signal in ($signals+@($receipt,"$dest/PATCH/FULL.RUN"))){if(Test-Path $signal){Move-Item -LiteralPath $signal -Destination $out}}
   }
  }
  $second=Join-Path $dest 'DOSPAIR.PIF'
  if(Test-Path $second){throw 'Second prompt fixture already exists'}
  & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $task $profile DOSPAIR.PIF
  if($LASTEXITCODE){throw 'Cannot stage second DOS prompt'}
  foreach($pair in @(@('A','B',$profile),@('B','A',$second))){
   $id,$peer,$pif=$pair
   $program="$dest\RUN$id.COM"
   foreach($path in $program,"$dest\PATCH\$id.RUN","$dest\PATCH\$id.END"){
    if(Test-Path $path){throw "Guest pair fixture already exists: $path"}
   }
   & "$repo/tools/prepare-test-inputs.cmd" -GuestRoot $dest $task "$profileCache/RUN$id.COM" "RUN$id.COM"
   if($LASTEXITCODE){throw 'Cannot stage guest pair witness'}
   & "$repo/assets/release/PIF.EXE" update $pif --title "S14 prompt $id" --w386-flags $flags --arguments "/c $program"
   if($LASTEXITCODE){throw 'Cannot configure windowed DOS prompt'}
  }
  $controller=Join-Path $task 'prompt-pair.exe'
  & "$repo/tools/prepare-test-inputs.cmd" $task "$profileCache/PAIR.EXE" prompt-pair.exe
  if($LASTEXITCODE){throw 'Cannot stage Windows pair witness'}
  Select-LaunchProfile '386-dosonly1' '386-dosonly1-pair'
  $out=Join-Path $task '386-dosonly1-pair'
  try {
   & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot $out -WindowsProbeImage $controller -Cases '386-dosonly1' -MouseProbeRoot $mouseTools -PromptPair
   $vmIds=foreach($id in 'A','B'){
    $entry=[IO.File]::ReadAllBytes("$dest/PATCH/$id.END")
    if($entry.Length -ne 5 -or $entry[0] -ne [byte][char]$id){throw "Invalid guest completion for $id"}
    if([BitConverter]::ToUInt16($entry,3)){throw "Fresh DOS prompt $id inherited an active mouse callback"}
    [BitConverter]::ToUInt16($entry,1)
   }
   if($vmIds -contains 0 -or $vmIds[0] -eq $vmIds[1]){throw "Expected distinct DOS VM IDs, observed $vmIds"}
  } catch {
   $failures+="setup=$mode 386-dosonly1-pair $($_.Exception.Message)"
  } finally {
   foreach($name in 'A.RUN','B.RUN','A.END','B.END','A.FOC','B.REQ','B.FOC','B.KEY','MREADY.BIN','MDOWN.BIN'){
    $path=Join-Path "$dest/PATCH" $name
    if(Test-Path $path){Move-Item -LiteralPath $path -Destination $out}
   }
  }
 } finally {
  try {
   if(@(Get-ChildItem $copy.Source -Force -Recurse -File).Count -ne $copy.Original.Count){throw 'Source installation inventory changed'}
   foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Source installation changed: $($row.path)"}}
  } finally {
   . "$repo/test/observation/isolated_package_cleanup.ps1"
   Remove-OwnedGuestProbeCopy $dest $task $copy.Source
  }
 }
}
if($failures.Count){throw ($failures -join [Environment]::NewLine)}
if($DosReentry){
 'PASS enhanced DOSONLY=0 DOS-child scheduling with NTVDM64 disabled/enabled; Setup used a shim'
}else{
 'PASS six interactive Win3.1 configurations, two Standard startup/exit controls, two enhanced prompt pairs and four fullscreen keyboard cases; Setup used a shim'
}
