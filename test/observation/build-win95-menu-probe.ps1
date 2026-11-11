param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build output required'}
New-Item -ItemType Directory $out|Out-Null
$cmd=@"
@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul || exit /b 1
cd /d "$out"
cl.exe /nologo /c /TC /O1 /Oi /GS- /Zl /arch:IA32 /D_WIN32_WINNT=0x0400 /DWINVER=0x0400 /Fomenu.obj "$PSScriptRoot\win95_menu_probe.c" || exit /b 1
link.exe /nologo /nodefaultlib /entry:entry /subsystem:windows /osversion:4.0 /machine:x86 /dynamicbase:no /nxcompat:no /safeseh:no /stack:0x100000,0x1000 /out:MENU95.EXE /map:MENU95.MAP menu.obj kernel32.lib user32.lib advapi32.lib shell32.lib ole32.lib uuid.lib
exit /b %errorlevel%
"@
[IO.File]::WriteAllText("$out/build.cmd",$cmd,[Text.Encoding]::ASCII)
& cmd.exe /c "$out/build.cmd"
if($LASTEXITCODE){throw 'Guest menu witness build failed'}
# Modern LINK no longer accepts subsystem 4.0; set only this independently
# authored test executable's OS/subsystem version metadata, never a guest.
$path="$out/MENU95.EXE";$bytes=[IO.File]::ReadAllBytes($path)
$pe=[BitConverter]::ToInt32($bytes,60);$opt=$pe+24
if([BitConverter]::ToUInt16($bytes,$pe+4) -ne 0x14c){throw 'Expected original guest ISA'}
foreach($at in @(($opt+40),($opt+48))){[BitConverter]::GetBytes([uint16]4).CopyTo($bytes,$at);[BitConverter]::GetBytes([uint16]0).CopyTo($bytes,$at+2)}
[IO.File]::WriteAllBytes($path,$bytes)
@{sha256=(Get-FileHash $path).Hash;source=(Get-FileHash "$PSScriptRoot/win95_menu_probe.c").Hash;role='independent Win95 menu/COM/own canary lifecycle witness'}|ConvertTo-Json|Set-Content "$out/build.json"
