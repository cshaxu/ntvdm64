param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][ValidateSet('x86','x64')][string]$Architecture,
    [switch]$Append,
    [switch]$IncludeTests,
    [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build root must be below repository build'}
$null=New-Item -ItemType Directory -Force $root
$name=if($Architecture -eq 'x86'){'ntw16s32'}else{'ntw16s64'}
$r=$repo.Replace('\','/').Replace(':','$:').Replace(' ','$ ')
$include=$repo.Replace('\','/')
$graph=@"
rule ntw16s_cc
  command = cl.exe /nologo /MT /O2 /W4 /WX /utf-8 /D_WIN32_WINNT=0x0A00 /DUNICODE /D_UNICODE /DNTW16S_BUILD /I"$include/src" /showIncludes /c "`$in" /Fo`$out
  deps = msvc
rule w16style_cc
  command = cl.exe /nologo /MT /O2 /W4 /WX /utf-8 /D_WIN32_WINNT=0x0A00 /DUNICODE /D_UNICODE /I"$include/src" /showIncludes /c "`$in" /Fo`$out
  deps = msvc
rule ntw16s_link
  command = link.exe /nologo /dll /manifest:embed /out:$name.dll /implib:$name.lib /map:$name.map `$in user32.lib gdi32.lib comctl32.lib dwmapi.lib uxtheme.lib
rule w16style_link
  command = link.exe /nologo /subsystem:windows /manifest:embed /out:`$out /map:W16Style.map `$in user32.lib gdi32.lib comctl32.lib

"@
$units=@('layout','scroll_layout','scroll_track','win16_style','menu_paint','menu_layout','menu_bar','menu_bar_paint','menu_items','menu_scope','controls','control_scope','font','dllmain','attachment','theme','native_api')
foreach($unit in $units){$graph+="build dll-$unit.obj: ntw16s_cc $r/src/ntw16s-dll/$unit.c`n"}
$objects=($units|ForEach-Object {"dll-$_.obj"}) -join ' '
$graph+="build $name.dll $name.lib: ntw16s_link $objects`n"
$graph+="build w16style-main.obj: w16style_cc $r/src/w16style-exe/main.c`n"
$graph+="build w16style-gallery.obj: w16style_cc $r/src/w16style-exe/gallery.c`n"
$graph+="build W16Style.exe: w16style_link w16style-main.obj w16style-gallery.obj $name.lib`n"
$graph+="default $name.dll W16Style.exe`n"
if($IncludeTests){
    $graph+="rule ntw16s_test_link`n  command = link.exe /nologo /subsystem:console /out:`$out `$in user32.lib gdi32.lib`n"
    $graph+="build ntw16s-dll-test.obj: w16style_cc $r/tests/app/ntw16s_dll.c`n"
    $graph+="build ntw16s-dll-test.exe: ntw16s_test_link ntw16s-dll-test.obj`n"
    $graph+="default ntw16s-dll-test.exe`n"
    $graph+="build title-marker-test.obj: w16style_cc $r/tests/app/title_marker_test.c`n"
    $graph+="build title-marker-test.exe: ntw16s_test_link title-marker-test.obj`n"
    $graph+="default title-marker-test.exe`n"
    $graph+="build style-attachment-test.obj: w16style_cc $r/tests/app/style_attachment.c`n"
    $graph+="build style-attachment-test.exe: ntw16s_test_link style-attachment-test.obj`n"
    $graph+="default style-attachment-test.exe`n"
    $graph+="build default-button-test.obj: w16style_cc $r/tests/app/win16_style_default_button.c`n"
    $graph+="build default-button-test.exe: ntw16s_test_link default-button-test.obj`n"
    $graph+="default default-button-test.exe`n"
    $graph+="build style-dialog-test.obj: w16style_cc $r/tests/app/style_dialog.c`n"
    $graph+="rule style_dialog_link`n  command = link.exe /nologo /manifest:embed /subsystem:console /out:`$out `$in user32.lib gdi32.lib comctl32.lib`n"
    $graph+="build style-dialog-test.exe: style_dialog_link style-dialog-test.obj`n"
    $graph+="default style-dialog-test.exe`n"
}
if($Append){Add-Content "$root/build.ninja" $graph -Encoding ascii}else{Set-Content "$root/build.ninja" $graph -Encoding ascii}
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
@"
@echo off
call "$VsDevCmd" -arch=$Architecture -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$root"
"$ninja" -f build.ninja %*
"@|Set-Content "$root/build-style.cmd" -Encoding ascii
