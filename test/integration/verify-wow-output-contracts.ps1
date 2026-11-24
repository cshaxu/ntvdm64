param([Parameter(Mandatory)][string]$BuildRoot)
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
 foreach($kind in 'embedding_root','fax_failure','font_output','palette_output'){
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
