@echo off
setlocal
cd /d "%~dp0..\..\.."
if not exist build\t478-s11 mkdir build\t478-s11
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul || exit /b 1
cl /nologo /MT artifacts/mvdm-audit/shard-3/xms-dispatch-s11-probe.c build/.ninja/msvc-x86-release/worker/obj/xms/xmsdisp.obj /Fe:build/t478-s11/xms-service-probe.exe /Fo:build/t478-s11/xms-dispatch-probe.obj /link /OPT:REF || exit /b 1
build\t478-s11\xms-service-probe.exe
exit /b %errorlevel%
