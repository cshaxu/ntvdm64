@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cmake --build --preset msvc-x64-release-ccache --target native-worker native-frontend native-monitor native-launcher native-service hook64
exit /b %errorlevel%
