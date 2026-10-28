[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot,
 [string]$CompilerDriver=(Join-Path $PSScriptRoot '../../build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd'),
 [string]$Nasm='nasm.exe',
 [switch]$NegativeOracle,
 [ValidateSet('SignFlags','Control','BitMemory','BiosDispatch','DummyInt','SystemDevices','DiskReset')][string]$Workload='SignFlags')
# Independent COM exercises selected CPU40 sign-extension and flag instructions; no original guest is
# modified/rebuilt, no substituted memory provider and no mapped drive.
$ErrorActionPreference='Stop'
if($Workload -eq 'BitMemory' -and $NegativeOracle){throw 'BitMemory uses its direct hardware expectation; NegativeOracle is not supported'}
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
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
 $assembler=(Get-Command $Nasm -ErrorAction Stop).Source
 $probe=Join-Path $PSScriptRoot (@{Control='ccpu_control_probe.asm';SignFlags='ccpu_sign_flag_probe.asm';BitMemory='ccpu_bit_memory_probe.asm';BiosDispatch='bios_dispatch_probe.asm';DummyInt='bios_dispatch_probe.asm';SystemDevices='system_capability.asm';DiskReset='disks_capability.asm'}[$Workload])
 $line='-f bin "'+$probe+'" -o "'+$root+'/EMSPROBE.COM"'
 if($NegativeOracle){$line+=' -DCPU_PROBE_NEGATIVE_ORACLE=1'}
 if($Workload -eq 'DummyInt'){$line+=' -DCPU_PROBE_DUMMY_INT_ONLY=1'}
 $p=Start-Process $assembler -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/assemble.stdout.txt" -RedirectStandardError "$root/assemble.stderr.txt";$null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$assembler;command=$line;timeoutSeconds=30}|ConvertTo-Json|Set-Content "$root/assemble.process.json"
 if(!$p.WaitForExit(30000)){taskkill.exe /PID $p.Id /T /F;throw 'Owned assembler deadline'}
 $p.WaitForExit();$code=$p.ExitCode;$p.Dispose();$code|Set-Content "$root/assemble.exit.txt";if($code){throw 'Independent COM assembly failed'}
 Get-FileHash $PSCommandPath,$probe,"$root/EMSPROBE.COM",$Observer,$LogonHelper,$assembler|Export-Csv -NoTypeInformation "$root/inputs.csv"
 $stage=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path;$scope=New-IsolatedPackageScope $stage
 $tests=Join-Path $stage tests;$null=New-Item -ItemType Directory $tests -Force
 # Reuse the existing test PIF producer: original no-PIF defaults deliberately
 # allocate zero EMS. This unchanged isolated test profile requests 1MiB EMS; CPU assertions do not depend on EMS. Installation
 # config and original guest binaries stay unchanged. The older memory-transition
 # runner requires SUBST and foreground compilation, so reuse its producer only.
 Copy-Item "$root/EMSPROBE.COM" "$tests/E30.COM"
 $line='/d /s /c ""'+(Resolve-Path $CompilerDriver).Path+'" cl.exe /nologo /MT /W4 /I"'+$repo+'/src/opennt-abi/source/public/internal/windows/inc" /Fo"'+$root+'/pif.obj" /Fe"'+$root+'/pif-builder.exe" "'+$PSScriptRoot+'/ems_pif_builder.c""'
 OwnedTool pif-build $env:ComSpec $line 120
 OwnedTool pif-profile "$root/pif-builder.exe" ('"'+$tests+'" --close-on-exit') 30
 Get-FileHash "$PSScriptRoot/ems_pif_builder.c",$CompilerDriver,"$tests/E30.COM","$tests/E30.PIF","$tests/E30.NT","$tests/E30AUTO.NT"|Export-Csv -NoTypeInformation "$root/profile-inputs.csv"
 Get-ChildItem "$stage/system32" -File|Where-Object Name -in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','COMMAND.COM','DOSX.EXE')|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 Add-Type -Path (Resolve-Path $LogonHelper).Path
 $logonIdentity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($logonIdentity.Token)}
 finally{$logonIdentity.Dispose()}
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
 $info.EnvironmentVariables['TEMP']=$stage;$info.EnvironmentVariables['TMP']=$stage
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 foreach($arg in @("$stage/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/E30.PIF",'--observation-timeout-ms','45000')){$info.ArgumentList.Add($arg)}
 $observerProcess=[Diagnostics.Process]::Start($info);$null=$observerProcess.Handle
 @{pid=$observerProcess.Id;start=$observerProcess.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=55}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$observerProcess.WaitForExit(55000)){throw 'Owned CPU40 observer deadline'}
 $observerProcess.WaitForExit();$observerProcess.ExitCode|Set-Content "$root/observer.exit.txt"
 $report=[IO.File]::ReadAllText("$root/observer.txt")
 $console=[IO.File]::ReadAllText("$root/observer.txt.console.txt")
 if([string]::IsNullOrWhiteSpace($report) -or [string]::IsNullOrWhiteSpace($console)){throw 'CPU40 guest observation output is empty'}
 if($observerProcess.ExitCode -or $report -notmatch 'result=exited'){throw 'CPU40 guest observation did not complete'}
 if($Workload -eq 'DiskReset'){
  if($NegativeOracle){
   if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'S28_INT13_RESET_FAIL' -or $console -match 'S28_DISKS_OK'){throw 'Wrong disk reset carry oracle was not rejected'}
  }elseif($report -notmatch 'exit=0x00000000' -or $console -notmatch 'S28_INT13_RESET_OK S28_DISKS_OK'){
   throw 'Original disk reset guest witness did not complete'
  }
 }elseif($Workload -eq 'SystemDevices'){
  if($NegativeOracle){
   if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'S24_CMOS_FAIL' -or $console -match 'S24_SYSTEM_OK'){throw 'Wrong CMOS oracle was not rejected after the timer check'}
  }elseif($report -notmatch 'exit=0x00000000' -or @('S24_TIMER_OK','S24_CMOS_OK','S24_RTC_PERIODIC_OK','S24_PIC_OK','S24_DMA_PORT_OK','S24_ROM_READ_OK','S24_SYSTEM_OK').Where({$console -notmatch $_}).Count){
   throw 'Original system-device guest witness did not complete'
  }
 }elseif($Workload -eq 'DummyInt'){
  if($NegativeOracle){
   if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'DUMMY INT FAIL phase=D' -or $console -match 'DUMMY INT PASS'){throw 'Wrong dummy-interrupt oracle was not rejected'}
  }elseif($report -notmatch 'exit=0x00000000' -or $console -notmatch 'DUMMY INT PASS cases=16'){
   throw 'Original dummy-interrupt guest witness did not complete'
  }
 }elseif($Workload -eq 'BiosDispatch'){
  if($NegativeOracle){
   if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'BIOS DISPATCH FAIL phase=C' -or $console -match 'BIOS DISPATCH PASS'){throw 'Deliberately wrong BIOS oracle was not rejected'}
  }elseif($report -notmatch 'exit=0x00000000' -or $console -notmatch 'BIOS DISPATCH PASS cases=836' -or $console -notmatch 'CONTROL768 ILLEGAL64 EMSBAD4'){
   throw 'Original BIOS dispatch/register/error contract did not complete'
  }
 }elseif($Workload -eq 'BitMemory'){
  if($report -notmatch 'exit=0x00000000' -or $console -notmatch 'BITMEM POSITIVE PASS' -or $console -notmatch 'BITMEM NEGATIVE PASS'){throw 'CPU40 word-memory bit-index expectation differs; inspect captured positions/carry'}
 }elseif($Workload -eq 'Control'){
  if($NegativeOracle){
   if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'CCPU CONTROL FAIL phase=A' -or $console -match 'CCPU CONTROL PASS'){throw 'Deliberately wrong control oracle was not rejected'}
  }elseif($report -notmatch 'exit=0x00000000' -or $console -notmatch 'CCPU CONTROL PASS' -or $console -notmatch 'NOP32 CLI32 CLTS4 WAIT4 IDT16' -or $console -notmatch 'DIVFAULT6 JCXZ4 STRFAULT1 RSRVD5'){
   throw 'CPU40 control/exception guest workload did not complete'
  }
 }elseif($NegativeOracle){
  if($report -notmatch 'exit=0x00000001' -or $console -notmatch 'CCPU SIGN FLAGS FAIL phase=A' -or $console -match 'CCPU SIGN FLAGS REVERSE COPY PASS'){throw 'Deliberately wrong CWDE oracle was not rejected'}
 }elseif($report -notmatch 'exit=0x00000000' -or $console -notmatch 'CCPU SIGN PASS cases=524288' -or $console -notmatch 'CCPU CDQ BOUNDARIES PASS cases=8' -or $console -notmatch 'CCPU FLAGS PASS cases=114688' -or $console -notmatch 'CCPU SIGN FLAGS REVERSE COPY PASS'){throw 'CPU40 guest instruction workload did not complete'}
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw 'Probe inputs changed'}}
 if($NegativeOracle){"PASS deliberately wrong $Workload expectation rejected; guest exit=1; no success marker"|Set-Content "$root/result.txt"}
 else{"PASS independent COM -> original CPU40 $Workload owners; normal launcher exit; no original guest modification"|Set-Content "$root/result.txt"}
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}
 if($observerProcess){$null=$observerProcess.Handle;if(!$observerProcess.HasExited){$observerProcess.Kill();$null=$observerProcess.WaitForExit(5000)};$observerProcess.Dispose()}
}
