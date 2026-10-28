@echo off
setlocal
if "%~1"=="" (
  echo Usage: %~nx0 OUTPUT_DIRECTORY
  exit /b 2
)
for %%I in ("%~dp0..\..\..") do set "repo=%%~fI"
for %%I in ("%~1") do set "out=%%~fI"
if not exist "%out%" mkdir "%out%" || exit /b 1
set "vs=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
if not exist "%vs%" (
  echo MSVC Build Tools not found: "%vs%"
  exit /b 1
)
call "%vs%" -arch=x86 -host_arch=x64 >nul || exit /b 1
set "worker=%repo%\build\.ninja\msvc-x86-release\worker\obj"
cl /nologo /MT /I"%repo%\src" /I"%repo%\src\ntvdm-exe\session" "%~dp0fdc-bound-session-scalar-probe.c" "%worker%\ntvdm\session\session.obj" "%worker%\ntvdm\session\guest_memory_lease.obj" "%worker%\adapter-basesrv\mvdm_command_guest_state.obj" "%worker%\adapter-softpc\mvdm_guest_location.obj" /Fe:"%out%\probe.exe" /Fo:"%out%\probe.obj" /link /OPT:REF
if errorlevel 1 exit /b 1
"%out%\probe.exe"
exit /b %errorlevel%
