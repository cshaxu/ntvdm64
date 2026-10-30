param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$IdleHost,
 [Parameter(Mandatory)][string]$IdleProbe,
 [Parameter(Mandatory)][string]$CbtProbe)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$base=(Resolve-Path -LiteralPath $PackageRoot).Path
New-Item -ItemType Directory -Path $root -ErrorAction Stop | Out-Null
. "$repo\tests\observation\isolated_package_cleanup.ps1"
$pairs=@(@($IdleHost,'8723024DF07355208DFE42D3251920A6687247A32CD65C9530A3FDEC2E533929'),@($IdleProbe,'D8A2B4374D399691F6B968F22F1C52AD51C67E6200D26DFA28A2AF64F34D0E71'),@($CbtProbe,'C133EEF5D498F91D2C06B80DB83DEA480129D2CECD24DFFC516141EBD8E4A7B6'))
foreach($pair in $pairs){if((Get-IsolatedFileSha256 $pair[0]) -ne $pair[1]){throw "Fixture hash mismatch: $($pair[0])"}}
$runtime=$null;$scope=$null;$process=$null
try {
 $runtime=New-PhysicalRuntimeStage $base -FreshPackageIdentity
 Copy-Item "$runtime\product-system32-manifest.json" "$root\manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 New-Item -ItemType Directory -Force "$runtime\tests"|Out-Null
 $names=@('IDLEHOST.EXE','IDLPROBE.EXE','CBTPROB.EXE')
 for($index=0;$index -lt $pairs.Count;$index++){
  Copy-Item -LiteralPath $pairs[$index][0] -Destination (Join-Path "$runtime\tests" $names[$index])
 }
 $scope.Paths+="$runtime\tests\IDLEHOST.EXE"
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path -LiteralPath $Observer).Path)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $start.EnvironmentVariables['TEMP']=$runtime;$start.EnvironmentVariables['TMP']=$runtime
 $start.EnvironmentVariables['PATH']="$runtime\system32;"+$env:SystemRoot+'\System32'
 $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $start.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$root\lifetime.txt"
 foreach($arg in @("$runtime\tests\IDLEHOST.EXE","$runtime\tests","$root\observer.txt","$runtime\tests\IDLPROBE.EXE","$runtime\tests\CBTPROB.EXE","$root\native.txt","$runtime\system32\ntvdm.exe",'--observation-timeout-ms','50000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start); $process.Id | Set-Content "$root\pid.txt"
 if(!$process.WaitForExit(55000)){throw 'Owned observer deadline'}
 if($process.ExitCode -ne 0 -or [IO.File]::ReadAllText("$root\observer.txt") -notmatch 'exit=0x00000000'){throw 'observer or host returned error'}
 $native=[IO.File]::ReadAllText("$root\native.txt")
 if($native -notmatch 'peer-exited registrar-still-running' -or $native -notmatch 'result=0 error=0'){throw 'missing task overlap/exit evidence'}
 foreach($name in @('IDLPROBE','CBTPROB')){
  $file="$runtime\tests\$name.BIN"; if(!(Test-Path $file)){$file="$runtime\$name.BIN"}
  if(!(Test-Path $file)){throw "missing test-owned $name checkpoint"};Copy-Item $file "$root\$name.bin"
 }
 $idle=[IO.File]::ReadAllBytes("$root\IDLPROBE.bin");$cbt=[IO.File]::ReadAllBytes("$root\CBTPROB.bin")
 if($idle.Length -lt 22 -or $idle.Length % 22 -or $cbt.Length -ne 600){throw 'invalid checkpoint sizes'}
 $i=$idle.Length-22
 if([Text.Encoding]::ASCII.GetString($idle,$i,4) -ne 'IDL1' -or [BitConverter]::ToUInt16($idle,$i+4) -ne 2 -or [BitConverter]::ToUInt16($idle,$i+6) -ne 0 -or [BitConverter]::ToUInt16($idle,$i+8) -eq 0 -or [BitConverter]::ToUInt16($idle,$i+10) -ne 16 -or [BitConverter]::ToUInt16($idle,$i+12) -ne 1){throw 'idle checkpoint mismatch'}
 $expected=@(@(0,0,0,0,0,0,0,0,0),@(1,0,2,2,1,0,2,0,0),@(2,0,3,3,1,1,2,0,0),@(4,0,4,4,1,1,3,1,1))
 for($r=0;$r -lt 4;$r++){
  $offset=$r*22
  if([Text.Encoding]::ASCII.GetString($cbt,$offset,4) -ne 'CBT1'){throw 'CBT record identity mismatch'}
  for($j=0;$j -lt 9;$j++){if([BitConverter]::ToUInt16($cbt,$offset+4+2*$j) -ne $expected[$r][$j]){throw "CBT checkpoint mismatch record=$r field=$j"}}
 }
 $a=[BitConverter]::ToUInt16($idle,$i+14)
 if(!$a){throw 'idle task identity invalid'}
 $trace=[IO.File]::ReadAllText("$root\lifetime.txt")
 $aPattern=('(?m)^(\d+) TaskRetirement .*task={0:X8} stage=complete status=0\r?$' -f $a)
 $aa=[regex]::Matches($trace,$aPattern)
 $all=[regex]::Matches($trace,'(?m)^(\d+) TaskRetirement .*task=([0-9A-Fa-f]{8}) stage=complete status=0\r?$')
 $bb=@($all|Where-Object {[Convert]::ToUInt32($_.Groups[2].Value,16) -ne $a})
 if($aa.Count -ne 1 -or $bb.Count -ne 1 -or $aa[0].Groups[1].Value -ne $bb[0].Groups[1].Value){throw 'missing distinct retirements in one worker'}
 $later=$trace.Substring($bb[0].Index)
 if($later -notmatch ('TaskRetirement .*task={0:X8} stage=idle-dispatch status=0' -f $a)){throw 'no surviving task idle after dialog task exit'}
 "PASS shared-worker=$($aa[0].Groups[1].Value) idle-task=$('{0:X4}' -f $a) cbt-task=$($bb[0].Groups[2].Value) nested=1 veto=1 unhook=1 later-idle=1" | Set-Content "$root\result.txt"
} finally {
 if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit()};$process.Dispose()}
 if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
