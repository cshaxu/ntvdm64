@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~3"=="" exit /b 2
if not "%~1"=="x86" if not "%~1"=="x64" exit /b 2
set "TARGET=%~2"
if not defined TARGET set "TARGET=guest-memory"
if not "%TARGET%"=="guest-memory" if not "%TARGET%"=="redirector-async" exit /b 2
for %%I in ("%~dp0..") do set "NTVDM_ROOT=%%~fI"
if not defined NTVDM_VSDEVCMD set "NTVDM_VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
call "%NTVDM_VSDEVCMD%" -arch=%~1 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cmake -S "%NTVDM_ROOT%\tools\build\fixtures" -B "%NTVDM_ROOT%\build\.ninja\fixtures-%~1" -G Ninja -DCMAKE_BUILD_TYPE=Release -DNTVDM_ARCH=%~1 -DNTVDM_USE_CCACHE=ON
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\fixtures-%~1" --target %TARGET%
exit /b %errorlevel%
