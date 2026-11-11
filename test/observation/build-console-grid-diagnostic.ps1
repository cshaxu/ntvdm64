param([Parameter(Mandatory)][string]$Cache,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;$cache=(Resolve-Path $Cache).Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned diagnostic required'}
$null=New-Item -ItemType Directory -Path $root
$graph=Get-Content "$cache/build.ninja"
$flags=($graph|Where-Object {$_ -like 'cflags = *'}|Select-Object -First 1).Substring(9).Replace('$:',':')
$edge=@($graph|Where-Object {$_ -match '^build ntcon.exe: link '})
if($edge.Count -ne 1){throw 'Missing native frontend link edge'}
$object="$root/grid.obj"
$inputs=@(($edge[0] -replace '^.*: link ','') -split ' '|ForEach-Object {
    if($_ -eq 'obj/opennt-abi-host-compat/console_grid.obj'){'"'+$object+'"'}
    else {'"'+(Resolve-Path (Join-Path $cache $_)).Path+'"'}
})
$script=@('@echo off',
 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul',
 'if errorlevel 1 exit /b %errorlevel%',('cd /d "'+$cache+'"'),
 ('cl.exe '+$flags+' /Fo"'+$object+'" "'+$PSScriptRoot+'/console_grid_diagnostic.c"'),
 'if errorlevel 1 exit /b %errorlevel%',
 ('link.exe /nologo /machine:x64 /subsystem:console /opt:ref /out:"'+$root+'/ntcon.exe" '+($inputs -join ' ')+' rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib'),
 'exit /b %errorlevel%')
[IO.File]::WriteAllLines("$root/build.cmd",$script,[Text.Encoding]::ASCII)
& "$root/build.cmd" *> "$root/build.log"
if($LASTEXITCODE){throw "Diagnostic build failed: $root/build.log"}
'PASS native test-only frontend with real grid/API failure observation'
