[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [ValidateSet('Boundaries','Existing')][string]$Case='Boundaries'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Require a fresh repository-build evidence directory'
}
$observerPath=(Resolve-Path $Observer).Path
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
New-Item -ItemType Directory -Path $out | Out-Null
$source=if($Case -eq 'Boundaries'){'xms_block_boundaries.asm'}else{'xms_capability.asm'}
$defines=@(); if($Case -eq 'Existing'){$defines+= '-DDEFAULT_INT15'}
& nasm @defines -f bin "$PSScriptRoot/$source" -o "$out/XMSCASE.COM"
if($LASTEXITCODE){throw 'XMS probe assembly failed'}
$runtime=$null;$scope=$null;$process=$null
$names=@('PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INITIAL_GEOMETRY')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    $runtime=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $runtime
    New-Item -ItemType Directory "$runtime/tests" -Force | Out-Null
    Copy-Item "$out/XMSCASE.COM" "$runtime/tests/XMSCASE.COM"
    $env:PATH="$runtime/system32;"+(Join-Path $env:SystemRoot 'System32')
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_INITIAL_GEOMETRY='80,30'
    $arguments=@("$runtime/system32/run16.exe","$runtime/tests","$out/report.txt",'--wait',"$runtime/tests/XMSCASE.COM",'--observation-timeout-ms','60000')
    $process=Start-Process -FilePath $observerPath -ArgumentList $arguments -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    if(!$process.WaitForExit(75000)){ $process.Kill();$process.WaitForExit();throw 'Observer timeout' }
    $report=Get-Content "$out/report.txt" -Raw
    $screen=Get-Content "$out/report.txt.console.txt" -Raw
    if($process.ExitCode -ne 0 -or $report -notmatch 'exit=0x00000000') {throw 'Guest XMS case failed'}
    if($Case -eq 'Boundaries' -and $screen -notmatch 'XMS-BOUNDARIES-PASS'){throw 'Missing boundary assertions'}
    if($Case -eq 'Existing' -and $screen -notmatch 'S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK'){throw 'Missing existing XMS assertions'}
    "PASS guest XMS case=$Case" | Set-Content "$out/result.txt"
    Get-Content "$out/result.txt"
} finally {
    if($process){$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
