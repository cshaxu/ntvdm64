@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~2"=="" exit /b 2
if not "%~3"=="" exit /b 2
for %%I in ("%~dp0..") do set "NTVDM_ROOT=%%~fI"
set "provider=%~f1"
set "output=%~f2"
if not exist "%provider%\build.ninja" exit /b 2
if not exist "%output%\.message-control-owned" exit /b 2
if not defined NTVDM_VSDEVCMD set "NTVDM_VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
call "%~dp0build-addon.cmd" hash
if errorlevel 1 exit /b %errorlevel%
set "normal="
"%NTVDM_ROOT%\build\.ninja\addon\hash\hash.exe" "%provider%\wow32.dll" >"%output%\normal.sha256"
if errorlevel 1 exit /b %errorlevel%
set /p "normal=" <"%output%\normal.sha256"
if not "%normal%"=="59E008EC1F09A2228FCF87FDB346F205473EB7B6DEE55ED39983C866A96C263C" (
 echo P21 control requires its exact unmodified provider objects 1>&2
 exit /b 2
)
call "%NTVDM_VSDEVCMD%" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
ninja.exe -C "%provider%" -t commands wow32.dll >"%output%\provider-commands.txt"
if errorlevel 1 exit /b %errorlevel%
git -C "%NTVDM_ROOT%" show 4b8a7c8ae6eaaca3350d640650dd9d57201fd581:src/mvdm-host/wow32/wmdisp32.c >"%output%\message_original_owner.c"
if errorlevel 1 exit /b %errorlevel%
set "compile="
set "link="
for /f "usebackq delims=" %%L in (`findstr /r /c:"/wmdisp32.c$" "%output%\provider-commands.txt"`) do set "compile=%%L"
for /f "usebackq delims=" %%L in (`findstr /b /c:"link /nologo /machine:x86 /dll " "%output%\provider-commands.txt"`) do set "link=%%L"
if not defined compile exit /b 2
if not defined link exit /b 2
setlocal EnableDelayedExpansion
rem Only the sealed pre-CMake owner profile is accepted here.
set "prefix=!compile:/Fo=|!"
if "!prefix!"=="!compile!" exit /b 2
for /f "tokens=1 delims=|" %%L in ("!prefix!") do set "compile=%%L/I"%output%" /Fo"%output%\message.obj" "%NTVDM_ROOT%\test\observation\wow_message_transaction_provider.c""
set "replacement=!link:obj/wmdisp32.obj=PLACEHOLDER_MESSAGE_OBJECT!"
if "!replacement!"=="!link!" exit /b 2
set "replacement=!replacement:PLACEHOLDER_MESSAGE_OBJECT="%output%\message.obj"!"
set "replacement=!replacement:/map:wow32.dll.map=/map:"%output%\WOW32.map"!"
set "replacement=!replacement:/out:wow32.dll=/out:"%output%\WOW32.DLL" /implib:"%output%\WOW32.lib"!"
>"%output%\compile.cmd" echo @!compile!
>"%output%\link.cmd" echo @!replacement!
endlocal
pushd "%provider%"
if errorlevel 1 exit /b %errorlevel%
call "%output%\compile.cmd"
set "result=%errorlevel%"
if not "%result%"=="0" goto finished
call "%output%\link.cmd"
set "result=%errorlevel%"
:finished
popd
exit /b %result%
