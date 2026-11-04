param([string]$BuildRoot='build/t475-s5/direct-mingw',[string]$Gcc32=$env:WIN16STYLE_GCC32,[string]$Gcc64=$env:WIN16STYLE_GCC64)
$ErrorActionPreference='Stop';$root=(Get-Location).Path;$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot));New-Item -ItemType Directory -Force $build | Out-Null
function Run([string]$file,[string[]]$arguments,[string]$dir,[string]$name) {
 $out=Join-Path $build "$name.stdout";$err=Join-Path $build "$name.stderr";$log=Join-Path $build "$name.process"
 $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $dir -RedirectStandardOutput $out -RedirectStandardError $err -WindowStyle Hidden -PassThru
 "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) command=$file $($arguments -join ' ') timeout=300s" | Set-Content $log
 if(!$p.WaitForExit(300000)){taskkill /PID $p.Id /T /F;throw "$name timeout"};$p.Refresh();"exit=$($p.ExitCode) end=$([DateTimeOffset]::UtcNow.ToString('o'))" | Add-Content $log
 if($p.ExitCode -ne 0){throw "$name failed: $out / $err"}
}
foreach($arch in @('x86','x64')){
 $gcc=if($arch -eq 'x86'){$Gcc32}else{$Gcc64}
 if(!$gcc){throw "supply the $arch GCC compiler path"}
 if(!(Test-Path -LiteralPath $gcc)){throw "missing compiler $gcc"}
 $dir=Join-Path $build $arch;New-Item -ItemType Directory -Force $dir | Out-Null
 $test=Join-Path $root 'tests/lib-win32/process-hook/payload_direct.c';$source=Join-Path $root 'src/lib-win32/process-hook/payload.c'
 $bin=Split-Path $gcc
 $lines=@('@echo off',"set `"PATH=$bin;%PATH%`"", "`"$gcc`" -std=c17 -Wall -Wextra -Werror -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0A00 -I `"$root/src`" `"$test`" `"$source`" -static -municode -o payload-direct.exe",'if errorlevel 1 exit /b %errorlevel%','exit /b 0')
 [IO.File]::WriteAllLines((Join-Path $dir 'build.cmd'),$lines,[Text.Encoding]::ASCII);Run 'cmd' @('/c','build.cmd') $dir "build-$arch"
}
foreach($from in @('x86','x64')){foreach($to in @('x86','x64')){
 $source=Join-Path $build "$from/payload-direct.exe";$target=Join-Path $build "$to/payload-direct.exe"
 Run $source @('"'+$target+'"') (Join-Path $build $from) "$from-to-$to"
 if((Get-Content -Raw (Join-Path $build "$from-to-$to.stdout")) -notmatch 'DIRECT_PAYLOAD_PASS'){throw 'missing payload marker'}
}}
$source=Join-Path $build 'x64/payload-direct.exe'
Run $source @('"'+$source+'"','--exhaust-low') (Join-Path $build 'x64') 'x64-low-exhausted'
if((Get-Content -Raw (Join-Path $build 'x64-low-exhausted.stdout')) -notmatch 'DIRECT_PAYLOAD_PASS'){throw 'missing low-exhaustion marker'}
'PASS MinGW x86/x64 helper-free payload matrix'
