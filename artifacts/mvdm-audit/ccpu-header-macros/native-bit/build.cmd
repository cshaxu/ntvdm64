@echo off
call "O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd" cl.exe /nologo /MT /W4 /WX /O2 /Fo"O:\repos.hobby\ntvdm64\build\t473-s10\ccpu-bit-memory/oracle.obj" /Fe"O:\repos.hobby\ntvdm64\build\t473-s10\ccpu-bit-memory/oracle.exe" "O:\repos.hobby\ntvdm64/tests/mvdm-host/ccpu_bit_memory_oracle.c"
exit /b %errorlevel%
