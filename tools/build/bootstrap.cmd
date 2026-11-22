@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..\..") do set "NTVDM_ROOT=%%~fI"
if not defined NTVDM_VSDEVCMD set "NTVDM_VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
if not exist "%NTVDM_VSDEVCMD%" (
  echo Missing Visual Studio toolchain: "%NTVDM_VSDEVCMD%" 1>&2
  exit /b 2
)
call "%NTVDM_VSDEVCMD%" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cmake -S "%~dp0native" -B "%NTVDM_ROOT%\build\.ninja\build-tools" -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\build-tools"
exit /b %errorlevel%
