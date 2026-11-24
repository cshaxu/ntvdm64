param([string]$BuildRoot='build/t475-s5/palette-trace')
$ErrorActionPreference='Stop';$root=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath($BuildRoot,$root);New-Item -ItemType Directory -Force $build | Out-Null
foreach($arch in @('x86','x64')) {
 $dir=Join-Path $build $arch;New-Item -ItemType Directory -Force $dir | Out-Null
 & "$root/tools/build-fixtures.cmd" $arch observer-palette *> "$build/$arch-build.stdout"
 if($LASTEXITCODE){throw "palette observer build failed"}
 $previous=$env:WIN101_PALETTE_TRACE
 try {
  $env:WIN101_PALETTE_TRACE=Join-Path $dir 'trace.log'
  $p=Start-Process "$root/build/.ninja/fixtures-$arch/observer-palette.exe" -WorkingDirectory $dir -RedirectStandardOutput "$build/$arch.stdout" -RedirectStandardError "$build/$arch.stderr" -WindowStyle Hidden -PassThru
  "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) timeout=60s" | Set-Content "$build/$arch.process"
  if(!$p.WaitForExit(60000)){taskkill /PID $p.Id /T /F;throw "$arch run timeout"};$p.Refresh();"exit=$($p.ExitCode)" | Add-Content "$build/$arch.process"
  if($p.ExitCode -or (Get-Content -Raw "$build/$arch.stdout") -notmatch 'PALETTE_TRACE_ORIGINAL_PASS'){throw "$arch original behavior failed"}
  $log=Get-Content -Raw $env:WIN101_PALETTE_TRACE
  foreach($name in @('text-cache','get-next','io-operation-27','gdi-fallback')){if($log -notmatch [regex]::Escape($name)){throw "$arch missing $name"}}
 } finally {$env:WIN101_PALETTE_TRACE=$previous}
}
'PASS C startup palette trace actual hooks, calls and errors x86/x64'
