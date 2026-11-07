$ErrorActionPreference='Stop'
$r='O:/repos.hobby/ntvdm64';$d="$r/build/t479-s6-livecmd"
Add-Type -Path "$r/build/t479-s6-integration/logon-env.dll"
$who=[Security.Principal.WindowsIdentity]::GetCurrent()
try{$entries=[WowMessageLogonEnvironment]::Read($who.Token)}finally{$who.Dispose()}
$info=[Diagnostics.ProcessStartInfo]::new("$d/rootwinenv.exe");$info.UseShellExecute=$false;$info.CreateNoWindow=$true
$info.EnvironmentVariables.Clear()
foreach($entry in $entries){$at=$entry.IndexOf('=',1);$info.EnvironmentVariables[$entry.Substring(0,$at)]=$entry.Substring($at+1)}
$info.EnvironmentVariables['PATH']='O:\tmp\n403\system32;'+$info.EnvironmentVariables['PATH']
$info.ArgumentList.Add('O:\tmp\n403');$info.ArgumentList.Add("$d/control-root190.txt")
$info.EnvironmentVariables['MVDM_MOUSE_CALLBACK_TRACE']='O:/repos.hobby/ntvdm64/build/t479-s6-livecmd/mouse190.txt'
$info.EnvironmentVariables['MVDM_S42_VXD_BOP_REPORT_PATH']='O:\repos.hobby\ntvdm64\build\t479-s6-livecmd\niddb190.txt'
$info.ArgumentList.Add("O:\tmp\w479m\PATCH\WIN386.CMD")
$p=[Diagnostics.Process]::Start($info)
[IO.File]::WriteAllText("$d/control-root190.pid",[string]$p.Id)
. "$r/tests/observation/inspect-session-windows.ps1" -ProcessIds @(0) -BuildRoot "$d/inspect190"
$ids=@($p.Id);$capture=0;$debugStarted=$false
while(!$p.WaitForExit(500)){
 $rows=@(Get-CimInstance Win32_Process)
 for($i=0;$i -lt 9;$i++){foreach($row in $rows){if($row.ParentProcessId -in $ids -and $row.ProcessId -notin $ids){$ids+=$row.ProcessId}}}
 foreach($front in @($rows | Where-Object {$_.ProcessId -in $ids -and $_.ExecutablePath -eq 'O:\tmp\n403\system32\ntcon.exe'})){ if($capture -lt 80){$capture++; & "$d/surface.exe" $front.ProcessId Default "$d/control190-frame-$capture.bmp" | Out-Null}}
 $ids=@($rows | Where-Object {$_.ExecutablePath -like 'O:\tmp\n403\system32\*'} | ForEach-Object {[int]$_.ProcessId})
 if(!$debugStarted){$worker=$rows | Where-Object {$_.ExecutablePath -eq 'O:\tmp\n403\system32\ntvdm.exe'} | Select-Object -First 1;if($worker){$observer=Start-Process "$d/attach-observer.exe" -ArgumentList @([string]$worker.ProcessId,"$d/control190-native.txt") -WindowStyle Hidden -PassThru;$debugStarted=$true;[IO.File]::WriteAllText("$d/control190-debug.pid",[string]$observer.Id)}}
 $v=[WindowInspector]::Read([int[]]$ids,'',$true,$false,$false)
 if($v){[IO.File]::AppendAllText("$d/control-root190.windows.txt",[DateTime]::UtcNow.ToString('o')+"`n"+$v)}
}
[IO.File]::WriteAllText("$d/control-root190.exit",[string]$p.ExitCode)