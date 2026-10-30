param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$PrebuiltRoot,
      [Parameter(Mandatory)][string]$Observer)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
$pre=(Resolve-Path $PrebuiltRoot).Path
$package=(Resolve-Path $PackageRoot).Path
$observer=(Resolve-Path $Observer).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
$expected=@{'D36T.COM'='E7C6398A4CE40639E6491B44B0FE47CD733A2E9CCFB24EBC155FCF060A9FE7A1';'D36N.COM'='9A1574AD8A537D901C6A0C390682D2F60872BD678D6893013B1F934DD2AD4DAD';'D36L.COM'='698DA97B3C254472F4C74CA17258FCED21AE04FC66657B66D9CBD0A131E2C5BD'}
foreach($n in $expected.Keys){if((Get-FileHash "$pre/$n").Hash -ne $expected[$n]){throw "Guest identity mismatch $n"}}
if(Test-Path $out){throw 'Fresh output required'}
New-Item -ItemType Directory $out|Out-Null
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$p=$null
try {
 $runtime=New-PhysicalRuntimeStage $package -FreshPackageIdentity
 Save-IsolatedPackageManifest $runtime "$out/manifest.json"
 $scope=New-IsolatedPackageScope $runtime
 New-Item -ItemType Directory -Force "$runtime/tests"|Out-Null
 Copy-Item "$pre/D36T.COM" "$runtime/tests/S6PROBE.COM"
 Copy-Item "$pre/D36N.COM","$pre/D36L.COM" "$runtime/tests"
 $start=[Diagnostics.ProcessStartInfo]::new($observer)
 $start.UseShellExecute=$false;$start.CreateNoWindow=$true
 $start.EnvironmentVariables['PATH']="$runtime\system32;"+$env:SystemRoot+'\System32'
 $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
 $start.EnvironmentVariables['MVDM_OBSERVER_MILESTONE_INPUT']='1'
 $start.EnvironmentVariables['MVDM_OBSERVER_SHORT_HISTORY']='1'
 foreach($arg in @("$runtime\system32\run16.exe",$runtime,"$out/guest.txt",'COMMAND.COM','/c',"$runtime\tests\S6PROBE.COM",'--observation-timeout-ms','25000')){$start.ArgumentList.Add($arg)}
 $p=[Diagnostics.Process]::Start($start)
 if(!$p.WaitForExit(35000)){throw 'Observer timeout'}
 if($p.ExitCode -or (Get-Content "$out/guest.txt" -Raw) -notmatch 'result=exited[\s\S]*exit=0x00000000'){throw 'Guest launcher did not exit zero'}
 $screen=Get-Content "$out/guest.txt.console.txt" -Raw
 if($screen -notmatch 'S36_DPMI_ALLOC_REALLOC_DATA_FREE_OK' -or
    ([regex]::Matches($screen,'S36_DPMI_EXIT_WITH_LIVE_ALLOCATION')).Count -ne 4 -or
    $screen -notmatch 'S36_DPMI_TASK_EXIT_CAPACITY_RESTORED_OK' -or
    $screen -match 'S36_DPMI_(TASK_EXIT_)?FAIL'){throw 'DPMI cleanup milestone missing or failed'}
 'PASS prebuilt DPMI task allocation exit and capacity restoration'|Set-Content "$out/result.txt"
} finally {
 if($p){if(!$p.HasExited){$p.Kill();$p.WaitForExit()};$p.Dispose()}
 if($scope){Stop-IsolatedPackageScope $scope}
 if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
