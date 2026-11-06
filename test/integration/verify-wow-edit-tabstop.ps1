param([string]$Object='build/.ninja/msvc-x86-release/wow32/obj/wmsgem.obj',[string]$Output='build/t478-s19/current')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$null=New-Item -ItemType Directory -Force $Output
$out=(Resolve-Path $Output).Path;$obj=(Resolve-Path $Object).Path
$driver='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
@('@echo off',"call `"$driver`" -arch=x86 -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /TC /c /O2 /MT `"$repo/test/integration/wow_edit_tabstop_test.c`" /Fo:`"$out/test.obj`"",
 'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /TC /c /O2 /MT `"$repo/artifacts/mvdm-audit/host-ch-scope/tabstop-release-seams.c`" /Fo:`"$out/seams.obj`"",
 'if errorlevel 1 exit /b %errorlevel%',
 "link /nologo /out:`"$out/test.exe`" /map:`"$out/test.map`" `"$out/test.obj`" `"$out/seams.obj`" `"$obj`" kernel32.lib user32.lib",
 'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /TC /c /O2 /MT `"$repo/artifacts/mvdm-audit/host-ch-scope/wordbreak-object-fixture.c`" /Fo:`"$out/wordbreak.obj`"",
 'if errorlevel 1 exit /b %errorlevel%',
 "link /nologo /out:`"$out/wordbreak.exe`" `"$out/wordbreak.obj`" `"$out/seams.obj`" `"$obj`" kernel32.lib user32.lib",
 'exit /b %errorlevel%') | Set-Content "$out/build.cmd" -Encoding ASCII
& cmd /d /c "$out/build.cmd" > "$out/build.txt" 2>&1
if($LASTEXITCODE){throw 'Clean-link fixture build failed'}
$rows=@()
foreach($count in @(0,1,-1)){foreach($heap in @(0,1)){foreach($result in @(0,1)){foreach($cached in @(0,1)){
 $line=& "$out/test.exe" $count $heap $result $cached
 $rows+=@{case="tabstop-$count-$heap-$result-$cached";exit=$LASTEXITCODE;output=($line -join "`n")}
}}}}
foreach($case in @('zero','guest-ldt','guest-high','native-fail','native-success','native-cached')){
 $line=& "$out/wordbreak.exe" $case
 $rows+=@{case="wordbreak-$case";exit=$LASTEXITCODE;output=($line -join "`n")}
}
$rows|ConvertTo-Json -Depth 4|Set-Content "$out/results.json"
$failed=@($rows|Where-Object {$_.exit -ne 0}).Count
"EDIT_TABSTOP cases=$($rows.Count) failures=$failed"|Tee-Object "$out/summary.txt"
if($failed){exit 1}
