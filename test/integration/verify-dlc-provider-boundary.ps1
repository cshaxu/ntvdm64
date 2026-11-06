param([string]$PackageRoot='build/t478-s27/package-final2',[string]$Output='build/t478-s27/guest-dlc',[string]$Observer='build/t478-s27/observer.exe')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path;Set-Location $repo
$out=[IO.Path]::GetFullPath((Join-Path $repo $Output))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Require fresh build-owned output'}
$null=New-Item -ItemType Directory -Force $out
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null;$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try{
 $source="$repo/tests/observation/vdmredir_dlc_probe.asm"
 & nasm -f bin -o "$out/VDMDLC.COM" $source > "$out/assemble.txt" 2>&1
 if($LASTEXITCODE){throw 'Existing independent probe assembly failed'}
 $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
 $scope=New-IsolatedPackageScope $runtime;$binary=Get-PackageBinaryRoot $runtime
 # Do not use this no-provider witness on a configured DLC host.
 foreach($p in @("$binary/DLCAPI.DLL","$env:WINDIR/SysWOW64/DLCAPI.DLL")){if(Test-Path $p){throw 'DLC provider present; no-provider assumption invalid'}}
 $tests=Join-Path $runtime 'tests';$null=New-Item -ItemType Directory -Force $tests
 Copy-Item "$out/VDMDLC.COM" "$tests/VDMDLC.COM"
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$binary;"+$env:SystemRoot+'\System32';$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path);$start.UseShellExecute=$false;$start.CreateNoWindow=$true
 foreach($arg in @("$binary/run16.exe",$tests,"$out/observer.txt",'--wait',"$tests/VDMDLC.COM",'--observation-timeout-ms','30000')){$start.ArgumentList.Add($arg)}
 $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
 if(!$process.WaitForExit(35000)){throw 'Owned DLC witness timeout'};$process.Refresh()
 $observation=Get-Content "$out/observer.txt" -Raw
 if($process.ExitCode -or $observation -notmatch 'result=exited' -or $observation -notmatch 'exit=0x00000000'){throw 'DLC witness did not exit successfully; inspect actual launcher status'}
 $console=[IO.File]::ReadAllText("$out/observer.txt.console.txt").Replace([string][char]0,'')
 if($console -notmatch 'DLC-REAL-RESULT=\s*07'){throw 'Expected original provider-unavailable result missing'}
 $inputs=@($source,"$out/VDMDLC.COM","$binary/VDMREDIR.DLL")|Get-FileHash|Select-Object Path,Hash
 @{result='pass';status='07';meaning='Original loader boundary only; no DLC worker or adapter execution';inputs=$inputs}|ConvertTo-Json -Depth 5|Set-Content "$out/result.json"
 'PASS real DOS DLC returns original unavailable-provider status 07'
}finally{
 if($process){if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(5000)};$process.Dispose()}
 try{if($scope){Stop-IsolatedPackageScope $scope}}finally{try{if($runtime){Remove-PhysicalRuntimeStage $runtime}}finally{foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}}}
}
