param([ValidateSet('absent','w31','opennt-debug')][string]$Case)
$ErrorActionPreference='Stop'
if ($PSVersionTable.PSVersion.Major -lt 7) { throw 'PowerShell 7 is required for ProcessStartInfo.ArgumentList' }
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$probe=Join-Path $PSScriptRoot 'ddeml-module-path-probe.tmp.exe'
$runtime=$null;$scope=$null;$proc=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 $runtime=New-PhysicalRuntimeStage (Resolve-Path "$repo/build/t473-s9/package-r10").Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item -LiteralPath $probe -Destination (Join-Path $tests 'DDPATH.EXE')
 $sources=@{'w31'='O:\winnt\tests\w31p\SYSTEM\DDEML.DLL';'opennt-debug'="$repo/src/mvdm/wow16/ddeml/debug/DDEML.DLL"}
 $expected=@{'w31'='6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B';'opennt-debug'='4A8FAB7786A388882687E1A79D810534C94D0F9E05A313D22173C0C68C83A443'}
 if($Case -ne 'absent') {
  $src=$sources[$Case]; if((Get-IsolatedFileSha256 $src) -ne $expected[$Case]) {throw 'Pinned source DLL differs'}
  Copy-Item -LiteralPath $src -Destination (Join-Path $tests 'DDEML.DLL')
  if((Get-IsolatedFileSha256 (Join-Path $tests 'DDEML.DLL')) -ne $expected[$Case]) {throw 'Staged DLL differs'}
 }
 if(Test-Path (Join-Path $runtime 'system32/DDEML.DLL')) {throw 'Package contains an unexpected system32 DDEML'}
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path "$repo/build/.ninja/observations-x64/console-startup-observer.exe").Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log=Join-Path $PSScriptRoot "ddeml-path-$Case.observer.txt"
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/DDPATH.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 if(!$proc.WaitForExit(45000)){throw 'Observer did not finish within existing fixture supervisory bound'}
 $proc.Refresh(); if($proc.ExitCode){throw "Observer failed: $($proc.ExitCode)"}; if ((Get-Content -LiteralPath $log -Raw) -notmatch '(?m)^result=exited\s*$') { throw 'Observer did not report guest process exit; retain log as non-pass.' }
 $r=[IO.File]::ReadAllBytes((Join-Path $tests 'DDPATH.BIN'))
 if($r.Length -ne 272 -or [Text.Encoding]::ASCII.GetString($r,0,4) -ne 'DDP1'){throw "Invalid result bytes: $($r.Length)"}
 $h=[BitConverter]::ToUInt16($r,4);$n=[BitConverter]::ToUInt16($r,6);$off=[BitConverter]::ToUInt16($r,8);$sel=[BitConverter]::ToUInt16($r,10)
 $nul=[array]::IndexOf($r,[byte]0,12); if($nul -lt 12){throw 'Path not terminated'}
 $actual=[Text.Encoding]::ASCII.GetString($r,12,$nul-12)
 $hash=if($h -ge 32 -and (Test-Path -LiteralPath $actual)) {Get-IsolatedFileSha256 $actual} else {$null}
 $receipt=[ordered]@{case=$Case;moduleHandle=$h;pathLength=$n;path=$actual;ddeInitializeOffset=$off;ddeInitializeSelector=$sel;resolvedFileSha256=$hash;inputSha256=if($Case -eq 'absent'){$null}else{$expected[$Case]};probeSha256=Get-IsolatedFileSha256 $probe;originalGuestModified=$false}
 $receipt|ConvertTo-Json|Set-Content (Join-Path $PSScriptRoot "ddeml-path-$Case.result.json") -Encoding utf8
 if($Case -eq 'absent' -and $h -ge 32) {throw 'Unexpected DDEML resolved for absent case'}
 if($Case -ne 'absent' -and ($h -lt 32 -or !$n -or !$sel -or $hash -ne $expected[$Case])){throw 'Guest did not resolve exact intended DDEML and exported ordinal'}
} finally {
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}} finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
