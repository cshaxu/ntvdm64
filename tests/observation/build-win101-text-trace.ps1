param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build output required'}
$cache=Join-Path $repo 'build/M0-T434/S3/r001-formal'
$graph=Get-Content "$cache/build.ninja" -Raw
$flags=[regex]::Match($graph,'(?m)^cflags = (.+)$').Groups[1].Value.Trim().Replace('$:',':')
if(!$flags){throw 'Formal compile flags missing'}
$edge=[regex]::Match($graph,'(?m)^build ntvdm\.exe[^\r\n]*: worker_link (?<inputs>[^\r\n]+)')
if(!$edge.Success){throw 'Formal link edge missing'}
New-Item -ItemType Directory -Path $root|Out-Null
[IO.File]::WriteAllText("$root/flags.rsp",$flags,[Text.Encoding]::ASCII)
$link=@('/nologo','/subsystem:console','/opt:ref','/include:_win101_palette_trace_entry',('/out:"'+$root+'\ntvdm.exe"'),('/map:"'+$root+'\ntvdm.exe.map"'),
    ('/implib:"'+$root+'\ntvdm.lib"'),('/def:"'+$cache+'\generated\ntvdm-wow32-provider.def"'),
    ('"'+$root+'\trace.obj"'))
foreach($name in @('detours','disasm','image','modules','creatwth')){
    $obj=Join-Path $repo "build/M0-T435/S2/r018-guard/$name.obj"
    if(!(Test-Path $obj)){throw 'Pinned test Detours cache absent'}
    $link+='"'+$obj+'"'
}
$link+=@($edge.Groups['inputs'].Value.Trim() -split '\s+'|ForEach-Object{'"'+(Join-Path $cache $_)+'"'})
$link+=@('rpcrt4.lib','kernel32.lib','user32.lib','gdi32.lib','advapi32.lib','ntdll.lib','libcmt.lib','libvcruntime.lib','libucrt.lib')
[IO.File]::WriteAllLines("$root/link.rsp",$link,[Text.Encoding]::ASCII)
$cmd=@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul','if errorlevel 1 exit /b %errorlevel%',
    ('cl.exe /nologo /MT /EHsc /std:c++14 /c /Fo"'+$root+'\trace.obj" "'+$PSScriptRoot+'\win101_palette_detour.cpp"'),
    'if errorlevel 1 exit /b %errorlevel%',('link.exe @"'+$root+'\link.rsp"'),'if errorlevel 1 exit /b %errorlevel%')
[IO.File]::WriteAllLines("$root/build.cmd",$cmd,[Text.Encoding]::ASCII)
& "$root/build.cmd" *> "$root/build.log"
if($LASTEXITCODE){throw "Trace link failed; inspect $root/build.log"}
'PASS private palette trace relink; no production publication'
