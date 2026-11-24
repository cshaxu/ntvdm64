param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$InstallSource,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$OutputRoot,
 [ValidateSet(0,1)][int]$Multitask=0,
 [string]$RejectNativeImage
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../..").Path
$package=(Resolve-Path $PackageRoot).Path;$observer=(Resolve-Path $Observer).Path
$copy=& "$repo/test/integration/ntvdm64-386/New-Win95ProbeCopy.ps1" `
 -DriverImage "$repo/assets/release/ntvdm64.386" -InstanceDriver "$repo/assets/release/insignia.386" `
 -InstallSource $InstallSource -InstallRoot 'O:/tmp/q95' -OutputRoot $OutputRoot
$dest=$copy.Destination;$task=$copy.Task;$savedPath=$env:PATH
try {
 & "$repo/tools/reapply-win95-test-launch.cmd" $dest $task $package 1 $Multitask *> "$task/select-launch.log"
 if($LASTEXITCODE){throw 'Guest-only launch preparation failed'}
 if((Get-Content "$dest/PATCH/CONFIG.NT" -Raw) -notmatch '(?im)^dosonly\s*$'){throw 'Guest-only profile lacks DOSONLY'}
  $selectedIni=Get-Content "$dest/SYSTEM.INI" -Raw
 if([bool]($selectedIni -match '(?im)^device=.*NTVDM64\.386') -ne [bool]$Multitask -or $selectedIni -notmatch '(?im)^device=.*INSIGNIA\.386'){throw 'Guest-only profile driver selection mismatch'}
 $template='ONLY.COM'
 if($RejectNativeImage){
  $image=(Resolve-Path -LiteralPath $RejectNativeImage).Path
  $pe=[IO.File]::ReadAllBytes($image);$header=[BitConverter]::ToInt32($pe,60)
  if([BitConverter]::ToUInt16($pe,$header+4) -ne 0x8664){throw 'Rejection witness requires the specified AMD64 image'}
  # DOS can execute a PE image's MZ stub. Pin its entry and exact refusal
  # instructions instead of incorrectly expecting EXEC to reject the format.
  $stub=[BitConverter]::ToUInt16($pe,8)*16
  if([BitConverter]::ToUInt32($pe,20) -ne 0 -or
     [BitConverter]::ToString($pe,$stub,14) -ne '0E-1F-BA-0E-00-B4-09-CD-21-B8-01-4C-CD-21'){
   throw 'Rejection witness requires the known DOS refusal stub'
  }
  & "$repo/tools/prepare-test-inputs.cmd" -GuestPatch $dest $task $image NATIVE.EXE
  if($LASTEXITCODE){throw 'Native refusal witness preparation failed'}
  Get-FileHash "$dest/PATCH/NATIVE.EXE" | ConvertTo-Json | Set-Content "$task/native-image.json"
  $template='REJECT.COM'
 }
 & "$repo/tools/build-fixtures.cmd" x86 win95-guest-only
 if($LASTEXITCODE){throw 'DOS probe native build failed'}
 & "$repo/tools/prepare-test-inputs.cmd" -GuestPatch $dest $task "$repo/build/.ninja/fixtures-x86/guest-probes/win95-guest-only/$template" ONLY.COM
 if($LASTEXITCODE){throw 'Guest-only witness preparation failed'}
 $fields=@('--fields',"$dest/PATCH/ONLY.COM",'@OUTPUT@',"$dest\PATCH\ONLY.BIN")
 if($RejectNativeImage){$fields+=@('@NATIVE@',"$dest\PATCH\NATIVE.EXE")}
 & "$repo/build/.ninja/fixtures-x86/win31-probe-paths.exe" @fields
 if($LASTEXITCODE){throw 'DOS probe path binding failed'}
 & "$repo/assets/release/PIF.EXE" create "$dest/PATCH/ONLY.PIF" --title 'Guest DOS' --program "$dest\PATCH\ONLY.COM" --directory "$dest\PATCH" --arguments '' --config '' --autoexec '' --close-on-exit --w386-flags 0x21002 --min-memory 128 --max-memory 640
 if($LASTEXITCODE){throw 'Guest-only PIF creation failed'}
 & "$repo/tools/prepare-guest-witness.cmd" $dest $task - load only
 if($LASTEXITCODE){throw 'Guest-only load profile preparation failed'}
 . "$repo/test/observation/isolated_package_cleanup.ps1"
 $stage=$null;$scope=$null;$process=$null
 try {
  $stage=New-PhysicalRuntimeStage $package -FreshPackageIdentity;$scope=New-IsolatedPackageScope $stage
  $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:MVDM_OBSERVER_OWN_JOB='1';$env:PATH="$stage/system32;C:\Windows\System32;C:\Windows"
  $process=Start-Process $observer -ArgumentList @("$stage/system32/run16.exe",$dest,"$task/observer.txt","$dest/PATCH/WIN.PIF",'--observation-timeout-ms','90000') -WindowStyle Hidden -PassThru
  $until=[DateTime]::UtcNow.AddSeconds(90)
  while(!(Test-Path "$dest/PATCH/ONLY.BIN") -and !$process.HasExited -and [DateTime]::UtcNow -lt $until){Start-Sleep -Milliseconds 200;$process.Refresh()}
  if(!(Test-Path "$dest/PATCH/ONLY.BIN") -or [IO.File]::ReadAllText("$dest/PATCH/ONLY.BIN") -ne 'PASS'){throw 'Guest DOS witness did not complete'}
  $workers=@(Get-CimInstance Win32_Process | Where-Object { $_.ExecutablePath -and $_.ExecutablePath -ieq "$stage\system32\ntvwm.exe" })
  if($workers.Count){throw 'Guest-only DOS witness unexpectedly started a native CUI worker'}
  if($RejectNativeImage){'PASS Win95 DOSONLY=1 executes the pinned AMD64 image DOS refusal stub with normal exit code 1'}
  'PASS Win95 DOSONLY=1 executes a guest DOS program; no native CUI worker present while it waits; test-owned session retired by fixture'
 } finally {
  if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
  if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}
 }
 foreach($row in $copy.Original){if((Get-FileHash ($copy.Source+$row.path)).Hash -ne $row.sha256){throw "Original changed: $($row.path)"}}
} finally {
 $env:PATH=$savedPath
 . "$repo/test/observation/isolated_package_cleanup.ps1"
 Remove-OwnedGuestProbeCopy $dest $task $copy.Source
}
