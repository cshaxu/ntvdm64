param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$CompilerDriver,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Nasm
)
# Run only from an owned hidden background supervisor.
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null;$peer=$null
$saved=@{};foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 & $Nasm -f bin -o "$root/DISCOVER.EXE" -l "$root/probe.lst" "$PSScriptRoot/win16_discovery_probe.asm" *> "$root/assemble.txt"
 if($LASTEXITCODE){throw 'Independent guest probe assembly failed'}
 Push-Location $root
 try {
  & $CompilerDriver cl.exe /nologo /O2 /MT /W4 /WX /D_CRT_SECURE_NO_WARNINGS "/Fo$root/peer.obj" "/Fe$root/peer.exe" "$PSScriptRoot/wow_discovery_peer.c" /link user32.lib *> "$root/compile.txt"
  if($LASTEXITCODE){throw 'Native peer build failed'}
 }finally{Pop-Location}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$root/DISCOVER.EXE" "$tests/DISCOVER.EXE"
 $inputs=@(@{file='probe-source';sha256=(Get-FileHash "$PSScriptRoot/win16_discovery_probe.asm").Hash},@{file='native-peer-source';sha256=(Get-FileHash "$PSScriptRoot/wow_discovery_peer.c").Hash},@{file='probe';sha256=(Get-FileHash "$root/DISCOVER.EXE").Hash})
 foreach($name in @('run16.exe','ntvdm.exe','WOW32.DLL','KRNL386.EXE','USER.EXE')){$inputs+=@{file="system32/$name";sha256=(Get-FileHash "$runtime/system32/$name").Hash}}
 $inputs|ConvertTo-Json|Set-Content "$root/inputs.json"
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$runtime/system32;"+$env:SystemRoot+'\System32';$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 foreach($arg in @("$runtime/system32/run16.exe",$tests,"$root/observer.txt",'--wait',"$tests/DISCOVER.EXE",'--observation-timeout-ms','40000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
 @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');command=@($start.ArgumentList);deadlineSeconds=45}|ConvertTo-Json|Set-Content "$root/observer-process.json"
 $peer=Start-Process "$root/peer.exe" -ArgumentList ('"NTVDMConsoleTest-'+$process.Id+'" "'+$tests+'"') -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/peer.txt" -RedirectStandardError "$root/peer.err"
 $null=$peer.Handle
 @{pid=$peer.Id;start=$peer.StartTime.ToUniversalTime().ToString('o');command='peer.exe';desktop="NTVDMConsoleTest-$($process.Id)";directory=$tests;deadlineSeconds=35}|ConvertTo-Json|Set-Content "$root/peer-process.json"
 if(!$peer.WaitForExit(35000)){throw 'Native peer timeout'};$peer.WaitForExit();$peer.Refresh()
 if(!$process.WaitForExit(45000)){throw 'Observer timeout'};$process.WaitForExit();$process.Refresh()
 if($peer.ExitCode){throw "Native peer failed $($peer.ExitCode)"}
 Copy-Item "$tests/RESULT.BIN" "$root/guest-report.bin"
 $bytes=[IO.File]::ReadAllBytes("$root/guest-report.bin")
 if($bytes.Length -ne 14 -or [Text.Encoding]::ASCII.GetString($bytes,0,4) -ne 'DSC1'){throw 'Invalid guest report'}
 $row=[ordered]@{};$names=@('errors','found','matched','queries','freed')
 for($i=0;$i -lt 5;$i++){$row[$names[$i]]=[BitConverter]::ToUInt16($bytes,4+2*$i)}
 $row|ConvertTo-Json|Set-Content "$root/guest-report.json"
 if($row.errors -or !$row.found -or $row.matched -ne 1 -or $row.queries -ne 2 -or $row.freed -ne 1){throw 'Guest discovery/query assertions failed'}
 $report=Get-Content "$root/observer.txt" -Raw
 if($process.ExitCode -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000'){throw 'Guest/launcher did not exit normally'}
 'PASS real Win16 FindWindow, EnumWindows callback/direct class queries, early stop and cleanup'|Set-Content "$root/result.txt"
 '0'|Set-Content "$root/result.exit"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/result.exit";throw}
finally {
 foreach($child in @($peer,$process)){if($child){if(!$child.HasExited){$child.Kill();$child.WaitForExit(5000)|Out-Null};$child.Dispose()}}
 try {if($runtime -and (Test-Path "$runtime/tests/RESULT.BIN")){Copy-Item "$runtime/tests/RESULT.BIN" "$root/guest-report.bin" -Force}}
 finally {try {if($scope){Stop-IsolatedPackageScope $scope}}finally {try {if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally {foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}}}
}
