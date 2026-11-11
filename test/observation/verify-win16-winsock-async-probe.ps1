param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Nasm,
 [ValidateSet('Completion','Cancel','TaskLoss','AsyncSelect','AsyncSelectTaskLoss')][string]$Mode='Completion'
)
# Run only from an owned hidden supervisor.  The real Win16 witness proves the
# original WSOCK32 post routine crosses the WOW completion conversion exactly
# once for its task-owned USER window.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 $defines=@();if($Mode -eq 'Cancel'){$defines+='-dCANCEL_REQUEST=1'}elseif($Mode -eq 'TaskLoss'){$defines+='-dTASK_LOSS=1'}elseif($Mode -eq 'AsyncSelect'){$defines+='-dASYNC_SELECT=1'}elseif($Mode -eq 'AsyncSelectTaskLoss'){$defines+='-dASYNC_SELECT=1';$defines+='-dASYNC_SELECT_TASK_LOSS=1'}
 & $Nasm @defines -f bin -o "$root/WSKASYNC.EXE" -l "$root/probe.lst" "$PSScriptRoot/win16_winsock_async_probe.asm" *> "$root/assemble.txt"
 if($LASTEXITCODE){throw 'Independent async Winsock guest probe assembly failed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/WSKASYNC.EXE" "$tests/WSKASYNC.EXE"
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime/system32;"+$env:SystemRoot+'\System32';$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/WSKASYNC.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
 if(!$process.WaitForExit(45000)){throw 'Observer timeout'};$process.WaitForExit();$process.Refresh()
 Copy-Item "$tests/WSKASYNC.BIN" "$root/guest-report.bin"
 $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 $recordLength=24
 if($bytes.Length -lt $recordLength -or ($bytes.Length % $recordLength)){throw 'Invalid async Winsock guest report length'}
 $bytes=$bytes[($bytes.Length-$recordLength)..($bytes.Length-1)]
 if([Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'WSA1'){throw 'Invalid async Winsock guest report'}
 $names=@('phase','errors','startup','task','completion','freed','cancel','select','disabled','closed');$row=[ordered]@{}
 for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,4+2*$i)}
 $row|ConvertTo-Json|Set-Content "$root/guest-report.json"
 if($process.ExitCode -or (Get-Content "$root/observer.txt" -Raw) -notmatch 'result=exited' -or $row.errors -or $row.startup -ne 0 -or ($Mode -notin @('AsyncSelect','AsyncSelectTaskLoss') -and !$row.task)){throw 'Async Winsock common witness failed'}
 if($Mode -eq 'Completion' -and ($row.phase -ne 2 -or $row.completion -ne 1 -or !$row.freed -or $row.cancel)){throw 'Async Winsock completion witness failed'}
 if($Mode -eq 'Cancel' -and ($row.phase -ne 2 -or $row.completion -or !$row.freed -or $row.cancel -ne 1)){throw 'Async Winsock cancellation witness failed'}
 if($Mode -eq 'TaskLoss' -and ($row.phase -ne 3 -or $row.completion -or $row.freed -or $row.cancel)){throw 'Async Winsock task-loss witness failed'}
 if($Mode -eq 'AsyncSelect' -and ($row.phase -ne 2 -or $row.completion -ne 1 -or !$row.select -or !$row.disabled -or !$row.closed)){throw 'Win16 WSAAsyncSelect witness failed'}
 if($Mode -eq 'AsyncSelectTaskLoss' -and ($row.phase -ne 3 -or $row.completion -or !$row.select -or $row.disabled -or $row.closed)){throw 'Win16 WSAAsyncSelect task-loss witness failed'}
 "PASS real Win16 Winsock async $Mode witness"|Set-Content "$root/result.txt"
 '0'|Set-Content "$root/result.exit"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/result.exit";throw}
finally {
 if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
 try {if($runtime -and (Test-Path "$runtime/tests/WSKASYNC.BIN")){Copy-Item "$runtime/tests/WSKASYNC.BIN" "$root/guest-report.bin" -Force}}
 finally {try {if($scope){Stop-IsolatedPackageScope $scope}}finally {try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}}}
}
