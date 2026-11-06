param([string]$Output='build/t478-s22/current',[string]$Source='src/mvdm-host/wow32/wgdi31.c')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$null=New-Item -ItemType Directory -Force $Output;$out=(Resolve-Path $Output).Path
$s=[IO.File]::ReadAllText((Resolve-Path $Source));$bodies=@();$types=@()
$h=[IO.File]::ReadAllText("$repo/src/mvdm-host/inc/wowgdi.h")
foreach($name in @('GetFontData','GetGlyphOutline','GetKerningPairs')){
 $m=[regex]::Match($s,'(?m)^ULONG\s+FASTCALL\s+WG32'+$name+'\s*\([^;{}]*\)\s*\{')
 if(!$m.Success){throw "Missing $name"};$i=$m.Index+$m.Length;$depth=1
 while($depth -and $i -lt $s.Length){if($s[$i] -eq '{'){++$depth}elseif($s[$i] -eq '}'){--$depth};++$i}
 if($depth){throw 'Unbalanced body'};$bodies+=$s.Substring($m.Index,$i-$m.Index)
 $t=$name.ToUpper()+'16';$types+=[regex]::Match($h,'(?s)typedef struct _'+$t+'.*?typedef '+$t+' UNALIGNED \*P'+$t+';').Value
}
[IO.File]::WriteAllText("$out/font_exact.h",($bodies -join "`n`n"))
[IO.File]::WriteAllText("$out/font_types.h",($types -join "`n`n"))
$driver='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$driver`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
"cl /nologo /TC /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS /I`"$out`" `"$repo/test/integration/wow_font_output_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/font-test.exe`" /link gdi32.lib user32.lib",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){Get-Content "$out/build.txt";throw 'Font fixture build failed'}
$p=Start-Process "$out/font-test.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.txt" -RedirectStandardError "$out/run.err"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Owned font fixture timeout'}
$code=$p.ExitCode;Get-Content "$out/run.txt"
Get-FileHash $Source,"$out/font_exact.h","$out/font-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
