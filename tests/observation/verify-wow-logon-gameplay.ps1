[CmdletBinding()]
param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$GameProbe,
 [Parameter(Mandatory)][string]$BuildRoot,
 [string]$PrerequisiteRoot,[switch]$Installed)
# Use the complete supported Userenv logon block, not an environment allowlist.
# This is not a read of another user's existing shell or a product env change.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$package=(Resolve-Path $PackageRoot).Path
if($Installed -and $package -ne 'O:\winnt'){throw 'Installed observation is confined to the admitted deployment'}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$env:TEMP=$root;$env:TMP=$root
$stage=$null;$scope=$null;$p=$null
try{
 if($PrerequisiteRoot -and !(Test-Path "$PrerequisiteRoot/exit.txt")){
  $raw=Get-Content ($PrerequisiteRoot+'.process.json') -Raw;$identity=$raw|ConvertFrom-Json
  $prior=Get-Process -Id $identity.pid
  $stamp=[regex]::Match($raw,'"start"\s*:\s*"([^"]+)"').Groups[1].Value
  $start=[DateTimeOffset]::Parse($stamp,[Globalization.CultureInfo]::InvariantCulture).UtcDateTime
  if([Math]::Abs(($prior.StartTime.ToUniversalTime()-$start).TotalSeconds) -gt 1){throw 'Prerequisite process identity changed'}
  @{pid=$prior.Id;start=$stamp;timeoutSeconds=6600}|ConvertTo-Json|Set-Content "$root/prerequisite.json"
  if(!$prior.WaitForExit(6600000)){throw 'Prerequisite still running; no runtime started'}
 }
 if($PrerequisiteRoot -and (Get-Content "$PrerequisiteRoot/exit.txt" -Raw).Trim() -ne '0'){throw 'Prerequisite failed'}
 Add-Type -OutputAssembly "$root/logon-env.dll" -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
public static class WowLogonEnvironment {
 [DllImport("userenv.dll",SetLastError=true)] static extern bool CreateEnvironmentBlock(out IntPtr block,IntPtr token,bool inherit);
 [DllImport("userenv.dll",SetLastError=true)] static extern bool DestroyEnvironmentBlock(IntPtr block);
 public static string[] Read(IntPtr token) {
  IntPtr block;
  if(!CreateEnvironmentBlock(out block,token,false))throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
  try {
   var entries=new List<string>();IntPtr at=block;
   while(Marshal.ReadInt16(at)!=0){string value=Marshal.PtrToStringUni(at);entries.Add(value);at=IntPtr.Add(at,(value.Length+1)*2);}
   return entries.ToArray();
  }finally{DestroyEnvironmentBlock(block);}
 }
}
'@
 Add-Type -Path "$root/logon-env.dll"
 $logonIdentity=[Security.Principal.WindowsIdentity]::GetCurrent()
 try{$entries=[WowLogonEnvironment]::Read($logonIdentity.Token)}
 finally{$logonIdentity.Dispose()}
 $shape=@($entries|ForEach-Object{$split=$_.IndexOf('=',1);[pscustomobject]@{name=$_.Substring(0,$split);characters=$_.Length+1}})
 $shape|Export-Csv -NoTypeInformation "$root/environment-shape.csv"
 @{source='CreateEnvironmentBlock current token inherit=false';entries=$shape.Count;characters=1+($shape|Measure-Object characters -Sum).Sum;filtering=$false;limit='Logon block, not a snapshot of an already-running shell'}|ConvertTo-Json|Set-Content "$root/environment-contract.json"
 Get-FileHash $Observer,$GameProbe,$PSCommandPath|Export-Csv -NoTypeInformation "$root/probe-inputs.csv"
 $shell=(Get-Process -Id $PID).Path
 foreach($mode in @('Off','Win16','All')){
  $out="$root/$mode";$null=New-Item -ItemType Directory $out
  if($Installed){
   # Deliberate installed test, not a bypass for ordinary staged cleanup.
   # No existing project process is adopted, and cleanup pins exact identities.
   $stage=$package
   $paths=@('run16.exe','ntsrv.exe','ntvdm.exe','ntcon.exe','ntvwm.exe')|ForEach-Object{Join-Path "$stage/system32" $_}
   if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -in $paths}).Count){throw 'Preserve existing installed project session'}
   $scope=[pscustomobject]@{Paths=$paths;Root=$stage}
  }else{$stage=New-PhysicalRuntimeStage $package;$scope=New-IsolatedPackageScope $stage}
  Get-ChildItem "$stage/system32" -File|Where-Object Name -in @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll','ntw16s32.dll','ntw16s64.dll','W16Style.exe')|Get-FileHash|Export-Csv -NoTypeInformation "$out/images.csv"
  $info=[Diagnostics.ProcessStartInfo]::new($Observer);$info.UseShellExecute=$false;$info.CreateNoWindow=$true
  $info.EnvironmentVariables.Clear()
  foreach($entry in $entries){$split=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$split)]=$entry.Substring($split+1)}
  $style=if($mode -eq 'Win16'){' --win16-style'}elseif($mode -eq 'All'){' --win16-style-all'}else{''}
  $info.Arguments='"'+$stage+'/system32/run16.exe" "'+$stage+'" "'+$out+'/observer.txt" --wait'+$style+' winmine --observation-timeout-ms 30000'
  $p=[Diagnostics.Process]::Start($info);$null=$p.Handle
  @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$info.Arguments;timeoutSeconds=60;desktop='inherited ordinary desktop';installed=[bool]$Installed}|ConvertTo-Json|Set-Content "$out/owner.json"
  $deadline=[DateTime]::UtcNow.AddSeconds(12);$worker=$null
  do{
   $worker=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'"|Where-Object {$_.ExecutablePath -eq "$stage\system32\ntvdm.exe"}|Select-Object -First 1
   if($worker -or $p.HasExited){break};Start-Sleep -Milliseconds 50
  }while([DateTime]::UtcNow -lt $deadline)
  if(!$worker){throw 'Owned WOW worker absent'}
  $marked=if($mode -eq 'Off'){'0'}else{'1'}
  $line='"'+$GameProbe+'" '+$worker.ProcessId+' - "'+$out+'/mine" '+$marked
  $q=Start-Process $env:ComSpec -ArgumentList ('/d /s /c "'+$line+'"') -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/game.txt" -RedirectStandardError "$out/game.err";$null=$q.Handle
  @{pid=$q.Id;start=$q.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=40}|ConvertTo-Json|Set-Content "$out/game-owner.json"
  if(!$q.WaitForExit(40000)){taskkill.exe /PID $q.Id /T /F;throw 'Gameplay deadline'}
  $q.WaitForExit();$code=$q.ExitCode;$q.Dispose();$code|Set-Content "$out/game.exit.txt";if($code){throw "Logon environment gameplay failed: $mode"}
  if(!$p.WaitForExit(35000)){throw 'Launcher completion deadline'}
  $p.WaitForExit();$report=Get-Content "$out/observer.txt" -Raw
  if($p.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'No successful normal launcher completion'}
  $p.Dispose();$p=$null
  Stop-IsolatedPackageScope $scope;$scope=$null
  if(!$Installed){Remove-PhysicalRuntimeStage $stage};$stage=$null
  0|Set-Content "$out/exit.txt"
 }
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";exit 1}
finally{if($scope){Stop-IsolatedPackageScope $scope};if($stage -and !$Installed){Remove-PhysicalRuntimeStage $stage};if($p){$null=$p.Handle;if(!$p.HasExited){$p.Kill()};$p.Dispose()}}
