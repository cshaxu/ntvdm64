param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$LogonHelper,
 [Parameter(Mandatory)][string]$BuildRoot,
 [ValidateSet('Off','Win16','All')][string]$WindowStyle='Off'
)
# Run as an owned hidden background test. Reuse the complete Userenv helper
# produced by the actual-message group; never modify/rebuild an original guest.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null;$process=$null;$peer=$null
try{
 & nasm.exe -f bin -o "$root/MODAL.EXE" -l "$root/modal.lst" "$PSScriptRoot/win16_native_modal_probe.asm" *> "$root/assemble.txt"
 if($LASTEXITCODE){throw 'Independent modal assembly failed'}
 Push-Location $root
 try{
  & $CompilerDriver cl.exe /nologo /MT /O2 /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root/native.obj" "/Fe$root/native.exe" "$PSScriptRoot/wow_native_modal_probe.c" /link user32.lib *> "$root/build.txt"
  if($LASTEXITCODE){throw 'Native modal probe build failed'}
 }finally{Pop-Location}
 Get-FileHash $PSCommandPath,"$PSScriptRoot/win16_native_modal_probe.asm","$PSScriptRoot/wow_native_modal_probe.c",$Observer,$LogonHelper,"$root/MODAL.EXE","$root/native.exe"|Export-Csv -NoTypeInformation "$root/probe-inputs.csv"
 $stage=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $stage
 $tests=Join-Path $stage tests;$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/MODAL.EXE" $tests
 Get-ChildItem "$stage/system32" -File|Where-Object Name -in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','ntw16s32.dll','ntw16s64.dll','W16Style.exe')|Get-FileHash|Export-Csv -NoTypeInformation "$root/images.csv"
 Add-Type -Path (Resolve-Path $LogonHelper).Path
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables.Clear()
 $logonIdentity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowMessageLogonEnvironment]::Read($logonIdentity.Token)}
 finally{$logonIdentity.Dispose()}
 foreach($entry in $entries){
  $split=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$split)]=$entry.Substring($split+1)
 }
 $info.EnvironmentVariables['TEMP']=$stage;$info.EnvironmentVariables['TMP']=$stage
 $info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $info.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root/lifetime.txt"
 $info.EnvironmentVariables.Keys|ForEach-Object {[pscustomobject]@{name=$_;characters=$_.Length+$info.EnvironmentVariables[$_].Length+2}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 $arguments=@("$stage/system32/run16.exe",$tests,"$root/observer.txt",'--wait')
 if($WindowStyle -eq 'Win16'){$arguments+='--win16-style'}elseif($WindowStyle -eq 'All'){$arguments+='--win16-style-all'}
 $arguments+=@("$tests/MODAL.EXE",'--observation-timeout-ms','60000')
 foreach($argument in $arguments){$info.ArgumentList.Add($argument)}
 $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=$arguments;timeoutSeconds=70}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 $deadline=[DateTime]::UtcNow.AddSeconds(12);$worker=$null
 do{
  $worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'"|Where-Object {$_.ExecutablePath -eq "$stage\system32\ntvdm.exe"}|Select-Object -First 1
  if($worker -or $process.HasExited){break};Start-Sleep -Milliseconds 50
 }while([DateTime]::UtcNow -lt $deadline)
 if(!$worker){throw 'Owned worker not observed'}
 $worker|Select-Object ProcessId,CreationDate,ExecutablePath|ConvertTo-Json|Set-Content "$root/worker.json"
 $line=([string]$worker.ProcessId)+' "NTVDMConsoleTest-'+$process.Id+'"'
 $peer=Start-Process "$root/native.exe" -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/native.stdout.txt" -RedirectStandardError "$root/native.stderr.txt"
 $null=$peer.Handle
 @{pid=$peer.Id;start=$peer.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=65}|ConvertTo-Json|Set-Content "$root/native-process.json"
 if(!$peer.WaitForExit(65000)){throw 'Native modal deadline'}
 $peer.WaitForExit();$peer.ExitCode|Set-Content "$root/native-exit.txt"
 if(!$process.WaitForExit(70000)){throw 'Guest modal completion deadline'}
 $process.WaitForExit();$process.ExitCode|Set-Content "$root/observer-exit.txt"
 if(Test-Path "$tests/MODAL.BIN"){Copy-Item "$tests/MODAL.BIN" "$root/modal.bin"}
 if($peer.ExitCode -or $process.ExitCode){throw 'Native/guest modal process failed'}
 $report=Get-Content "$root/observer.txt" -Raw
 if($report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Missing normal guest completion'}
 $bytes=[IO.File]::ReadAllBytes("$root/modal.bin")
 if($bytes.Length -ne 42){throw 'Expected exactly three original guest return records'}
 $rows=@()
 foreach($offset in @(0,14,28)){
  if([Text.Encoding]::ASCII.GetString($bytes,$offset,4) -ne 'MDL1'){throw 'Invalid modal record'}
  $rows+=[pscustomobject]@{task=[BitConverter]::ToUInt16($bytes,$offset+4);phase=[BitConverter]::ToUInt16($bytes,$offset+6);message=[BitConverter]::ToUInt16($bytes,$offset+8);about=[BitConverter]::ToUInt16($bytes,$offset+10);errors=[BitConverter]::ToUInt16($bytes,$offset+12)}
 }
 $rows|ConvertTo-Json|Set-Content "$root/guest.json"
 if(($rows.phase -join ',') -ne '0,1,2' -or !$rows[0].task -or @($rows|Where-Object {$_.task -ne $rows[0].task -or $_.errors}).Count -or $rows[1].message -ne 1 -or $rows[2].message -ne 1 -or $rows[2].about -ne 1){throw 'Original guest modal continuation/result mismatch'}
 $trace=Get-Content "$root/lifetime.txt" -Raw;$identity='{0:X8}' -f [int]$rows[0].task
 if([regex]::Matches($trace,'(?m)^'+$worker.ProcessId+' TaskRetirement .*task='+$identity+' stage=complete status=0\r?$').Count -ne 1){throw 'Actual modal task must retire exactly once'}
 'PASS actual USER16 MessageBox, SHELL16 ShellAbout, both IDOK results, guest continuation and one normal task retirement'|Set-Content "$root/result.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 try{if($scope){Stop-IsolatedPackageScope $scope}}
 finally{
  try{foreach($p in @($peer,$process)){if($p){$null=$p.Handle;if(!$p.HasExited){$p.Kill();$p.WaitForExit(5000)|Out-Null};$p.Dispose()}}}
  finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
 }
}
