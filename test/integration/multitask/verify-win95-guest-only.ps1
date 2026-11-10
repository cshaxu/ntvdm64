param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$package=(Resolve-Path $PackageRoot).Path;$observer=(Resolve-Path $Observer).Path
$copy=& "$repo/test/integration/ntvdm64-386/New-Win95ProbeCopy.ps1" `
 -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
 -InstallSource $InstallSource -InstallRoot 'O:/tmp/q95' -OutputRoot $OutputRoot
$dest=$copy.Destination;$task=$copy.Task;$savedPath=$env:PATH
try {
 $enc=[Text.Encoding]::GetEncoding(28591);$ini="$dest/SYSTEM.INI"
 $text=$enc.GetString([IO.File]::ReadAllBytes($ini))
 $text=[regex]::Replace($text,'(?im)^\s*device\s*=.*(?:ntvdm64\.(?:386|vxd)|insignia\.386)\s*\r*\n','')
 [IO.File]::WriteAllBytes($ini,$enc.GetBytes($text))
 foreach($name in 'CONFIG.NT','AUTOEXEC.NT','WIN.PIF','WIN.CMD','compat95.sys','SETVER.EXE','INSIGNIA.386','NTVDM64.386','MULTITASK.INI.BAK','MULTITASK.INI.NEW','MULTITASK.HASH','MULTITASK.LAUNCH'){
  if(Test-Path "$dest/PATCH/$name"){Remove-Item -LiteralPath "$dest/PATCH/$name"}
 }
 if(Test-Path "$dest/PATCH/TEMP"){Remove-Item -LiteralPath "$dest/PATCH/TEMP"}
 if(Test-Path "$dest/SYSTEM/MOUSE.DRV.BAK"){Remove-Item -LiteralPath "$dest/SYSTEM/MOUSE.DRV.BAK"}
 [IO.File]::WriteAllText("$task/input.txt","$dest`r`n1`r`n",[Text.Encoding]::ASCII)
 $env:PATH="$package/system32;$savedPath"
 & cmd.exe /d /c "`"$repo/tools/win95-launch/APPLY.CMD`" < `"$task/input.txt`"" *> "$task/apply.log"
 if($LASTEXITCODE){throw 'Guest-only launch preparation failed'}
 if((Get-Content "$dest/PATCH/CONFIG.NT" -Raw) -notmatch '(?im)^dosonly\s*$'){throw 'Guest-only profile lacks DOSONLY'}
 foreach($name in 'INSIGNIA.386','NTVDM64.386'){if(Test-Path "$dest/PATCH/$name"){throw "Unexpected driver $name"}}
 & nasm -f bin "$PSScriptRoot/win95-guest-only.asm" -o "$dest/PATCH/ONLY.COM"
 if($LASTEXITCODE){throw 'DOS probe assembly failed'}
 $bytes=[IO.File]::ReadAllBytes("$dest/PATCH/ONLY.COM");$i=[Text.Encoding]::ASCII.GetString($bytes).IndexOf('@OUTPUT@')
 $path=[Text.Encoding]::ASCII.GetBytes("$dest\PATCH\ONLY.BIN")
 if($i -lt 0 -or $path.Length -ge 128){throw 'Invalid DOS probe path'}
 [Array]::Clear($bytes,$i,128);[Array]::Copy($path,0,$bytes,$i,$path.Length);[IO.File]::WriteAllBytes("$dest/PATCH/ONLY.COM",$bytes)
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/ONLY.PIF" --title 'Guest DOS' --program "$dest\PATCH\ONLY.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002
 if($LASTEXITCODE){throw 'Guest-only PIF creation failed'}
 & "$repo/test/integration/ntvdm64-386/Set-Win95ProbePifMemory.ps1" -Path "$dest/PATCH/ONLY.PIF"
 $win="$dest/WIN.INI";$text=$enc.GetString([IO.File]::ReadAllBytes($win))
 $text=[regex]::Replace($text,'(?im)^load=[^\r\n]*',"load=$dest\PATCH\ONLY.PIF")
 [IO.File]::WriteAllBytes($win,$enc.GetBytes($text))
 . "$repo/tests/observation/isolated_package_cleanup.ps1"
 $stage=$null;$scope=$null;$process=$null
 try {
  $stage=New-PhysicalRuntimeStage $package -FreshPackageIdentity;$scope=New-IsolatedPackageScope $stage
  $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:MVDM_OBSERVER_OWN_JOB='1';$env:PATH="$stage/system32;C:\Windows\System32;C:\Windows"
  $process=Start-Process $observer -ArgumentList @("$stage/system32/run16.exe",$dest,"$task/observer.txt","$dest/PATCH/WIN.PIF",'--observation-timeout-ms','90000') -WindowStyle Hidden -PassThru
  $until=[DateTime]::UtcNow.AddSeconds(90)
  while(!(Test-Path "$dest/PATCH/ONLY.BIN") -and !$process.HasExited -and [DateTime]::UtcNow -lt $until){Start-Sleep -Milliseconds 200;$process.Refresh()}
  if(!(Test-Path "$dest/PATCH/ONLY.BIN") -or [IO.File]::ReadAllText("$dest/PATCH/ONLY.BIN") -ne 'PASS'){throw 'Guest DOS witness did not complete'}
  'PASS Win95 DOSONLY=1 launches and executes a guest DOS program; test-owned session retired by fixture'
 } finally {
  if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
  if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}
 }
 foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
} finally {
 $env:PATH=$savedPath
 . "$repo/tests/observation/isolated_package_cleanup.ps1"
 Remove-OwnedGuestProbeCopy $dest $task $copy.Source
}
