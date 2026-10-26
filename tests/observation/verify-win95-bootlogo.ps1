[CmdletBinding()]
param(
 [Parameter(Mandatory)][string]$RuntimeRoot,
 [Parameter(Mandatory)][string]$AddonBuildRoot,
 [Parameter(Mandatory)][string]$Logo,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$GuestObserver,
 [Parameter(Mandatory)][string]$Python,
 [Parameter(Mandatory)][string]$WorkerMap,
 [Parameter(Mandatory)][string]$NativeLayout,
 [Parameter(Mandatory)][string]$OutputRoot,
 [ValidateSet('Enabled','Missing','Invalid','Disabled')][string]$LogoCase='Enabled',
 [string]$WindowsRoot='O:\WINDOWS'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned evidence required'}
New-Item -ItemType Directory "$out/package/system32" -Force|Out-Null
$runtime=(Resolve-Path $RuntimeRoot).Path
$system=Join-Path $runtime 'system32'
Get-ChildItem $system -File|Copy-Item -Destination "$out/package/system32"
Copy-Item $Logo "$out/LOGO.SYS"
Copy-Item $WorkerMap "$out/current-worker.map"
Copy-Item $NativeLayout "$out/layout.json"
Copy-Item "$AddonBuildRoot/compat95.lst" "$out/compat95.lst"
Copy-Item "$AddonBuildRoot/compat95.sys" "$out/compat95.sys"
(Get-FileHash "$system/ntvdm.exe").Hash|Set-Content "$out/current-worker.sha256"
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null;$process=$null;$saved=@();$logoExact=$false;$desktop=$false
$phases=[Collections.Generic.HashSet[int]]::new();$retired=$false;$stopped=$null;$postHandoffSamples=0
$logPath="$WindowsRoot\PATCH\TESTS\OBS95.LOG"
$createdTests=!(Test-Path "$WindowsRoot/PATCH/TESTS")
try {
 $stage=New-PhysicalRuntimeStage "$out/package" -TopLevelNames @('system32')
 $scope=New-IsolatedPackageScope $stage
 Copy-Item "$AddonBuildRoot/compat95.sys" "$stage/compat95.sys"
 Copy-Item $Logo "$stage/LOGO.SYS"
 Copy-Item $GuestObserver "$stage/DSP95.EXE"
 New-Item -ItemType Directory "$stage/TEMP","$WindowsRoot/PATCH/TESTS" -Force|Out-Null
 foreach($file in @('WIN.INI','SYSTEM.INI','SYSTEM.DAT','USER.DAT','PATCH/TESTS/OBS95.LOG')) {
  $path=Join-Path $WindowsRoot $file
  $row=@{path=$path;exists=[IO.File]::Exists($path)}
  if($row.exists){$row.backup=Join-Path $out ($file.Replace('/','_')+'.before');$row.hash=(Get-FileHash -LiteralPath $path).Hash;$row.attributes=(Get-Item -LiteralPath $path -Force).Attributes;[IO.File]::Copy($path,$row.backup,$true)}
  $saved+=,$row
 }
 $config=[IO.File]::ReadAllText("$WindowsRoot/PATCH/CONFIG.NT")
 $logoOption=switch($LogoCase){'Enabled'{" /LOGO=$stage\LOGO.SYS"};'Missing'{" /LOGO=$stage\MISSING.SYS"};'Invalid'{[IO.File]::WriteAllBytes("$stage/BAD.SYS",[byte[]](1,2,3));" /LOGO=$stage\BAD.SYS"};'Disabled'{''}}
 $config=[regex]::Replace($config,'(?im)^device=.*(?:W95COMP|COMPAT|COMPAT95)\.SYS[^\r\n]*',"device=$stage\compat95.sys$logoOption")
 [IO.File]::WriteAllText("$stage/CONFIG.NT",$config,[Text.Encoding]::ASCII)
 $auto="@echo off`r`nSET windir=$WindowsRoot`r`nSET winbootdir=$WindowsRoot`r`nSET TEMP=$stage\TEMP`r`nSET TMP=$stage\TEMP`r`nSET PATH=$WindowsRoot;$WindowsRoot\SYSTEM;$stage\system32`r`n"
 [IO.File]::WriteAllText("$stage/AUTOEXEC.NT",$auto,[Text.Encoding]::ASCII)
 Copy-Item "$WindowsRoot/PATCH/WIN.PIF" "$stage/WIN.PIF"
 & "$system/PIF.EXE" update "$stage/WIN.PIF" --config "$stage/CONFIG.NT" --autoexec "$stage/AUTOEXEC.NT"
 if($LASTEXITCODE){throw 'Test PIF edit failed'}
 $encoding=[Text.Encoding]::GetEncoding(936)
 $ini=$encoding.GetString([IO.File]::ReadAllBytes("$WindowsRoot/WIN.INI"))
 $ini=[regex]::Replace($ini,'(?im)^run=[^\r\n]*',"run=$stage\DSP95.EXE")
 [IO.File]::SetAttributes("$WindowsRoot/WIN.INI",[IO.FileAttributes]::Normal)
 [IO.File]::WriteAllBytes("$WindowsRoot/WIN.INI",$encoding.GetBytes($ini))
 if(Test-Path -LiteralPath $logPath){Remove-Item -LiteralPath $logPath -Force}
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.WindowStyle='Hidden';$start.WorkingDirectory=$WindowsRoot
 $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1';$start.Environment['MVDM_OBSERVER_OWN_JOB']='1'
 $start.Environment['PATH']="$stage\system32;$env:SystemRoot\System32"
 $start.Environment['TEMP']="$stage\TEMP";$start.Environment['TMP']="$stage\TEMP"
 foreach($arg in @("$stage/system32/run16.exe",$WindowsRoot,"$out/observer.txt","$stage/WIN.PIF",'--observation-timeout-ms','65000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start)
 @{pid=$process.Id;started=$process.StartTime;command=$start.FileName;arguments=@($start.ArgumentList);stage=$stage}|ConvertTo-Json -Depth 4|Set-Content "$out/process.json"
 $watch=[Diagnostics.Stopwatch]::StartNew();$sample=0;$next=0
 while(!$process.WaitForExit(200)) {
  if($watch.Elapsed.TotalSeconds -gt 85){throw 'Owned boot observation deadline'}
  if($watch.Elapsed.TotalSeconds -lt $next){continue};$next=$watch.Elapsed.TotalSeconds+0.5
  $worker=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'"|Where-Object ExecutablePath -eq "$stage\system32\ntvdm.exe")
  if($worker.Count -ne 1){continue}
  $prefix=Join-Path $out ('frame-'+$sample.ToString('000'));$sample++
  & $Python "$PSScriptRoot/capture-win95-bootlogo.py" $worker[0].ProcessId $prefix $out "$stage\system32\ntvdm.exe" *> ($prefix+'.capture.txt')
  if(Test-Path ($prefix+'.json')) {
   $frame=Get-Content ($prefix+'.json') -Raw|ConvertFrom-Json
   if($LogoCase -ne 'Enabled' -and ($frame.driver.logo_active -ne 0 -or $frame.driver.logo_timer_hooked -ne 0 -or $frame.driver.logo_frames -ne 0 -or $frame.driver.timerVectorOwned -or $frame.driver.videoVectorOwned)){throw 'Negative logo case installed animation hooks'}
   if($frame.logoExact){$logoExact=$true}
   if($frame.logoExact -and $null -ne $frame.palettePhase){$null=$phases.Add([int]$frame.palettePhase)}
   if($frame.driver.logo_handoffs -gt 0 -and $frame.driver.logo_active -eq 0 -and $frame.driver.logo_timer_hooked -in @(0,2) -and !$frame.driver.timerVectorOwned -and !$frame.driver.videoVectorOwned){
    if($null -eq $stopped){$stopped=$frame.driver.logo_frames}
    if($frame.driver.logo_frames -ne $stopped -or $frame.driver.logo_stopped_frames -ne $stopped){throw 'Animation continued after display handoff'}
    $postHandoffSamples++;if($postHandoffSamples -ge 3){$retired=$true}
   }
   if(($logoExact -or $LogoCase -ne 'Enabled') -and $frame.width -eq 640 -and [Math]::Abs($frame.height) -eq 480 -and $frame.rowsNonzero -eq 480){$desktop=$true}
  }
 }
 if(Test-Path -LiteralPath $logPath){Copy-Item -LiteralPath $logPath -Destination "$out/OBS95.LOG"}
 $guest=if(Test-Path "$out/OBS95.LOG"){[IO.File]::ReadAllText("$out/OBS95.LOG")}else{''}
 $logoPassed=if($LogoCase -eq 'Enabled'){$logoExact -and $phases.Count -ge 2 -and $retired}else{!$logoExact -and $phases.Count -eq 0}
 $passed=$logoPassed -and $desktop -and $guest.Contains('OBSERVE_COMPLETE') -and $guest.Contains('class=Shell_TrayWnd') -and $guest.Contains('class=Progman')
 @{passed=$passed;logoCase=$LogoCase;logoExact=$logoExact;animationPhases=@($phases);animationRetired=$retired;postHandoffSamples=$postHandoffSamples;desktopAfterLogo=$desktop;samples=$sample;observerExit=$process.ExitCode;computerUse=$false;inputInjected=$false;workerSha256=(Get-FileHash "$stage/system32/ntvdm.exe").Hash;driverSha256=(Get-FileHash "$stage/compat95.sys").Hash}|ConvertTo-Json|Set-Content "$out/result.json"
 if(!$passed){throw 'Exact guest logo or subsequent desktop gate failed'}
} catch {($_|Out-String)+$_.ScriptStackTrace|Set-Content "$out/failure.txt";throw}
finally {
 if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
 if($scope){Stop-IsolatedPackageScope $scope}
 foreach($row in $saved){if($row.exists){[IO.File]::SetAttributes($row.path,[IO.FileAttributes]::Normal);[IO.File]::Copy($row.backup,$row.path,$true);[IO.File]::SetAttributes($row.path,$row.attributes);if((Get-FileHash -LiteralPath $row.path).Hash -ne $row.hash){throw 'Guest configuration restoration failed'}}elseif(Test-Path -LiteralPath $row.path){Remove-Item -LiteralPath $row.path -Force}}
 if($createdTests -and (Test-Path "$WindowsRoot/PATCH/TESTS") -and @(Get-ChildItem "$WindowsRoot/PATCH/TESTS" -Force).Count -eq 0){Remove-Item -LiteralPath "$WindowsRoot/PATCH/TESTS"}
 if($stage -and (Test-Path "$out/result.json") -and (Get-Content "$out/result.json" -Raw|ConvertFrom-Json).passed){Remove-PhysicalRuntimeStage $stage}
}
