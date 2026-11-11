[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$BaselineRoot,
 [Parameter(Mandatory)][string]$BuildCache,
 [Parameter(Mandatory)][string]$Wow32,
 [Parameter(Mandatory)][string]$NativeHook,
 [Parameter(Mandatory)][string]$NativeHook64,
 [Parameter(Mandatory)][string]$NativeWorker,
 [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$log=[IO.Path]::GetFullPath($LogRoot)
$build=(Join-Path $repo 'build')+'\'
if(!$log.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Require fresh build-only negative evidence'}
$null=New-Item -ItemType Directory -Path $log
$base=@{BaselineRoot=$BaselineRoot;BuildCache=$BuildCache;Wow32=$Wow32;
 NativeHook=$NativeHook;NativeHook64=$NativeHook64;NativeWorker=$NativeWorker}
$cases=@(
 @{Name='wrong-worker-machine';Worker=(Join-Path $BaselineRoot 'system32\ntvwm.exe');Hook64=$NativeHook64;Expected='Wrong PE machine input: ntvwm.exe'},
 @{Name='missing-hook64';Worker=$NativeWorker;Hook64='';Expected='Native worker requires both Hooks'},
 @{Name='outside-build';Worker='O:\winnt\system32\ntvwm.exe';Hook64=$NativeHook64;Expected='Native worker requires both Hooks'}
)
foreach($case in $cases) {
 $arguments=@{};foreach($key in $base.Keys){$arguments[$key]=$base[$key]}
 $arguments.NativeWorker=$case.Worker;$arguments.NativeHook64=$case.Hook64
 $arguments.OutputRoot=Join-Path $log $case.Name
 $errorText=$null
 try {& "$repo/tools/build/Stage-System32ProductPackage.ps1" @arguments}catch{$errorText=$_.Exception.Message}
 if(!$errorText -or !$errorText.Contains($case.Expected)){throw "Wrong negative result: $($case.Name): $errorText"}
 [pscustomobject]@{Case=$case.Name;Expected=$case.Expected;Actual=$errorText}|
  ConvertTo-Json|Set-Content (Join-Path $log ($case.Name+'.json'))
 "PASS native worker staging rejection $($case.Name)"
}
# Formal graph import rejects the old carrier before writing its output.
$rejected=Join-Path $log 'rejected-worker.exe'
$errorText=$null
try {& "$repo/tools/build/Stage-NativeWorkerImage.ps1" -InputFile (Join-Path $BaselineRoot 'system32\ntvwm.exe') -OutputFile $rejected}catch{$errorText=$_.Exception.Message}
if(!$errorText -or !$errorText.Contains('Expected AMD64 native worker EXE') -or (Test-Path $rejected)){throw 'Formal worker import admitted an x86 carrier'}
'PASS formal native worker import rejects I386 before output'
# Staging copies are test evidence only. No process, endpoint, substitution,
# deployment or normal-lifecycle cleanup is exercised by these negatives.
