param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh repository build output required'}
$null=New-Item -ItemType Directory $out
function Invoke-Owned([string]$name,[string]$command,[int]$seconds=120){
    $start=[DateTime]::UtcNow
    $p=Start-Process cmd.exe -ArgumentList ('/d /s /c "call "'+$VsDevCmd+'" -arch=x86 -host_arch=x64 >nul && '+$command+'"') -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.stdout.log" -RedirectStandardError "$out/$name.stderr.log"
    $null=$p.Handle;@{pid=$p.Id;start=$start.ToString('o');deadline=$start.AddSeconds($seconds).ToString('o');command=$command}|ConvertTo-Json|Set-Content "$out/$name.process.json"
    if(!$p.WaitForExit($seconds*1000)){& taskkill.exe /PID $p.Id /T /F *> "$out/$name.timeout.log";throw "$name timeout"};$p.Refresh();$p.ExitCode|Set-Content "$out/$name.exit";if($p.ExitCode){throw "$name failed: $($p.ExitCode)"}
}
try {
    & "$PSScriptRoot/extract-wow-accelerator-fixture.ps1" -BuildRoot $out | Set-Content "$out/extract.txt"
    & "$PSScriptRoot/extract-wow-accelerator-alias-fixture.ps1" -BuildRoot $out | Set-Content "$out/extract-alias.txt"
    Invoke-Owned 'compile' ('cl.exe /nologo /TC /c /MT /W4 /WX /I"'+$out+'" /Fo"'+$out+'/fixture.obj" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_accelerator_call_fixture.c"')
    Invoke-Owned 'link' ('link.exe /nologo /machine:x86 /subsystem:console /out:"'+$out+'/fixture.exe" "'+$out+'/fixture.obj" user32.lib')
    $p=Start-Process "$out/fixture.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.stdout.log" -RedirectStandardError "$out/run.stderr.log";$null=$p.Handle;if(!$p.WaitForExit(30000)){throw 'run timeout'};$p.ExitCode|Set-Content "$out/run.exit";if($p.ExitCode){throw "run failed: $($p.ExitCode)"}
    if(!(Select-String -LiteralPath "$out/run.stdout.log" -SimpleMatch 'WOW_ACCEL_CREATE32 errors=0 entries=2 partial_tail=1')){throw 'Accelerator marker missing'}
    Invoke-Owned 'alias-compile' ('cl.exe /nologo /TC /c /MT /W4 /WX /I"'+$out+'" /Fo"'+$out+'/alias.obj" "'+$repo+'/tests/adapter-mvdm-host-out/wow/wow_accelerator_alias_fixture.c"')
    Invoke-Owned 'alias-link' ('link.exe /nologo /machine:x86 /subsystem:console /out:"'+$out+'/alias.exe" "'+$out+'/alias.obj"')
    $p=Start-Process "$out/alias.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/alias-run.stdout.log" -RedirectStandardError "$out/alias-run.stderr.log";$null=$p.Handle;if(!$p.WaitForExit(30000)){throw 'alias run timeout'};$p.ExitCode|Set-Content "$out/alias-run.exit";if($p.ExitCode){throw "alias run failed: $($p.ExitCode)"}
    if(!(Select-String -LiteralPath "$out/alias-run.stdout.log" -SimpleMatch 'WOW_ACCEL_ALIAS_RETIRE errors=0 flags=2 destroys=2')){throw 'Accelerator alias marker missing'}
    0|Set-Content "$out/suite.exit"
}catch{$_|Out-String|Set-Content "$out/failure.txt";1|Set-Content "$out/suite.exit";exit 1}
