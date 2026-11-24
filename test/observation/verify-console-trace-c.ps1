param([string]$BuildRoot='build/t475-s5/console-trace')
$ErrorActionPreference='Stop';$root=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath($BuildRoot,$root)
New-Item -ItemType Directory -Force $build | Out-Null
foreach($arch in @('x86','x64')) {
 $dir=Join-Path $build $arch;New-Item -ItemType Directory -Force $dir | Out-Null
 & "$root/tools/build-fixtures.cmd" $arch observer-console *> "$build/$arch-build.stdout"
 if($LASTEXITCODE){throw "console observer build failed"}
 $previous=$env:WIN31_CONSOLE_TRACE
 try {
  $env:WIN31_CONSOLE_TRACE=Join-Path $dir 'trace.log'
  $p=Start-Process "$root/build/.ninja/fixtures-$arch/observer-console.exe" -WorkingDirectory $dir -RedirectStandardOutput "$build/$arch.stdout" -RedirectStandardError "$build/$arch.stderr" -WindowStyle Hidden -PassThru
  "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) timeout=60s" | Set-Content "$build/$arch.process"
  if(!$p.WaitForExit(60000)){taskkill /PID $p.Id /T /F;throw "$arch run timeout"};$p.Refresh();"exit=$($p.ExitCode)" | Add-Content "$build/$arch.process"
  if($p.ExitCode -or (Get-Content -Raw "$build/$arch.stdout") -notmatch 'CONSOLE_TRACE_ORIGINAL_PASS' -or (Get-Content -Raw $env:WIN31_CONSOLE_TRACE) -notmatch 'C-TRACE-WITNESS'){throw "$arch trace behavior failed"}
 } finally {$env:WIN31_CONSOLE_TRACE=$previous}
}
'PASS C startup Console trace actual hook and original behavior x86/x64'
