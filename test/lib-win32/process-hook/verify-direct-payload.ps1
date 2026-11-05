param([string]$BuildRoot='build/t475-s5/direct-payload')
$ErrorActionPreference='Stop'; $root=(Get-Location).Path
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot)); New-Item -ItemType Directory -Force $build | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
function Invoke-Owned([string]$file,[string[]]$arguments,[string]$dir,[string]$name) {
 $out=Join-Path $build "$name.stdout"; $err=Join-Path $build "$name.stderr"; $log=Join-Path $build "$name.process"
 $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $dir -RedirectStandardOutput $out -RedirectStandardError $err -WindowStyle Hidden -PassThru
 "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) command=$file $($arguments -join ' ') timeout=300s" | Set-Content $log
 if (!$p.WaitForExit(300000)) { taskkill /PID $p.Id /T /F; throw "$name timed out" }
 $p.Refresh(); "exit=$($p.ExitCode) end=$([DateTimeOffset]::UtcNow.ToString('o'))" | Add-Content $log
 if ($p.ExitCode -ne 0) { throw "$name failed: $out / $err" }
}
foreach ($arch in @('x86','x64')) {
 $dir=Join-Path $build $arch; New-Item -ItemType Directory -Force $dir | Out-Null
 $test=Join-Path $root 'test/lib-win32/process-hook/payload_direct.c';$source=Join-Path $root 'src/lib-win32/process-hook/payload.c'
 $lines=@('@echo off',"call `"$vs`" -arch=$arch -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',
 "cl /nologo /std:c11 /MT /W4 /we4013 /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /I `"$root/src`" `"$test`" `"$source`" /Fe:payload-direct.exe /link kernel32.lib",'if errorlevel 1 exit /b %errorlevel%','exit /b 0')
 [IO.File]::WriteAllLines((Join-Path $dir 'build.cmd'),$lines,[Text.Encoding]::ASCII)
 Invoke-Owned 'cmd' @('/c','build.cmd') $dir "build-$arch"
}
foreach ($from in @('x86','x64')) { foreach ($to in @('x86','x64')) {
 $source=Join-Path $build "$from/payload-direct.exe";$target=Join-Path $build "$to/payload-direct.exe"
 Invoke-Owned $source @('"'+$target+'"') (Join-Path $build $from) "$from-to-$to"
 if ((Get-Content -Raw (Join-Path $build "$from-to-$to.stdout")) -notmatch 'DIRECT_PAYLOAD_PASS') {throw 'missing direct payload marker'}
} }
$source=Join-Path $build 'x64/payload-direct.exe'
Invoke-Owned $source @('"'+$source+'"','--exhaust-low') (Join-Path $build 'x64') 'x64-low-exhausted'
if((Get-Content -Raw (Join-Path $build 'x64-low-exhausted.stdout')) -notmatch 'DIRECT_PAYLOAD_PASS'){throw 'missing low-exhaustion marker'}
'PASS helper-free payload-only staging across all four x86/x64 pairs'
