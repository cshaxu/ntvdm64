param(
    [Parameter(Mandatory)][string]$BuildRoot,
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
try {
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
build layout.obj: cc @NINJA_REPO@/src/win16-style/layout.c
build style.obj: cc @NINJA_REPO@/src/win16-style/win16_style.c
build demo.obj: cc @NINJA_REPO@/tests/app/win16_style_demo.c
build win16-style.lib: archive layout.obj style.obj
build win16-style-demo.exe: link demo.obj win16-style.lib
default win16-style-demo.exe
'@.Replace('@REPO@',$r).Replace('@NINJA_REPO@',$r.Replace(':','$:').Replace(' ','$ ')) | Set-Content "$dir/build.ninja" -Encoding ascii
        @"
@echo off
call "$VsDevCmd" -arch=$arch -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "$dir"
"$ninja" -f build.ninja
"@ | Set-Content "$dir/build.cmd" -Encoding ascii
        & $env:ComSpec /d /c "$dir/build.cmd" *> "$dir/build.txt"
        if($LASTEXITCODE){throw "Build failed $arch"}
        $p=Start-Process "$dir/win16-style-demo.exe" -ArgumentList @('--test',"$dir/capture.bmp") -WindowStyle Hidden -PassThru -RedirectStandardOutput "$dir/test.txt" -RedirectStandardError "$dir/error.txt"
        @{pid=$p.Id;start=$p.StartTime;deadline=$p.StartTime.AddSeconds(30)}|ConvertTo-Json|Set-Content "$dir/owner.json"
        if(!$p.WaitForExit(30000)){$p.Kill($true);throw "Fixture timeout $arch"}
        if($p.ExitCode){throw "Fixture failed $arch"}
        Add-Type -AssemblyName System.Drawing
        foreach($name in @('capture','capture.bmp-pressed','capture.bmp-canceled')){
            $bitmap=[Drawing.Bitmap]::FromFile("$dir/$name.bmp")
            try{$bitmap.Save("$dir/$name.png",[Drawing.Imaging.ImageFormat]::Png)}finally{$bitmap.Dispose()}
        }
        Get-Content "$dir/test.txt"
    }
    Get-FileHash "$repo/src/win16-style/*","$repo/tests/app/win16_style_demo.c","$repo/tests/observation/verify-win16-style.ps1"|ConvertTo-Json|Set-Content "$root/inputs.json"
    '0'|Set-Content "$root/exit-code.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit-code.txt";exit 1}
