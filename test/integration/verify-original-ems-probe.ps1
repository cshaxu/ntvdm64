[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('Mapping','CallReturn')][string]$Case='Mapping')
# Independent COM exercises original INT67 services; no original guest is
# modified/rebuilt, no substituted memory provider and no mapped drive.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null;$observerProcess=$null
function OwnedTool($name,$image,$line,$seconds){
 $p=Start-Process $image -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$image;command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 try{
  if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Owned $name deadline"}
  $p.WaitForExit();$p.ExitCode|Set-Content "$root/$name.exit.txt";if($p.ExitCode){throw "$name failed"}
 }finally{$p.Dispose()}
}
try{
 $probeSource=if($Case -eq 'CallReturn'){"$PSScriptRoot/../observation/ems_call_return_probe.asm"}else{"$PSScriptRoot/../observation/ems_mapping_probe.asm"}
 $expectedMarker=if($Case -eq 'CallReturn'){'T430_EMS_CALL_RETURN_STACK_MAP_OK'}else{'EMS MAP SWITCH ALIAS UNMAP MOVE EXCHANGE WINDOW FREE PASS'}
 $variant=if($Case -eq 'CallReturn'){'CallReturn'}else{'Mapping'}
 OwnedTool probe-build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 cpu-guest-probes"') 300
 $native=Join-Path $repo 'build/.ninja/fixtures-x86'
 Copy-Item "$native/guest-probes/cpu-integration/$variant.COM" "$root/EMSPROBE.COM"
 $pifBuilder=Join-Path $native 'ems_pif_builder.exe'
 Get-FileHash $PSCommandPath,$probeSource,"$root/EMSPROBE.COM",$Observer,$pifBuilder|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $stage=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path;$scope=New-IsolatedPackageScope $stage
 $tests=Join-Path $stage tests;$null=New-Item -ItemType Directory $tests -Force
 # Reuse the existing test PIF producer: original no-PIF defaults deliberately
 # allocate zero EMS. Only this isolated profile requests 1MiB; installation
 # config and original guest binaries stay unchanged. The older memory-transition
 # runner requires SUBST and foreground compilation, so reuse its producer only.
 Copy-Item "$root/EMSPROBE.COM" "$tests/E30.COM"
 OwnedTool pif-profile $pifBuilder ('"'+$tests+'" --close-on-exit') 30
 Get-FileHash "$PSScriptRoot/../observation/ems_pif_builder.c",$pifBuilder,"$tests/E30.COM","$tests/E30.PIF","$tests/E30.NT","$tests/E30AUTO.NT"|Export-Csv -NoTypeInformation "$root/profile-inputs.csv"
 Get-ChildItem "$stage/system32" -File|Where-Object Name -in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','COMMAND.COM')|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 OwnedTool environment-build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x64 logon-environment"') 300
 . "$PSScriptRoot/../observation/logon-environment.ps1"
 $environmentExe=Join-Path $repo 'build/.ninja/fixtures-x64/logon-environment.exe'
 Get-FileHash $environmentExe,"$PSScriptRoot/../observation/logon-environment.ps1"|Export-Csv -NoTypeInformation -Append "$root/inputs.csv"
 $entries=Read-TestLogonEnvironment $environmentExe "$root/environment-process.json"
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
 $info.EnvironmentVariables['TEMP']=$stage;$info.EnvironmentVariables['TMP']=$stage
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 foreach($arg in @("$stage/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/E30.PIF",'--observation-timeout-ms','45000')){$info.ArgumentList.Add($arg)}
 $observerProcess=[Diagnostics.Process]::Start($info);$null=$observerProcess.Handle
 @{pid=$observerProcess.Id;start=$observerProcess.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=55}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$observerProcess.WaitForExit(55000)){throw 'Owned EMS observer deadline'}
 $observerProcess.WaitForExit();$observerProcess.ExitCode|Set-Content "$root/observer.exit.txt"
 $report=Get-Content "$root/observer.txt" -Raw
 $console=Get-Content "$root/observer.txt.console.txt" -Raw
 if($observerProcess.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000' -or $console -notmatch [regex]::Escape($expectedMarker)){throw 'Original EMS workload did not complete; absent EMS is not a pass'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Probe inputs changed'}}
 "PASS independent COM -> original INT67 $Case; marker=$expectedMarker; normal launcher exit; no original guest or profile edit"|Set-Content "$root/result.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}
 if($observerProcess){$null=$observerProcess.Handle;if(!$observerProcess.HasExited){$observerProcess.Kill();$null=$observerProcess.WaitForExit(5000)};$observerProcess.Dispose()}
}
