param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Nasm,
 [ValidateSet('Original','Fixed')][string]$Expectation='Fixed'
)
# Run only from an owned hidden background supervisor.  The probe is a real
# Win16 NE program; it uses only loopback UDP and the archived WINSOCK.DLL.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 & $Nasm -f bin -o "$root/WSKPROBE.EXE" -l "$root/probe.lst" "$PSScriptRoot/win16_winsock_probe.asm" *> "$root/assemble.txt"
 if($LASTEXITCODE){throw 'Independent Winsock guest probe assembly failed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/WSKPROBE.EXE" "$tests/WSKPROBE.EXE"
 $inputs=@(@{file='probe-source';sha256=(Get-FileHash "$PSScriptRoot/win16_winsock_probe.asm").Hash},@{file='probe';sha256=(Get-FileHash "$root/WSKPROBE.EXE").Hash})
 foreach($name in @('run16.exe','ntvdm.exe','WOW32.DLL','KRNL386.EXE','USER.EXE','WINSOCK.DLL')){$inputs+=@{file="system32/$name";sha256=(Get-FileHash "$runtime/system32/$name").Hash}}
 $inputs|ConvertTo-Json|Set-Content "$root/inputs.json"
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime/system32;"+$env:SystemRoot+'\System32';$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/WSKPROBE.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');command=@($start.ArgumentList);deadlineSeconds=45;expectation=$Expectation}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 if(!$process.WaitForExit(45000)){throw 'Observer timeout'};$process.WaitForExit();$process.Refresh()
 Copy-Item "$tests/WSKPROBE.BIN" "$root/guest-report.bin"
 $allBytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 # The independently authored guest witness checkpoints before startup,
 # before the blocking receive, and at teardown.  It intentionally appends
 # fixed-size WSK1 records, so validate every record and assert the final
 # lifecycle state rather than pretending this is a one-write report.
 if(!$allBytes.Length -or ($allBytes.Length % 28)){throw 'Invalid Winsock guest report length'}
 for($offset=0;$offset -lt $allBytes.Length;$offset+=28){if([Text.Encoding]::ASCII.GetString($allBytes,$offset,4) -ne 'WSK1'){throw 'Invalid Winsock guest report record'}}
 $bytes=$allBytes[($allBytes.Length-28)..($allBytes.Length-1)]
 $row=[ordered]@{};$names=@('phase','errors','startup','calls','cancelled','cancel_error','selected','restored','closed','freed','cleaned','task')
 for($i=0;$i -lt $names.Count;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,4+2*$i)}
 $row|ConvertTo-Json|Set-Content "$root/guest-report.json"
 if($process.ExitCode -or (Get-Content "$root/observer.txt" -Raw) -notmatch 'result=exited' -or $row.startup -ne 0 -or !$row.calls -or $row.cancelled -ne 1 -or $row.cancel_error -ne 10004 -or $row.selected -ne 0 -or !$row.task){throw 'Common real guest Winsock assertions failed'}
 if($Expectation -eq 'Original'){
  if($row.phase -ne 1 -or $row.errors -ne 1 -or $row.restored -or $row.closed -or $row.freed -or $row.cleaned){throw 'Original unhook rejection baseline changed'}
 } elseif($row.phase -ne 2 -or $row.errors -or !$row.restored -or !$row.closed -or !$row.freed -or !$row.cleaned){throw 'Fixed unhook/teardown assertions failed'}
 "PASS real Win16 Winsock $Expectation blocking-hook cancellation and unhook witness"|Set-Content "$root/result.txt"
 '0'|Set-Content "$root/result.exit"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/result.exit";throw}
finally {
 if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit(5000)|Out-Null};$process.Dispose()}
 try {if($runtime -and (Test-Path "$runtime/tests/WSKPROBE.BIN")){Copy-Item "$runtime/tests/WSKPROBE.BIN" "$root/guest-report.bin" -Force}}
 finally {try {if($scope){Stop-IsolatedPackageScope $scope}}finally {try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}}}
}
