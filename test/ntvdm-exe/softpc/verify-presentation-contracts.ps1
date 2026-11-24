param([Parameter(Mandatory)][string]$OutputRoot,[ValidateSet('all','font','startup')][string]$Kind='all')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot,$repo)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 "presentation-contract-$Kind" *> "$out/build.log"
if($LASTEXITCODE){throw 'Presentation fixture build failed'}
$kinds=if($Kind -eq 'all'){@('font','startup')}else{@($Kind)}
foreach($contract in $kinds){
 foreach($case in 'fixed','regression-control'){
  $p=Start-Process "$repo/build/.ninja/fixtures-x86/presentation-$contract-$case.exe" -WorkingDirectory $out -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$contract-$case.txt" -RedirectStandardError "$out/$contract-$case.err"
  try {
   if(!$p.WaitForExit(10000)){$p.Kill();$p.WaitForExit();throw 'Owned presentation fixture timeout'}
   $code=$p.ExitCode;$expected=if($case -eq 'fixed'){0}else{1}
   if($code -ne $expected){throw "Unexpected $contract $case exit: $code (expected $expected)"}
   "PASS $contract $case expected exit=$code"
  } finally {$p.Dispose()}
 }
}
