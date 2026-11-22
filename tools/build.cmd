@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..") do set "NTVDM_ROOT=%%~fI"
if not defined NTVDM_VSDEVCMD set "NTVDM_VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
if not "%~2"=="" exit /b 2
if "%~1"=="x64" goto architecture
if "%~1"=="x86" goto architecture
if not "%~1"=="" (
  echo Usage: build.cmd [x64^|x86] 1>&2
  exit /b 2
)
call "%~f0" x64
if errorlevel 1 exit /b %errorlevel%
call "%~f0" x86
exit /b %errorlevel%
:architecture
call "%NTVDM_VSDEVCMD%" -arch=%~1 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
pushd "%NTVDM_ROOT%"
if errorlevel 1 exit /b %errorlevel%
cmake --preset msvc-%~1-release-ccache
if errorlevel 1 goto failed
if "%~1"=="x64" (
  cmake --build --preset msvc-x64-release-ccache --target native-worker native-frontend native-monitor native-launcher native-service hook64
) else (
  cmake --build --preset msvc-x86-release-ccache --target wow32
)
:failed
set "result=%errorlevel%"
popd
exit /b %result%
