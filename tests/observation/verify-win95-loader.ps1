[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
 [Parameter(Mandatory)][string]$GuestProbe,
 [Parameter(Mandatory)][string]$Lnk,
 [ValidateSet('Keep','On','Off')][string]$Paging='Keep',[switch]$AfterMigration,[switch]$MenuOnly,[string]$WindowsRoot='O:\WINDOWS',
 [int]$XmsKB=65535,
 [string]$RuntimeRoot='O:\WINNT',
 [string]$GuestDesktopObserver='O:\assets\win95-ntvdm-research\display256\hostfix\OBS95.EXE',
 [string]$Observer='O:\WINNT\logs\win95-research\observer.exe')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($OutputRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build output required'}
$availability=[Diagnostics.Stopwatch]::StartNew()
while(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){if($availability.Elapsed.TotalSeconds -gt 90){throw 'Shared runtime busy; no case started'};Start-Sleep -Seconds 1}
New-Item -ItemType Directory "$out/package/system32" -Force|Out-Null
Get-ChildItem "$RuntimeRoot/system32" -File|Copy-Item -Destination "$out/package/system32"
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null;$process=$null;$saved=@();$passed=$false
$createdTests=!(Test-Path "$WindowsRoot/PATCH/TESTS")
$logPath="$WindowsRoot/PATCH/TESTS/MENU95.LOG"
try {
 $stage=New-PhysicalRuntimeStage "$out/package" -TopLevelNames @('system32')
 $scope=New-IsolatedPackageScope $stage
 Copy-Item $GuestProbe "$stage/MENU95.EXE"
 if($MenuOnly){[IO.File]::WriteAllText("$stage/INSPECT.TAG",'explicit menu-only gate; program launch remains a separate non-pass')}
 Copy-Item $GuestDesktopObserver "$stage/DSP95.EXE"
 if(!$AfterMigration){Copy-Item -LiteralPath 'O:\Program Files' -Destination "$stage/APPS95" -Recurse}
 $apps=if($AfterMigration){"$WindowsRoot\APPS95"}else{"$stage\APPS95"}
 $dosChinese=[Text.Encoding]::GetEncoding(437).GetString([Text.Encoding]::GetEncoding(936).GetBytes('测试'))+'.LNK'
 foreach($item in @(@{file='NOTE.LNK';target="$WindowsRoot\NOTEPAD.EXE"},@{file='PAINT.LNK';target="$apps\ACCESS~1\MSPAINT.EXE"},@{file='WORD.LNK';target="$apps\ACCESS~1\WORDPAD.EXE"},@{file=$dosChinese;target="$WindowsRoot\NOTEPAD.EXE"})){
  & $Lnk create (Join-Path $stage $item.file) --target $item.target --directory (Split-Path $item.target) --codepage 936
  if($LASTEXITCODE){throw 'Classic canary link creation failed'}
 }
 New-Item -ItemType Directory "$stage/TEMP","$WindowsRoot/PATCH/TESTS" -Force|Out-Null
 foreach($file in @('WIN.INI','SYSTEM.INI','SYSTEM.DAT','USER.DAT','PATCH/TESTS/MENU95.LOG','PATCH/TESTS/OBS95.LOG')){
  $path=Join-Path $WindowsRoot $file;$row=@{path=$path;exists=[IO.File]::Exists($path)}
  if($row.exists){$row.backup=Join-Path $out ($file.Replace('/','_')+'.before');$row.hash=(Get-FileHash -LiteralPath $path).Hash;$row.attributes=(Get-Item -LiteralPath $path -Force).Attributes;[IO.File]::Copy($path,$row.backup,$true)}
  $saved+=,$row
 }
 if($Paging -ne 'Keep'){
  $sysEncoding=[Text.Encoding]::GetEncoding(936)
  $sysPath="$WindowsRoot/SYSTEM.INI"
  $sys=$sysEncoding.GetString([IO.File]::ReadAllBytes($sysPath))
  $sys=[regex]::Replace($sys,'(?im)^Paging=[^\r\n]*',"Paging=$Paging")
  [IO.File]::SetAttributes($sysPath,[IO.FileAttributes]::Normal)
  [IO.File]::WriteAllBytes($sysPath,$sysEncoding.GetBytes($sys))
 }
 Copy-Item "$WindowsRoot/PATCH/CONFIG.NT" "$stage/CONFIG.NT"
 $auto="@echo off`r`nSET windir=$WindowsRoot`r`nSET winbootdir=$WindowsRoot`r`nSET TEMP=$stage\TEMP`r`nSET TMP=$stage\TEMP`r`nSET PATH=$WindowsRoot;$WindowsRoot\SYSTEM;$stage\system32`r`n"
 [IO.File]::WriteAllText("$stage/AUTOEXEC.NT",$auto,[Text.Encoding]::ASCII)
 Copy-Item "$WindowsRoot/PATCH/WIN.PIF" "$stage/WIN.PIF"
 & "$RuntimeRoot/system32/PIF.EXE" update "$stage/WIN.PIF" --config "$stage/CONFIG.NT" --autoexec "$stage/AUTOEXEC.NT" --w386-max-xms $XmsKB
 if($LASTEXITCODE){throw 'Witness PIF edit failed'}
 $encoding=[Text.Encoding]::GetEncoding(936);$ini=$encoding.GetString([IO.File]::ReadAllBytes("$WindowsRoot/WIN.INI"))
 $ini=[regex]::Replace($ini,'(?im)^run=[^\r\n]*',"run=$stage\MENU95.EXE")
 $ini=[regex]::Replace($ini,'(?im)^load=[^\r\n]*',"load=$stage\DSP95.EXE")
 [IO.File]::SetAttributes("$WindowsRoot/WIN.INI",[IO.FileAttributes]::Normal)
 [IO.File]::WriteAllBytes("$WindowsRoot/WIN.INI",$encoding.GetBytes($ini))
 if([IO.File]::Exists($logPath)){Remove-Item -LiteralPath $logPath -Force}
 if([IO.File]::Exists("$WindowsRoot/PATCH/TESTS/OBS95.LOG")){Remove-Item -LiteralPath "$WindowsRoot/PATCH/TESTS/OBS95.LOG" -Force}
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.WindowStyle='Hidden';$start.WorkingDirectory=$WindowsRoot
 $start.Environment['MVDM_OBSERVER_PRIVATE_DESKTOP']='1';$start.Environment['MVDM_OBSERVER_OWN_JOB']='1'
 $start.Environment['PATH']="$stage\system32;$env:SystemRoot\System32"
 $start.Environment['TEMP']="$stage\TEMP";$start.Environment['TMP']="$stage\TEMP"
 foreach($arg in @("$stage/system32/run16.exe",$WindowsRoot,"$out/observer.txt","$stage/WIN.PIF",'--observation-timeout-ms','60000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start)
 @{pid=$process.Id;started=$process.StartTime;command=$start.FileName;arguments=@($start.ArgumentList);stage=$stage}|ConvertTo-Json -Depth 4|Set-Content "$out/process.json"
 if(!$process.WaitForExit(80000)){throw 'Menu witness diagnostic deadline'}
 if([IO.File]::Exists($logPath)){Copy-Item -LiteralPath $logPath "$out/MENU95.LOG"}
 if([IO.File]::Exists("$stage/CHILD95.LOG")){Copy-Item -LiteralPath "$stage/CHILD95.LOG" "$out/CHILD95.LOG"}
 if([IO.File]::Exists("$WindowsRoot/PATCH/TESTS/OBS95.LOG")){Copy-Item -LiteralPath "$WindowsRoot/PATCH/TESTS/OBS95.LOG" "$out/OBS95.LOG"}
 $log=if([IO.File]::Exists("$out/MENU95.LOG")){[IO.File]::ReadAllText("$out/MENU95.LOG",$encoding)}else{''}
 $passed=$log.Contains('LOADER_DIAGNOSTIC_COMPLETE=1')
 @{diagnosticComplete=$passed;paging=$Paging;computerUse=$false;inputInjected=$false;workerSha256=(Get-FileHash "$stage/system32/ntvdm.exe").Hash;probeSha256=(Get-FileHash $GuestProbe).Hash}|ConvertTo-Json|Set-Content "$out/result.json"
 if(!$passed){throw 'Loader diagnostic did not complete; retain partial log'}
}catch{($_|Out-String)+$_.ScriptStackTrace|Set-Content "$out/failure.txt";throw}
finally{
 if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
 if($scope){Stop-IsolatedPackageScope $scope}
 foreach($row in $saved){if($row.exists){[IO.File]::SetAttributes($row.path,[IO.FileAttributes]::Normal);[IO.File]::Copy($row.backup,$row.path,$true);[IO.File]::SetAttributes($row.path,$row.attributes);if((Get-FileHash -LiteralPath $row.path).Hash -ne $row.hash){throw 'Guest configuration restoration failed'}}elseif([IO.File]::Exists($row.path)){Remove-Item -LiteralPath $row.path -Force}}
 if($createdTests -and (Test-Path "$WindowsRoot/PATCH/TESTS") -and @(Get-ChildItem "$WindowsRoot/PATCH/TESTS" -Force).Count -eq 0){Remove-Item -LiteralPath "$WindowsRoot/PATCH/TESTS"}
 if($stage -and $passed){Remove-PhysicalRuntimeStage $stage}
}
