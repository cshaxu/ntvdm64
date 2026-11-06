param([string]$Output='build/t478-s27/current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory -Force $out
$source='src/mvdm-host/vdmredir/vrdlc5c.c'
$s=[IO.File]::ReadAllText((Resolve-Path $source))
$start=$s.LastIndexOf("BOOLEAN`nInitializeDlcWorkerThread(")
if($start -lt 0){$start=$s.LastIndexOf("BOOLEAN`r`nInitializeDlcWorkerThread(")}
if($start -lt 0){throw 'Missing definition'}
$i=$s.IndexOf('{',$start)+1;$depth=1
while($depth -and $i -lt $s.Length){if($s[$i] -eq '{'){++$depth}elseif($s[$i] -eq '}'){--$depth};++$i}
if($depth){throw 'Unbalanced body'}
[IO.File]::WriteAllText("$out/dlc_worker_exact.h",$s.Substring($start,$i-$start))
$vc='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$vc`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /TC /O2 /MT /I`"$out`" `"$repo/test/integration/dlc_worker_event_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/dlc-event-test.exe`"",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){Get-Content "$out/build.txt";throw 'DLC fixture compile failed'}
& "$out/dlc-event-test.exe" > "$out/run.txt" 2>&1
$code=$LASTEXITCODE;Get-Content "$out/run.txt"
Get-FileHash $source,"$out/dlc_worker_exact.h","$out/dlc-event-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
