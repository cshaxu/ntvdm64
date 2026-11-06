param([string]$PackageRoot,[string]$OutputRoot,[string]$Observer='build/t478-s22/observer.exe')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build evidence directory'}
$null=New-Item -ItemType Directory $out
& nasm -f bin test/integration/wow_kerning_guest.asm -o "$out/KERNTEST.EXE"
if($LASTEXITCODE){throw 'NE fixture assembly failed'}
$runtime=$null;$scope=$null;$proc=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path -FreshPackageIdentity
 $scope=New-IsolatedPackageScope $runtime
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory $tests -Force
 Copy-Item "$out/KERNTEST.EXE" $tests
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$(Get-PackageBinaryRoot $runtime);$env:SystemRoot\System32";$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $log="$out/observer.txt"
 foreach($arg in @((Join-Path (Get-PackageBinaryRoot $runtime) 'run16.exe'),$tests,$log,'--wait',"$tests/KERNTEST.EXE",'--observation-timeout-ms','30000')){$start.ArgumentList.Add($arg)}
 $proc=[Diagnostics.Process]::Start($start);$null=$proc.Handle
 if(!$proc.WaitForExit(45000)){throw 'Owned observer timeout'}
 $result="$tests/KERNTEST.BIN"
 if(!(Test-Path $result)){throw 'Missing guest report'}
 Copy-Item $result "$out/report.bin"
 $b=[IO.File]::ReadAllBytes($result)
 if($b.Length -ne 12 -or [Text.Encoding]::ASCII.GetString($b,0,4) -ne 'KRN1'){throw 'Invalid guest record'}
 $row=@{errors=[BitConverter]::ToUInt16($b,4);count=[BitConverter]::ToUInt16($b,6);requested=[BitConverter]::ToUInt16($b,8);returned=[BitConverter]::ToUInt16($b,10);package=(Get-FileHash "$PackageRoot/product-system32-manifest.json").Hash;wow=(Get-FileHash (Join-Path (Get-PackageBinaryRoot $runtime) 'WOW32.DLL')).Hash}
 $row|ConvertTo-Json|Set-Content "$out/result.json";$row|ConvertTo-Json
 if($row.errors -or !$row.count -or $row.returned -ne $row.count -or $row.requested -ne $row.count+3){throw 'Guest kerning output failed'}
 $logText=Get-Content $log -Raw
 if($proc.ExitCode -or $logText -notmatch '(?m)^result=exited\r?$' -or $logText -notmatch '(?m)^exit=0x00000000\r?$'){throw 'Guest failed ordinary exit'}
} finally {
 foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
 if($proc){$proc.Dispose()}
 try {if($scope){Stop-IsolatedPackageScope $scope}}finally{if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
