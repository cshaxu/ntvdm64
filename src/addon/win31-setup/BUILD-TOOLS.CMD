@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~2"=="" (echo Usage: BUILD-TOOLS.CMD OUTPUT-DIRECTORY BUILD-DIRECTORY& exit /b 64)
set "OUTPUT=%~f1"
set "BUILDROOT=%~f2"
for %%I in ("%~dp0..\..\..") do set "REPO=%%~fI"
if not exist "%BUILDROOT%\" mkdir "%BUILDROOT%" || exit /b 3
if not exist "%OUTPUT%\" mkdir "%OUTPUT%" || exit /b 3
"%REPO%\assets\release\PIF.EXE" create "%BUILDROOT%\template.pif" --title "" --program "" --directory "" --arguments "" --config "" --autoexec "" --close-on-exit --w386-max-xms 65535 --w386-flags 4099 --nt31-flags 1 || exit /b 4
call "%~dp0BUILD.CMD" "%BUILDROOT%\template.pif" "%OUTPUT%\PATCHSET.EXE" "%BUILDROOT%" || exit /b 5
call "%~dp0BUILD-SZDDPACK.CMD" "%OUTPUT%\SZDDPACK.EXE" "%BUILDROOT%" || exit /b 6
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul || exit /b 7
cl.exe /nologo /c /TC /std:c11 /W4 /MT /Fo"%BUILDROOT%\mediapatch.obj" "%~dp0mediapatch.c" || exit /b 8
link.exe /nologo /MACHINE:X64 /OUT:"%OUTPUT%\MEDIAPATCH.EXE" "%BUILDROOT%\mediapatch.obj" kernel32.lib bcrypt.lib || exit /b 9
exit /b 0
