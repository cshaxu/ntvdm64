param([string]$BuildRoot='build/t475-s5/cross-payload')
$ErrorActionPreference='Stop'
$root=(Get-Location).Path
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
New-Item -ItemType Directory -Force $build | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs)) { throw 'MSVC environment not found' }
function Invoke-Owned([string]$file,[string[]]$arguments,[string]$dir,[string]$name) {
    $out=Join-Path $build "$name.stdout"; $err=Join-Path $build "$name.stderr"; $log=Join-Path $build "$name.process"
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $dir -RedirectStandardOutput $out -RedirectStandardError $err -WindowStyle Hidden -PassThru
    "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) command=$file $($arguments -join ' ') timeout=300s" | Set-Content $log
    if (!$p.WaitForExit(300000)) { taskkill /PID $p.Id /T /F; throw "$name timed out" }
    $p.Refresh(); "exit=$($p.ExitCode) end=$([DateTimeOffset]::UtcNow.ToString('o'))" | Add-Content $log
    if ($p.ExitCode -ne 0) { throw "$name failed: $out / $err" }
}
$stage=Join-Path $root 'src/lib-win32/process-hook/staging.c'
$helper=Join-Path $root 'src/lib-win32/process-hook/helper.c'
$test=Join-Path $root 'test/lib-win32/process-hook/cross_payload.c'
$dll=Join-Path $root 'test/lib-win32/process-hook/cross_helper.c'
$def=Join-Path $root 'test/lib-win32/process-hook/cross_helper.def'
foreach ($arch in @('x86','x64')) {
    $dir=Join-Path $build $arch; New-Item -ItemType Directory -Force $dir | Out-Null
    $lines=@('@echo off',"call `"$vs`" -arch=$arch -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
        "cl /nologo /std:c11 /MT /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /I `"$root/src`" `"$test`" `"$stage`" `"$helper`" /Fe:cross-payload.exe /link kernel32.lib user32.lib",'if errorlevel 1 exit /b %errorlevel%',
        "cl /nologo /std:c11 /MT /LD /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /I `"$root/src`" `"$dll`" `"$stage`" `"$helper`" /Fe:cross-helper.dll /link /def:`"$def`" kernel32.lib user32.lib",'if errorlevel 1 exit /b %errorlevel%',
        'exit /b 0')
    [IO.File]::WriteAllLines((Join-Path $dir 'build.cmd'),$lines,[Text.Encoding]::ASCII)
    Invoke-Owned 'cmd' @('/c','build.cmd') $dir "build-$arch"
}
foreach ($from in @('x86','x64')) {
    foreach ($to in @('x86','x64')) {
        $source=Join-Path $build "$from/cross-payload.exe"
        $target=Join-Path $build "$to/cross-payload.exe"
        $dll=Join-Path $build "$to/cross-helper.dll"
        $mode=if($from -eq $to){'same'}else{'helper'}
        Invoke-Owned $source @('"'+$target+'"','"'+$dll+'"',$mode) (Join-Path $build $from) "$from-to-$to"
        if ((Get-Content -Raw (Join-Path $build "$from-to-$to.stdout")) -notmatch 'CROSS_PAYLOAD_PASS') { throw "missing $from-to-$to marker" }
    }
}
foreach ($pair in @(@('x86','x64'),@('x64','x86'))) {
    $from=$pair[0]; $to=$pair[1]
    $source=Join-Path $build "$from/cross-payload.exe"
    $target=Join-Path $build "$to/cross-payload.exe"
    $dll=Join-Path $build "$to/cross-helper.dll"
    Invoke-Owned $source @('"'+$target+'"','"'+$dll+'"','reject') (Join-Path $build $from) "$from-reject-$to"
    if ((Get-Content -Raw (Join-Path $build "$from-reject-$to.stdout")) -notmatch 'CROSS_REJECT_PASS') { throw "missing rejection marker" }
}
'PASS payload-only staging across all four x86/x64 pairs'
