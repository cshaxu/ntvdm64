param(
 [Parameter(Mandatory)][string]$X86,
 [Parameter(Mandatory)][string]$X64,
 [string]$BuildRoot='build/t475-s5/missing-hook-dll'
)
$ErrorActionPreference='Stop';$root=(Get-Location).Path
$build=[IO.Path]::GetFullPath((Join-Path $root $BuildRoot))
$allowed=(Resolve-Path 'build').Path+[IO.Path]::DirectorySeparatorChar
if(!$build.StartsWith($allowed,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)){throw 'Use a fresh build-owned directory'}
New-Item -ItemType Directory -Force $build | Out-Null
$source=@{x86=(Resolve-Path $X86).Path;x64=(Resolve-Path $X64).Path}
foreach($arch in @('x86','x64')) {
 $dir=Join-Path $build $arch;New-Item -ItemType Directory -Force $dir | Out-Null
 Copy-Item -LiteralPath $source[$arch] -Destination (Join-Path $dir 'nthook-install-test.exe')
}
foreach($from in @('x86','x64')) { foreach($to in @('x86','x64')) {
 $dir=Join-Path $build $from;$exe=Join-Path $dir 'nthook-install-test.exe';$target=Join-Path $build "$to/nthook-install-test.exe"
 foreach($hook in @('nthook32.dll','nthook64.dll')) {if(Test-Path (Join-Path $dir $hook)){throw "unexpected DLL: $hook"}}
 $name="$from-to-$to";$out=Join-Path $build "$name.stdout";$err=Join-Path $build "$name.stderr";$log=Join-Path $build "$name.process"
 $p=Start-Process $exe -ArgumentList @('missing-dll',('"'+$target+'"')) -WorkingDirectory $dir -RedirectStandardOutput $out -RedirectStandardError $err -WindowStyle Hidden -PassThru
 "pid=$($p.Id) start=$([DateTimeOffset]::UtcNow.ToString('o')) command=$exe missing-dll $target timeout=120s" | Set-Content $log
 if(!$p.WaitForExit(120000)){taskkill /PID $p.Id /T /F;throw "$name timed out"}
 $p.Refresh();"exit=$($p.ExitCode) end=$([DateTimeOffset]::UtcNow.ToString('o'))" | Add-Content $log
 if($p.ExitCode -ne 0 -or (Get-Content -Raw $out) -notmatch 'MISSING_HOOK_DLL_PAYLOAD_PASS'){throw "$name failed: $out / $err"}
} }
'PASS real product payload path with neither Hook DLL across all four width pairs'
