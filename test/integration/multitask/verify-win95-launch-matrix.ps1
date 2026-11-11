param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [switch]$DosReentry,
 [switch]$NativeScheduling,
 [switch]$RestartCheck
)
$ErrorActionPreference='Stop'
if($DosReentry -and $NativeScheduling){throw 'Select one scheduling child type'}
$schedule=[bool]($DosReentry -or $NativeScheduling)
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $output|Out-Null
$mouseTools=Join-Path $output 'mouse-tools'
if($schedule){
 & "$repo/test/observation/build-win101-mouse-window-probe.ps1" -BuildRoot ([IO.Path]::GetRelativePath($repo,$mouseTools))
}
$rows=@();$savedPath=$env:PATH
foreach($dos in 0,1){foreach($multitask in 0,1){
 if($schedule -and $dos){continue}
 $name="dos$dos-multitask$multitask";$task=Join-Path $output $name
 $copy=& "$repo/test/integration/ntvdm64-386/New-Win95ProbeCopy.ps1" -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" -MouseDriver "$repo/assets/release/MOUSE31.DRV" -InstallSource $InstallSource -InstallRoot O:/tmp/q95 -OutputRoot $task
 $dest=$copy.Destination
 try {
  foreach($file in 'PIF.EXE','INSIGNIA.386','NTVDM64.386'){Copy-Item "$repo/assets/release/$file" "$dest/PATCH/$file"}
  Copy-Item "$repo/tools/win95-launch/WIN.CMD" "$dest/PATCH/WIN.CMD"
  & cmd.exe /d /c "`"$dest/PATCH/WIN.CMD`" --install" *> "$task/install.log"
  if($LASTEXITCODE){throw 'Dynamic launcher installation failed'}
  $shim=Join-Path $task 'selector';New-Item -ItemType Directory $shim|Out-Null
  [IO.File]::WriteAllText("$shim/run16.cmd","@echo off`r`nexit /b 0`r`n")
  [IO.File]::WriteAllText("$task/input.txt","$dos`r`n$multitask`r`n")
  $env:PATH="$shim;$savedPath"
  & cmd.exe /d /c "`"$dest/PATCH/WIN.CMD`" < `"$task/input.txt`"" *> "$task/select.log"
  if($LASTEXITCODE){throw 'Dynamic launcher selection failed'}
  $env:PATH=$savedPath
  $ini=Get-Content "$dest/SYSTEM.INI" -Raw
  foreach($file in 'INSIGNIA.386','NTVDM64.386'){
   $pattern='(?im)^device='+[regex]::Escape("$dest\PATCH\$file")+'\s*$'
   if([regex]::IsMatch($ini,$pattern) -ne [bool]($file -eq 'INSIGNIA.386' -or $multitask)){throw "Unexpected enabled driver: $file"}
  }
  $runtimeOptions=@{}
  if($schedule){
   Copy-Item "$PackageRoot/system32/COMMAND.COM" "$dest/PATCH/COMMAND.COM"
   & "$repo/assets/release/PIF.EXE" create "$dest/DOSCHECK.PIF" --title 'Scheduling prompt' --program "$dest\PATCH\COMMAND.COM" --directory $dest --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
   if($LASTEXITCODE){throw 'Cannot create interactive scheduling prompt'}
   & "$repo/test/integration/ntvdm64-386/Set-Win95ProbePifMemory.ps1" -Path "$dest/DOSCHECK.PIF"
   & nasm -f bin "-DW31_INSTALL='$dest'" "$PSScriptRoot/win31-scheduling-child.asm" -o "$dest/SCHED.COM"
   if($LASTEXITCODE){throw 'Scheduling DOS child assembly failed'}
   $controllerOptions=@('-DW31_SCHEDULING_INTERACTIVE')
   $expectedScheduling='nonblocking';$route='SCHED.COM'
   if($NativeScheduling){
    $nativeCmd=Join-Path ([Environment]::SystemDirectory) 'cmd.exe'
    $hostVersion=(& $nativeCmd /d /c ver | Out-String).Trim()
    if($LASTEXITCODE -or !$hostVersion){throw 'Cannot obtain host CMD version'}
    $nativeScript="$dest\SCHED.CMD"
    [IO.File]::WriteAllText($nativeScript,"@echo off`r`nver >$dest\PATCH\SCHED.RUN`r`n:wait`r`nif not exist $dest\PATCH\SCHED.GO goto wait`r`nver >$dest\PATCH\EXEC.TXT`r`necho done>$dest\PATCH\SCHED.END`r`n",[Text.Encoding]::ASCII)
    & "$repo/assets/release/PIF.EXE" update "$dest/DOSCHECK.PIF" --arguments "/c $nativeCmd /d /c $nativeScript"
    if($LASTEXITCODE){throw 'Cannot configure explicit native scheduling command'}
    $controllerOptions=@();$route='cmd.exe'
    if(!$multitask){$expectedScheduling='blocking'}
   }
   & nasm -f bin -DW31_PROMPT_CHECK -DW31_SCHEDULING_CHECK @controllerOptions "-DW31_INSTALL='$dest'" "$repo/test/observation/win31_startup_contract.asm" -o "$task/controller.exe"
   if($LASTEXITCODE){throw 'Scheduling controller assembly failed'}
   $runtimeOptions=@{WindowsProbeImage="$task/controller.exe";Scheduling=$expectedScheduling;ScheduledRun16Command=$route;MouseProbeRoot=$mouseTools}
  }
  & "$repo/test/integration/win31_runtime_matrix_test.ps1" -Windows95 -Win95ExitBoundary -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/startup" -Cases "386-dosonly$dos" -TimeoutMs 90000 @runtimeOptions
  $observed=Get-Content "$task/startup/matrix-results.json" -Raw|ConvertFrom-Json
  if($RestartCheck){
   if($schedule){throw 'Restart control requires the startup/exit controller'}
   & "$repo/test/integration/win31_runtime_matrix_test.ps1" -Windows95 -Win95ExitBoundary -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot "$task/restart" -Cases "386-dosonly$dos" -TimeoutMs 90000
  }
  if($DosReentry -and (Get-Content "$dest/PATCH/EXEC.TXT" -Raw) -cne 'S14_OK'){throw 'Incorrect DOS scheduling result'}
  if($NativeScheduling -and ((Get-Content "$dest/PATCH/EXEC.TXT" -Raw).Trim() -cne $hostVersion)){throw 'Incorrect native scheduling result'}
  $rows+= [pscustomobject]@{dosonly=$dos;instanceDriver=$true;multitask=$multitask;startup=$true;normalExit=[bool]$observed.normalExit;exitAccepted=[bool]$observed.exitAccepted;knownExitOne=[bool]$observed.knownExitOne;restartVerified=[bool]$RestartCheck;interaction='pending';passed=$false}
 } catch {
  $failure=$_.Exception.Message
  $observed=if(Test-Path "$task/startup/matrix-results.json"){Get-Content "$task/startup/matrix-results.json" -Raw|ConvertFrom-Json}else{$null}
  $rows+= [pscustomobject]@{dosonly=$dos;instanceDriver=$true;multitask=$multitask;startup=[bool]($observed.probeStarted -and $observed.progmanAndMode);normalExit=[bool]$observed.normalExit;exitAccepted=$false;restartVerified=$false;interaction='pending';passed=$false;error=$failure}
 } finally {
  $env:PATH=$savedPath
  if($NativeScheduling -and (Test-Path "$dest/PATCH/SCHED.RUN") -and (Test-Path "$dest/PATCH/EXEC.TXT")){
   $started=(Get-Content "$dest/PATCH/SCHED.RUN" -Raw).Trim()
   $completed=(Get-Content "$dest/PATCH/EXEC.TXT" -Raw).Trim()
   [ordered]@{expected=$hostVersion;started=$started;completed=$completed;passed=($started -ceq $hostVersion -and $completed -ceq $hostVersion)} |
    ConvertTo-Json | Set-Content "$task/native-owner.json"
  }
  $rows|ConvertTo-Json -Depth 5|Set-Content "$output/matrix.json"
  . "$repo/test/observation/isolated_package_cleanup.ps1"
  Remove-OwnedGuestProbeCopy $dest $task $copy.Source
 }
}}
if(@($rows|Where-Object {!$_.startup -or !$_.exitAccepted}).Count){throw 'Win95 startup/retirement controls failed; inspect matrix.json'}
if($NativeScheduling){'PASS two explicit native scheduling controls; editor acceptance remains separate'}
elseif($DosReentry){'PASS two interactive DOS reentry scheduling controls; editor acceptance remains separate'}
else{'PASS four startup controls only; interactive acceptance remains pending'}
