@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~4"=="" exit /b 2
if not "%~2"=="wow-startup-failure" if not "%~2"=="wow-shell-exception" if not "%~2"=="wow-common-return" if not "%~2"=="win101-control-fixtures" if not "%~2"=="ccpu-header-selection" if not "%~2"=="wow-message-transaction" if not "%~2"=="wow-attach-failure" if not "%~2"=="wow-missing-init" if not "%~2"=="wow-callback-abort" if not "%~2"=="media-owner" if not "%~2"=="command-redirection" if not "%~2"=="crt-contracts" if not "%~2"=="crt-missing" if not "%~2"=="crt-duplicate" if not "%~2"=="wow-page-cost" if not "%~2"=="serial-discovery" if not "%~2"=="win16-message" if not "%~2"=="win16-message-guest" if not "%~2"=="win16-print" if not "%~2"=="win16-ole-peer" if not "%~2"=="wow-callback-cost" if not "%~2"=="wow-graph-cost" if not "%~3"=="" exit /b 2
set "SERIAL_OPTION="
set "MEDIA_OPTION=-DNTVDM_MEDIA_ORIGINAL_ROOT="
if "%~2"=="media-owner" goto media_arguments
if "%~2"=="win16-ole-peer" goto ole_arguments
if "%~2"=="win16-print" goto print_arguments
if "%~2"=="win16-message" goto message_arguments
if "%~2"=="win16-message-guest" goto message_arguments
if not "%~2"=="serial-discovery" goto arguments
if "%~3"=="" exit /b 2
for /f "delims=0123456789" %%I in ("%~3") do exit /b 2
set "SERIAL_MASK=%~3"
if not "%SERIAL_MASK:~3%"=="" exit /b 2
if not "%SERIAL_MASK%"=="0" if "%SERIAL_MASK:~0,1%"=="0" exit /b 2
if %SERIAL_MASK% GTR 511 exit /b 2
set "SERIAL_OPTION=-DNTVDM_SERIAL_MASK=%SERIAL_MASK%"
goto arguments
:message_arguments
if "%~3"=="" exit /b 2
for /f "delims=0123456789" %%I in ("%~3") do exit /b 2
set "MESSAGE_MASK=%~3"
if not "%MESSAGE_MASK:~4%"=="" exit /b 2
if not "%MESSAGE_MASK%"=="0" if "%MESSAGE_MASK:~0,1%"=="0" exit /b 2
if %MESSAGE_MASK% GTR 8191 exit /b 2
set "SERIAL_OPTION=-DNTVDM_MESSAGE_MASK=%MESSAGE_MASK%"
goto arguments
:print_arguments
if "%~3"=="" exit /b 2
set SERIAL_OPTION="-DNTVDM_PRINT_OUTPUT_DIR=%~3"
goto arguments
:media_arguments
if "%~3"=="" exit /b 2
if not exist "%~3\dos\dem\demgset.c" exit /b 2
if not exist "%~3\dos\dem\dosdef.h" exit /b 2
set MEDIA_OPTION="-DNTVDM_MEDIA_ORIGINAL_ROOT=%~3"
goto arguments
:ole_arguments
if "%~3"=="" exit /b 2
if not exist "%~3\wow32.dll.map" exit /b 2
if not exist "%~3\wow32.dll" exit /b 2
set SERIAL_OPTION="-DNTVDM_OLE_PROVIDER=%~3"
:arguments
if not "%~1"=="x86" if not "%~1"=="x64" exit /b 2
set "TARGET=%~2"
if not defined TARGET set "TARGET=guest-memory"
set "SELECTED="
if "%TARGET%"=="wow-copydata-lifetime" set "SELECTED=1"
if "%TARGET%"=="wow-copydata-lifetime" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-common-dialog" set "SELECTED=1"
if "%TARGET%"=="wow-common-dialog" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-callback-abort-ccpu" set "SELECTED=1"
if "%TARGET%"=="wow-callback-abort-ccpu" if not "%~1"=="x86" exit /b 2
for %%T in (wow-creation wow-creation-hook wow-creation-menu wow-creation-timer wow-creation-idle wow-creation-common-dialog) do if "%TARGET%"=="%%T" set "SELECTED=1"
if "%TARGET:~0,12%"=="wow-creation" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-bitmap" set "SELECTED=1"
if "%TARGET%"=="wow-bitmap" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-accelerator" set "SELECTED=1"
if "%TARGET%"=="wow-accelerator" if not "%~1"=="x86" exit /b 2
for %%T in (softpc-observer-error software-video-publisher) do if "%TARGET%"=="%%T" set "SELECTED=1"
if "%TARGET%"=="softpc-contracts" set "SELECTED=1"
if "%TARGET%"=="softpc-contracts" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="serial-baud" set "SELECTED=1"
if "%TARGET%"=="serial-baud" if not "%~1"=="x86" exit /b 2
for %%T in (redirector-remote-name redirector-server-enum) do if "%TARGET%"=="%%T" set "SELECTED=1"
if "%TARGET%"=="redirector-remote-name" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="redirector-server-enum" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="video7-packed-painter" set "SELECTED=1"
if "%TARGET%"=="video7-packed-painter" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="planar-painter" set "SELECTED=1"
if "%TARGET%"=="planar-painter" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="ntvwm-modern-edit-mouse" set "SELECTED=1"
if "%TARGET%"=="ntvwm-modern-edit-mouse" if not "%~1"=="x64" exit /b 2
if "%TARGET%"=="niddb-no-retry" set "SELECTED=1"
if "%TARGET%"=="niddb-no-retry" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="dpmi-buffer-state" set "SELECTED=1"
if "%TARGET%"=="dpmi-buffer-state" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="dem-directory-reset" set "SELECTED=1"
if "%TARGET%"=="dem-directory-reset" if not "%~1"=="x86" exit /b 2
for %%I in (console-graphics-publication console-input-batch) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="console-input-batch" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="media-owner" set "SELECTED=1"
if "%TARGET%"=="media-owner" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-thunk-nls" set "SELECTED=1"
if "%TARGET%"=="wow-thunk-nls" if not "%~1"=="x86" exit /b 2
for %%I in (crt-contracts crt-missing crt-duplicate) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (crt-contracts crt-missing crt-duplicate) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
for %%I in (win31-matrix win31-dosonly win31-con-trace) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (win31-matrix win31-dosonly win31-con-trace) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
for %%I in (wow-sol-gameplay wow-sol-options wow-write-lifecycle) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (wow-sol-gameplay wow-sol-options wow-write-lifecycle) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-page-cost-unit" set "SELECTED=1"
if "%TARGET%"=="wow-page-cost-unit" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-page-cost" set "SELECTED=1"
if "%TARGET%"=="wow-page-cost" if not "%~1"=="x86" exit /b 2
for %%I in (wow-multi-dialog wow-observer-probes) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (wow-multi-dialog wow-observer-probes) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="logon-environment" set "SELECTED=1"
if "%TARGET%"=="wow-latency-probes" set "SELECTED=1"
if "%TARGET%"=="wow-latency-probes" if not "%~1"=="x86" exit /b 2
for %%I in (wow-mine-host wow-mine-probes) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (wow-mine-host wow-mine-probes) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
for %%I in (wow-idle-hosts wow-idle-probes) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (wow-idle-hosts wow-idle-probes) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-file-cache" set "SELECTED=1"
if "%TARGET%"=="wow-file-cache" if not "%~1"=="x86" exit /b 2
for %%I in (wow-message-external wow-message-cross wow-message-pending) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="wow-edit-contracts" set "SELECTED=1"
if "%TARGET%"=="wow-edit-contracts" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="wow-clipboard-probes" set "SELECTED=1"
if "%TARGET%"=="wow-clipboard-probes" if not "%~1"=="x86" exit /b 2
for %%I in (wow-callback-cost wow-callback-cost-unit wow-graph-cost wow-graph-cost-unit) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (wow-callback-cost wow-callback-cost-unit wow-graph-cost wow-graph-cost-unit) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
for %%I in (win16-shell-probes win16-ole-peer) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (win16-shell-probes win16-ole-peer) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
if "%TARGET%"=="win16-print" set "SELECTED=1"
if "%TARGET%"=="win16-print" if not "%~1"=="x86" exit /b 2
for %%I in (win16-message win16-message-guest) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (win16-message win16-message-guest) do if "%TARGET%"=="%%I" if not "%~1"=="x86" exit /b 2
for %%I in (guest-memory redirector-async guest-location wow-task-frame wow-callback-frame xms-mapped redirector-contract xms-startup dem-file-lifecycle redirector-guest-copy) do if "%TARGET%"=="%%I" set "SELECTED=1"
for %%I in (guest-probes guest-redir guest-video-int10 guest-video-direct guest-video-graphics guest-video-handshake guest-system guest-keymouse guest-bios guest-support guest-disks guest-comms guest-comms-medium guest-dem guest-command guest-vdd command-tools) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="memory-profiles" set "SELECTED=1"
if "%TARGET%"=="system-fixtures" set "SELECTED=1"
if "%TARGET%"=="redirector-lifecycle" set "SELECTED=1"
if "%TARGET%"=="package-resources" set "SELECTED=1"
if "%TARGET%"=="presentation-font" set "SELECTED=1"
if "%TARGET%"=="thread-lifecycle" set "SELECTED=1"
if "%TARGET%"=="keyboard-contracts" set "SELECTED=1"
if "%TARGET%"=="win95-probes" if "%~1"=="x86" set "SELECTED=1"
for %%I in (wow-callback-abort wow-attach-failure wow-missing-init wow-message-transaction wow-window-sizing wow-startup-failure wow-shell-exception wow-common-return) do if "%TARGET%"=="%%I" if "%~1"=="x86" set "SELECTED=1"
if "%TARGET%"=="window-contracts" set "SELECTED=1"
if "%TARGET%"=="frontend-library" if "%~1"=="x86" set "SELECTED=1"
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
if "%TARGET%"=="win16-discovery" set "SELECTED=1"
if "%TARGET%"=="win16-discovery" if not "%~1"=="x86" exit /b 2
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
if "%TARGET%"=="dos-identity-observer" if "%~1"=="x86" set "SELECTED=1"
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
for %%I in (window-render worker-primitives worker-primitives-high-resolution) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="ccpu-header-selection" if "%~1"=="x86" set "SELECTED=1"
if "%TARGET%"=="win101-control-fixtures" if "%~1"=="x86" set "SELECTED=1"
for %%I in (base-classifier-path base-resource base-vdm-config) do if "%TARGET%"=="%%I" set "SELECTED=1"
if "%TARGET%"=="blocked-video-tick" set "SELECTED=1"
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
for %%I in (wow-message-external wow-message-cross wow-message-pending) do if "%TARGET%"=="%%I" goto message_native
if "%TARGET%"=="ccpu-header-selection" goto worker_existing
if "%TARGET%"=="win101-control-fixtures" goto worker_existing
if "%TARGET%"=="oem-tests" goto worker
if "%TARGET:~0,12%"=="wow-creation" goto wowcallback
if "%TARGET%"=="wow-bitmap" goto wowcallback
if "%TARGET%"=="wow-callback-cost" goto wowcallback
if "%TARGET%"=="wow-graph-cost" goto wowcallback
for %%I in (wow-callback-abort wow-attach-failure wow-missing-init wow-message-transaction wow-window-sizing wow-startup-failure wow-shell-exception wow-common-return) do if "%TARGET%"=="%%I" goto wowcallback
for %%I in (crt-contracts crt-missing crt-duplicate) do if "%TARGET%"=="%%I" goto wowcallback
if "%TARGET%"=="pic-bios-contracts" goto worker
if "%TARGET%"=="command-keyboard-shadow" goto worker
if "%TARGET%"=="native-input-contracts" goto inputtests
if "%TARGET%"=="wow-edit-contracts" goto inputtests
if "%TARGET%"=="wow-file-cache" goto inputtests
if "%TARGET%"=="command-initialization" goto worker
if "%TARGET%"=="command-launch" goto worker
if "%TARGET%"=="frontend-library" goto worker
if "%TARGET%"=="worker-diagnostics" goto worker
if "%TARGET%"=="dos-identity-observer" goto worker
if "%TARGET%"=="wow-page-cost" if not "%~3"=="" goto worker_existing
if "%TARGET%"=="wow-page-cost" goto worker
if "%TARGET%"=="native-diagnostics" goto nativediagnostics
if "%TARGET%"=="frame-codepage" if "%~1"=="x86" goto worker
if "%TARGET%"=="frame-codepage" goto nativediagnostics
for %%I in (dpmi-contracts ccpu-port-privilege dpmi-swint-selected pm-entry-contract) do if "%TARGET%"=="%%I" goto worker
for %%I in (cpu-contracts ccpu-neg64 ccpu-neg64-negative ccpu-leaf ccpu-leaf-negative ccpu-notification native-int-prefix) do if "%TARGET%"=="%%I" goto worker
if "%TARGET%"=="dem-declarations" goto worker
if "%TARGET%"=="console-contracts" goto worker
if "%TARGET%"=="host-thread-boundaries" goto worker
for %%T in (redirector-remote-name redirector-server-enum) do if "%TARGET%"=="%%T" goto worker
if "%TARGET%"=="softpc-contracts" goto worker
if "%TARGET%"=="video7-packed-painter" goto worker
if "%TARGET%"=="planar-painter" goto worker
if "%TARGET%"=="dem-directory-reset" goto worker
if "%TARGET%"=="wow-callback-abort-ccpu" goto worker
if "%TARGET%"=="ccpu-callback" goto worker
if "%TARGET%"=="wow-controls" goto controls
if "%TARGET%"=="command-redirection" if not "%~3"=="" goto worker_existing
if "%TARGET%"=="command-redirection" goto worker
for %%I in (redirector-providers redirector-capacity) do if "%TARGET%"=="%%I" goto worker
cmake -S "%NTVDM_ROOT%\tools\build\fixtures" -B "%NTVDM_ROOT%\build\.ninja\fixtures-%~1" -G Ninja -DCMAKE_BUILD_TYPE=Release -DNTVDM_ARCH=%~1 -DNTVDM_USE_CCACHE=ON %SERIAL_OPTION% %MEDIA_OPTION%
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
:worker_existing
if not exist "%~3\CMakeCache.txt" exit /b 2
cmake --build "%~3" --target %TARGET%
exit /b %errorlevel%
:controls
cmake -S "%NTVDM_ROOT%\tools\build\wow-controls" -B "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\call-boundaries-native" -G Ninja -DCMAKE_BUILD_TYPE=Release -DNTVDM_USE_CCACHE=ON
if errorlevel 1 exit /b %errorlevel%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\call-boundaries-native" --target wow-controls
exit /b %errorlevel%
:wowcallback
set "CALLBACK_ROOT=%~3"
if not defined CALLBACK_ROOT set "CALLBACK_ROOT=%NTVDM_ROOT%\build\.ninja\msvc-x86-release\wow32"
if not exist "%CALLBACK_ROOT%\CMakeCache.txt" exit /b 2
cmake --build "%CALLBACK_ROOT%" --target %TARGET%
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
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-x86-release\wow32" --target %TARGET%
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
:message_native
pushd "%NTVDM_ROOT%"
if errorlevel 1 exit /b %errorlevel%
cmake --preset msvc-%~1-release-ccache
set "result=%errorlevel%"
popd
if not "%result%"=="0" exit /b %result%
cmake --build "%NTVDM_ROOT%\build\.ninja\msvc-%~1-release" --target %TARGET% --parallel 8
exit /b %errorlevel%
