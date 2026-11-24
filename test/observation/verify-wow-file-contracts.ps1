param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 wow-file-contracts *> "$out/build.log"
if($LASTEXITCODE){throw 'WOW file/conversion fixture build failed'}
foreach($name in 'wow-file-file_attributes_cleanup','wow-file-known_dll','wow-file-module_filename_boundary','wow-dde-conversion16','wow-dde-conversion32'){
 $p=Start-Process "$repo/build/.ninja/fixtures-x86/$name.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$name.txt" -RedirectStandardError "$out/$name.err"
 try {
  if(!$p.WaitForExit(30000)){$p.Kill();$p.WaitForExit();throw 'Owned WOW fixture timeout'}
  $code=$p.ExitCode;$text=Get-Content "$out/$name.txt" -Raw
  $text
  if($code){throw "$name exit $code"}
  if($name -match 'conversion(16|32)$'){
   if($text -notmatch "conversion$($Matches[1])_errors=0 guest_proof=0" -or ([regex]::Matches($text,'(?m)^case=')).Count -ne 4){throw 'Incomplete converter results'}
  }
 }finally{$p.Dispose()}
}
