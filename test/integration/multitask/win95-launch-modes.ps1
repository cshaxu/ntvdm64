param([Parameter(Mandatory)][string]$BuildRoot, [Parameter(Mandatory)][string]$Win95Source)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$source=(Resolve-Path $Win95Source).Path
if((Get-FileHash "$repo/assets/release/SETVER.EXE").Hash -ne (Get-FileHash "$repo/tools/win101-setup/SETVER.EXE").Hash){throw 'Released SETVER differs from the preserved original'}
New-Item -ItemType Directory $root|Out-Null
$oldPath=$env:PATH
try {
 $env:PATH="O:\winnt\system32;$oldPath"
 foreach($mode in 0,1){ foreach($drivers in 0,1){
  $target=Join-Path $root "w$mode$drivers";New-Item -ItemType Directory "$target/SYSTEM" -Force|Out-Null
  foreach($name in 'WIN.COM','SYSTEM/VMM32.VXD','SYSTEM/KRNL386.EXE'){Copy-Item "$source/$name" "$target/$name"}
  Copy-Item "$repo/assets/release/MOUSE31.DRV" "$target/SYSTEM/MOUSE.DRV"
  [IO.File]::WriteAllText("$target/SYSTEM.INI","[boot]`r`nmouse.drv=MOUSE.DRV`r`n[386Enh]`r`n",[Text.Encoding]::ASCII)
  $ini=(Get-FileHash "$target/SYSTEM.INI").Hash
  [IO.File]::WriteAllText("$root/input.txt","$target`r`n",[Text.Encoding]::ASCII)
  & cmd.exe /d /c "`"$repo/tools/win95-launch/APPLY.CMD`" < `"$root/input.txt`"" *> "$root/apply$mode.log"
  if($LASTEXITCODE){throw "Mode $mode apply failed; see apply$mode.log"}
  $shim=Join-Path $root "shim$mode$drivers";New-Item -ItemType Directory $shim|Out-Null
  [IO.File]::WriteAllText("$shim/run16.cmd","@echo off`r`nexit /b 0`r`n")
  [IO.File]::WriteAllText("$root/input.txt","$mode`r`n$drivers`r`n",[Text.Encoding]::ASCII)
  $env:PATH="$shim;$oldPath"
  & cmd.exe /d /c "`"$target/PATCH/WIN.CMD`" < `"$root/input.txt`"" *> "$root/select$mode$drivers.log"
  $env:PATH="O:\winnt\system32;$oldPath"
  if($LASTEXITCODE){throw "Profile $mode/$drivers selection failed"}
  $config=Get-Content "$target/PATCH/CONFIG.NT" -Raw
  if(($config -match '(?im)^dosonly\s*$') -ne ($mode -eq 1)){throw "Incorrect mode $mode profile"}
  foreach($name in 'INSIGNIA.386','NTVDM64.386'){
   $present=Test-Path "$target/PATCH/$name"
   if(!$present){throw "Incorrect mode $mode driver $name"}
   $iniText=Get-Content "$target/SYSTEM.INI" -Raw
   $prefix=if($name -eq 'INSIGNIA.386' -or $drivers){''}else{';'}
   if(![regex]::IsMatch($iniText,'(?m)^'+[regex]::Escape("${prefix}device=$target\PATCH\$name")+'\r?$')){throw "Wrong enabled state $mode/$drivers $name"}
   if($present -and (Get-FileHash "$target/PATCH/$name").Hash -ne (Get-FileHash "$repo/assets/release/$name").Hash){throw "Driver mismatch $name"}
  }
  [IO.File]::WriteAllText("$root/input.txt","$target`r`n",[Text.Encoding]::ASCII)
  & cmd.exe /d /c "`"$repo/tools/win95-launch/UNAPPLY.CMD`" < `"$root/input.txt`"" *> "$root/unapply$mode.log"
  if($LASTEXITCODE){throw "Mode $mode unapply failed; see unapply$mode.log"}
  if((Get-FileHash "$target/SYSTEM.INI").Hash -ne $ini){throw "Mode $mode failed to restore SYSTEM.INI"}
 }
 }
 $target=Join-Path $root 'existing';New-Item -ItemType Directory "$target/SYSTEM","$target/PATCH" -Force|Out-Null
 foreach($name in 'WIN.COM','SYSTEM/VMM32.VXD','SYSTEM/KRNL386.EXE'){Copy-Item "$source/$name" "$target/$name"}
 [IO.File]::WriteAllText("$target/SYSTEM.INI","[386Enh]`r`ndevice=PATCH\NTVDM64.386`r`n",[Text.Encoding]::ASCII)
 [IO.File]::WriteAllText("$target/PATCH/NTVDM64.386",'owner data')
 [IO.File]::WriteAllText("$root/input.txt","$target`r`n1`r`n",[Text.Encoding]::ASCII)
 & cmd.exe /d /c "`"$repo/tools/win95-launch/APPLY.CMD`" < `"$root/input.txt`"" *> "$root/existing.log"
 if($LASTEXITCODE -ne 5 -or [IO.File]::ReadAllText("$target/PATCH/NTVDM64.386") -ne 'owner data'){throw 'Existing multitask payload not preserved'}
 'PASS all four Win95 launch profiles, released drivers and SYSTEM.INI restore'
} finally {$env:PATH=$oldPath}
