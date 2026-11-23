@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~4"=="" exit /b 2
if not "%~2"=="serial-discovery" if not "%~3"=="" exit /b 2
set "SERIAL_OPTION="
if not "%~2"=="serial-discovery" goto arguments
if "%~3"=="" exit /b 2
for /f "delims=0123456789" %%I in ("%~3") do exit /b 2
set "SERIAL_MASK=%~3"
if not "%SERIAL_MASK:~3%"=="" exit /b 2
if not "%SERIAL_MASK%"=="0" if "%SERIAL_MASK:~0,1%"=="0" exit /b 2
if %SERIAL_MASK% GTR 511 exit /b 2
set "SERIAL_OPTION=-DNTVDM_SERIAL_MASK=%SERIAL_MASK%"
:arguments
if not "%~1"=="x86" if not "%~1"=="x64" exit /b 2
set "TARGET=%~2"
if not defined TARGET set "TARGET=guest-memory"
set "SELECTED="
for %%I in (guest-memory redirector-async guest-location wow-task-frame wow-callback-frame xms-mapped redirector-contract xms-startup dem-file-lifecycle redirector-guest-copy) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (guest-probes guest-redir guest-video-int10 guest-video-direct guest-video-graphics guest-video-handshake guest-system guest-keymouse guest-bios guest-support guest-disks guest-comms guest-comms-medium guest-dem guest-command guest-vdd command-tools) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="memory-profiles" set "SELECTED=1"
if "%TARGET%"=="system-fixtures" set "SELECTED=1"
if "%TARGET%"=="redirector-lifecycle" set "SELECTED=1"
if "%TARGET%"=="package-resources" set "SELECTED=1"
if "%TARGET%"=="presentation-font" set "SELECTED=1"
if "%TARGET%"=="thread-lifecycle" set "SELECTED=1"
if "%TARGET%"=="keyboard-contracts" set "SELECTED=1"
if "%TARGET%"=="window-contracts" set "SELECTED=1"
if "%TARGET%"=="native-payload-probes" set "SELECTED=1"
if "%TARGET%"=="native-guest-output" set "SELECTED=1"
if "%TARGET%"=="native-guest-output" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="console-api-probes" set "SELECTED=1"
if "%TARGET%"=="clipboard-epoch" set "SELECTED=1"
if "%TARGET%"=="cpu-guest-probes" set "SELECTED=1"
if "%TARGET%"=="cpu-guest-probes" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="pic-bios-contracts" set "SELECTED=1"
if "%TARGET%"=="command-keyboard-shadow" set "SELECTED=1"
if "%TARGET%"=="command-keyboard-shadow" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="serial-discovery" set "SELECTED=1"
if "%TARGET%"=="serial-discovery" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="win16-probes" set "SELECTED=1"
if "%TARGET%"=="win16-probes" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="pic-bios-contracts" if not "%~1"=="x86" exit /b 2
for %%I in (host-cpu-endian host-cpu-endian-default host-cpu-endian-reject) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="console-api-probes" if not "%~1"=="x64" exit /b 2
if "%TARGET%"=="dos-observation-probes" set "SELECTED=1"
if "%TARGET%"=="dos-observation-probes" if not "%~1"=="x64" exit /b 2
if "%TARGET%"=="frame-codepage" set "SELECTED=1"
if "%TARGET%"=="private-mouse-tools" set "SELECTED=1"
if "%TARGET%"=="private-mouse-tools" if not "%~1"=="x64" exit /b 2
if "%TARGET%"=="native-input-contracts" set "SELECTED=1"
if "%TARGET%"=="native-input-contracts" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="keyboard-contracts" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="console-contracts" set "SELECTED=1"
if "%TARGET%"=="host-thread-boundaries" set "SELECTED=1"
if "%TARGET%"=="ccpu-callback" set "SELECTED=1"
if "%TARGET%"=="wow-controls" set "SELECTED=1"
if "%TARGET%"=="wow-controls" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="ccpu-callback" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="host-thread-boundaries" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="console-contracts" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="observation-tools" set "SELECTED=1"
if "%TARGET%"=="oem-tests" set "SELECTED=1"
if "%TARGET%"=="oem-tests" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="command-initialization" set "SELECTED=1"
if "%TARGET%"=="command-launch" set "SELECTED=1"
if "%TARGET%"=="worker-diagnostics" set "SELECTED=1"
if "%TARGET%"=="native-diagnostics" set "SELECTED=1"
if "%TARGET%"=="native-diagnostics" if not "%~1"=="x64" exit /b 2
for %%I in (dpmi-contracts ccpu-port-privilege dpmi-swint-selected pm-entry-contract) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (dpmi-contracts ccpu-port-privilege dpmi-swint-selected pm-entry-contract) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
for %%I in (cpu-contracts ccpu-neg64 ccpu-neg64-negative ccpu-leaf ccpu-leaf-negative ccpu-notification native-int-prefix) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (cpu-contracts ccpu-neg64 ccpu-neg64-negative ccpu-leaf ccpu-leaf-negative ccpu-notification native-int-prefix) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="worker-diagnostics" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="command-launch" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="command-initialization" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="dem-declarations" set "SELECTED=1"
if "%TARGET%"=="dem-declarations" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="command-redirection" set "SELECTED=1"
if "%TARGET%"=="command-redirection" if not "%~1"=="x86" exit /b 2
for %%I in (redirector-providers redirector-capacity) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (redirector-providers redirector-capacity) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="suballoc-lifecycle" set "SELECTED=1"
if "%TARGET%"=="suballoc-lifecycle" if not "%~1"=="x86" exit /b 2
if not defined SELECTED exit /b 2
if "%TARGET%"=="xms-mapped" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="xms-startup" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="command-tools" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="guest-keymouse" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="memory-profiles" if not "%~1"=="x86" exit /b 2
for %%I in ("%~dp0..") do set "NTVDM_ROOT=%%~fI"
if not defined NTVDM_VSDEVCMD set "NTVDM_VSDEVCMD=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
if "%TARGET%"=="window-contracts" call "%~dp0build\bootstrap.cmd"
if "%TARGET%"=="window-contracts" if errorlevel 1 exit /b %errorlevel%
call "%NTVDM_VSDEVCMD%" -arch=%~1 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
if "%TARGET%"=="oem-tests" goto worker
if "%TARGET%"=="pic-bios-contracts" goto worker
if "%TARGET%"=="command-keyboard-shadow" goto worker
if "%TARGET%"=="native-input-contracts" goto inputtests
if "%TARGET%"=="command-initialization" goto worker
if "%TARGET%"=="command-launch" goto worker
if "%TARGET%"=="worker-diagnostics" goto worker
if "%TARGET%"=="native-diagnostics" goto nativediagnostics
if "%TARGET%"=="frame-codepage" if "%~1"=="x86" goto worker
if "%TARGET%"=="frame-codepage" goto nativediagnostics
for %%I in (dpmi-contracts ccpu-port-privilege dpmi-swint-selected pm-entry-contract) do if "%TARGET%"=="%%I" goto worker
for %%I in (cpu-contracts ccpu-neg64 ccpu-neg64-negative ccpu-leaf ccpu-leaf-negative ccpu-notification native-int-prefix) do if "%TARGET%"=="%%I" goto worker
if "%TARGET%"=="dem-declarations" goto worker
if "%TARGET%"=="console-contracts" goto worker
if "%TARGET%"=="host-thread-boundaries" goto worker
if "%TARGET%"=="ccpu-callback" goto worker
if "%TARGET%"=="wow-controls" goto controls
if "%TARGET%"=="command-redirection" goto worker
for %%I in (redirector-providers redirector-capacity) do if "%TARGET%"=="%%I" goto worker
cmake -S "%NTVDM_ROOT%\tools\build\fixtures" -B "%NTVDM_ROOT%\build\.ninja\fixtures-%~1" -G Ninja -DCMAKE_BUILD_TYPE=Release -DNTVDM_ARCH=%~1 -DNTVDM_USE_CCACHE=ON %SERIAL_OPTION%
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\fixtures-%~1" --target %TARGET%
exit /b %errorlevel%
:worker
pushd "%NTVDM_ROOT%"
if errorlevel 1 exit /b %errorlevel%
cmake --preset msvc-x86-release-ccache
set "result=%errorlevel%"
popd
if not "%result%"=="0" exit /b %result%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\worker" --target %TARGET%
exit /b %errorlevel%
:controls
cmake -S "%NTVDM_ROOT%\tools\build\wow-controls" -B "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\call-boundaries-native" -G Ninja -DCMAKE_BUILD_TYPE=Release -DNTVDM_USE_CCACHE=ON
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\call-boundaries-native" --target wow-controls
exit /b %errorlevel%
:inputtests
pushd "%NTVDM_ROOT%"
if errorlevel 1 exit /b %errorlevel%
cmake --preset msvc-x86-release-ccache
set "result=%errorlevel%"
popd
if not "%result%"=="0" exit /b %result%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x86-release" --target wow32
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\wow32" --target native-input-contracts
exit /b %errorlevel%
:nativediagnostics
pushd "%NTVDM_ROOT%"
if errorlevel 1 exit /b %errorlevel%
cmake --preset msvc-x64-release-ccache
set "result=%errorlevel%"
popd
if not "%result%"=="0" exit /b %result%
if "%TARGET%"=="frame-codepage" cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x64-release\frontend" --target frame-codepage
if "%TARGET%"=="frame-codepage" exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x64-release\frontend" --target native-diagnostics
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x64-release\service" --target native-diagnostics
exit /b %errorlevel%
