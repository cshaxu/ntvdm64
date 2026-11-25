param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$PackageRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-output-contracts *> "$out/build.log"
if($LASTEXITCODE){throw 'WOW output fixture build failed'}
New-Item -ItemType Directory "$out/data"|Out-Null
$saved=$env:T478_EMBEDDING_ROOT
try {
 $env:T478_EMBEDDING_ROOT="$out/data"
 foreach($kind in 'embedding_root','fax_failure','font_output'){
  $p=Start-Process "$repo/build/.ninja/fixtures-x86/wow-output-$kind.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$kind.txt" -RedirectStandardError "$out/$kind.err"
  try {
   if(!$p.WaitForExit(10000)){$p.Kill();$p.WaitForExit();throw "Owned $kind fixture timeout"}
   $code=$p.ExitCode
   Get-Content "$out/$kind.txt"
   if($code){throw "WOW output contract failed: $kind ($code)"}
  } finally {$p.Dispose()}
 }
 if(Test-Path "$out/data/NTVDM.REG"){throw 'Embedding fixture did not retire its file'}
} finally {$env:T478_EMBEDDING_ROOT=$saved}
Get-FileHash "$repo/build/.ninja/fixtures-x86/wow-output-*.exe" | ConvertTo-Json | Set-Content "$out/hashes.json"
& nasm -f bin -o "$out/GDIPROBE.EXE" "$repo/test/observation/win16_gdi_output.asm"
if($LASTEXITCODE){throw 'GDI guest assembly failed'}
. "$repo/test/observation/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $PackageRoot -FreshPackageIdentity -TestFiles @("$out/GDIPROBE.EXE")
$scope=New-IsolatedPackageScope $runtime
$previousDesktop=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 & "$repo/build/.ninja/fixtures-x64/console-startup-observer.exe" "$runtime/system32/run16.exe" "$runtime/tests" "$out/gdi.txt" --wait "$runtime/tests/GDIPROBE.EXE" --observation-timeout-ms 40000
 if($LASTEXITCODE -or !(Test-Path "$runtime/tests/GDIOK.DAT")){throw 'Win16 GDI output verification failed'}
 Copy-Item -LiteralPath "$runtime/tests/GDIOK.DAT" -Destination $out
} finally {
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$previousDesktop
 Stop-IsolatedPackageScope $scope
 Remove-PhysicalRuntimeStage $runtime
}
