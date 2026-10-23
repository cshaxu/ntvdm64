[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [ValidateSet('NoDelete','NoPressure','Deleted')][string]$Case='NoDelete'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Require a fresh repository-build evidence directory'
}
$observerPath=(Resolve-Path $Observer).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
New-Item -ItemType Directory -Path $out | Out-Null
$options=@('-f','bin')
if($Case -eq 'NoDelete'){$options+='-DNO_DELETE'}
if($Case -eq 'NoPressure'){$options+='-DNO_PRESSURE'}
& nasm @options "$PSScriptRoot/t448_dem_delete_probe.asm" -o "$out/DEMRESET.COM"
if($LASTEXITCODE){throw 'DEM probe assembly failed'}
$runtime=$null;$scope=$null;$process=$null
$names=@('PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INITIAL_GEOMETRY')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    $runtime=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $runtime
    Copy-Item "$out/DEMRESET.COM" "$runtime/DEMRESET.COM"
    $env:PATH="$runtime/system32;"+(Join-Path $env:SystemRoot 'System32')
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_INITIAL_GEOMETRY='80,30'
    $arguments=@("$runtime/system32/run16.exe",$runtime,"$out/report.txt","$runtime/DEMRESET.COM",'--observation-timeout-ms','60000')
    $process=Start-Process -FilePath $observerPath -ArgumentList $arguments -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    if(!$process.WaitForExit(75000)){ $process.Kill();$process.WaitForExit();throw 'Observer timeout' }
    $report=Get-Content "$out/report.txt" -Raw
    $screen=Get-Content "$out/report.txt.console.txt" -Raw
    if($Case -eq 'Deleted') {
        if($process.ExitCode -ne 0 -or $report -notmatch 'exit=0x00000000' -or $screen -notmatch 'DEM-RESET-CONTINUED') {
            throw 'Deleted-anchor continuation failed'
        }
        'PASS DOS deleted-anchor continuation' | Set-Content "$out/result.txt"
    } else {
        if($process.ExitCode -ne 0 -or $report -notmatch 'exit=0x00000000' -or $screen -notmatch 'DEM-RESET-CONTINUED') {
            throw 'DOS directory continuation control failed'
        }
        "PASS DOS directory continuation case=$Case" | Set-Content "$out/result.txt"
    }
    Get-Content "$out/result.txt"
} finally {
    if($process){$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
