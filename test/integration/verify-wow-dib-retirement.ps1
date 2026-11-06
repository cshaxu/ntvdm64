param([string]$Output='build/t478-s21/current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$null=New-Item -ItemType Directory -Force $Output;$out=(Resolve-Path $Output).Path
$s=[IO.File]::ReadAllText("$repo/src/mvdm-host/wow32/wdib.c")
$bodies=@()
foreach($name in @('W32FreeDibInfoHandle','W32CheckAndFreeDibInfo','W32FreeDibInfo','W32FindAndLockDibInfo','WK32FindAndReleaseDib')){
 $m=[regex]::Match($s,'(?m)^(?:BOOL|VOID|HDC|ULONG)\s+(?:FASTCALL\s+)?'+$name+'\s*\([^;{}]*\)\s*\{')
 if(!$m.Success){throw "Missing original $name"};$i=$m.Index+$m.Length;$depth=1
 while($depth -and $i -lt $s.Length){if($s[$i] -eq '{'){++$depth}elseif($s[$i] -eq '}'){--$depth};++$i}
 if($depth){throw 'Unbalanced original owner'};$bodies+=$s.Substring($m.Index,$i-$m.Index)
}
[IO.File]::WriteAllText("$out/dib_retirement_exact.h",($bodies -join "`n`n"))
$dib=[IO.File]::ReadAllText("$repo/src/mvdm-host/wow32/wdib.h")
$types=[regex]::Match($dib,'(?s)typedef struct _DIBINFO.*?} DIBINFO, \*PDIBINFO;').Value
$kernel=[IO.File]::ReadAllText("$repo/src/mvdm-host/inc/wowkrn.h")
$types+="`n"+[regex]::Match($kernel,'(?s)typedef struct _FINDANDRELEASEDIB16.*?typedef FINDANDRELEASEDIB16 UNALIGNED \*PFINDANDRELEASEDIB16;').Value
if(!$types.Contains('PFINDANDRELEASEDIB16')){throw 'Missing original ABI record'}
[IO.File]::WriteAllText("$out/dib_retirement_types.h",$types)
$driver='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$driver`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
"cl /nologo /TC /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS /I`"$out`" `"$repo/test/integration/wow_dib_retirement_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/dib-test.exe`" /link gdi32.lib user32.lib",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){throw 'DIB retirement fixture build failed'}
$p=Start-Process "$out/dib-test.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.txt" -RedirectStandardError "$out/run.err"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Owned DIB fixture timeout'}
$code=$p.ExitCode;Get-Content "$out/run.txt"
Get-FileHash "$repo/src/mvdm-host/wow32/wdib.c","$out/dib_retirement_exact.h","$out/dib-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
