param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-dib-contracts *> "$out/build.log"
if($LASTEXITCODE){throw 'DIB fixture build failed'}
foreach($kind in 'failure_cleanup','retirement'){
 $p=Start-Process "$repo/build/.ninja/fixtures-x86/wow-dib-$kind.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$kind.txt" -RedirectStandardError "$out/$kind.err"
 try {
  if(!$p.WaitForExit(10000)){$p.Kill();$p.WaitForExit();throw 'Owned DIB fixture timeout'}
  $code=$p.ExitCode
  Get-Content "$out/$kind.txt"
  if($code){throw "DIB contract failed: $kind ($code)"}
 } finally {$p.Dispose()}
}
