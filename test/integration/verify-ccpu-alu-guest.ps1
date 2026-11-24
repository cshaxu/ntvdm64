[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Oracle,
 [switch]$NegativeOracle)
# Independent COM exercises selected CPU40 arithmetic/logic instructions; no original guest is
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
 OwnedTool token-context "$env:SystemRoot/System32/whoami.exe" '/groups /fo csv' 15
 $label=[regex]::Match((Get-Content "$root/token-context.stdout.txt" -Raw),'S-1-16-\d+').Value
 if($label -ne 'S-1-16-8192'){throw "Expected desktop-capable medium token, got $label"}
 @{parent_pid=$PID;host_path=(Get-Process -Id $PID).Path;integrity_sid=$label;token_class='medium';script_sha256=(Get-FileHash $PSCommandPath).Hash;private_desktop=$true}|ConvertTo-Json|Set-Content "$root/runner-context.json"
 $dataRoot=Join-Path $root 'data'
 $null=New-Item -ItemType Directory $dataRoot
 OwnedTool oracle (Resolve-Path $Oracle).Path ('"'+$dataRoot+'"') 120
 $expectedFiles=@()
 foreach($stem in @('ADD','ADC0_','ADC1_','AND','CMP','DEC0_','DEC1_','INC0_','INC1_','NEG','OR','SBB0_','SBB1_','SUB','TEST','XOR')){
  foreach($width in @(8,16,32)){
   $name=$stem+$width+'.BIN';$expectedFiles+=$name
   $file=Get-Item -LiteralPath (Join-Path $dataRoot $name)
   $size=if($width -eq 8){917504}else{16744}
   if($file.Length -ne $size){throw "Oracle record size mismatch: $name"}
  }
 }
 if(@(Get-ChildItem $dataRoot -File).Count -ne 48){throw 'Unexpected oracle file set'}
 if($NegativeOracle){
  $file=[IO.File]::Open((Join-Path $dataRoot 'ADD8.BIN'),[IO.FileMode]::Open,[IO.FileAccess]::ReadWrite,[IO.FileShare]::None)
  try{$null=$file.Seek(8,[IO.SeekOrigin]::Begin);$old=$file.ReadByte();$null=$file.Seek(8,[IO.SeekOrigin]::Begin);$file.WriteByte($old -bxor 1)}finally{$file.Dispose()}
 }
 Get-ChildItem $dataRoot -File|Get-FileHash|Export-Csv -NoTypeInformation "$root/oracle-data-inputs.csv"
 $variant='Alu'
 OwnedTool probe-build $env:ComSpec ('/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 cpu-guest-probes"') 300
 $native=Join-Path $repo 'build/.ninja/fixtures-x86'
 Copy-Item "$native/guest-probes/cpu-integration/$variant.COM" "$root/EMSPROBE.COM"
 $pifBuilder=Join-Path $native 'ems_pif_builder.exe'
 Get-FileHash (Resolve-Path $Oracle).Path,$PSCommandPath,"$PSScriptRoot/../observation/ccpu_alu_probe.asm","$root/EMSPROBE.COM",$Observer,$pifBuilder|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $stage=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path;$scope=New-IsolatedPackageScope $stage
 $tests=Join-Path $stage tests;$null=New-Item -ItemType Directory $tests -Force
 # Reuse the existing test PIF producer: original no-PIF defaults deliberately
 # allocate zero EMS. This unchanged isolated test profile requests 1MiB EMS; CPU assertions do not depend on EMS. Installation
 # config and original guest binaries stay unchanged. The older memory-transition
 # runner requires SUBST and foreground compilation, so reuse its producer only.
 Copy-Item "$root/EMSPROBE.COM" "$tests/E30.COM"
 foreach($name in $expectedFiles){Copy-Item -LiteralPath (Join-Path $dataRoot $name) -Destination (Join-Path $tests $name)}
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
 foreach($arg in @("$stage/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/E30.PIF",'--observation-timeout-ms','120000')){$info.ArgumentList.Add($arg)}
 $observerProcess=[Diagnostics.Process]::Start($info);$null=$observerProcess.Handle
 @{pid=$observerProcess.Id;start=$observerProcess.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=130}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$observerProcess.WaitForExit(130000)){throw 'Owned CPU40 observer deadline'}
 $observerProcess.WaitForExit();$observerProcess.ExitCode|Set-Content "$root/observer.exit.txt"
 $report=Get-Content "$root/observer.txt" -Raw
 $console=Get-Content "$root/observer.txt.console.txt" -Raw
 if($observerProcess.ExitCode -or $report -notmatch 'result=exited'){throw 'CPU40 guest observation did not complete'}
 if($NegativeOracle){
  if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'CCPU ALU FAIL phase=01 form=01 index=0000 reason=V' -or $console -match 'CCPU ALU PASS phases=48 records=1086848 instructions=3260544'){throw 'Deliberately wrong ADD8 reference record was not rejected'}
 }elseif($report -notmatch 'exit=0x00000000' -or $console -notmatch 'CCPU ALU PASS phases=48 records=1086848 instructions=3260544'){throw 'CPU40 ALU guest workload did not complete'}
 foreach($row in Import-Csv "$root/oracle-data-inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Oracle data changed during guest execution'}}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Probe inputs changed'}}
 if($NegativeOracle){'PASS deliberately wrong ADD8 reference record rejected; guest exit=1; no success marker'|Set-Content "$root/result.txt"}
 else{'PASS independent COM -> original CPU40 decoder/arithmetic/logic owners; normal launcher exit; no original guest modification'|Set-Content "$root/result.txt"}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}
 if($observerProcess){$null=$observerProcess.Handle;if(!$observerProcess.HasExited){$observerProcess.Kill();$null=$observerProcess.WaitForExit(5000)};$observerProcess.Dispose()}
}
