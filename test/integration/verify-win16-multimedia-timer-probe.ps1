[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot,
 [switch]$LoadOnly,
 [switch]$AppLocalMmsystem,
 [switch]$NoTimer,
 [string]$DebugObserver)
# Independently authored fixed Win16 DLL callback; original MMSYSTEM unchanged.
# No Win16 Style modes or original guest build. Owned background invocation only.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
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
 $source="$PSScriptRoot/../observation/win16_multimedia_timer_probe.asm"
 $library="$PSScriptRoot/../observation/win16_multimedia_timer_library.asm"
 $probe='MMTPROB';$assembler=(Get-Command nasm.exe).Source
 Owned assemble-library $assembler ('-f bin "'+$library+'" -o "'+$root+'/MMTPROBE.DLL"') 30
 $defines=if($LoadOnly){'-DLOAD_ONLY '}elseif($NoTimer){'-DNO_TIMER '}else{''}
 Owned assemble $assembler ($defines+'-f bin "'+$source+'" -o "'+$root+'/'+$probe+'.EXE"') 30
 Get-FileHash $library,"$root/MMTPROBE.DLL"|Export-Csv -NoTypeInformation "$root/library-inputs.csv"
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 Save-IsolatedPackageManifest $runtime "$root/manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/$probe.EXE" "$tests/$probe.EXE"
 Copy-Item "$root/MMTPROBE.DLL" "$tests/MMTPROBE.DLL"
 if($AppLocalMmsystem){
  Copy-Item "$runtime/system/MMSYSTEM.DLL" "$tests/MMSYSTEM.DLL"
  if((Get-FileHash "$tests/MMSYSTEM.DLL").Hash -ne (Get-FileHash "$runtime/system/MMSYSTEM.DLL").Hash){throw 'App-local immutable copy differs'}
 }

 Get-FileHash $source,$PSCommandPath,$Observer,$LogonHelper,$assembler,"$root/$probe.EXE"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 Get-ChildItem "$runtime/system32" -File|Get-FileHash|Export-Csv -NoTypeInformation "$root/runtime-inputs.csv"
 Add-Type -Path (Resolve-Path $LogonHelper).Path
 $identity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($identity.Token)}finally{$identity.Dispose()}
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
 $info.EnvironmentVariables.Clear()
 foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
 $info.EnvironmentVariables['TEMP']=$runtime;$info.EnvironmentVariables['TMP']=$runtime
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables['MVDM_WOW_CALLBACK_CPU_TRACE_PATH']="$root/cpu.txt"
 $info.EnvironmentVariables['MVDM_WOW_DISPATCH_TRACE_PATH']="$root/dispatch.txt"
 $info.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root/windows.txt"
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/$probe.EXE",'--observation-timeout-ms','45000')){$info.ArgumentList.Add($arg)}
 if($DebugObserver){
  # Observe the existing private-desktop runner's owned process tree. This is
  # diagnostic only; preserving exceptions is not a capability acceptance.
  $line=(@($info.ArgumentList)|ForEach-Object {'"'+$_+'"'}) -join ' '
  $observed=$info.FileName
  $info.FileName=(Resolve-Path $DebugObserver).Path
  $info.ArgumentList.Clear()
  foreach($arg in @('--exe',$observed,$line,"$root/exceptions.txt")){$info.ArgumentList.Add($arg)}
  Get-FileHash $DebugObserver,"$PSScriptRoot/../observation/wow_exception_observer.c"|Export-Csv -NoTypeInformation "$root/debugger-inputs.csv"
 }
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=$(if($DebugObserver){135}else{55})}|ConvertTo-Json|Set-Content "$root/observer.process.json"
 if(!$process.WaitForExit($(if($DebugObserver){135000}else{55000}))){throw 'Owned guest observer deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer.exit"
 if(Test-Path "$tests/$probe.BIN"){Copy-Item "$tests/$probe.BIN" "$root/interim.bin"}
 $report=Get-Content "$root/observer.txt" -Raw
 if($DebugObserver){throw 'Diagnostic debugger run; inspect exceptions, not acceptance'}
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher did not complete normally'}
 Copy-Item "$tests/$probe.BIN" "$root/guest-report.bin"
 $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 if($bytes.Length -ne 276 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'MMT1'){throw 'Invalid multimedia receipt'}
 $record=[ordered]@{};$names=@('timer','before','stop','settled','final','bad')
 for($i=0;$i -lt $names.Count;$i++){$record[$names[$i]]=[BitConverter]::ToUInt16($bytes,4+2*$i)}
 if($true){
  $end=[array]::IndexOf($bytes,[byte]0,16)
  $record.modulePath=[Text.Encoding]::ASCII.GetString($bytes,16,$end-16)
  if($record.modulePath){$record.moduleHash=(Get-FileHash $record.modulePath).Hash}
 }
 $record|ConvertTo-Json|Set-Content "$root/guest-report.json"
 if($LoadOnly){if($record.timer -lt 32 -or $record.before -lt 32){throw 'Original MMSYSTEM or test DLL failed to load'}}
 elseif($NoTimer){if($record.timer -or $record.before -or $record.stop -or $record.final -or $record.bad){throw 'No-event control differs'}}
 elseif(!$record.timer -or $record.before -lt 17 -or $record.stop -ne 0 -or $record.settled -ne $record.final -or $record.bad){throw 'Multimedia callback/cancel contract failed'}
 if(!$LoadOnly -and !$NoTimer){'PASS original Win16 multimedia periodic callback, user data and cancellation'|Set-Content "$root/result.txt"}
 '0'|Set-Content "$root/exit.txt"
} catch { $_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw }
finally {
 if($process){if(!$process.HasExited){taskkill.exe /PID $process.Id /T /F | Out-Null};$process.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally {
  if($tests -and $probe -and (Test-Path "$tests/$probe.BIN")){Copy-Item "$tests/$probe.BIN" "$root/interim.bin" -Force}
  if($runtime){Remove-PhysicalRuntimeStage $runtime}
 }
}
