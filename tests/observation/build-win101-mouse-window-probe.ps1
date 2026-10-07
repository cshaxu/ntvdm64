param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build output required'}
New-Item -ItemType Directory -Path $root|Out-Null
$lines=@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul',
    'if errorlevel 1 exit /b %errorlevel%',
    ('cl.exe /nologo /MT /std:c11 /LD /I"'+$repo+'\src\ntcon-exe" /Fo"'+$root+'\hook.obj" /Fe"'+$root+'\mouse-hook.dll" "'+$PSScriptRoot+'\private-mouse-input.c" user32.lib /link /EXPORT:MouseInputHook /IMPLIB:"'+$root+'\mouse-hook.lib"'),
    'if errorlevel 1 exit /b %errorlevel%',
    ('cl.exe /nologo /MT /std:c11 /Fo"'+$root+'\probe.obj" /Fe"'+$root+'\probe.exe" "'+$PSScriptRoot+'\win101_mouse_window_probe.c" user32.lib'),
    'if errorlevel 1 exit /b %errorlevel%')
[IO.File]::WriteAllLines("$root/build.cmd",$lines,[Text.Encoding]::ASCII)
& "$root/build.cmd" *> "$root/build.log"
if($LASTEXITCODE){throw "Probe build failed; inspect $root/build.log"}
'PASS private frontend mouse test tools build; no runtime/helper publication'
