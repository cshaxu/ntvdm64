[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$BaselineRoot,
 [Parameter(Mandatory)][string]$BuildCache,
 [Parameter(Mandatory)][string]$Wow32,
 [Parameter(Mandatory)][string]$NativeHook64,
 [Parameter(Mandatory)][string]$NativeWorker,
 [Parameter(Mandatory)][string]$NativeFrontend,
 [Parameter(Mandatory)][string]$NativeMonitor,
 [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Require fresh build-only negative evidence'}
$null=New-Item -ItemType Directory -Path $log
$base=@{BaselineRoot=$BaselineRoot;BuildCache=$BuildCache;Wow32=$Wow32;
 NativeHook=(Join-Path $BuildCache 'nthook32.dll');NativeHook64=$NativeHook64;
 NativeWorker=$NativeWorker;NativeFrontend=$NativeFrontend;NativeMonitor=$NativeMonitor}
$results=@(foreach($case in @(
 @{name='wrong-monitor-machine';input=(Join-Path $BaselineRoot 'system32/ntmon.exe');expected='Wrong PE machine input: ntmon.exe'},
 @{name='missing-native-selection';input='';expected='Wrong PE machine input: ntmon.exe'},
 @{name='outside-build';input='O:\winnt\system32\ntmon.exe';expected='Native monitor requires the native frontend and a build-owned input'}
)) {
 $arguments=@{};foreach($key in $base.Keys){$arguments[$key]=$base[$key]}
 $arguments.NativeMonitor=$case.input;$arguments.OutputRoot=Join-Path $log $case.name
 $errorText=$null
 try {& "$repo/tools/build/Stage-System32ProductPackage.ps1" @arguments}catch{$errorText=$_.Exception.Message}
 if(!$errorText -or !$errorText.Contains($case.expected)){throw "Incorrect negative result: $($case.name)/$errorText"}
 [pscustomobject]@{case=$case.name;rejected=$errorText}
})
$errorText=$null;$output=Join-Path $log 'wrong-import.exe'
try {& "$repo/tools/build/Stage-NativeWorkerImage.ps1" -InputFile (Join-Path $BaselineRoot 'system32/ntmon.exe') -OutputFile $output}catch{$errorText=$_.Exception.Message}
if(!$errorText -or !$errorText.Contains('Expected AMD64 native worker EXE') -or (Test-Path $output)){throw 'Invalid monitor import was not rejected before output'}
$results+=@{case='formal-import';rejected=$errorText}
$results|ConvertTo-Json|Set-Content (Join-Path $log 'results.json')
'PASS four native monitor package/import negatives; no x86 fallback'
