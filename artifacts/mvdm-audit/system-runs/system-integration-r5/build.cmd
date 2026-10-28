@echo off
call "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd" "C:\Users\neko\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\ninja.exe" -j2 pic-reset-eoi-test.exe
exit /b %errorlevel%
