param([string]$Output='build/t478-s23/current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$null=New-Item -ItemType Directory -Force $Output;$out=(Resolve-Path $Output).Path
& python tests/observation/extract-wow-palette-thunk.py "$out/extracted"
if($LASTEXITCODE){throw 'Palette extraction failed'}
Copy-Item "$out/extracted/*.inc" $out
$driver='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$driver`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
"cl /nologo /TC /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS /I`"$out`" `"$repo/test/integration/wow_palette_output_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/palette-test.exe`" /link gdi32.lib user32.lib",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){Get-Content "$out/build.txt";throw 'Palette fixture build failed'}
$p=Start-Process "$out/palette-test.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.txt" -RedirectStandardError "$out/run.err"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Owned palette fixture timeout'}
$code=$p.ExitCode;Get-Content "$out/run.txt"
Get-FileHash src/mvdm-host/wow32/wgpal.c,"$out/original-getpaletteentries.inc","$out/original-putpaletteentry.inc","$out/palette-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
