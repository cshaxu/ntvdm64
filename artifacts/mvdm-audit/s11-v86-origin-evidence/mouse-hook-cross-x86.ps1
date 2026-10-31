$ErrorActionPreference='Stop'; $r='O:\repos.hobby\ntvdm64\build\t473-s11'; $driver='O:\repos.hobby\ntvdm64\build\.ninja\msvc-x86-release\wow32\msvc-x86.cmd'
& $driver cl.exe /nologo /MT /LD /Fo"$r\v86-hook-cross-x86-dll.obj" /Fe"$r\v86-hook-cross-x86.dll" "$r\v86-hook-cross-x86-dll.c" user32.lib; if($LASTEXITCODE){exit $LASTEXITCODE}
& $driver cl.exe /nologo /MT /Fo"$r\v86-hook-cross-x86.obj" /Fe"$r\v86-hook-cross-x86.exe" "$r\v86-hook-cross-x86.c" user32.lib; if($LASTEXITCODE){exit $LASTEXITCODE}
& "$r\v86-hook-cross-x86.exe"; exit $LASTEXITCODE
