@echo off
call "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd" link.exe /nologo /machine:x86 /subsystem:console /out:"O:\repos.hobby\ntvdm64\build\t473-s10\neg64-native-r2\run/fixture.exe" /map:"O:\repos.hobby\ntvdm64\build\t473-s10\neg64-native-r2\run/fixture.map" "O:\repos.hobby\ntvdm64\build\t473-s10\neg64-native-r2\run/normal.obj" "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\worker\obj\ccpu\c_neg64.obj" kernel32.lib legacy_stdio_definitions.lib
exit /b %errorlevel%
