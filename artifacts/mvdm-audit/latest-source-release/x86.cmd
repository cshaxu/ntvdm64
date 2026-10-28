@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cmake --build --preset msvc-x86-release-ccache --target wow32
if errorlevel 1 exit /b %errorlevel%
call "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\worker\run-ninja-parallel.cmd" VDMREDIR.dll nthook32.dll
exit /b %errorlevel%
