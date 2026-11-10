[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [string]$MouseDriver
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
if(!$MouseDriver){$MouseDriver=Join-Path $repo 'assets/release/MOUSE31.DRV'}
$output=[IO.Path]::GetFullPath($OutputRoot)
if(!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $output|Out-Null
$savedPath=$env:PATH;$savedPause=$env:NTVDM_SETUP_TEST_NO_PAUSE
$mouseTools=Join-Path $output 'mouse-tools'
& "$repo/tests/observation/build-win101-mouse-window-probe.ps1" -BuildRoot ([IO.Path]::GetRelativePath($repo,$mouseTools))
$failures=@()
foreach($mode in 0,1){
 $task=Join-Path $output "mode$mode"
 $copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" `
  -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
  -InstallSource $InstallSource -InstallRoot 'O:/tmp/q31' -OutputRoot $task -MouseDriver $MouseDriver
 $dest=$copy.Destination
 try {
  # Remove only the two exact entries and files added by the copy helper.
  $ini="$dest/SYSTEM.INI";$enc=[Text.Encoding]::GetEncoding(28591)
  $text=$enc.GetString([IO.File]::ReadAllBytes($ini))
  foreach($name in 'INSIGNIA.386','NTVDM64.386'){
   $line='(?im)^device='+[regex]::Escape("$dest\PATCH\$name")+'\r*\n'
   if([regex]::Matches($text,$line).Count -ne 1){throw "Expected one copied driver entry: $name"}
   $text=[regex]::Replace($text,$line,'')
   if((Get-FileHash "$dest/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Unexpected copied driver: $name"}
   Remove-Item -LiteralPath "$dest/PATCH/$name"
  }
  [IO.File]::WriteAllBytes($ini,$enc.GetBytes($text))
  foreach($name in 'CONFIG.NT','AUTOEXEC.NT','WINSTD.PIF','WIN386.PIF','WINSTD.CMD','WIN386.CMD'){
   $path=Join-Path "$dest/PATCH" $name;if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path}
  }
  $guest=@{};foreach($name in 'WIN.COM','SYSTEM/DOSX.EXE','SYSTEM/KRNL386.EXE','SYSTEM/WIN386.EXE'){$guest[$name]=(Get-FileHash "$dest/$name").Hash}
  $media=Join-Path $task 'media/PATCH';$shim=Join-Path $task 'shim'
  New-Item -ItemType Directory -Path $media,$shim|Out-Null
  foreach($name in 'SETUP.CMD','WIN.CMD'){Copy-Item "$repo/tools/win31-setup/$name" "$media/$name"}
  foreach($name in 'PIF.EXE','INSIGNIA.386','NTVDM64.386'){Copy-Item "$repo/assets/release/$name" "$media/$name"}
  [IO.File]::WriteAllText("$shim/run16.cmd","@echo off`r`nexit /b 0`r`n",[Text.Encoding]::ASCII)
  [IO.File]::WriteAllText("$task/input.txt","$dest`r`n",[Text.Encoding]::ASCII)
  $env:PATH="$shim;$savedPath";$env:NTVDM_SETUP_TEST_NO_PAUSE='1'
  & cmd.exe /d /c "`"$media/SETUP.CMD`" < `"$task/input.txt`"" *> "$task/setup.log"
  if($LASTEXITCODE){throw 'Setup finalization failed'}
  $env:PATH=$savedPath
  function Select-LaunchProfile([string]$case) {
   $enhanced=$case.StartsWith('386-');$dos=[int]$case.EndsWith('dosonly1')
   $selection="$(if($enhanced){2}else{1})`r`n$dos`r`n"
   if($enhanced){$selection+="$([int]($mode -eq 0))`r`n"}
   [IO.File]::WriteAllText("$task/input.txt",$selection)
   $env:PATH="$shim;$savedPath"
   try {
    & cmd.exe /d /c "`"$dest/PATCH/WIN.CMD`" < `"$task/input.txt`"" *> "$task/select-$case.log"
    if($LASTEXITCODE){throw "Launcher selection failed: $case"}
   } finally {$env:PATH=$savedPath}
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
  Copy-Item -LiteralPath "$dest/DOSPRMPT.PIF" -Destination $profile
  $nativeCmd=Join-Path ([Environment]::SystemDirectory) 'cmd.exe'
  $hostVersion=(& $nativeCmd /d /c ver | Out-String).Trim()
  if($LASTEXITCODE -or !$hostVersion){throw 'Cannot obtain native CMD version for comparison'}
  $nativeScript=Join-Path $dest 'S14.CMD'
  if(Test-Path $nativeScript){throw 'Native command fixture already exists'}
  [IO.File]::WriteAllText($nativeScript,"@echo off`r`nver >$dest\PATCH\SCHED.RUN`r`n:wait`r`nif not exist $dest\PATCH\SCHED.GO goto wait`r`nver >$receipt.tmp`r`nmove /y $receipt.tmp $receipt >nul`r`necho done>$dest\PATCH\SCHED.END`r`n",[Text.Encoding]::ASCII)
  $scheduleChild=Join-Path $dest 'SCHED.COM'
  & nasm -f bin "-DW31_INSTALL='$dest'" "$PSScriptRoot/win31-scheduling-child.asm" -o $scheduleChild
  if($LASTEXITCODE){throw 'Scheduling DOS child assembly failed'}
  $controller=Join-Path $task 'prompt-check.exe'
  & nasm -f bin -DW31_PROMPT_CHECK -DW31_SCHEDULING_CHECK "-DW31_INSTALL='$dest'" "$repo/tests/observation/win31_startup_contract.asm" -o $controller
  if($LASTEXITCODE){throw 'Prompt controller assembly failed'}
  foreach($case in 'std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1'){
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
   $argument=if($case.EndsWith('dosonly0')){"/c $nativeCmd /d /c $nativeScript"}else{"/c $scheduleChild"}
   $expected=if($case.EndsWith('dosonly0')){$hostVersion}else{'S14_OK'}
   $scheduling=if($case.StartsWith('std-') -or ($case -eq '386-dosonly0' -and $mode -eq 1)){'blocking'}else{'nonblocking'}
   & "$repo/assets/release/PIF.EXE" update $profile --arguments $argument
   if($LASTEXITCODE){throw "Cannot configure DOS prompt for $case"}
   $signals=@("$dest/PATCH/MREADY.BIN","$dest/PATCH/MDOWN.BIN")
   foreach($signal in $signals){if(Test-Path $signal){throw "Stale fixture signal: $signal"}}
   try {
    & "$repo/test/integration/win31_runtime_matrix_test.ps1" -PreparedProfile -PackageRoot $PackageRoot -InstallRoot $dest -Observer $Observer -OutputRoot $out -WindowsProbeImage $controller -Cases $case -MouseProbeRoot $mouseTools -Scheduling $scheduling
    if(!(Test-Path $receipt) -or (Get-Content -LiteralPath $receipt -Raw).Trim() -cne $expected){throw "Unexpected command output for $case"}
    $owner=if($case.EndsWith('dosonly0')){$hostVersion}else{'DOS'}
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
  # Enhanced DOSONLY=1 must schedule two guest prompts even without addons.
  & nasm -f bin -DW31_PROMPT_CHECK "-DW31_INSTALL='$dest'" "$repo/tests/observation/win31_startup_contract.asm" -o $controller
  if($LASTEXITCODE){throw 'Interactive controller assembly failed'}
  # Preserve the single-prompt source flags except for windowed/background.
  $description=& "$repo/assets/release/PIF.EXE" show $profile
  $flagsLine=$description | Select-String '^W386\.FLAGS=0x([0-9a-f]+)$'
  if($LASTEXITCODE -or !$flagsLine){throw 'Cannot read DOS prompt display flags'}
  $flags=([Convert]::ToUInt32($flagsLine.Matches[0].Groups[1].Value,16) -band (-bnot 8)) -bor 2
  $program="$dest\FULLKEY.COM"
  if(Test-Path $program){throw 'Fullscreen fixture path already in use'}
  & nasm -f bin "-DW31_INSTALL='$dest'" "$repo/tests/observation/win31_fullscreen_key.asm" -o $program
  if($LASTEXITCODE){throw 'Fullscreen guest fixture assembly failed'}
  & "$repo/assets/release/PIF.EXE" update $profile --arguments "/c $program" --w386-flags $flags
  if($LASTEXITCODE){throw 'Cannot configure windowed DOS prompt for Alt+Enter'}
  foreach($case in '386-dosonly1','386-dosonly0'){
   Select-LaunchProfile $case
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
  Copy-Item $profile $second
  foreach($pair in @(@('A','B',$profile),@('B','A',$second))){
   $id,$peer,$pif=$pair
   $program="$dest\RUN$id.COM"
   foreach($path in $program,"$dest\PATCH\$id.RUN","$dest\PATCH\$id.END"){
    if(Test-Path $path){throw "Guest pair fixture already exists: $path"}
   }
   & nasm -f bin "-DW31_INSTALL='$dest'" "-DROLE='$id'" "-DPEER='$peer'" "$repo/tests/observation/win31_dos_pair.asm" -o $program
   if($LASTEXITCODE){throw 'DOS pair assembly failed'}
   & "$repo/assets/release/PIF.EXE" update $pif --title "S14 prompt $id" --w386-flags $flags --arguments "/c $program"
   if($LASTEXITCODE){throw 'Cannot configure windowed DOS prompt'}
  }
  $controller=Join-Path $task 'prompt-pair.exe'
  & nasm -f bin -DW31_PROMPT_CHECK -DW31_PROMPT_PAIR "-DW31_INSTALL='$dest'" "$repo/tests/observation/win31_startup_contract.asm" -o $controller
  if($LASTEXITCODE){throw 'DOS pair controller assembly failed'}
  Select-LaunchProfile '386-dosonly1'
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
  $env:PATH=$savedPath;$env:NTVDM_SETUP_TEST_NO_PAUSE=$savedPause
  try {
   if(@(Get-ChildItem $copy.Source -Force -Recurse -File).Count -ne $copy.Original.Count){throw 'Source installation inventory changed'}
   foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Source installation changed: $($row.path)"}}
  } finally {
   . "$repo/tests/observation/isolated_package_cleanup.ps1"
   Remove-OwnedGuestProbeCopy $dest $task $copy.Source
  }
 }
}
if($failures.Count){throw ($failures -join [Environment]::NewLine)}
'PASS six interactive Win3.1 configurations, two Standard startup/exit controls, two enhanced prompt pairs and four fullscreen keyboard cases; Setup used a shim'
