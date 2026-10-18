param(
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot
)
# Pure native reference for verify-win16-native-modal-probe. Invoke as an
# owned hidden background run; never loads WOW/style or modifies a guest.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$null=New-Item -ItemType Directory $root
$observerProcess=$null;$peer=$null;$target=$null
try{
 Push-Location $root
 try{
  & $CompilerDriver cl.exe /nologo /MT /O2 /W4 /WX "/Fo$root/control.obj" "/Fe$root/native-modal-control.exe" "$PSScriptRoot/native_modal_control.c" /link /MACHINE:X86 user32.lib shell32.lib *> "$root/build.txt"
  if($LASTEXITCODE){throw 'Native control build failed'}
  & $CompilerDriver cl.exe /nologo /MT /O2 /W4 /WX "/Fo$root/peer.obj" "/Fe$root/peer.exe" "$PSScriptRoot/wow_native_modal_probe.c" /link /MACHINE:X86 user32.lib *> "$root/peer-build.txt"
  if($LASTEXITCODE){throw 'Native peer build failed'}
 }finally{Pop-Location}
 Get-FileHash $PSCommandPath,"$PSScriptRoot/native_modal_control.c","$PSScriptRoot/wow_native_modal_probe.c","$root/native-modal-control.exe","$root/peer.exe",$Observer|Export-Csv -NoTypeInformation "$root/input-sha256.csv"
 $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $info.UseShellExecute=$false;$info.CreateNoWindow=$true;$info.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 foreach($arg in @("$root/native-modal-control.exe",$root,"$root/observer.txt",'--observation-timeout-ms','60000')){$info.ArgumentList.Add($arg)}
 $observerProcess=[Diagnostics.Process]::Start($info);$null=$observerProcess.Handle
 @{pid=$observerProcess.Id;start=$observerProcess.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList);timeoutSeconds=70}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 $deadline=[DateTime]::UtcNow.AddSeconds(10)
 do{
  $target=Get-CimInstance Win32_Process -Filter "Name='native-modal-control.exe'"|Where-Object {$_.ExecutablePath -eq "$root\native-modal-control.exe"}|Select-Object -First 1
  if($target -or $observerProcess.HasExited){break};Start-Sleep -Milliseconds 50
 }while([DateTime]::UtcNow -lt $deadline)
 if(!$target){throw 'Native target not observed'}
 $target|Select-Object ProcessId,CreationDate,ExecutablePath|ConvertTo-Json|Set-Content "$root/target.json"
 $line=([string]$target.ProcessId)+' "NTVDMConsoleTest-'+$observerProcess.Id+'"'
 $peer=Start-Process "$root/peer.exe" -ArgumentList $line -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/native.stdout.txt" -RedirectStandardError "$root/native.stderr.txt"
 $null=$peer.Handle
 @{pid=$peer.Id;start=$peer.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=65}|ConvertTo-Json|Set-Content "$root/native-process.json"
 if(!$peer.WaitForExit(65000)){throw 'Native peer deadline'};$peer.WaitForExit();$peer.ExitCode|Set-Content "$root/native-exit.txt"
 if(!$observerProcess.WaitForExit(70000)){throw 'Native observer deadline'};$observerProcess.WaitForExit();$observerProcess.ExitCode|Set-Content "$root/observer-exit.txt"
 $report=Get-Content "$root/observer.txt" -Raw
 $console=Get-Content "$root/observer.txt.console.txt" -Raw
 if($peer.ExitCode -or $observerProcess.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000' -or $console -notmatch 'NATIVE_CONTROL MessageBox=1' -or $console -notmatch 'NATIVE_CONTROL ShellAbout=1'){throw 'Native control failed actual modal return/completion'}
 'PASS pure native USER MessageBox=1 ShellAbout=1 normal exit; no WOW/style adapter'|Set-Content "$root/result.txt"
 0|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";1|Set-Content "$root/exit.txt";exit 1}
finally{
 foreach($p in @($peer,$observerProcess)){if($p){$null=$p.Handle;if(!$p.HasExited){$p.Kill();$p.WaitForExit(5000)|Out-Null};$p.Dispose()}}
 if($target){$p=Get-Process -Id $target.ProcessId -ErrorAction SilentlyContinue;if($p -and [Math]::Abs(($p.StartTime.ToUniversalTime()-$target.CreationDate.ToUniversalTime()).TotalSeconds) -lt 1){$p.Kill();$p.WaitForExit(5000)|Out-Null}}
}
