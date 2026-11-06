param([string]$Output='build/t478-s26/current',[string]$Source='src/mvdm-host/wow32/wspool.c')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory -Force $out
$s=[IO.File]::ReadAllText((Resolve-Path $Source));$bodies=@()
foreach($name in @('GetPrn16','Prn32','FreePrn','WG32OpenJob','WG32CloseJob')){
 $m=[regex]::Match($s,'(?m)^(?:VOID|HANDLE|WORD|ULONG\s+FASTCALL)\s+'+$name+'\s*\(')
 if(!$m.Success){throw "Missing $name"}
 $i=$s.IndexOf('{',$m.Index)+1;$depth=1
 while($depth -and $i -lt $s.Length){if($s[$i] -eq '{'){++$depth}elseif($s[$i] -eq '}'){--$depth};++$i}
 if($depth){throw 'Unbalanced source'}
 $bodies+=$s.Substring($m.Index,$i-$m.Index)
}
[IO.File]::WriteAllText("$out/spool_exact.h",($bodies -join "`n`n"))
$vc='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$vc`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /TC /O2 /MT /D_CRT_SECURE_NO_WARNINGS /I`"$out`" `"$repo/test/integration/wow_spool_failure_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/spool-test.exe`"",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){Get-Content "$out/build.txt";throw 'Spool fixture compile failed'}
& "$out/spool-test.exe" > "$out/run.txt" 2>&1
$code=$LASTEXITCODE;Get-Content "$out/run.txt"
Get-FileHash $Source,"$out/spool_exact.h","$out/spool-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
