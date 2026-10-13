param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$ScrollApiOnly,
    [switch]$ViewportOnly,
    [string]$ReferenceFont,
    [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){
    throw 'Use a fresh repository build evidence root'
}
$null=New-Item -ItemType Directory $root
Set-Location $root
$env:TEMP=$root;$env:TMP=$root
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
if($ScrollApiOnly -and $ViewportOnly){throw 'Choose only one focused mode'}
@{mode=if($ScrollApiOnly){'scroll-api-only'}elseif($ViewportOnly){'viewport-only'}else{'full'};architectures=@('x86','x64')} |
    ConvertTo-Json | Set-Content "$root/selection.json"
try {
    if($ReferenceFont -and (Get-FileHash $ReferenceFont).Hash -ne '7BAEFA14EF8EF2355374D0B5061043DD5F4F8C33FF57680366E4A725F91646F8'){throw 'Unexpected original font identity'}
    foreach($arch in @('x86','x64')){
        $dir=Join-Path $root $arch
        $null=New-Item -ItemType Directory $dir
        $r=$repo.Replace('\','/')
        @'
rule cc
  command = cl.exe /nologo /MT /O2 /W4 /WX /utf-8 /D_WIN32_WINNT=0x0A00 /DUNICODE /D_UNICODE /I"@REPO@/src" /showIncludes /c "$in" /Fo$out
  deps = msvc
rule archive
  command = lib.exe /nologo /out:$out $in
rule link
  command = link.exe /nologo /subsystem:windows /entry:wmainCRTStartup /out:$out $in user32.lib gdi32.lib comctl32.lib dwmapi.lib
rule probe_link
  command = link.exe /nologo /subsystem:console /out:$out $in user32.lib gdi32.lib
rule style_probe_link
  command = link.exe /nologo /manifest:embed /subsystem:console /out:$out $in user32.lib gdi32.lib comctl32.lib dwmapi.lib
rule menu_v6_link
  command = link.exe /nologo /subsystem:windows /entry:wmainCRTStartup /manifest:embed /manifestdependency:"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'" /out:$out $in user32.lib gdi32.lib
build layout.obj: cc @NINJA_REPO@/src/win16-style/layout.c
build scroll-layout.obj: cc @NINJA_REPO@/src/win16-style/scroll_layout.c
build scroll-track.obj: cc @NINJA_REPO@/src/win16-style/scroll_track.c
build scroll-geometry.obj: cc @NINJA_REPO@/tests/app/win16_style_scroll_geometry.c
build scroll-geometry.exe: probe_link scroll-geometry.obj scroll-layout.obj scroll-track.obj
build scroll-api.obj: cc @NINJA_REPO@/tests/app/win16_style_scroll_api.c
build scroll-api.exe: probe_link scroll-api.obj
build style.obj: cc @NINJA_REPO@/src/win16-style/win16_style.c
build font.obj: cc @NINJA_REPO@/src/win16-style/font.c
build font-test.obj: cc @NINJA_REPO@/tests/app/win16_style_font.c
build font-test.exe: probe_link font-test.obj font.obj
build menu-dispatch.obj: cc @NINJA_REPO@/tests/app/win16_style_menu_dispatch.c
build menu-dispatch.exe: style_probe_link menu-dispatch.obj win16-style.lib
build control-present.obj: cc @NINJA_REPO@/tests/app/win16_style_control_present.c
build control-present.exe: style_probe_link control-present.obj win16-style.lib
build list-tabs.obj: cc @NINJA_REPO@/tests/app/win16_style_list_tabs.c
build list-tabs.exe: style_probe_link list-tabs.obj win16-style.lib
build control-lifetime.obj: cc @NINJA_REPO@/tests/app/win16_style_control_lifetime.c
build control-lifetime.exe: style_probe_link control-lifetime.obj win16-style.lib
build sizebox.obj: cc @NINJA_REPO@/tests/app/win16_style_sizebox.c
build sizebox.exe: style_probe_link sizebox.obj win16-style.lib
build dc-failure.obj: cc @NINJA_REPO@/tests/app/win16_style_dc_failure.c
build menu-paint.obj: cc @NINJA_REPO@/src/win16-style/menu_paint.c
build menu-layout.obj: cc @NINJA_REPO@/src/win16-style/menu_layout.c
build menu-bar.obj: cc @NINJA_REPO@/src/win16-style/menu_bar.c
build menu-items.obj: cc @NINJA_REPO@/src/win16-style/menu_items.c
build menu-scope.obj: cc @NINJA_REPO@/src/win16-style/menu_scope.c
build controls.obj: cc @NINJA_REPO@/src/win16-style/controls.c
build control-scope.obj: cc @NINJA_REPO@/src/win16-style/control_scope.c
build dc-failure.exe: link dc-failure.obj layout.obj scroll-layout.obj scroll-track.obj menu-paint.obj menu-layout.obj menu-bar.obj menu-items.obj menu-scope.obj controls.obj font.obj
build demo.obj: cc @NINJA_REPO@/tests/app/win16_style_demo.c
build gallery.obj: cc @NINJA_REPO@/tests/app/win16_style_gallery.c
build font-probe.obj: cc @NINJA_REPO@/tests/app/win31_font_probe.c
build font-probe.exe: probe_link font-probe.obj
build native-menu.obj: cc @NINJA_REPO@/tests/app/native_menu_probe.c
build native-menu-probe.exe: link native-menu.obj
build native-menu-v6-probe.exe: menu_v6_link native-menu.obj
build win16-style.lib: archive layout.obj scroll-layout.obj scroll-track.obj style.obj menu-paint.obj menu-layout.obj menu-bar.obj menu-items.obj menu-scope.obj controls.obj control-scope.obj font.obj
build win16-style-demo.exe: link demo.obj gallery.obj win16-style.lib
default win16-style-demo.exe font-probe.exe font-test.exe menu-dispatch.exe control-present.exe control-lifetime.exe sizebox.exe list-tabs.exe native-menu-probe.exe native-menu-v6-probe.exe dc-failure.exe scroll-geometry.exe scroll-api.exe
'@.Replace('@REPO@',$r).Replace('@NINJA_REPO@',$r.Replace(':','$:').Replace(' ','$ ')) | Set-Content "$dir/build.ninja" -Encoding ascii
        $selectedTarget=if($ScrollApiOnly){'scroll-api.exe'}else{''}
        @"
@echo off
call "$VsDevCmd" -arch=$arch -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$dir"
"$ninja" -f build.ninja $selectedTarget
"@ | Set-Content "$dir/build.cmd" -Encoding ascii
        & $env:ComSpec /d /c "$dir/build.cmd" *> "$dir/build.txt"
        if($LASTEXITCODE){throw "Build failed $arch"}
        if(!$ScrollApiOnly){
            $viewport=Start-Process "$dir/win16-style-demo.exe" -ArgumentList @('--viewport-test',"$dir/viewport.bmp") -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/viewport.txt" -RedirectStandardError "$dir/viewport.err"
            @{pid=$viewport.Id;start=$viewport.StartTime;deadline=$viewport.StartTime.AddSeconds(30)}|ConvertTo-Json|Set-Content "$dir/viewport-owner.json"
            if(!$viewport.WaitForExit(30000)){$viewport.Kill($true);throw "Viewport timeout $arch"}
            if($viewport.ExitCode){throw "Viewport failed $arch"}
        }
        if($ViewportOnly){
            $present=Start-Process "$dir/control-present.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/control-present.txt" -RedirectStandardError "$dir/control-present.err"
            @{pid=$present.Id;start=$present.StartTime;deadline=$present.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/control-present-owner.json"
            if(!$present.WaitForExit(20000)){$present.Kill($true);throw "Control presentation timeout $arch"}
            if($present.ExitCode){throw "Control presentation failed $arch"}
            continue
        }
        if(!$ScrollApiOnly){
        $lifetime=Start-Process "$dir/control-lifetime.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/control-lifetime.txt" -RedirectStandardError "$dir/control-lifetime.err"
        @{pid=$lifetime.Id;start=$lifetime.StartTime;deadline=$lifetime.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/lifetime-owner.json"
        if(!$lifetime.WaitForExit(20000)){$lifetime.Kill($true);throw "Control lifetime timeout $arch"}
        if($lifetime.ExitCode){throw "Control lifetime failed $arch"}
        foreach($mode in @('native','styled')){
            $sizebox=Start-Process "$dir/sizebox.exe" -ArgumentList $(if($mode -eq 'styled'){'--styled'}else{'--native'}) -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/sizebox-$mode.txt" -RedirectStandardError "$dir/sizebox-$mode.err"
            @{pid=$sizebox.Id;start=$sizebox.StartTime;deadline=$sizebox.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/sizebox-$mode-owner.json"
            if(!$sizebox.WaitForExit(20000)){$sizebox.Kill($true);throw "Sizebox timeout $arch/$mode"}
            if($sizebox.ExitCode){throw "Sizebox failed $arch/$mode"}
        }
        foreach($mode in @('normal','dpi96')){
            $fontArgs=if($mode -eq 'dpi96'){'--dpi96'}else{'--normal'}
            $fontRun=Start-Process "$dir/font-test.exe" -ArgumentList $fontArgs -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/font-$mode.txt" -RedirectStandardError "$dir/font-$mode.err"
            @{pid=$fontRun.Id;start=$fontRun.StartTime;deadline=$fontRun.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/font-$mode-owner.json"
            if(!$fontRun.WaitForExit(20000)){$fontRun.Kill($true);throw "Font fixture timeout $arch/$mode"}
            if($fontRun.ExitCode){throw "Font fixture failed $arch/$mode"}
        }
        $menuDispatch=Start-Process "$dir/menu-dispatch.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/menu-dispatch.txt" -RedirectStandardError "$dir/menu-dispatch.err"
        @{pid=$menuDispatch.Id;start=$menuDispatch.StartTime;deadline=$menuDispatch.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/menu-dispatch-owner.json"
        if(!$menuDispatch.WaitForExit(20000)){$menuDispatch.Kill($true);throw "Menu dispatch timeout $arch"}
        if($menuDispatch.ExitCode){throw "Menu dispatch failed $arch"}
        $controlPresent=Start-Process "$dir/control-present.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/control-present.txt" -RedirectStandardError "$dir/control-present.err"
        @{pid=$controlPresent.Id;start=$controlPresent.StartTime;deadline=$controlPresent.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/control-present-owner.json"
        if(!$controlPresent.WaitForExit(20000)){$controlPresent.Kill($true);throw "Control presentation timeout $arch"}
        if($controlPresent.ExitCode){throw "Control presentation failed $arch"}
        $controlLeak=Start-Process "$dir/control-present.exe" -ArgumentList '--leak-negative' -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/control-leak.txt" -RedirectStandardError "$dir/control-leak.err"
        @{pid=$controlLeak.Id;start=$controlLeak.StartTime;deadline=$controlLeak.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/control-leak-owner.json"
        if(!$controlLeak.WaitForExit(20000)){$controlLeak.Kill($true);throw "Control leak negative timeout $arch"}
        if($controlLeak.ExitCode -ne 1 -or !(Select-String -Quiet -Path "$dir/control-leak.txt" -Pattern 'INJECTED PAINTER LEAK live=') -or
            !(Select-String -Quiet -Path "$dir/control-leak.err" -Pattern 'CONTROL PRESENT FAIL')){throw "Control leak was not detected $arch"}
        $listTabs=Start-Process "$dir/list-tabs.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/list-tabs.txt" -RedirectStandardError "$dir/list-tabs.err"
        @{pid=$listTabs.Id;start=$listTabs.StartTime;deadline=$listTabs.StartTime.AddSeconds(20)}|ConvertTo-Json|Set-Content "$dir/list-tabs-owner.json"
        if(!$listTabs.WaitForExit(20000)){$listTabs.Kill($true);throw "List tabs oracle timeout $arch"}
        if($listTabs.ExitCode){throw "List tabs oracle failed $arch"}
        $geometry=Start-Process "$dir/scroll-geometry.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/scroll-geometry.txt" -RedirectStandardError "$dir/scroll-geometry.err"
        @{pid=$geometry.Id;start=$geometry.StartTime;deadline=$geometry.StartTime.AddSeconds(15)}|ConvertTo-Json|Set-Content "$dir/geometry-owner.json"
        if(!$geometry.WaitForExit(15000)){$geometry.Kill($true);throw "Geometry fixture timeout $arch"}
        if($geometry.ExitCode){throw "Geometry fixture failed $arch"}
        }
        $api=Start-Process "$dir/scroll-api.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/scroll-api.txt" -RedirectStandardError "$dir/scroll-api.err"
        @{pid=$api.Id;start=$api.StartTime;deadline=$api.StartTime.AddSeconds(15)}|ConvertTo-Json|Set-Content "$dir/api-owner.json"
        if(!$api.WaitForExit(15000)){$api.Kill($true);throw "Scroll API fixture timeout $arch"}
        if($api.ExitCode){throw "Scroll API fixture failed $arch"}
        if($ScrollApiOnly){Get-Content "$dir/scroll-api.txt";continue}
        # Native scrollbar tracking changes queue/capture state during diagnosis.
        # Give it a separate process/private desktop, not the menu test's queue.
        $scroll=Start-Process "$dir/win16-style-demo.exe" -ArgumentList '--scroll-test' -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/scroll-test.txt" -RedirectStandardError "$dir/scroll-error.txt"
        # Expanded list/EDIT boundary matrix measured 30.60s in r151's owned
        # diagnostic. Keep all cases, with a 60s whole-suite deadline.
        @{pid=$scroll.Id;start=$scroll.StartTime;deadline=$scroll.StartTime.AddSeconds(60)}|ConvertTo-Json|Set-Content "$dir/scroll-owner.json"
        if(!$scroll.WaitForExit(60000)){$scroll.Kill($true);throw "Scroll fixture timeout $arch"}
        if($scroll.ExitCode){throw "Scroll fixture failed $arch"}
        $p=Start-Process "$dir/win16-style-demo.exe" -ArgumentList @('--test',"$dir/capture.bmp") -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/test.txt" -RedirectStandardError "$dir/error.txt"
        # Expanded control gallery also exercises each added outer system menu.
        # Keep a bounded whole-suite deadline, independent of tool observation.
        @{pid=$p.Id;start=$p.StartTime;deadline=$p.StartTime.AddSeconds(60)}|ConvertTo-Json|Set-Content "$dir/owner.json"
        if(!$p.WaitForExit(60000)){$p.Kill($true);throw "Fixture timeout $arch"}
        if($p.ExitCode){throw "Fixture failed $arch"}
        $dc=Start-Process "$dir/dc-failure.exe" -ArgumentList @("$dir/menu-paint.bmp") -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/dc-failure.txt" -RedirectStandardError "$dir/dc-failure.err"
        @{pid=$dc.Id;start=$dc.StartTime;deadline=$dc.StartTime.AddSeconds(15)}|ConvertTo-Json|Set-Content "$dir/dc-owner.json"
        if(!$dc.WaitForExit(15000)){$dc.Kill($true);throw "DC fixture timeout $arch"}
        if($dc.ExitCode){throw "DC fixture failed $arch"}
        if($ReferenceFont){
            & "$dir/font-probe.exe" $ReferenceFont *> "$dir/font-probe.txt"
            if($LASTEXITCODE){throw "Original font probe failed $arch"}
        }
        Add-Type -AssemblyName System.Drawing
        foreach($name in (Get-ChildItem $dir -Filter '*.bmp'|ForEach-Object BaseName)){
            $bitmap=[Drawing.Bitmap]::FromFile("$dir/$name.bmp")
            try{$bitmap.Save("$dir/$name.png",[Drawing.Imaging.ImageFormat]::Png)}finally{$bitmap.Dispose()}
        }
        Get-Content "$dir/test.txt"
    }
    Get-FileHash "$repo/src/win16-style/*","$repo/tests/app/win16_style_demo.c","$repo/tests/app/win16_style_gallery.c","$repo/tests/app/win16_style_font.c","$repo/tests/app/win16_style_menu_dispatch.c","$repo/tests/app/win16_style_control_present.c","$repo/tests/app/win16_style_control_lifetime.c","$repo/tests/app/win16_style_sizebox.c","$repo/tests/app/win16_style_list_tabs.c","$repo/tests/app/win16_style_scroll_geometry.c","$repo/tests/app/win16_style_scroll_api.c","$repo/tests/app/win16_style_dc_failure.c","$repo/tests/app/win31_font_probe.c","$repo/tests/app/native_menu_probe.c","$repo/tests/observation/verify-win16-style.ps1"|ConvertTo-Json|Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit-code.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
