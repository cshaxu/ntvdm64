param([ValidateSet('w31','opennt-debug')][string]$Case)
$ErrorActionPreference='Stop'
if ($PSVersionTable.PSVersion.Major -lt 7) {throw 'PowerShell 7 is required'}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$probe=Join-Path $PSScriptRoot 'ddeml-init-probe.tmp.exe'
$sourceMap=@{'w31'='O:\winnt\tests\w31p\SYSTEM\DDEML.DLL';'opennt-debug'="$repo/src/mvdm/wow16/ddeml/debug/DDEML.DLL"}
$expected=@{'w31'='6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B';'opennt-debug'='4A8FAB7786A388882687E1A79D810534C94D0F9E05A313D22173C0C68C83A443'}
$runtime=$null;$scope=$null;$proc=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 if((Get-IsolatedFileSha256 $sourceMap[$Case]) -ne $expected[$Case]){throw 'Input DDEML hash changed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path "$repo/build/t473-s9/package-r10").Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item -LiteralPath $probe -Destination (Join-Path $tests 'DDINIT.EXE')
 Copy-Item -LiteralPath $sourceMap[$Case] -Destination (Join-Path $tests 'DDEML.DLL')
 if((Get-IsolatedFileSha256 (Join-Path $tests 'DDEML.DLL')) -ne $expected[$Case]){throw 'Copied DDEML differs'}
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path "$repo/build/.ninja/observations-x64/console-startup-observer.exe").Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log=Join-Path $PSScriptRoot "ddeml-init-$Case.observer.txt"
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/DDINIT.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 if(!$proc.WaitForExit(45000)){throw 'Observer did not finish within test supervisory bound'}
 $proc.Refresh();if($proc.ExitCode){throw "Observer process failed: $($proc.ExitCode)"}
 if((Get-Content -LiteralPath $log -Raw) -notmatch '(?m)^result=exited\s*$'){throw 'Observer did not report process exit'}
 $resultFile=Join-Path $tests 'DINIT.BIN'
 if(!(Test-Path -LiteralPath $resultFile)){throw 'Win16 probe did not produce result; inspect observer'}
 $r=[IO.File]::ReadAllBytes($resultFile)
 if($r.Length -ne 286 -or [Text.Encoding]::ASCII.GetString($r,0,4) -ne 'DDI1'){throw "Invalid result bytes: $($r.Length)"}
 $nul=[array]::IndexOf($r,[byte]0,26);if($nul -lt 26){throw 'Module path not terminated'}
 $actual=[Text.Encoding]::ASCII.GetString($r,26,$nul-26)
 $hash=if(Test-Path -LiteralPath $actual){Get-IsolatedFileSha256 $actual}else{$null}
 $record=[ordered]@{case=$Case;moduleHandle=[BitConverter]::ToUInt16($r,4);pathLength=[BitConverter]::ToUInt16($r,6);invalidReserved=[BitConverter]::ToUInt16($r,8);init=[BitConverter]::ToUInt16($r,10);instance=[BitConverter]::ToUInt32($r,12);reinit=[BitConverter]::ToUInt16($r,16);reinitInstance=[BitConverter]::ToUInt32($r,18);uninit=[BitConverter]::ToUInt16($r,22);secondUninit=[BitConverter]::ToUInt16($r,24);modulePath=$actual;resolvedFileSha256=$hash;inputSha256=$expected[$Case];probeSha256=Get-IsolatedFileSha256 $probe;scope='one process, synchronous init/reinit/teardown; no DDE transactions requested; callback invocation not instrumented';originalGuestModified=$false}
 $record|ConvertTo-Json|Set-Content (Join-Path $PSScriptRoot "ddeml-init-$Case.result.json") -Encoding utf8
 if($record.resolvedFileSha256 -ne $expected[$Case]){throw 'Unexpected loaded DDEML identity'}
 if($record.invalidReserved -ne 0x4006 -or $record.init -ne 0 -or !$record.instance -or $record.reinit -ne 0 -or $record.reinitInstance -ne $record.instance -or $record.uninit -eq 0 -or $record.secondUninit -ne 0){throw 'Init/reinit/uninit observed behavior did not satisfy probe contract; preserve result'}
} finally {
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}} finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
