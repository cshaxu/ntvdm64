param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
foreach($arch in @('x86','x64')){
 $dir=Join-Path $root $arch;$null=New-Item -ItemType Directory $dir
 @"
@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=$arch -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$dir"
cl /nologo /MT /O2 /W4 /WX /I"$repo/src" "$repo/src/run16-exe/launch_options.c" "$repo/tests/app/run16_launch_options_test.c" /Feoptions.exe
if errorlevel 1 exit /b %errorlevel%
options.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /O2 /W4 /WX /I"$repo/src" "$repo/src/common/codec/native_launch.c" "$repo/tests/app/window_style_packet_test.c" /Fepacket.exe
if errorlevel 1 exit /b %errorlevel%
packet.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /O2 /W4 /WX /I"$repo/src" /I"$repo/src/wow32-dll/include" "$repo/src/wow32-dll/source/wow_task_parent.c" "$repo/tests/app/wow_task_parent_test.c" /Feparent.exe
if errorlevel 1 exit /b %errorlevel%
parent.exe
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /O2 /W4 /WX /I"$repo/src" /I"$repo/src/ntvdm-exe/softpc/include" /I"$repo/src/ntvdm-exe/command/include" "$repo/src/ntvdm-exe/command/source/mvdm_command_native_child.c" "$repo/tests/app/dos_style_snapshot_test.c" /Fesnapshot.exe
if errorlevel 1 exit /b %errorlevel%
snapshot.exe
"@|Set-Content "$dir/run.cmd" -Encoding ascii
 & $env:ComSpec /d /c "$dir/run.cmd" *> "$dir/result.txt"
 if($LASTEXITCODE){throw "CLI test failed: $arch"}
}
'0'|Set-Content "$root/exit-code.txt"
