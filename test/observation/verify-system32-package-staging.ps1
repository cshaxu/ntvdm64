param([Parameter(Mandatory)][string]$BaselineRoot,
      [Parameter(Mandatory)][string]$BuildCache,
      [Parameter(Mandatory)][string]$Wow32,
      [Parameter(Mandatory)][string]$NativeWorker,
      [Parameter(Mandatory)][string]$NativeHook,
      [Parameter(Mandatory)][string]$NativeHook64,
      [string]$NativeFrontend='', [string]$NativeMonitor='',
      [string]$NativeLauncher='', [string]$NativeService='',
      [Parameter(Mandatory)][string]$RunRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
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
. "$PSScriptRoot/Invoke-System32Stage.ps1"
$output=Join-Path $run 'candidate'
$common=@{BaselineRoot=$fixture;BuildCache=$BuildCache;Wow32=$Wow32;
 NativeWorker=$NativeWorker;NativeHook=$NativeHook;NativeHook64=$NativeHook64;
 NativeFrontend=$NativeFrontend;NativeMonitor=$NativeMonitor;
 NativeLauncher=$NativeLauncher;NativeService=$NativeService}
$common.OutputRoot=$output
Invoke-System32Stage $common
$rows=Get-Content -LiteralPath (Join-Path $output 'product-system32-manifest.json') -Raw | ConvertFrom-Json
if($rows.Count -ne 11 -or @($rows | Select-Object -ExpandProperty PackageId -Unique).Count -ne 1){
 throw 'Expected eleven components with one package identity'
}
foreach($name in @('VCDEX.DLL','run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')){
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
try{$common.OutputRoot=Join-Path $run 'conflict';Invoke-System32Stage $common}
catch{$conflictRejected=$_.Exception.Message.Contains('Conflicting Registry overlays')}
if(!$conflictRejected -or (Get-FileHash $registry).Hash -ne $hash){throw 'Conflicting overlays were merged or source mutated'}
if(Test-Path -LiteralPath (Join-Path $run 'conflict/product-system32-manifest.json')){
 throw 'Failed staging retained a completion manifest'
}
$negativeCases=@(
 @{name='wrong-worker';key='NativeWorker';value=$Wow32;expected='Wrong PE machine input: ntvwm.exe'},
 @{name='missing-hook64';key='NativeHook64';value='';expected='Native worker requires both Hooks'},
 @{name='outside-worker';key='NativeWorker';value="$repo/assets/release/ntvwm.exe";expected='Native worker requires both Hooks'},
 @{name='outside-diagnostic';key='DiagnosticNtvdm';value="$repo/assets/release/ntvdm.exe";expected='Require a build-owned diagnostic NTVDM image and link map'},
 @{name='missing-diagnostic-map';key='DiagnosticNtvdm';value="$run/absent.exe";expected='Require a build-owned diagnostic NTVDM image and link map'},
 @{name='missing-vcdex';key='Vcdex';value="$run/absent.dll";expected='Wrong PE machine input: VCDEX.DLL'},
 @{name='empty-identity';key='PackageId';value='00000000-0000-0000-0000-000000000000';expected='Native package identity write failed'}
)
foreach($case in $negativeCases){
 $argsCase=$common.Clone();$argsCase[$case.key]=$case.value
 $argsCase.OutputRoot=Join-Path $run $case.name
 # Avoid the intentional registry conflict created by the earlier case.
 $argsCase.BaselineRoot=$baseline
 $message=''
 try {Invoke-System32Stage $argsCase}catch{$message=$_.Exception.Message}
 if(!$message.Contains($case.expected)){throw "Wrong staging rejection $($case.name): $message"}
 if(Test-Path (Join-Path $argsCase.OutputRoot 'product-system32-manifest.json')){throw 'Rejected package has completion manifest'}
}
$existing=$common.Clone();$existing.OutputRoot=$output
$beforeManifest=(Get-FileHash (Join-Path $output 'product-system32-manifest.json')).Hash
$message='';try{Invoke-System32Stage $existing}catch{$message=$_.Exception.Message}
if(!$message.Contains('fresh repository-build staging root') -or (Get-FileHash (Join-Path $output 'product-system32-manifest.json')).Hash -ne $beforeManifest){throw 'Existing stage was not preserved'}
[pscustomobject]@{OverlayMovedByteExactly=$true;SourcePreserved=$true;
    MixedLayoutRejected=$mixedRejected;ConflictingOverlaysRejected=$conflictRejected}|
 ConvertTo-Json|Set-Content (Join-Path $run 'results.json')
'PASS system32 eleven-file staging, one package identity, overlay preservation and conflict/mixed-layout negatives'
