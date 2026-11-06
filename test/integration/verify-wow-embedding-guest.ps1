param([string]$PackageRoot,[string]$OutputRoot,[string]$Observer='build/t478-s24/observer.exe',[switch]$ExpectStall)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build evidence directory'}
$null=New-Item -ItemType Directory $out
& nasm -f bin test/integration/wow_embedding_guest.asm -o "$out/EMBTEST.EXE"
if($LASTEXITCODE){throw 'NE fixture assembly failed'}
$runtime=$null;$scope=$null;$proc=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 if(Test-Path (Join-Path (Get-PackageBinaryRoot $runtime) 'NTVDM.REG')){throw 'Require absent shadow file: no classes, hence no native profile writes'}
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$out/EMBTEST.EXE" $tests
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log="$out/observer.txt"
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/EMBTEST.EXE",'--observation-timeout-ms','30000')){$start.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 if(!$proc.WaitForExit(45000)){throw 'Owned observer timeout'}
 $result="$tests/EMBTEST.BIN"
 if(!(Test-Path $result)){throw 'Missing guest report'}
 Copy-Item $result "$out/report.bin"
 $b=[IO.File]::ReadAllBytes("$out/report.bin")
 if($b.Length -notin @(12,24) -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'EMB1'){throw 'Invalid guest record'}
 $last=$b.Length-12
 $row=@{phase=[BitConverter]::ToUInt16($b,$last+4);errors=[BitConverter]::ToUInt16($b,$last+6);returned=[BitConverter]::ToUInt16($b,$last+8);registryBytes=[BitConverter]::ToUInt16($b,$last+10);recordBytes=$b.Length;expectedStall=[bool]$ExpectStall;package=(Get-FileHash "$PackageRoot/product-system32-manifest.json").Hash;wow=(Get-FileHash (Join-Path (Get-PackageBinaryRoot $runtime) 'WOW32.DLL')).Hash;worker=(Get-FileHash (Join-Path (Get-PackageBinaryRoot $runtime) 'ntvdm.exe')).Hash}
 $row|ConvertTo-Json|Set-Content "$out/result.json";$row|ConvertTo-Json
 $logText=Get-Content $log -Raw
 if($ExpectStall){
  if($row.phase -ne 1 -or $row.errors -or $b.Length -ne 12 -or $logText -notmatch '(?m)^result=timeout\r?$'){throw 'Expected pre-fix enumeration stall was not observed'}
 } else {
  if($row.registryBytes -ne 6 -or $row.phase -ne 2 -or $row.errors -or $b.Length -ne 24 -or $proc.ExitCode -or $logText -notmatch '(?m)^result=exited\r?$' -or $logText -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Embedding guest did not complete'}
 }

} finally {
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally{if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
