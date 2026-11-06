param([string]$Source='src/mvdm-host/wow32/wdib.c',[string]$Output='build/t478-s20/current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$null=New-Item -ItemType Directory -Force $Output;$out=(Resolve-Path $Output).Path
$s=[IO.File]::ReadAllText((Resolve-Path $Source));$a=$s.IndexOf('HDC W32HandleDibDrv');$end=$s.IndexOf('BOOL W32AddDibInfo',$a)
if($a -lt 0 -or $end -lt 0){throw 'Missing original owner'}
$owner=$s.Substring($a,$end-$a);$start=$owner.IndexOf('finally {')
if($start -lt 0){throw 'Missing original cleanup'}
$start=$owner.IndexOf('{',$start);$i=$start+1;$depth=1
while($depth -and $i -lt $owner.Length){if($owner[$i] -eq '{'){++$depth}elseif($owner[$i] -eq '}'){--$depth};++$i}
if($depth){throw 'Unbalanced cleanup'}
[IO.File]::WriteAllText("$out/dib_failure_cleanup.h",$owner.Substring($start,$i-$start))
$driver='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$driver`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
"cl /nologo /TC /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS /I`"$out`" `"$repo/test/integration/wow_dib_failure_cleanup_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/dib-test.exe`" /link gdi32.lib user32.lib",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){throw 'DIB cleanup fixture build failed'}
$p=Start-Process "$out/dib-test.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.txt" -RedirectStandardError "$out/run.err"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Owned DIB fixture timeout'}
$code=$p.ExitCode
Get-Content "$out/run.txt"
Get-FileHash $Source,"$out/dib_failure_cleanup.h","$out/dib-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
