param([string]$BuildRoot='build/t475-s5/portability-checked')
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
New-Item -ItemType Directory -Force $build | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs)) { throw 'MSVC environment not found' }
$hook=Join-Path $root 'src/lib-win32/process-hook'
$shim=Join-Path $root 'src/compat/include/process_hook_msvc.h'
foreach ($arch in @('x86','x64')) {
    $dir=Join-Path $build $arch
    New-Item -ItemType Directory -Force $dir | Out-Null
    $hde=if($arch -eq 'x86'){'hde32'}else{'hde64'}
    $sources=@('staging.c','helper.c','startup_hooks.c','late_patch.c','late_guard.c','minhook/src/buffer.c','minhook/src/trampoline.c',"minhook/src/hde/$hde.c")
    $quoted=($sources | ForEach-Object { '"'+(Join-Path $hook $_)+'"' }) -join ' '
    $lines=@('@echo off',"call `"$vs`" -arch=$arch -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%')
    foreach ($name in @('link','payload')) {
        $test=Join-Path $root "tests/lib-win32/process-hook/$name.c"
        $lines+="cl /nologo /std:c11 /MT /FI `"$shim`" /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /I `"$root/src`" `"$test`" $quoted /Fe:$name.exe /link kernel32.lib user32.lib"
        $lines+='if errorlevel 1 exit /b %errorlevel%'
        $lines+="$name.exe"
        $lines+='if errorlevel 1 exit /b %errorlevel%'
    }
    $lines+='exit /b 0'
    $cmd=Join-Path $dir 'run.cmd'; [IO.File]::WriteAllLines($cmd,$lines,[Text.Encoding]::ASCII)
    $out=Join-Path $build "$arch.stdout"; $err=Join-Path $build "$arch.stderr"; $log=Join-Path $build "$arch.process"
    $p=Start-Process cmd -ArgumentList '/c','run.cmd' -WorkingDirectory $dir -RedirectStandardOutput $out -RedirectStandardError $err -WindowStyle Hidden -PassThru
    "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) command=$cmd timeout=300s" | Set-Content $log
    if (!$p.WaitForExit(300000)) { taskkill /PID $p.Id /T /F; throw "$arch timed out" }
    $p.Refresh(); "exit=$($p.ExitCode) end=$([DateTimeOffset]::UtcNow.ToString('o'))" | Add-Content $log
    if ($p.ExitCode -ne 0) { throw "$arch failed: $out / $err" }
    $text=Get-Content -Raw $out
    if ($text -notmatch 'PROCESS_HOOK_LINK_PASS' -or $text -notmatch 'PAYLOAD_ONLY_PASS') { throw "$arch missing fixture markers" }
}
Write-Output 'PASS original process-hook link and same-width payload fixtures x86/x64'
