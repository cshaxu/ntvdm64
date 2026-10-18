[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$WorkerMap,
 [Parameter(Mandatory)][string]$WowMap,
 [string]$HostWordSymbol,
 [switch]$Installed)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$package=(Resolve-Path $PackageRoot).Path
if($Installed -and $package -ne 'O:\winnt'){throw 'Installed scope mismatch'}
function Start-Owned($name,$exe,$line,$seconds){
 $p=Start-Process $exe -ArgumentList $line -WindowStyle Hidden -WorkingDirectory $root -PassThru -RedirectStandardOutput "$root/$name.stdout.txt" -RedirectStandardError "$root/$name.stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');image=$exe;command=$line;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root/$name.process.json"
 return $p
}
function Finish-Owned($p,$name,$seconds){
 if(!$p.WaitForExit($seconds*1000)){taskkill.exe /PID $p.Id /T /F;throw "Deadline $name"}
 $p.WaitForExit();$code=$p.ExitCode;$code|Set-Content "$root/$name.exit.txt"
 # On failure the caller still owns this object in its finally block.
 # Disposing before throwing would abort that cleanup at p.Handle and leave
 # the owned worker/broker alive, blocking the next isolated package.
 if($code){throw "$name failed $code"};$p.Dispose()
}
$stage=$null;$scope=$null;$observerProcess=$null;$game=$null;$sampler=$null;$gate=$null
try{
 foreach($pair in @(@('ntvdm.exe',$WorkerMap),@('WOW32.DLL',$WowMap))){
  $image=Join-Path (Split-Path $pair[1]) $pair[0]
  if((Get-FileHash $image).Hash -ne (Get-FileHash "$package/system32/$($pair[0])").Hash){throw "Map image mismatch: $image"}
  Copy-Item $pair[1] "$root/$($pair[0]).map"
 }
 $wordRva=$null
 if($HostWordSymbol){
  if($HostWordSymbol -notmatch '^[A-Za-z_][A-Za-z_0-9]*$'){throw 'Host diagnostic symbol must be a plain MAP name'}
  $mapText=Get-Content $WorkerMap -Raw
  $preferred=[Convert]::ToInt64(([regex]::Match($mapText,'Preferred load address is ([0-9A-Fa-f]+)')).Groups[1].Value,16)
  $symbols=[regex]::Matches($mapText,'(?m)^\s+[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+'+[regex]::Escape($HostWordSymbol)+'\s+([0-9A-Fa-f]{8})\s')
  if($symbols.Count -ne 1){throw 'Diagnostic requires one unambiguous symbol in exact worker MAP'}
  $wordRva=[Convert]::ToInt64($symbols[0].Groups[1].Value,16)-$preferred
  if($wordRva -le 0 -or $wordRva -gt [uint32]::MaxValue){throw 'Invalid native host symbol RVA'}
  @{symbol=$HostWordSymbol;rva=$wordRva;interpretation='One native DWORD read before statistical thread sampling; non-atomic, no remote writes or guest pointer decoding'}|ConvertTo-Json|Set-Content "$root/host-word.json"
 }
 Get-FileHash "$package/system32/ntvdm.exe","$package/system32/WOW32.DLL",$WorkerMap,$WowMap,$Observer,$CompilerDriver,$PSCommandPath,"$PSScriptRoot/wow_mine_latency_probe.c","$PSScriptRoot/wow_mine_gameplay_probe.c","$PSScriptRoot/wow_native_pc_sample.c"|Export-Csv -NoTypeInformation "$root/inputs.csv"
 foreach($spec in @(@('latency','wow_mine_latency_probe.c','user32.lib gdi32.lib dwmapi.lib'),@('sample','wow_native_pc_sample.c','user32.lib'))){
  $line='/d /s /c "call "'+$CompilerDriver+'" cl.exe /nologo /MT /O2 /W3 /D_CRT_SECURE_NO_WARNINGS /Fo"'+$root+'/'+$spec[0]+'.obj" /Fe"'+$root+'/'+$spec[0]+'.exe" "'+$PSScriptRoot+'/'+$spec[1]+'" /link '+$spec[2]+'"'
  $p=Start-Owned ('compile-'+$spec[0]) $env:ComSpec $line 120
  Finish-Owned $p ('compile-'+$spec[0]) 120
 }
 Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public static class MineLatencyEnvironment {
 [DllImport("userenv.dll",SetLastError=true)] static extern bool CreateEnvironmentBlock(out IntPtr block,IntPtr token,bool inherit);
 [DllImport("userenv.dll")] static extern bool DestroyEnvironmentBlock(IntPtr block);
 public static string[] Read(IntPtr token) {
  IntPtr block;if(!CreateEnvironmentBlock(out block,token,false))throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
  try {var result=new List<string>();IntPtr at=block;while(Marshal.ReadInt16(at)!=0){string value=Marshal.PtrToStringUni(at);result.Add(value);at=IntPtr.Add(at,(value.Length+1)*2);}return result.ToArray();}
  finally{DestroyEnvironmentBlock(block);}
 }
}
'@
 $logonIdentity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[MineLatencyEnvironment]::Read($logonIdentity.Token)}
 finally{$logonIdentity.Dispose()}
 $entries|ForEach-Object {$at=$_.IndexOf('=',1);[pscustomobject]@{name=$_.Substring(0,$at);characters=$_.Length+1}}|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 foreach($case in @('normal','normal-sampled','private')){
  $out="$root/$case";$null=New-Item -ItemType Directory $out
  if($Installed){
   $stage=$package;$paths=@('run16.exe','ntsrv.exe','ntvdm.exe','ntcon.exe','ntvwm.exe')|ForEach-Object {Join-Path "$stage/system32" $_}
   if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -in $paths}).Count){throw 'Preserve existing installed session'}
   $scope=[pscustomobject]@{Paths=$paths;Root=$stage}
  }else{$stage=New-PhysicalRuntimeStage $package;$scope=New-IsolatedPackageScope $stage}
  $info=[Diagnostics.ProcessStartInfo]::new($Observer);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
  $info.EnvironmentVariables.Clear()
  foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
  if($case -eq 'private'){$info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'}
  $info.Arguments='"'+$stage+'/system32/run16.exe" "'+$stage+'" "'+$out+'/observer.txt" --wait winmine --observation-timeout-ms 60000'
  $observerProcess=[Diagnostics.Process]::Start($info);$null=$observerProcess.Handle
  @{pid=$observerProcess.Id;start=$observerProcess.StartTime.ToUniversalTime().ToString('o');command=$info.Arguments;timeoutSeconds=75;environment='Complete Userenv logon block; no variable filtering';case=$case}|ConvertTo-Json|Set-Content "$out/observer.process.json"
  $deadline=[DateTime]::UtcNow.AddSeconds(12);$worker=$null
  do{$worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'"|Where-Object {$_.ExecutablePath -eq "$stage\system32\ntvdm.exe"}|Select-Object -First 1;if($worker -or $observerProcess.HasExited){break};Start-Sleep -Milliseconds 50}while([DateTime]::UtcNow -lt $deadline)
  if(!$worker){throw 'Owned worker absent'}
  $worker|Select-Object ProcessId,ParentProcessId,ExecutablePath,CreationDate,CommandLine|ConvertTo-Json|Set-Content "$out/worker.json"
  $gateName='Local\NTVDMMineLatency-'+$PID+'-'+$case
  $gate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,$gateName)
  $desktop=if($case -eq 'private'){'NTVDMConsoleTest-'+$observerProcess.Id}else{'-'}
  $line=$worker.ProcessId.ToString()+' "'+$desktop+'" "'+$out+'/mine" "'+$gateName+'"'
  $game=Start-Owned ($case+'-game') "$root/latency.exe" $line 45
  $deadline=[DateTime]::UtcNow.AddSeconds(20)
  while(!(Test-Path "$out/mine.ready") -and !$game.HasExited -and [DateTime]::UtcNow -lt $deadline){Start-Sleep -Milliseconds 50}
  if(!(Test-Path "$out/mine.ready")){throw 'Actual board not ready'}
  $readyQpc=[Diagnostics.Stopwatch]::GetTimestamp()
  if($case -eq 'normal-sampled'){
   $sampleLine=$worker.ProcessId.ToString()+' 1000'
   if($null -ne $wordRva){$sampleLine+=' '+$wordRva.ToString('X8')}
   $sampler=Start-Owned ($case+'-sample') "$root/sample.exe" $sampleLine 30
  }
  $null=$gate.Set()
  Finish-Owned $game ($case+'-game') 45;$game=$null
  if($sampler){Finish-Owned $sampler ($case+'-sample') 30;$sampler=$null}
  if($case -eq 'normal-sampled' -and $null -ne $wordRva){
   $observed=@(Get-Content "$root/$case-sample.stdout.txt"|Where-Object {$_ -match '^host-word rva=[0-9A-F]{8} address=[0-9A-F]{8} value=[0-9A-F]{8} qpc=\d+ non-atomic=1$'})
   if($observed.Count -ne 1){throw 'Native host-word observation missing'}
   $observed|Set-Content "$root/host-word.txt"
  }
  $gate.Dispose();$gate=$null
  Finish-Owned $observerProcess ($case+'-observer') 35;$observerProcess=$null
  $report=Get-Content "$out/observer.txt" -Raw
  if($report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'No normal launcher success'}
  $latency=@(Get-Content "$root/$case-game.stdout.txt"|ForEach-Object {if($_ -match '^latency operation=(\d+) microseconds=(\d+) .*start-qpc=(\d+) stop-qpc=(\d+) frequency=(\d+)'){[pscustomobject]@{operation=[int]$Matches[1];microseconds=[long]$Matches[2];start=[long]$Matches[3];stop=[long]$Matches[4];frequency=[long]$Matches[5]}}})
  if($latency.Count -ne 12){throw 'Missing actual operation measurements'}
  if($case -eq 'normal-sampled' -and $null -ne $wordRva){
   $word=[regex]::Match((Get-Content "$root/host-word.txt" -Raw),'qpc=(\d+)')
   $at=[long]$word.Groups[1].Value
   if($at -lt $readyQpc -or $at -gt $latency[-1].stop){throw 'Host word was not read while the ready board was under input measurement'}
   @{ready=$readyQpc;observed=$at;lastInput=$latency[-1].stop;frequency=[Diagnostics.Stopwatch]::Frequency}|ConvertTo-Json|Set-Content "$root/host-word-interval.json"
  }
  $latency|Export-Csv -NoTypeInformation "$out/latency.csv"
  Stop-IsolatedPackageScope $scope;$scope=$null
  if(!$Installed){Remove-PhysicalRuntimeStage $stage};$stage=$null
  0|Set-Content "$out/exit.txt"
 }
 foreach($row in Import-Csv "$root/inputs.csv"){if((Get-FileHash $row.Path).Hash -ne $row.Hash){throw "Input changed: $($row.Path)"}}
 'PASS actual twelve-input latency measurement and normal exit, ordinary/sampled/private; physical mouse latency not asserted'|Set-Content "$root/result.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
finally{
 foreach($p in @($game,$sampler,$observerProcess)){if($p){$null=$p.Handle;if(!$p.HasExited){$p.Kill();$null=$p.WaitForExit(5000)};$p.Dispose()}}
 if($gate){$gate.Dispose()};if($scope){Stop-IsolatedPackageScope $scope};if($stage -and !$Installed){Remove-PhysicalRuntimeStage $stage}
}
