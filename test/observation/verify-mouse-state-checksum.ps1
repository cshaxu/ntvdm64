[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot
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
& nasm -f bin "$PSScriptRoot/t448_mouse_checksum_probe.asm" -o "$out/MSTATE.COM"
if($LASTEXITCODE){throw 'Mouse probe assembly failed'}
$runtime=$null;$scope=$null;$process=$null
$names=@('PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_INITIAL_GEOMETRY')
$saved=@{};foreach($name in $names){$saved[$name]=[Environment]::GetEnvironmentVariable($name)}
try {
    $runtime=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $runtime
    Copy-Item "$out/MSTATE.COM" "$runtime/MSTATE.COM"
    $env:PATH="$runtime/system32;"+(Join-Path $env:SystemRoot 'System32')
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_OBSERVER_INITIAL_GEOMETRY='80,30'
    $arguments=@("$runtime/system32/run16.exe",$runtime,"$out/report.txt","$runtime/MSTATE.COM",'--observation-timeout-ms','60000')
    $process=Start-Process -FilePath $observerPath -ArgumentList $arguments -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    if(!$process.WaitForExit(75000)){ $process.Kill();$process.WaitForExit();throw 'Observer timeout' }
    $report=Get-Content "$out/report.txt" -Raw
    $screen=Get-Content "$out/report.txt.console.txt" -Raw
    $checksum=[regex]::Match($screen,'SUM=([0-9A-F]{2}) SAVED=([0-9A-F]{2})')
    if($process.ExitCode -ne 0 -or $report -notmatch 'result=exited' -or $report -notmatch 'exit=0x00000000' -or
       !$checksum.Success -or $checksum.Groups[1].Value -ne $checksum.Groups[2].Value -or
       $screen -notmatch 'MOUSE-STATE-PASS') { throw 'Mouse save/restore witness failed' }
    'PASS actual INT33 state save/restore, invalid checksum and invalid cookie' | Set-Content "$out/result.txt"
    Get-Content "$out/result.txt"
} finally {
    if($process){$process.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($runtime){Remove-PhysicalRuntimeStage $runtime}
    foreach($name in $names){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
}
