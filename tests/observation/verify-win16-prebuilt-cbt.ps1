param([Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [Parameter(Mandatory)][string]$PrebuiltProbe,
      [Parameter(Mandatory)][string]$Observer)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath($BuildRoot)
$probe=(Resolve-Path $PrebuiltProbe).Path
$observer=(Resolve-Path $Observer).Path
$package=(Resolve-Path $PackageRoot).Path
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned output required'}
if ((Get-FileHash $probe).Hash -ne 'C133EEF5D498F91D2C06B80DB83DEA480129D2CECD24DFFC516141EBD8E4A7B6') { throw 'Prebuilt guest identity mismatch' }
if (Test-Path $out) { throw 'Fresh output required' }
New-Item -ItemType Directory $out | Out-Null
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$runtime=$null; $scope=$null; $process=$null
try {
    $runtime=New-PhysicalRuntimeStage $package -FreshPackageIdentity
    Save-IsolatedPackageManifest $runtime "$out/manifest.json"
    $scope=New-IsolatedPackageScope $runtime
    New-Item -ItemType Directory -Force "$runtime/tests" | Out-Null
    Copy-Item $probe "$runtime/tests/CBTPROB.EXE"
    $start=[Diagnostics.ProcessStartInfo]::new($observer)
    $start.UseShellExecute=$false; $start.CreateNoWindow=$true
    $start.EnvironmentVariables['TEMP']=$runtime; $start.EnvironmentVariables['TMP']=$runtime
    $start.EnvironmentVariables['PATH']="$runtime/system32;"+$env:SystemRoot+'\System32'
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.EnvironmentVariables['MVDM_WOW_WINDOW_TRACE_PATH']="$out/windows.txt"
    foreach($arg in @("$runtime/system32/run16.exe",$runtime,"$out/observer.txt",'--wait',"$runtime/tests/CBTPROB.EXE",'--observation-timeout-ms','30000')) { $start.ArgumentList.Add($arg) }
    $process=[Diagnostics.Process]::Start($start)
    if(!$process.WaitForExit(40000)){throw 'Observer deadline'}
    if($process.ExitCode -or (Get-Content "$out/observer.txt" -Raw) -notmatch 'result=exited[\s\S]*exit=0x00000000'){throw 'Guest launcher failed'}
    $report="$runtime/tests/CBTPROB.BIN"
    if(!(Test-Path $report)){$report="$runtime/CBTPROB.BIN"}
    if(!(Test-Path $report)){throw 'Missing guest report'}
    Copy-Item $report "$out/report.bin"
    $data=[IO.File]::ReadAllBytes($report)
    $records=@()
    for($i=0;$i -le $data.Length-22;$i++) {
        if([Text.Encoding]::ASCII.GetString($data,$i,4) -ne 'CBT1'){continue}
        $v=@(); for($j=0;$j -lt 9;$j++){$v += [BitConverter]::ToUInt16($data,$i+4+2*$j)}
        $records+=,[pscustomobject]@{phase=$v[0];errors=$v[1];created=$v[2];queried=$v[3];nested=$v[4];vetoed=$v[5];destroyed=$v[6];unhooked=$v[7];freed=$v[8]}
        $i+=21
    }
    $records|ConvertTo-Json|Set-Content "$out/report.json"
    if($records.Count -ne 4 -or ($records.phase -join ',') -ne '0,1,2,4' -or @($records|Where-Object errors -ne 0).Count){throw 'Guest phase/error contract failed'}
    $end=$records[-1]
    if($records[1].created -ne 2 -or $records[1].queried -ne 2 -or $records[1].nested -ne 1 -or $records[1].vetoed -ne 0 -or $records[1].destroyed -ne 2 -or
       $records[2].created -ne 3 -or $records[2].queried -ne 3 -or $records[2].nested -ne 1 -or $records[2].vetoed -ne 1 -or $records[2].destroyed -ne 2){throw 'Guest intermediate hook contract failed'}
    if($end.created -ne 4 -or $end.queried -ne 4 -or $end.nested -ne 1 -or $end.vetoed -ne 1 -or $end.destroyed -ne 3 -or $end.unhooked -ne 1 -or $end.freed -ne 1){throw 'Guest final hook contract failed'}
    'PASS prebuilt Win16 CBT nested creation/veto/query/unhook and launcher exit'|Set-Content "$out/result.txt"
} finally {
    if($process){if(!$process.HasExited){$process.Kill();$process.WaitForExit()};$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
