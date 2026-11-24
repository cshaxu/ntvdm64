[CmdletBinding()]
param(
 [ValidateSet(0,1)][int]$DosOnly,
 [ValidatePattern('^[A-Za-z0-9_-]+$')][string]$Run='a',
 [Parameter(Mandatory)][string]$BaselineRoot,
 [Parameter(Mandatory)][string]$Observer
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$base=Join-Path $repo 'build/side-dos-identity'
$out=Join-Path $base "dos$DosOnly-$Run"
$dest="O:\tmp\i31$DosOnly"
$pwsh=(Get-Process -Id $PID).Path
$BaselineRoot=(Resolve-Path -LiteralPath $BaselineRoot).Path
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$package=Join-Path $out 'package'
$cache="$repo/build/.ninja/msvc-x86-release/worker"
$diagnostic="$cache/diagnostics/dos-identity/ntvdm.exe"
$copy=& "$repo/test/integration/ntvdm64-386/New-Win31ProbeCopy.ps1" -DriverImage "$repo/assets/release/ntvdm64.386" -InstallSource 'O:/w31' -InstallRoot $dest -OutputRoot $out
. "$repo/test/observation/isolated_package_cleanup.ps1"
$proc=$null
$savedLog=$env:MVDM_DOS_IDENTITY_LOG
try {
 & "$repo/tools/build-fixtures.cmd" x86 dos-identity-observer *> "$out/diagnostic-build.log"
 if($LASTEXITCODE){throw 'Diagnostic worker build failed'}
 & "$repo/tools/stage-system32.cmd" -BaselineRoot $BaselineRoot -BuildCache $cache -Wow32 "$repo/build/.ninja/msvc-x86-release/wow32/wow32.dll" -OutputRoot $package -NativeHook "$cache/nthook32.dll" -NativeHook64 "$repo/assets/release/nthook64.dll" -NativeWorker "$cache/ntvwm.exe" -NativeFrontend "$cache/ntcon.exe" -NativeMonitor "$cache/ntmon.exe" -NativeLauncher "$cache/run16.exe" -NativeService "$cache/ntsrv.exe" -DiagnosticNtvdm $diagnostic *> "$out/diagnostic-stage.log"
 if($LASTEXITCODE){throw 'Diagnostic package stage failed'}
 & "$repo/tools/prepare-dos-identity.cmd" $dest $out $DosOnly *> "$out/guest-prepare.log"
 if($LASTEXITCODE){throw 'Identity guest preparation failed'}
 $log="$dest\PATCH\DOSID.LOG"
 if(Test-Path -LiteralPath $log){throw 'Diagnostic output already exists in the copied installation'}
 $env:MVDM_DOS_IDENTITY_LOG=$log
 $args=@('-NoProfile','-ExecutionPolicy','Bypass','-File',"$repo/test/integration/win31_runtime_matrix_test.ps1",'-PreparedProfile','-PackageRoot',$package,'-InstallRoot',$dest,'-Observer',$Observer,'-OutputRoot',"$out/run",'-WindowsProbeImage',"$out/fixtures/controller.exe",'-Cases',"386-dosonly$DosOnly",'-TimeoutMs','120000')
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
 function Save-Run16Snapshot([string]$Name){
  $inputRecord=Get-Content -LiteralPath "$out/run/386-dosonly$DosOnly.inputs.json" -Raw|ConvertFrom-Json
  $image=Join-Path $inputRecord.stage 'system32\run16.exe'
  $rows=@(Get-CimInstance Win32_Process -Filter "Name='run16.exe'" |
   Where-Object {$_.ExecutablePath -ieq $image} |
   Select-Object ProcessId,ParentProcessId,CommandLine)
  ConvertTo-Json -InputObject $rows|Set-Content "$out/$Name"
 }
 Wait-Marker 'B.RUN'
 foreach($role in 'A','B'){Copy-Item "$dest/PATCH/$role.RUN" "$out/$role.RUN"}
 Save-Run16Snapshot 'both-run16.json'
 Copy-Item -LiteralPath $log "$out/both-events.log"
 [IO.File]::WriteAllText("$dest/PATCH/A.GO",'go')
 Wait-Marker 'A.END'
 Start-Sleep -Seconds 2
 Save-Run16Snapshot 'after-a-run16.json'
 Copy-Item -LiteralPath $log "$out/after-a-events.log"
 [IO.File]::WriteAllText("$dest/PATCH/B.GO",'go')
 Wait-Marker 'B.END'
 Start-Sleep -Seconds 2
 Copy-Item -LiteralPath $log "$out/after-b-events.log"
 foreach($role in 'A','B'){Copy-Item "$dest/PATCH/$role.END" "$out/$role.END"}
 [IO.File]::WriteAllText("$dest/PATCH/ALL.DONE",'done')
 if(!$proc.WaitForExit(30000)){throw 'Runner did not retire'}
 "runner_exit=$($proc.ExitCode)"|Set-Content "$out/result.txt"
} catch {
 $_|Out-String|Set-Content "$out/failure.txt"
 throw
} finally {
 $env:MVDM_DOS_IDENTITY_LOG=$savedLog
 if($proc -and !$proc.HasExited){
  # Do not remove a guest copy while its owning integration runner is active.
  [void]$proc.WaitForExit(140000)
 }
 if(!$proc -or $proc.HasExited){Remove-OwnedGuestProbeCopy $dest $out $copy.Source}
}
