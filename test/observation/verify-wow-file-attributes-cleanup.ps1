param(
 [string]$Source='src/mvdm-host/wow32/wkfileio.c',
 [Parameter(Mandatory)][string]$Output
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
Set-Location $repo
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory $out
$sourcePath=(Resolve-Path $Source).Path
$s=[IO.File]::ReadAllText($sourcePath)
$start=$s.IndexOf('ULONG FASTCALL WK32FileSetAttributes(')
$end=$s.IndexOf('ULONG FASTCALL WK32FileGetDateTime(',$start)
if($start -lt 0 -or $end -le $start){throw 'Missing original thunk boundaries'}
[IO.File]::WriteAllText("$out/thunk.inc",$s.Substring($start,$end-$start))
$fixture="$repo/test/adapter-mvdm-host-out/wow/wow_file_attributes_cleanup_fixture.c"
"#define THUNK_SOURCE `"thunk.inc`"`n#include `"$($fixture.Replace('\','/'))`""|Set-Content "$out/test.c" -Encoding ASCII
@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul','if errorlevel 1 exit /b %errorlevel%',"cl /nologo /TC /O2 /MT /I`"$out`" `"$out/test.c`" /Fo`"$out/test.obj`" /Fe`"$out/test.exe`"",'exit /b %errorlevel%')|Set-Content "$out/build.cmd" -Encoding ASCII
& cmd.exe /c "$out/build.cmd" *> "$out/build.log"
if($LASTEXITCODE){throw 'File attributes fixture build failed'}
& "$out/test.exe" *> "$out/run.log"
$code=$LASTEXITCODE
Get-Content "$out/run.log"
Get-FileHash $sourcePath,$fixture,"$out/thunk.inc","$out/test.exe"|ConvertTo-Json|Set-Content "$out/hashes.json"
$code|Set-Content "$out/run.exit"
exit $code
