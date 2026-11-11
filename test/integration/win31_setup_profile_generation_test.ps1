param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build fixture required'}
$patch=Join-Path $root 'media/PATCH';$bin=Join-Path $root 'bin';$install=Join-Path $root 'W'
New-Item -ItemType Directory -Path $patch,$bin,(Join-Path $install 'SYSTEM')|Out-Null
foreach($n in 'SETUP.CMD','WIN.CMD'){Copy-Item "$repo/tools/win31-setup/$n" "$patch/$n"}
foreach($n in 'PIF.EXE','INSIGNIA.386','NTVDM64.386'){Copy-Item "$repo/assets/release/$n" "$patch/$n"}
[IO.File]::WriteAllText("$bin/run16.cmd","@echo off`r`necho %*>>`"$root/launched.txt`"`r`nexit /b 0`r`n")
$before=@{}
foreach($name in 'WIN.COM','SYSTEM/DOSX.EXE','SYSTEM/KRNL386.EXE','SYSTEM/WIN386.EXE','SYSTEM/MOUSE.DRV','PROGMAN.INI'){
 $f=Join-Path $install $name;[IO.File]::WriteAllText($f,"fixture $name");$before[$f]=(Get-FileHash $f).Hash
}
$ini="[boot]`r`nmouse.drv=mouse.drv`r`n`r`n[386Enh]`r`ndevice=*vmd`r`n; unrelated comment`r`ndevice=other.386`r`n[other]`r`nkeep=value`r`n"
[IO.File]::WriteAllText("$install/SYSTEM.INI",$ini)
$oldPath=$env:PATH;$oldPause=$env:NTVDM_SETUP_TEST_NO_PAUSE
function Invoke-CmdFile($script,$inputText){
 [IO.File]::WriteAllText("$root/input.txt",$inputText)
 $log=(& cmd.exe /d /c "`"$script`" < `"$root/input.txt`"" 2>&1|Out-String)
 if($LASTEXITCODE){throw "CMD failed $LASTEXITCODE : $log"}
}
try {
 $env:PATH="$bin;$oldPath";$env:NTVDM_SETUP_TEST_NO_PAUSE='1'
 New-Item -ItemType Directory "$install/PATCH"|Out-Null
 foreach($name in 'WINSTD.CMD','WIN386.CMD','WINSTD.PIF','WIN386.PIF'){[IO.File]::WriteAllText("$install/PATCH/$name",'obsolete generated launcher')}
 Invoke-CmdFile "$patch/SETUP.CMD" "$install`r`n"
 $dest="$install/PATCH"
 foreach($n in 'INSIGNIA.386','NTVDM64.386'){
  if((Get-FileHash "$dest/$n").Hash -ne (Get-FileHash "$patch/$n").Hash){throw 'Driver copy differs'}
 }
 if(Test-Path "$dest/WIN.PIF"){throw 'Install should not generate a selected PIF'}
 foreach($row in @(@(2,0,1),@(1,0,0),@(2,1,0),@(2,1,1),@(1,1,0),@(2,0,0),@(2,0,1))){
  $mode,$dos,$drivers=$row
  $inputText="$mode`r`n$dos`r`n";if($mode -eq 2){$inputText+="$drivers`r`n"}
  Invoke-CmdFile "$dest/WIN.CMD" $inputText
  $text=(& "$patch/PIF.EXE" show "$dest/WIN.PIF"|Out-String)
  if($LASTEXITCODE){throw 'Invalid PIF'}
  $arg=if($mode -eq 1){'/S'}else{'/3'}
  foreach($v in @("PROGRAM=$install\WIN.COM","DIRECTORY=$install","ARGUMENTS=$arg","CONFIG=$install\PATCH\CONFIG.NT","AUTOEXEC=$install\PATCH\AUTOEXEC.NT",'W386.FLAGS=0x00001003','NT31.FLAGS=0x00000001')){if(!$text.Contains($v)){throw "Missing PIF field $v"}}
  if(((Get-Content "$dest/CONFIG.NT" -Raw) -match '(?im)^dosonly\s*$') -ne ($dos -eq 1)){throw 'DOSONLY mismatch'}
  $actual=[IO.File]::ReadAllText("$install/SYSTEM.INI")
  foreach($n in 'INSIGNIA.386','NTVDM64.386'){
   $prefix=if(($mode -eq 2) -and ($n -eq 'INSIGNIA.386' -or $drivers)){''}else{';'}
   if(@([regex]::Matches($actual,[regex]::Escape($n))).Count -ne 1 -or ![regex]::IsMatch($actual,'(?m)^'+[regex]::Escape("${prefix}device=$install\PATCH\$n")+'\r?$')){throw "Duplicate/wrong driver state $n"}
  }
  $unrelated=[regex]::Replace($actual,'(?im)^;?device=.*(?:INSIGNIA|NTVDM64)\.386\r?\n','')
  if($unrelated.Replace("`r`n","`n") -cne $ini.Replace("`r`n","`n")){throw 'Unrelated INI changed'}
  if(Test-Path "$dest/WINCFG"){throw 'Staging retained'}
 }
 # Valid choices preserve the selected child's exit status.
 [IO.File]::WriteAllText("$bin/run16.cmd","@echo off`r`nexit /b 37`r`n")
 [IO.File]::WriteAllText("$root/input.txt","1`r`n0`r`n")
 & cmd.exe /d /c "`"$dest/WIN.CMD`" < `"$root/input.txt`"" | Out-Null
 if($LASTEXITCODE -ne 37){throw 'Launcher lost child exit code'}
 [IO.File]::WriteAllText("$bin/run16.cmd","@echo off`r`necho launch>>`"$root/launched.txt`"`r`nexit /b 0`r`n")
 $count=@(Get-Content "$root/launched.txt").Count
 [IO.File]::WriteAllText("$root/input.txt","9`r`n")
 & cmd.exe /d /c "`"$dest/WIN.CMD`" < `"$root/input.txt`"" | Out-Null
 if($LASTEXITCODE -ne 4 -or @(Get-Content "$root/launched.txt").Count -ne $count){throw 'Invalid choice launched Windows'}
 [IO.File]::WriteAllText("$install/SYSTEM.INI","[boot]`r`nkeep=value`r`n")
 [IO.File]::WriteAllText("$root/input.txt","1`r`n0`r`n")
 & cmd.exe /d /c "`"$dest/WIN.CMD`" < `"$root/input.txt`"" | Out-Null
 if($LASTEXITCODE -ne 5 -or @(Get-Content "$root/launched.txt").Count -ne $count){throw 'Invalid INI launched stale profile'}
 foreach($f in $before.Keys){if((Get-FileHash $f).Hash -ne $before[$f]){throw "Guest changed: $f"}}
 foreach($n in 'WINSTD.CMD','WIN386.CMD','WINSTD.PIF','WIN386.PIF','MULTITASK.CMD','MULTITASK.HASH','MULTITASK.INI.NEW','MULTITASK.INI.BAK','WINCFG'){if(Test-Path "$dest/$n"){throw "Unexpected output $n"}}
 'PASS dynamic six-profile selection, repeated driver toggle, generated PIF/NT files, preserved INI/guest and failure without launch; Setup/run16 are shims'
} finally {$env:PATH=$oldPath;$env:NTVDM_SETUP_TEST_NO_PAUSE=$oldPause}