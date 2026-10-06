[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$NativeBuild,
 [Parameter(Mandatory)][string]$BaselineRoot,
 [Parameter(Mandatory)][string]$BuildCache,
 [Parameter(Mandatory)][string]$Wow32,
 [Parameter(Mandatory)][string]$NativeHook64,
 [Parameter(Mandatory)][string]$NativeWorker,
 [Parameter(Mandatory)][string]$LogRoot
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$build=(Resolve-Path $NativeBuild).Path
$log=[IO.Path]::GetFullPath($LogRoot)
if(!$log.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $log)){throw 'Require fresh build-only evidence'}
$null=New-Item -ItemType Directory -Path $log
$results=[Collections.Generic.List[object]]::new()
foreach($name in 'frontend-session-arguments','frontend-window-library','frontend-window-controller','frontend-window-keyboard','frontend-window-mouse','console-pointer-contract','console-frontend','console-video') {
 $output=& (Join-Path $build ($name+'-test.exe')) 2>&1
 $code=$LASTEXITCODE
 $output|Set-Content (Join-Path $log ($name+'.txt'))
 $results.Add(@{case=$name;exit=$code})
 if($code){throw "Native frontend fixture failed: $name/$code"}
}
# This storage fixture needs a real Console. Launch a hidden, test-owned one;
# never attach to or change the owner's Console. Its own assertions stay intact.
$report=Join-Path $log 'text-handoff.txt'
$child=Start-Process -FilePath (Join-Path $build 'frontend-text-handoff-test.exe') -ArgumentList ('"'+$report+'"') -WindowStyle Hidden -PassThru
try {
 if(!$child.WaitForExit(20000)){$child.Kill();throw 'Text handoff fixture timeout'}
 if($child.ExitCode -or !(Test-Path $report)){throw 'Text handoff fixture failed or omitted report'}
 $results.Add(@{case='text-handoff';exit=$child.ExitCode})
}finally{$child.Dispose()}
foreach($name in 'console-channel-lifetime','console-frame-failure') {
 $report=Join-Path $log ($name+'.txt')
 $output=& (Join-Path $build ($name+'-test.exe')) --private-desktop-full $report
 $code=$LASTEXITCODE
 $output|Set-Content (Join-Path $log ($name+'-launch.txt'))
 if($code -or !(Test-Path $report)){throw "Native frontend channel fixture failed: $name/$code"}
 $results.Add(@{case=$name;exit=$code})
}
$arguments=@{BaselineRoot=$BaselineRoot;BuildCache=$BuildCache;Wow32=$Wow32;
 NativeHook=(Join-Path $BuildCache 'nthook32.dll');NativeHook64=$NativeHook64;NativeWorker=$NativeWorker}
foreach($case in @(
 @{name='wrong-frontend-machine';image=(Join-Path $BaselineRoot 'system32/ntcon.exe');expected='Wrong PE machine input: ntcon.exe'},
 @{name='outside-build-frontend';image='O:\winnt\system32\ntcon.exe';expected='Native frontend requires the native worker and a build-owned input'}
)) {
 $arguments.NativeFrontend=$case.image;$arguments.OutputRoot=Join-Path $log $case.name
 $message=$null
 try {& "$repo/tools/build/Stage-System32ProductPackage.ps1" @arguments}catch{$message=$_.Exception.Message}
 if(!$message -or !$message.Contains($case.expected)){throw "Incorrect negative result: $($case.name)/$message"}
 $results.Add(@{case=$case.name;rejected=$message})
}
$results|ConvertTo-Json -Depth 4|Set-Content (Join-Path $log 'results.json')
'PASS eleven AMD64 frontend fixtures and two explicit package-input negatives'
