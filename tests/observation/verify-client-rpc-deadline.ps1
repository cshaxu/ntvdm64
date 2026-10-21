param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Fixture,
    [switch]$WorkerExit,
    [switch]$StalledService,
    [switch]$Startup,
    [ValidateSet(5,6,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37)][int]$Control,
    [string]$ThreadSnapshot
)
$ErrorActionPreference='Stop'
if(([int][bool]$WorkerExit+[int][bool]$StalledService+[int][bool]$Startup+[int][bool]$Control) -gt 1){throw 'Select one deadline scenario'}
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
    $serviceStart=[Diagnostics.ProcessStartInfo]::new("$runtime\system32\ntsrv.exe")
    $serviceStart.UseShellExecute=$false;$serviceStart.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    if($Control -eq 9){$serviceStart.EnvironmentVariables['NTVDM_BOOTSTRAP_TRACE']="$root\frontend-trace.txt"}
    $service=[Diagnostics.Process]::Start($serviceStart)
    $null=$service.Handle
    # Bootstrap settling is not a readiness assertion: the real Connect below
    # must authenticate the service; no work is retried if it fails.
    Start-Sleep -Milliseconds 200
    $start=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
    $start.UseShellExecute=$false;$start.CreateNoWindow=$true
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $mode=if($WorkerExit){'--worker-exit-timeout'}elseif($StalledService){'--finalization-service-timeout'}elseif($Startup){'--startup-service-timeout'}else{'--frontend-timeout-parent'}
    $expected=if($WorkerExit){'PASS: real worker exit RPC deadline without poisoning shared connection'}elseif($StalledService){'PASS: finalization deadline with stalled service and local disconnect'}elseif($Startup){'PASS: startup deadline with stalled service and no cleanup RPC'}else{'PASS: real RPC timeout without service death or request replay'}
    if($Control){
        $mode='--control-service-timeout'
        $expected=if($Control -eq 17){'PASS: ordinary disconnect deadline and idempotent local cleanup'}elseif($Control -eq 9){'PASS: actual NTCON timeout retirement and input restoration with service paused'}elseif($Control -eq 8){'PASS: management close RPC deadline without poisoning shared connection'}else{'PASS: frontend control deadline and local disconnect'}
    }
    $arguments=@($image,$runtime,"$root\observer.txt",$mode)
    if($Control){$arguments+=[string]$Control}
    $arguments+=@('--observation-timeout-ms','25000')
    foreach($arg in $arguments){
        $start.ArgumentList.Add($arg)
    }
    $process=[Diagnostics.Process]::Start($start);$null=$process.Handle
    @{observer=$process.Id;service=$service.Id;start=[DateTime]::UtcNow}|ConvertTo-Json|Set-Content "$root/processes.json"
    if($ThreadSnapshot){
        if($Control -ne 9){throw 'Thread diagnosis is limited to the actual frontend case'}
        Start-Sleep -Seconds 4
        $frontend=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -ieq "$runtime\system32\ntcon.exe"})
        if($frontend.Count -ne 1){throw 'Exact diagnostic frontend not found'}
        & $ThreadSnapshot ([string]$frontend[0].ProcessId) *> "$root/frontend-stacks.txt"
        if($LASTEXITCODE){throw 'Thread diagnostic failed'}
    }
    if(!$process.WaitForExit(30000)){throw 'RPC deadline observer exceeded diagnostic envelope'}
    $report=Get-Content "$root/observer.txt" -Raw
    $screen=Get-Content "$root/observer.txt.console.txt" -Raw
    if($process.ExitCode -or $report -notmatch 'exit=0x00000000' -or
        !$screen.Contains($expected)){
        throw 'Real RPC deadline fixture failed; retain report and Console assertions'
    }
    $expected|Set-Content "$root/result.txt"
} finally {
    if($process){
        if(!$process.HasExited){$process.Kill();$null=$process.WaitForExit(10000)}
        $process.Dispose()
    }
    if($scope){Stop-IsolatedPackageScope $scope}
    if($service){$service.Dispose()}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
}
