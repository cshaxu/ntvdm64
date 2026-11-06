@echo off
setlocal
cd /d "%~dp0..\.."
if not exist build\t478-s9 mkdir build\t478-s9
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul || exit /b 1
set w=build\.ninja\msvc-x86-release\worker
cl /nologo /MT /Isrc test\integration\fdc_command_session_test.c %w%\obj\ntvdm\session\session.obj %w%\obj\ntvdm\session\guest_memory_lease.obj %w%\obj\adapter-basesrv\mvdm_command_guest_state.obj %w%\obj\adapter-softpc\mvdm_guest_location.obj %w%\common-worker-thread.lib %w%\lib-thread.lib /Fe:build\t478-s9\fdc-session-test.exe /Fo:build\t478-s9\fdc-session-test.obj /link /OPT:REF || exit /b 1
build\t478-s9\fdc-session-test.exe
exit /b %errorlevel%
