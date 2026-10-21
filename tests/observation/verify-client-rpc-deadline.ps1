param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Fixture
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $root)){throw 'Fresh repository build evidence directory required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$service=$null;$process=$null
try {
    $runtime=New-PhysicalRuntimeStage (Resolve-Path $PackageRoot).Path
    $scope=New-IsolatedPackageScope $runtime
    $image="$runtime\system32\base-client-rpc-first-test.exe"
    Copy-Item -LiteralPath $Fixture -Destination $image
    $scope.Paths+=$image
    @($Fixture,$image,"$runtime\system32\ntsrv.exe")|Get-FileHash|
        Select-Object Path,Hash|ConvertTo-Json|Set-Content "$root/inputs.json"
    $service=Start-Process "$runtime\system32\ntsrv.exe" -WindowStyle Hidden -PassThru
    $null=$service.Handle
    # Bootstrap settling is not a readiness assertion: the real Connect below
    # must authenticate the service; no work is retried if it fails.
    Start-Sleep -Milliseconds 200
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    foreach($arg in @($image,$runtime,"$root\observer.txt",
        '--frontend-timeout-parent','--observation-timeout-ms','25000')){
        $start.ArgumentList.Add($arg)
    }
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    @{observer=$process.Id;service=$service.Id;start=[DateTime]::UtcNow}|ConvertTo-Json|Set-Content "$root/processes.json"
    if(!$process.WaitForExit(30000)){throw 'RPC deadline observer exceeded diagnostic envelope'}
    $report=Get-Content "$root/observer.txt" -Raw
    $screen=Get-Content "$root/observer.txt.console.txt" -Raw
    if($process.ExitCode -or $report -notmatch 'exit=0x00000000' -or
        $screen -notmatch 'PASS: real RPC timeout without service death or request replay'){
        throw 'Real RPC deadline fixture failed; retain report and Console assertions'
    }
    'PASS ten-second live-service RPC timeout and immediate local finalization/disconnect'|
        Set-Content "$root/result.txt"
} finally {
    if($process){
        if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(10000)}
        $process.Dispose()
    }
    if($scope){Stop-IsolatedPackageScope $scope}
    if($service){$service.Dispose()}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
