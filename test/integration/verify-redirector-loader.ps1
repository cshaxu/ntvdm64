[CmdletBinding()]
param([string]$BaselineRef='3de5b9a21')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
Set-Location $repo
$out=Join-Path $repo 'build/t478-s12/loader'
$null=New-Item -ItemType Directory "$out/current","$out/baseline" -Force
$sourcePath='src/mvdm-host/softpc.new/host/src/nt_bop.c'
$current=[IO.File]::ReadAllText((Join-Path $repo $sourcePath))
$baseline=(& git show "${BaselineRef}:$sourcePath") -join "`n"
if($LASTEXITCODE){throw 'Cannot read admitted baseline'}
foreach($entry in @(@('current',$current),@('baseline',$baseline))){
    $source=$entry[1]
    $start=$source.IndexOf('ULONG DefaultVrInitialized(VOID);')
    $end=$source.IndexOf('/*::::::::::::::::::::::::::::::::::::::::::::::::::::::: More MS BOP stubs */',$start)
    if($start -lt 0 -or $end -le $start){throw 'Original owner extraction anchors changed'}
    [IO.File]::WriteAllText("$out/$($entry[0])/redirector_loader_selected.inc",$source.Substring($start,$end-$start))
}
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$commands=@('@echo off',"call `"$vs`" -arch=x86 -host_arch=x64 >nul || exit /b 1",
    "cl /nologo /MT /LD test/integration/redirector_loader_dll.c /Fe:`"$out/owner-fixture.dll`" /Fo:`"$out/owner-fixture.obj`" /link /IMPLIB:`"$out/owner-fixture.lib`" || exit /b 1")
foreach($kind in @('baseline','current')){
    $commands+="cl /nologo /MT /I`"$out/$kind`" test/integration/redirector_loader_test.c /Fe:`"$out/$kind/owner-test.exe`" /Fo:`"$out/$kind/owner-test.obj`" /link /OPT:REF || exit /b 1"
}
[IO.File]::WriteAllLines("$out/build.cmd",$commands)
& cmd /c "$out/build.cmd"
if($LASTEXITCODE){throw 'Loader fixture compilation failed'}
foreach($kind in @('baseline','current')){
    $p=Start-Process "$out/$kind/owner-test.exe" -WindowStyle Hidden -ArgumentList @("`"$out/owner-fixture.dll`"") -PassThru -RedirectStandardOutput "$out/$kind/result.txt" -RedirectStandardError "$out/$kind/error.txt"
    if(!$p.WaitForExit(15000)){$p.Kill();throw "Owned $kind fixture exceeded 15 seconds"}
    $code=$p.ExitCode
    Get-Content "$out/$kind/result.txt"
    if(($kind -eq 'baseline' -and $code -ne 1) -or ($kind -eq 'current' -and $code -ne 0)){throw "Unexpected $kind fixture result: $code"}
}
'PASS original loader native module lifecycle; baseline fails and corrected owner passes'
