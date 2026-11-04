param([Parameter(Mandatory)][string]$BuildRoot,[switch]$FaultInjection)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned diagnostic output required'
}
New-Item -ItemType Directory -Path $root|Out-Null
$commands=@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul','if errorlevel 1 exit /b %errorlevel%')
$sources=@("$PSScriptRoot/win31_console_api_trace.c")
foreach($name in @('late_guard.c','late_patch.c','minhook/src/buffer.c','minhook/src/trampoline.c','minhook/src/hde/hde64.c')){$sources+="$repo/src/lib-win32/process-hook/$name"}
$objects=@()
$testFlag=if($FaultInjection){' /DPROCESS_HOOK_LATE_GUARD_TEST'}else{''}
foreach($source in $sources) {
    $obj="$root/$([IO.Path]::GetFileNameWithoutExtension($source)).obj"
    $commands+='cl.exe /nologo /MT /std:c11'+$testFlag+' /I"'+$repo+'/src" /FI"'+$repo+'/src/compat/include/process_hook_msvc.h" /c /Fo"'+$obj+'" "'+$source+'"'
    $commands+='if errorlevel 1 exit /b %errorlevel%';$objects+='"'+$obj+'"'
}
$commands+='cl.exe /nologo /MT /std:c11 /DWIN31_CONSOLE_API_TRACE /c /I"'+$repo+'/src" /I"'+$repo+'/src/ntcon-exe" /Fo"'+$root+'/hook.obj" "'+$PSScriptRoot+'/private-mouse-input.c"'
$commands+='if errorlevel 1 exit /b %errorlevel%'
$commands+='link.exe /nologo /DLL /OUT:"'+$root+'/mouse-hook.dll" /IMPLIB:"'+$root+'/hook.lib" /EXPORT:MouseInputHook '+($objects -join ' ')+' "'+$root+'/hook.obj" user32.lib kernel32.lib'
$commands+='if errorlevel 1 exit /b %errorlevel%'
$commands+='cl.exe /nologo /MT /std:c11 /Fo"'+$root+'/probe.obj" /Fe"'+$root+'/probe.exe" "'+$PSScriptRoot+'/win101_mouse_window_probe.c" user32.lib'
$commands+='if errorlevel 1 exit /b %errorlevel%'
[IO.File]::WriteAllLines("$root/build.cmd",$commands,[Text.Encoding]::ASCII)
& "$root/build.cmd" *> "$root/build.log"
if($LASTEXITCODE){throw "Diagnostic probe build failed: $root/build.log"}
[ordered]@{role='private-NTCON-API-failure-observation-not-product';
    sourceSha256=(Get-FileHash "$PSScriptRoot/win31_console_api_trace.c").Hash;
    sources=@($sources | ForEach-Object { @{path=$_;sha256=(Get-FileHash $_).Hash} });
    inputHookSha256=(Get-FileHash "$PSScriptRoot/private-mouse-input.c").Hash;faultInjected=[bool]$FaultInjection;
    dllSha256=(Get-FileHash "$root/mouse-hook.dll").Hash;
    pointerWidth=64;originalCallResultAndLastErrorPreserved=$true} |
    ConvertTo-Json|Set-Content "$root/build.json"
'PASS diagnostic x64 API observer/input probe build; never publish these binaries'
