param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)){
    throw 'Require a fresh repository build evidence directory'
}
$null=New-Item -ItemType Directory $build
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach($arch in @('x86','x64')){
    $dir=Join-Path $build $arch
    $null=New-Item -ItemType Directory $dir
    $lines=@('@echo off',"call `"$vs`" -arch=$arch -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
        "cl /nologo /std:c11 /W4 /WX /MT /D_WIN32_WINNT=0x0A00 /DPROCESS_HOOK_LATE_GUARD_TEST /I `"$repo/src`" `"$PSScriptRoot/late_guard_race.c`" `"$repo/src/lib-win32/process-hook/late_guard.c`" /Fe:late-guard-race.exe",
        'if errorlevel 1 exit /b %errorlevel%','late-guard-race.exe','exit /b %errorlevel%')
    [IO.File]::WriteAllLines((Join-Path $dir 'run.cmd'),$lines,[Text.Encoding]::ASCII)
    $p=Start-Process cmd -ArgumentList '/c','run.cmd' -WorkingDirectory $dir -WindowStyle Hidden -PassThru -RedirectStandardOutput "$build/$arch.stdout" -RedirectStandardError "$build/$arch.stderr"
    "pid=$($p.Id)" | Set-Content "$build/$arch.process"
    if(!$p.WaitForExit(120000)){& taskkill /PID $p.Id /T /F;throw "$arch test timeout"}
    $p.Refresh();"exit=$($p.ExitCode)" | Add-Content "$build/$arch.process"
    if($p.ExitCode){throw "$arch test failed; inspect $build/$arch.stdout and stderr"}
    $output=Get-Content -Raw "$build/$arch.stdout"
    if($output -notmatch 'new thread before suspension rejected' -or $output -notmatch 'new thread after suspension rejected'){
        throw "$arch test markers missing"
    }
    Write-Output "PASS $arch late-guard thread-set change tests"
}
