param([Parameter(Mandatory)][string]$BaselineRoot,
      [Parameter(Mandatory)][string]$BuildCache,
      [Parameter(Mandatory)][string]$Wow32,
      [Parameter(Mandatory)][string]$RunRoot,
      [string]$NativeWorker32='',[string]$NativeWorker64='',
      [string]$NativeHook32='')
$ErrorActionPreference='Stop'
$baseline=(Resolve-Path $BaselineRoot).Path
$run=[IO.Path]::GetFullPath($RunRoot)
$build=(Resolve-Path "$PSScriptRoot/../../build").Path+'\'
if(!$run.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $run)){
 throw 'Require a fresh repository-build fixture root'
}
$null=New-Item -ItemType Directory -Path $run
$fixture=Join-Path $run 'baseline'
Copy-Item -LiteralPath $baseline -Destination $fixture -Recurse
# Authored overlay fixture, not guest media or a running user's Registry.
$registry=Join-Path $fixture 'NTVDM.REG'
if(Test-Path $registry){throw 'Use a baseline without user overlay for this fixture'}
[IO.File]::WriteAllText($registry,'S5-REGISTRY-PRESERVATION-FIXTURE',[Text.Encoding]::ASCII)
$hash=(Get-FileHash $registry).Hash
$stage="$PSScriptRoot/../../tools/build/Stage-System32ProductPackage.ps1"
$output=Join-Path $run 'candidate'
& $stage -BaselineRoot $fixture -BuildCache $BuildCache -Wow32 $Wow32 -OutputRoot $output
foreach($name in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL')){
 if((Test-Path (Join-Path $output $name)) -or !(Test-Path (Join-Path $output ('system32\'+$name)))){
  throw 'Wrong host layout'
 }
}
if((Get-FileHash (Join-Path $output 'system32\NTVDM.REG')).Hash -ne $hash -or
   (Test-Path (Join-Path $output 'NTVDM.REG')) -or (Get-FileHash $registry).Hash -ne $hash){
 throw 'Registry bytes/source ownership changed'
}
$mixedRejected=$false
. "$PSScriptRoot/isolated_package_cleanup.ps1"
Copy-Item -LiteralPath (Join-Path $output 'system32\run16.exe') -Destination (Join-Path $output 'run16.exe')
try{$null=Get-PackageBinaryRoot $output}catch{$mixedRejected=$_.Exception.Message -eq 'Mixed product binary layout'}
if(!$mixedRejected){throw 'Mixed test-cleanup layout was accepted'}
$conflict=Join-Path $fixture 'system32\NTVDM.REG'
[IO.File]::WriteAllText($conflict,'DIFFERENT-OVERLAY',[Text.Encoding]::ASCII)
$conflictRejected=$false
try{& $stage -BaselineRoot $fixture -BuildCache $BuildCache -Wow32 $Wow32 -OutputRoot (Join-Path $run 'conflict')}
catch{$conflictRejected=$_.Exception.Message -eq 'Conflicting Registry overlays'}
if(!$conflictRejected -or (Get-FileHash $registry).Hash -ne $hash){throw 'Conflicting overlays were merged or source mutated'}
$dualNegatives=@()
if($NativeWorker32 -or $NativeWorker64 -or $NativeHook32){
 if(!$NativeWorker32 -or !$NativeWorker64 -or !$NativeHook32){throw 'Supply all three native fixture inputs'}
 $cases=@(
   @{Name='incomplete';Arguments=@{NativeWorker32=$NativeWorker32};Expected='Dual-width package requires both workers and both hooks'},
   @{Name='wrong-worker';Arguments=@{NativeWorker32=$NativeWorker32;NativeWorker64=$NativeWorker32;NativeHook=$NativeHook32;NativeHook64=$NativeWorker64};Expected='Wrong machine input: ntvwm64.exe'},
   @{Name='wrong-hook';Arguments=@{NativeWorker32=$NativeWorker32;NativeWorker64=$NativeWorker64;NativeHook=$NativeHook32;NativeHook64=$NativeWorker64};Expected='Wrong image type: nthook64.dll'}
 )
 foreach($case in $cases){
   $rejected=$false;$negativeOutput=Join-Path $run $case.Name
   $arguments=$case.Arguments
   try{& $stage -BaselineRoot $fixture -BuildCache $BuildCache -Wow32 $Wow32 -OutputRoot $negativeOutput @arguments}
   catch{$rejected=$_.Exception.Message -eq $case.Expected}
   if(!$rejected -or (Test-Path $negativeOutput)){throw "Dual-width preflight failed: $($case.Name)"}
   $dualNegatives+=$case.Name
 }
}
[pscustomobject]@{OverlayMovedByteExactly=$true;SourcePreserved=$true;
    MixedLayoutRejected=$mixedRejected;ConflictingOverlaysRejected=$conflictRejected;
    DualWidthPreflightNegatives=$dualNegatives}|
 ConvertTo-Json|Set-Content (Join-Path $run 'results.json')
'PASS system32 eight-file staging, overlay preservation and conflict/mixed-layout negatives'
