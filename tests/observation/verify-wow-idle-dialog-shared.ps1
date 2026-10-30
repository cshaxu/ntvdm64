param([Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][string]$IdleHost,
 [Parameter(Mandatory)][string]$IdleProbe,
 [Parameter(Mandatory)][string]$DialogProbe)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned root required'}
$base=(Resolve-Path -LiteralPath $PackageRoot).Path
New-Item -ItemType Directory -Path $root -ErrorAction Stop | Out-Null
. "$repo\tests\observation\isolated_package_cleanup.ps1"
$pairs=@(@($IdleHost,'8723024DF07355208DFE42D3251920A6687247A32CD65C9530A3FDEC2E533929'),@($IdleProbe,'D8A2B4374D399691F6B968F22F1C52AD51C67E6200D26DFA28A2AF64F34D0E71'),@($DialogProbe,'406D84F9E4EA04CD269045B354E95C47A342C986E0400A2C4F6A58DC818D3A02'))
foreach($pair in $pairs){if((Get-IsolatedFileSha256 $pair[0]) -ne $pair[1]){throw "Fixture hash mismatch: $($pair[0])"}}
$runtime=$null;$scope=$null;$process=$null
try {
 $runtime=New-PhysicalRuntimeStage $base -FreshPackageIdentity
 Copy-Item "$runtime\product-system32-manifest.json" "$root\manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 New-Item -ItemType Directory -Force "$runtime\tests"|Out-Null
 $names=@('IDLEHOST.EXE','IDLPROBE.EXE','CDLPROBE.EXE')
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
 foreach($arg in @("$runtime\tests\IDLEHOST.EXE","$runtime\tests","$root\observer.txt","$runtime\tests\IDLPROBE.EXE","$runtime\tests\CDLPROBE.EXE","$root\native.txt","$runtime\system32\ntvdm.exe",'--observation-timeout-ms','50000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start); $process.Id | Set-Content "$root\pid.txt"
 if(!$process.WaitForExit(55000)){throw 'Owned observer deadline'}
 if($process.ExitCode -ne 0 -or [IO.File]::ReadAllText("$root\observer.txt") -notmatch 'exit=0x00000000'){throw 'observer or host returned error'}
 $native=[IO.File]::ReadAllText("$root\native.txt")
 if($native -notmatch 'peer-exited registrar-still-running' -or $native -notmatch 'result=0 error=0'){throw 'missing task overlap/exit evidence'}
 foreach($name in @('IDLPROBE','CDLPROBE')){
  $file="$runtime\tests\$name.BIN"; if(!(Test-Path $file)){$file="$runtime\$name.BIN"}
  if(!(Test-Path $file)){throw "missing test-owned $name checkpoint"};Copy-Item $file "$root\$name.bin"
 }
 $idle=[IO.File]::ReadAllBytes("$root\IDLPROBE.bin");$cd=[IO.File]::ReadAllBytes("$root\CDLPROBE.bin")
 if($idle.Length -lt 22 -or $cd.Length -lt 24 -or $idle.Length % 22 -or $cd.Length % 24){throw 'invalid checkpoint sizes'}
 $i=$idle.Length-22;$c=$cd.Length-24
 if([Text.Encoding]::ASCII.GetString($idle,$i,4) -ne 'IDL1' -or [BitConverter]::ToUInt16($idle,$i+4) -ne 2 -or [BitConverter]::ToUInt16($idle,$i+6) -ne 0 -or [BitConverter]::ToUInt16($idle,$i+8) -eq 0 -or [BitConverter]::ToUInt16($idle,$i+10) -ne 16 -or [BitConverter]::ToUInt16($idle,$i+12) -ne 1){throw 'idle checkpoint mismatch'}
 if([Text.Encoding]::ASCII.GetString($cd,$c,4) -ne 'CDL2' -or [BitConverter]::ToUInt16($cd,$c+4) -ne 2 -or [BitConverter]::ToUInt16($cd,$c+6) -ne 0 -or [BitConverter]::ToUInt16($cd,$c+8) -ne 17 -or [BitConverter]::ToUInt16($cd,$c+10) -ne 16 -or [BitConverter]::ToUInt16($cd,$c+12) -ne 17 -or [BitConverter]::ToUInt16($cd,$c+14) -ne 1){throw 'dialog checkpoint mismatch'}
 $a=[BitConverter]::ToUInt16($idle,$i+14);$b=[BitConverter]::ToUInt16($cd,$c+16)
 if(!$a -or !$b -or $a -eq $b){throw 'task identities invalid'}
 $trace=[IO.File]::ReadAllText("$root\lifetime.txt")
 $aPattern=('(?m)^(\d+) TaskRetirement .*task={0:X8} stage=complete status=0\r?$' -f $a)
 $bPattern=('(?m)^(\d+) TaskRetirement .*task={0:X8} stage=complete status=0\r?$' -f $b)
 $aa=[regex]::Matches($trace,$aPattern);$bb=[regex]::Matches($trace,$bPattern)
 if($aa.Count -ne 1 -or $bb.Count -ne 1 -or $aa[0].Groups[1].Value -ne $bb[0].Groups[1].Value){throw 'missing distinct retirements in one worker'}
 $later=$trace.Substring($bb[0].Index)
 if($later -notmatch ('TaskRetirement .*task={0:X8} stage=idle-dispatch status=0' -f $a)){throw 'no surviving task idle after dialog task exit'}
 "PASS shared-worker=$($aa[0].Groups[1].Value) idle-task=$('{0:X4}' -f $a) dialog-task=$('{0:X4}' -f $b) dialogs=17 closed=16 later-idle=1" | Set-Content "$root\result.txt"
} finally {
 if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit()};$process.Dispose()}
 if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
