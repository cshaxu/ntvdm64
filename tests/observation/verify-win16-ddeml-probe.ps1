[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('init','register')][string]$Case='init',
 [Parameter(Mandatory)][string]$DdemlImage,
 [Parameter(Mandatory)][string]$ExpectedDdemlSha256,
 [switch]$SelfCallbackControl,
 [switch]$WindowWordControl)
# Current-package runner for existing independently authored DDEML probes.
# Original DLL remains unchanged and app-local. Owned background invocation only.
$ErrorActionPreference='Stop'
if(($SelfCallbackControl -or $WindowWordControl) -and $Case -ne 'register'){throw 'Callback/window controls require register'}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
function Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $repo -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/$name.exit";if($code){throw "$name failed"}
}
$runtime=$null;$scope=$null;$process=$null
try {
 Owned token whoami.exe '/groups /fo csv /nh' 15
 $label=[regex]::Match((Get-Content "$root/token.out" -Raw),'S-1-16-\d+').Value
 if($label -ne 'S-1-16-8192'){throw 'Require desktop-capable medium token'}
 @{pid=$PID;host=(Get-Process -Id $PID).Path;integrity=$label;script_sha256=(Get-FileHash $PSCommandPath).Hash}|ConvertTo-Json|Set-Content "$root/runner-context.json"

 $source="$PSScriptRoot/win16_ddeml_${Case}_probe.asm"
 if($WindowWordControl){$source="$PSScriptRoot/win16_ddeml_window_word_probe.asm"}
 $probe=if($Case -eq 'init'){'DDINIT'}else{'DDREG'}
 $assembler=(Get-Command nasm.exe).Source
 $defines=if($SelfCallbackControl){'-DSELF_CALLBACK_CONTROL '}else{''}
 Owned assemble $assembler ($defines+'-f bin "'+$source+'" -o "'+$root+'/'+$probe+'.EXE"') 30
 $dll=(Resolve-Path $DdemlImage).Path
 if((Get-FileHash $dll).Hash -ne $ExpectedDdemlSha256){throw 'Original DDEML input differs'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 Save-IsolatedPackageManifest $runtime "$root/manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/$probe.EXE" "$tests/$probe.EXE"
 Copy-Item $dll "$tests/DDEML.DLL"
 Get-FileHash $source,$PSCommandPath,$Observer,$LogonHelper,$assembler,"$root/$probe.EXE",$dll|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Get-ChildItem "$runtime/system32" -File|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 Add-Type -Path (Resolve-Path $LogonHelper).Path
 $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($identity.Token)}finally{$identity.Dispose()}
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
 $info.EnvironmentVariables['TEMP']=$runtime;$info.EnvironmentVariables['TMP']=$runtime
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables['MVDM_WOW_DISPATCH_TRACE_PATH']="$root/dispatch.txt"
 $info.EnvironmentVariables['MVDM_WOW_CALLBACK_FRAME_TRACE_PATH']="$root/callbacks.txt"
 $info.EnvironmentVariables['MVDM_WOW_CALLBACK_TRACE_PATH']="$root/wndproc.txt"
 $info.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root/windows.txt"
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/$probe.EXE",'--observation-timeout-ms','45000')){$info.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=55}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$process.WaitForExit(55000)){throw 'Owned guest observer deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer.exit"
 foreach($name in @('DPRE.BIN','DPOST.BIN','DSTAGE.BIN','DINIT.BIN','DREG.BIN')){if(Test-Path "$tests/$name"){Copy-Item "$tests/$name" "$root/$name"}}
 if($WindowWordControl -and (Test-Path "$root/DPRE.BIN")){
  $pre=[IO.File]::ReadAllBytes("$root/DPRE.BIN")
  if($pre.Length -ne 300){throw 'Invalid pre-registration receipt'}
  @{window=[BitConverter]::ToUInt16($pre,32);windowWord=[BitConverter]::ToUInt16($pre,34);instance=[BitConverter]::ToUInt32($pre,10)}|ConvertTo-Json|Set-Content "$root/window-word.json"
 }
 $report=Get-Content "$root/observer.txt" -Raw
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher did not complete normally'}
 $file=if($Case -eq 'init'){'DINIT.BIN'}else{'DREG.BIN'}
 $r=[IO.File]::ReadAllBytes("$root/$file")
 $pathOffset=if($Case -eq 'init'){26}else{40}
 $signature=if($Case -eq 'init'){'DDI1'}else{'DDR1'}
 if($r.Length -ne ($pathOffset+260) -or [Text.Encoding]::ASCII.GetString($r,0,4) -ne $signature){throw 'Invalid guest receipt'}
 $nul=[array]::IndexOf($r,[byte]0,$pathOffset);if($nul -lt $pathOffset){throw 'Module path not terminated'}
 $actual=[Text.Encoding]::ASCII.GetString($r,$pathOffset,$nul-$pathOffset)
 $hash=(Get-FileHash $actual).Hash
 if($hash -ne $ExpectedDdemlSha256){throw 'Loaded DDEML identity differs'}
 if($Case -eq 'init'){
  $record=[ordered]@{modulePath=$actual;sha256=$hash;invalidReserved=[BitConverter]::ToUInt16($r,8);init=[BitConverter]::ToUInt16($r,10);instance=[BitConverter]::ToUInt32($r,12);reinit=[BitConverter]::ToUInt16($r,16);reinitInstance=[BitConverter]::ToUInt32($r,18);uninit=[BitConverter]::ToUInt16($r,22);secondUninit=[BitConverter]::ToUInt16($r,24)}
  $record|ConvertTo-Json|Set-Content "$root/result.json"
  if($record.invalidReserved -ne 0x4006 -or $record.init -ne 0 -or !$record.instance -or $record.reinit -ne 0 -or $record.reinitInstance -ne $record.instance -or !$record.uninit -or $record.secondUninit -ne 0){throw 'Init/reinit/uninit differs'}
 } else {
  $record=[ordered]@{modulePath=$actual;sha256=$hash;init=[BitConverter]::ToUInt16($r,8);instance=[BitConverter]::ToUInt32($r,10);service=[BitConverter]::ToUInt32($r,14);register=[BitConverter]::ToUInt32($r,18);unregister=[BitConverter]::ToUInt32($r,22);free=[BitConverter]::ToUInt16($r,26);uninit=[BitConverter]::ToUInt16($r,28);callbackCount=[BitConverter]::ToUInt16($r,30);registerCount=[BitConverter]::ToUInt16($r,32);unregisterCount=[BitConverter]::ToUInt16($r,34);firstType=[BitConverter]::ToUInt16($r,36);lastType=[BitConverter]::ToUInt16($r,38)}
  $record|ConvertTo-Json|Set-Content "$root/result.json"
  $direct=[int][bool]$SelfCallbackControl
  if($record.callbackCount -ne (2+$direct) -or $record.registerCount -ne (1+$direct) -or $record.unregisterCount -ne (2+$direct) -or $record.firstType -ne 0x80a2 -or $record.lastType -ne 0x80d2){throw 'Original registration callbacks differ'}
  if($record.init -ne 0 -or !$record.instance -or !$record.service -or !$record.register -or !$record.unregister -or !$record.free -or !$record.uninit){throw 'Registration lifecycle differs'}
 }
 '0'|Set-Content "$root/exit.txt"
} catch { $_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw }
finally {
 if($process){if(!$process.HasExited){taskkill.exe /PID $process.Id /T /F | Out-Null};$process.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally {
  if($tests){foreach($name in @('DPRE.BIN','DPOST.BIN','DSTAGE.BIN','DINIT.BIN','DREG.BIN')){if(Test-Path "$tests/$name"){Copy-Item "$tests/$name" "$root/$name" -Force}}}
  if($runtime){Remove-PhysicalRuntimeStage $runtime}
 }
}
