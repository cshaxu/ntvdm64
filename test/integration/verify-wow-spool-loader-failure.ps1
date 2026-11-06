param([string]$Output='build/t478-s26/loader-current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory -Force $out
$prefixes=@()
foreach($name in @('WG32DeviceMode','WG32ExtDeviceMode','WG32DeviceCapabilities','WG32GetEnvironment')){
 $source=if($name -eq 'WG32GetEnvironment'){'src/mvdm-host/wow32/wgdi.c'}else{'src/mvdm-host/wow32/wgprnset.c'}
 $s=[IO.File]::ReadAllText((Resolve-Path $source))
 $m=[regex]::Match($s,'(?m)^ULONG\s+FASTCALL\s+'+$name+'\s*\(')
 if(!$m.Success){throw "Missing $name"}
 $decision=$s.IndexOf('    if (!(*spoolerapis[',$m.Index)
 $i=$s.IndexOf('{',$decision)+1;$depth=1
 while($depth -and $i -lt $s.Length){if($s[$i] -eq '{'){++$depth}elseif($s[$i] -eq '}'){--$depth};++$i}
 if($depth -or $s.Substring($decision,$i-$decision) -notmatch 'LoadLibraryAndGetProcAddresses'){throw 'Missing exact DLL boundary'}
 # The appended sentinel is unreachable in the exercised failed-load path.
 $prefixes+=$s.Substring($m.Index,$i-$m.Index)+"`nreturn 0xfeed;`n}"
}
[IO.File]::WriteAllText("$out/loader_prefixes.h",($prefixes -join "`n`n"))
$vc='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$vc`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /TC /O2 /MT /D_CRT_SECURE_NO_WARNINGS /I`"$out`" `"$repo/test/integration/wow_spool_loader_failure_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/loader-test.exe`" /link user32.lib",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){Get-Content "$out/build.txt";throw 'Loader fixture compile failed'}
& "$out/loader-test.exe" > "$out/run.txt" 2>&1
$code=$LASTEXITCODE;Get-Content "$out/run.txt"
Get-FileHash src/mvdm-host/wow32/wgdi.c,src/mvdm-host/wow32/wgprnset.c,"$out/loader_prefixes.h","$out/loader-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
