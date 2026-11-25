@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..") do set "NTVDM_ROOT=%%~fI"
if not defined NTVDM_VSDEVCMD set "NTVDM_VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
if not "%~2"=="" exit /b 2
set "component=%~1"
if not defined component set "component=all"
set "selected="
for %%I in (all grp hash ini lnk patch31 patch95 path31 path95 pif szdd compat95 compat95-control mouse101 mouse31 mouse101-tests mouse31-tests insignia ntvdm64) do if "%component%"=="%%I" set "selected=1"
if not defined selected (
  echo Unknown addon target: "%component%" 1>&2
  exit /b 2
)
call "%NTVDM_VSDEVCMD%" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cmake -S "%NTVDM_ROOT%\tools\build\addon" -B "%NTVDM_ROOT%\build\.ninja\addon" -G Ninja -DCMAKE_BUILD_TYPE=Release -DNTVDM_USE_CCACHE=ON
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\addon" --target %component%
exit /b %errorlevel%
