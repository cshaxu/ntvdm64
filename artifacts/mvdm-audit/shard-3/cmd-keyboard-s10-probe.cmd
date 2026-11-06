@echo off
setlocal
cd /d "%~dp0..\..\.."
if not exist build\t478-s10\system32 mkdir build\t478-s10\system32
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul || exit /b 1
set w=build\.ninja\msvc-x86-release\worker
cl /nologo /MT /W4 /Isrc artifacts/mvdm-audit/shard-3/cmd-keyboard-s10-probe.c %w%\obj\command\cmdkeyb.obj %w%\obj\adapter-softpc\mvdm_shadow_registry.obj %w%\common-root.lib advapi32.lib user32.lib legacy_stdio_definitions.lib /Fe:build/t478-s10/system32/keyboard-probe.exe /Fo:build/t478-s10/keyboard-probe.obj /link /OPT:REF || exit /b 1
build\t478-s10\system32\keyboard-probe.exe "%CD%\build\t478-s9\package-final2\system32"
exit /b %errorlevel%
