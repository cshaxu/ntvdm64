param([ValidateSet('w31','opennt-debug')][string]$Case)
$ErrorActionPreference='Stop'
if ($PSVersionTable.PSVersion.Major -lt 7) {throw 'PowerShell 7 is required'}
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$probe=Join-Path $PSScriptRoot 'ddeml-register-probe.tmp.exe'
$sourceMap=@{'w31'='O:\winnt\tests\w31p\SYSTEM\DDEML.DLL';'opennt-debug'="$repo/src/mvdm/wow16/ddeml/debug/DDEML.DLL"}
$expected=@{'w31'='6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B';'opennt-debug'='4A8FAB7786A388882687E1A79D810534C94D0F9E05A313D22173C0C68C83A443'}
$runtime=$null;$scope=$null;$proc=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_WOW_CALLBACK_CPU_TRACE_PATH','MVDM_WOW_CALLBACK_CPU_SELECTOR')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 if((Get-IsolatedFileSha256 $sourceMap[$Case]) -ne $expected[$Case]){throw 'Input DDEML hash changed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path "$repo/build/package-uuid/package-c").Path -FreshPackageIdentity
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Path $tests -Force
 Copy-Item -LiteralPath $probe -Destination (Join-Path $tests 'DDREG.EXE')
 Copy-Item -LiteralPath $sourceMap[$Case] -Destination (Join-Path $tests 'DDEML.DLL')
 if((Get-IsolatedFileSha256 (Join-Path $tests 'DDEML.DLL')) -ne $expected[$Case]){throw 'Copied DDEML differs'}
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:MVDM_WOW_CALLBACK_CPU_TRACE_PATH=Join-Path $runtime 'callback-trace.txt';$env:MVDM_WOW_CALLBACK_CPU_SELECTOR='0270'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path "$repo/build/.ninja/observations-x64/console-startup-observer.exe").Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log=Join-Path $PSScriptRoot "ddeml-register-selector0270-$Case.observer.txt"
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/DDREG.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 if(!$proc.WaitForExit(45000)){throw 'Observer did not finish within test supervisory bound'}
 $stageFile=Join-Path $tests 'DSTAGE.BIN';if(Test-Path -LiteralPath $stageFile){Copy-Item -LiteralPath $stageFile -Destination (Join-Path $PSScriptRoot "ddeml-register-selector0270-$Case.stage.bin") -Force}
 $preFile=Join-Path $tests 'DPRE.BIN';if(Test-Path -LiteralPath $preFile){Copy-Item -LiteralPath $preFile -Destination (Join-Path $PSScriptRoot "ddeml-register-selector0270-$Case.pre.bin") -Force}
 if(Test-Path -LiteralPath $preFile){
  $pre=[IO.File]::ReadAllBytes($preFile);if($pre.Length -ne 300 -or [Text.Encoding]::ASCII.GetString($pre,0,4) -ne 'DDR1'){throw 'invalid pre-call receipt'}
  $end=[array]::IndexOf($pre,[byte]0,40);if($end -lt 40){throw 'invalid pre-call path'}
  $prePath=[Text.Encoding]::ASCII.GetString($pre,40,$end-40)
  $preRecord=[ordered]@{modulePath=$prePath;resolvedFileSha256=if(Test-Path -LiteralPath $prePath){Get-IsolatedFileSha256 $prePath}else{$null};inputSha256=$expected[$Case];init=[BitConverter]::ToUInt16($pre,8);instance=[BitConverter]::ToUInt32($pre,10);service=[BitConverter]::ToUInt32($pre,14);callbackOffset=[BitConverter]::ToUInt16($pre,36);callbackSelector=[BitConverter]::ToUInt16($pre,38);probeSha256=Get-IsolatedFileSha256 $probe}
  $preRecord|ConvertTo-Json|Set-Content (Join-Path $PSScriptRoot "ddeml-register-selector0270-$Case.pre.json") -Encoding utf8
 }
 $traceFile=Join-Path $runtime 'callback-trace.txt';if(Test-Path -LiteralPath $traceFile){Copy-Item -LiteralPath $traceFile -Destination (Join-Path $PSScriptRoot "ddeml-register-selector0270-$Case.callback-trace.txt") -Force}
 $proc.Refresh();if($proc.ExitCode){throw "Observer process failed: $($proc.ExitCode)"}
 if((Get-Content -LiteralPath $log -Raw) -notmatch '(?m)^result=exited\s*$'){throw 'Observer did not report process exit'}
 $resultFile=Join-Path $tests 'DREG.BIN'
 if(!(Test-Path -LiteralPath $resultFile)){throw 'Win16 probe did not produce result; inspect observer'}
 $r=[IO.File]::ReadAllBytes($resultFile)
 if($r.Length -ne 300 -or [Text.Encoding]::ASCII.GetString($r,0,4) -ne 'DDR1'){throw "Invalid result bytes: $($r.Length)"}
 $nul=[array]::IndexOf($r,[byte]0,40);if($nul -lt 40){throw 'Module path not terminated'}
 $actual=[Text.Encoding]::ASCII.GetString($r,40,$nul-40)
 $hash=if(Test-Path -LiteralPath $actual){Get-IsolatedFileSha256 $actual}else{$null}
 $record=[ordered]@{case=$Case;moduleHandle=[BitConverter]::ToUInt16($r,4);pathLength=[BitConverter]::ToUInt16($r,6);init=[BitConverter]::ToUInt16($r,8);instance=[BitConverter]::ToUInt32($r,10);service=[BitConverter]::ToUInt32($r,14);register=[BitConverter]::ToUInt32($r,18);unregister=[BitConverter]::ToUInt32($r,22);free=[BitConverter]::ToUInt16($r,26);uninit=[BitConverter]::ToUInt16($r,28);callbackCount=[BitConverter]::ToUInt16($r,30);registerCount=[BitConverter]::ToUInt16($r,32);unregisterCount=[BitConverter]::ToUInt16($r,34);firstType=[BitConverter]::ToUInt16($r,36);lastType=[BitConverter]::ToUInt16($r,38);modulePath=$actual;resolvedFileSha256=$hash;inputSha256=$expected[$Case];probeSha256=Get-IsolatedFileSha256 $probe;scope='one server instance, service registration and unregister; no DDE transaction';originalGuestModified=$false}
 $record|ConvertTo-Json|Set-Content (Join-Path $PSScriptRoot "ddeml-register-selector0270-$Case.result.json") -Encoding utf8
 if($record.resolvedFileSha256 -ne $expected[$Case]){throw 'Unexpected loaded DDEML identity'}
 if($record.init -ne 0 -or !$record.instance -or !$record.service -or !$record.register -or !$record.unregister -or !$record.free -or !$record.uninit){throw 'Registration API failed; receipt preserved'}

} finally {
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}} finally {if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
