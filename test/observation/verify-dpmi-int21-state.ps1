param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$Observer)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
$package=(Resolve-Path $PackageRoot).Path
$observer=(Resolve-Path $Observer).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$results=@()
foreach($mode in @('low','high')) {
 $probe="$out/$mode.com"
 $asmArgs=@('-f','bin',"$PSScriptRoot/dpmi_int21_state.asm",'-o',$probe)
 if($mode -eq 'high'){$asmArgs=@('-DHIGH_BUFFER=1')+$asmArgs}
 & nasm @asmArgs
 if($LASTEXITCODE){throw "Probe compilation failed: $mode"}
 foreach($route in @('direct','nested')) {
  $runtime=$null;$scope=$null;$p=$null
  $case="$out/$mode-$route"
  New-Item -ItemType Directory $case|Out-Null
  try {
   $runtime=New-PhysicalRuntimeStage $package -FreshPackageIdentity
   Save-IsolatedPackageManifest $runtime "$case/manifest.json"
   $scope=New-IsolatedPackageScope $runtime
   New-Item -ItemType Directory -Force "$runtime/tests"|Out-Null
   Copy-Item $probe "$runtime/tests/D38F.COM"
   if(Test-Path "$runtime/tests/D38IO.TMP"){throw 'Probe file must initially be absent'}
   $start=[Diagnostics.ProcessStartInfo]::new($observer)
   $start.UseShellExecute=$false;$start.CreateNoWindow=$true
   $start.WorkingDirectory=$runtime
   $start.EnvironmentVariables['PATH']="$runtime\system32;"+$env:SystemRoot+'\System32'
   $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
   $start.EnvironmentVariables['MVDM_OBSERVER_MILESTONE_INPUT']='1'
   $start.EnvironmentVariables['MVDM_OBSERVER_SHORT_HISTORY']='1'
   $guestArgs=@('tests\D38F.COM')
   if($route -eq 'nested'){$guestArgs=@('COMMAND.COM','/c','COMMAND.COM','/c')+$guestArgs}
   foreach($arg in (@("$runtime\system32\run16.exe",$runtime,"$case/guest.txt")+$guestArgs+@('--observation-timeout-ms','25000'))){$start.ArgumentList.Add($arg)}
   $watch=[Diagnostics.Stopwatch]::StartNew()
   $p=[Diagnostics.Process]::Start($start)
   if(!$p.WaitForExit(35000)){throw 'Observer timeout'}
   $watch.Stop()
   if($p.ExitCode -or (Get-Content "$case/guest.txt" -Raw) -notmatch 'result=exited[\s\S]*exit=0x00000000'){throw "Guest did not exit zero: $mode/$route"}
   $screen=Get-Content "$case/guest.txt.console.txt" -Raw
   if($screen -notmatch 'S38_INT21_IO_STATE_FAILURE_CLEANUP_OK' -or $screen -match 'S38_INT21_FAIL'){throw "State/data check failed: $mode/$route"}
   if($mode -eq 'high' -and $screen -notmatch 'S38_INT21_HIGH_BUFFER_CHUNKS_EOF_CLEANUP_OK'){throw 'High buffer marker missing'}
   if(Test-Path "$runtime/tests/D38IO.TMP"){throw 'Guest failed to delete its file'}
   $results+=@{Mode=$mode;Route=$route;Pid=$p.Id;ElapsedMs=$watch.ElapsedMilliseconds;ProbeSha256=(Get-FileHash $probe).Hash;Result='pass'}
   $results|ConvertTo-Json -Depth 4|Set-Content "$out/results.json"
   Write-Output "PASS INT21 state/data/cleanup: $mode/$route"
  } finally {
   if($p){if(!$p.HasExited){$p.Kill();$p.WaitForExit()};$p.Dispose()}
   if($scope){Stop-IsolatedPackageScope $scope}
   if($runtime){Remove-PhysicalRuntimeStage $runtime}
  }
 }
}
