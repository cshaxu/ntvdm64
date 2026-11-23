[CmdletBinding()]
param([ValidateSet(0,1)][int]$DosOnly,[string]$Run='a')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$base=Join-Path $repo 'build/side-dos-identity'
$out=Join-Path $base "dos$DosOnly-$Run"
$dest="O:\tmp\i31$DosOnly"
$pwsh=(Get-Process -Id $PID).Path
$package=Join-Path $base 'package'
& "$repo/tools/build-fixtures.cmd" x86 dos-identity-observer
if($LASTEXITCODE){throw 'Diagnostic worker build failed'}
$diagnostic=Join-Path $repo 'build/.ninja/msvc-x86-release/worker/diagnostics/dos-identity'
if(!(Test-Path $package)){
 New-Item -ItemType Directory $package|Out-Null
 Copy-Item "$repo/build/t481-s16/candidate-residue/*" $package -Recurse
 Copy-Item "$diagnostic/ntvdm.exe" "$package/system32/ntvdm.exe" -Force
}
$manifest=Get-Content "$package/product-system32-manifest.json" -Raw|ConvertFrom-Json
$entry=$manifest|Where-Object Name -eq 'ntvdm.exe'
& "$repo/tools/build/Set-PackageIdentity.ps1" -Image "$package/system32/ntvdm.exe" -PackageId ([guid]$entry.PackageId)
$entry.Sha256=(Get-FileHash "$package/system32/ntvdm.exe").Hash
$manifest|ConvertTo-Json -Depth 5|Set-Content "$package/product-system32-manifest.json"
$copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" -DriverImage "$repo/assets/release/ntvdm64.386" -InstallSource 'O:/w31' -InstallRoot $dest -OutputRoot $out
. "$repo/test/observation/isolated_package_cleanup.ps1"
$proc=$null
try {
 $enc=[Text.Encoding]::GetEncoding(28591)
 $ini=$enc.GetString([IO.File]::ReadAllBytes("$dest/SYSTEM.INI"))
 $ini=[regex]::Replace($ini,'(?im)^\s*;?\s*device\s*=[^\r\n]*(?:insignia|ntvdm64)\.386[^\r\n]*','')
 [IO.File]::WriteAllBytes("$dest/SYSTEM.INI",$enc.GetBytes($ini))
 [IO.File]::WriteAllText("$dest/PATCH/CONFIG.NT","dos=high,umb`r`ndevice=%SystemRoot%\system32\himem.sys`r`nfiles=128`r`n"+$(if($DosOnly){"dosonly`r`n"}else{''}),[Text.Encoding]::ASCII)
 & "$repo/tools/build-dos-identity.cmd" "$out/fixtures" $dest
 if($LASTEXITCODE){throw 'Identity guest fixture build failed'}
 foreach($role in 'A','B'){
  Copy-Item "$out/fixtures/$role.COM" "$dest/$role.COM"
  Copy-Item "$dest/DOSPRMPT.PIF" "$dest/ID$role.PIF"
  $show=& "$repo/assets/release/PIF.EXE" show "$dest/ID$role.PIF"
  $line=$show|Select-String '^W386.FLAGS=0x([0-9a-f]+)$'
  $flags=([Convert]::ToUInt32($line.Matches[0].Groups[1].Value,16) -band (-bnot 8)) -bor 2
  & "$repo/assets/release/PIF.EXE" update "$dest/ID$role.PIF" --title "Identity $role" --directory $dest --arguments '' --w386-flags $flags
  if($LASTEXITCODE){throw 'Child PIF failed'}
 }
 & "$repo/assets/release/PIF.EXE" update "$dest/PATCH/WIN.PIF" --arguments '/3'
 if($LASTEXITCODE){throw 'Windows PIF failed'}
 Copy-Item "$out/fixtures/controller.exe" "$out/controller.exe"
 $args=@('-NoProfile','-ExecutionPolicy','Bypass','-File',"$repo/test/integration/win31_runtime_matrix_test.ps1",'-PreparedProfile','-PackageRoot',$package,'-InstallRoot',$dest,'-Observer',"$repo/build/t481-s16/job/observer.exe",'-OutputRoot',"$out/run",'-WindowsProbeImage',"$out/controller.exe",'-Cases',"386-dosonly$DosOnly",'-TimeoutMs','120000')
 $proc=Start-Process $pwsh -ArgumentList $args -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/run.out" -RedirectStandardError "$out/run.err"
 $proc.Id|Set-Content "$out/runner.pid"
 function Wait-Marker([string]$name){
  $limit=[DateTime]::UtcNow.AddSeconds(100)
  while(!(Test-Path "$dest/PATCH/$name")){
   if($proc.HasExited){throw "Runner exited before $name"}
   if([DateTime]::UtcNow -gt $limit){throw "Deadline waiting for $name"}
   Start-Sleep -Milliseconds 100
  }
 }
 Wait-Marker 'B.RUN'
 foreach($role in 'A','B'){Copy-Item "$dest/PATCH/$role.RUN" "$out/$role.RUN"}
 Get-CimInstance Win32_Process | Where-Object {$_.Name -eq 'run16.exe'} | Select-Object ProcessId,ParentProcessId,CommandLine | ConvertTo-Json | Set-Content "$out/both-run16.json"
 Copy-Item "$diagnostic/events.log" "$out/both-events.log"
 [IO.File]::WriteAllText("$dest/PATCH/A.GO",'go')
 Wait-Marker 'A.END'
 Start-Sleep -Seconds 2
 Get-CimInstance Win32_Process | Where-Object {$_.Name -eq 'run16.exe'} | Select-Object ProcessId,ParentProcessId,CommandLine | ConvertTo-Json | Set-Content "$out/after-a-run16.json"
 Copy-Item "$diagnostic/events.log" "$out/after-a-events.log"
 [IO.File]::WriteAllText("$dest/PATCH/B.GO",'go')
 Wait-Marker 'B.END'
 Start-Sleep -Seconds 2
 Copy-Item "$diagnostic/events.log" "$out/after-b-events.log"
 foreach($role in 'A','B'){Copy-Item "$dest/PATCH/$role.END" "$out/$role.END"}
 [IO.File]::WriteAllText("$dest/PATCH/ALL.DONE",'done')
 if(!$proc.WaitForExit(30000)){throw 'Runner did not retire'}
 "runner_exit=$($proc.ExitCode)"|Set-Content "$out/result.txt"
} catch {
 $_|Out-String|Set-Content "$out/failure.txt"
 throw
} finally {
 if($proc -and !$proc.HasExited){
  # Do not remove a guest copy while its owning integration runner is active.
  [void]$proc.WaitForExit(140000)
 }
 if(!$proc -or $proc.HasExited){Remove-OwnedGuestProbeCopy $dest $out 'O:\w31'}
}
