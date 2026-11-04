param([string]$BuildRoot='build/t475-s5/console-trace')
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
New-Item -ItemType Directory -Force $build | Out-Null
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
foreach($arch in @('x86','x64')) {
 $dir=Join-Path $build $arch;New-Item -ItemType Directory -Force $dir | Out-Null
 $hde=if($arch -eq 'x86'){'hde32'}else{'hde64'}
 $files=@('tests/observation/fixtures/console_trace_fixture.c','tests/observation/win31_console_trace.c','src/lib-win32/process-hook/startup_hooks.c','src/lib-win32/process-hook/minhook/src/buffer.c','src/lib-win32/process-hook/minhook/src/trampoline.c',"src/lib-win32/process-hook/minhook/src/hde/$hde.c")
 $sources=($files | ForEach-Object {'"'+(Join-Path $root $_)+'"'}) -join ' '
 $shim=Join-Path $root 'src/compat/include/process_hook_msvc.h'
 $cmd=@('@echo off',"call `"$vs`" -arch=$arch -host_arch=x64 >nul",'if errorlevel 1 exit /b %errorlevel%',"cl /nologo /std:c11 /MT /W4 /we4013 /D_WIN32_WINNT=0x0A00 /FI `"$shim`" $sources /Fe:trace.exe /link kernel32.lib",'if errorlevel 1 exit /b %errorlevel%','exit /b 0')
 [IO.File]::WriteAllLines((Join-Path $dir 'build.cmd'),$cmd,[Text.Encoding]::ASCII)
 $p=Start-Process cmd -ArgumentList '/c','build.cmd' -WorkingDirectory $dir -RedirectStandardOutput "$build/$arch-build.stdout" -RedirectStandardError "$build/$arch-build.stderr" -WindowStyle Hidden -PassThru
 "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) timeout=300s" | Set-Content "$build/$arch-build.process"
 if(!$p.WaitForExit(300000)){taskkill /PID $p.Id /T /F;throw "$arch build timeout"};$p.Refresh();"exit=$($p.ExitCode)" | Add-Content "$build/$arch-build.process";if($p.ExitCode){throw "$arch build failed"}
 $previous=$env:WIN31_CONSOLE_TRACE
 try {
  $env:WIN31_CONSOLE_TRACE=Join-Path $dir 'trace.log'
  $p=Start-Process (Join-Path $dir 'trace.exe') -WorkingDirectory $dir -RedirectStandardOutput "$build/$arch.stdout" -RedirectStandardError "$build/$arch.stderr" -WindowStyle Hidden -PassThru
  "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) timeout=60s" | Set-Content "$build/$arch.process"
  if(!$p.WaitForExit(60000)){taskkill /PID $p.Id /T /F;throw "$arch run timeout"};$p.Refresh();"exit=$($p.ExitCode)" | Add-Content "$build/$arch.process"
  if($p.ExitCode -or (Get-Content -Raw "$build/$arch.stdout") -notmatch 'CONSOLE_TRACE_ORIGINAL_PASS' -or (Get-Content -Raw $env:WIN31_CONSOLE_TRACE) -notmatch 'C-TRACE-WITNESS'){throw "$arch trace behavior failed"}
 } finally {$env:WIN31_CONSOLE_TRACE=$previous}
}
'PASS C startup Console trace actual hook and original behavior x86/x64'
