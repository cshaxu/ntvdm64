param([string]$Output='build/t478-s24/current',[string]$ProviderSource='src/ntvdm-exe/softpc/mvdm_shadow_registry.c')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$target=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$target.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $target)){throw 'Require a fresh build-owned fixture output directory'}
$null=New-Item -ItemType Directory -Force $Output;$out=(Resolve-Path $Output).Path
$provider=(Resolve-Path $ProviderSource).Path.Replace([char]92,[char]47)
[IO.File]::WriteAllText("$out/embedding_provider_path.h",("#include `""+$provider+"`"`n"))
$s=[IO.File]::ReadAllText("$repo/src/mvdm-host/wow32/mapembed.c")
$bodies=@()
foreach($name in @('UpdateEmbeddingAllKeys','UpdateClassesRootSubKey')){
 $m=[regex]::Match($s,'VOID\s+'+$name+'\s*\(')
 if(!$m.Success){throw "Missing original $name"}
 $i=$s.IndexOf('{',$m.Index)+1;$depth=1
 while($depth -and $i -lt $s.Length){if($s[$i] -eq '{'){++$depth}elseif($s[$i] -eq '}'){--$depth};++$i}
 if($depth){throw 'Unbalanced source body'}
 $bodies+=$s.Substring($m.Index,$i-$m.Index)
}
[IO.File]::WriteAllText("$out/embedding_exact.h",($bodies -join "`n`n"))
$driver='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$driver`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
"cl /nologo /TC /O2 /MT /D_CRT_SECURE_NO_WARNINGS /I`"$repo/src`" /I`"$repo/src/ntvdm-exe/softpc/include`" /I`"$out`" `"$repo/test/integration/wow_embedding_root_test.c`" /Fo:`"$out/test.obj`" /Fe:`"$out/embedding-test.exe`" /link advapi32.lib ntdll.lib user32.lib",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){Get-Content "$out/build.txt";throw 'Embedding fixture build failed'}
$null=New-Item -ItemType Directory "$out/data"
$env:T478_EMBEDDING_ROOT="$out/data"
$p=Start-Process "$out/embedding-test.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.txt" -RedirectStandardError "$out/run.err"
if(!$p.WaitForExit(10000)){$p.Kill();throw 'Owned embedding fixture timeout'}
$code=$p.ExitCode;Get-Content "$out/run.txt"
if(Test-Path "$out/data/NTVDM.REG"){throw 'Fixture did not retire its own file'}
Get-FileHash $ProviderSource,src/mvdm-host/wow32/mapembed.c,"$out/embedding_exact.h","$out/embedding-test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
exit $code
