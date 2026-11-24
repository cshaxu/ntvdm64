[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$RuntimeRoot,
    [string]$Observer = "$PSScriptRoot/../../build/.ninja/fixtures-x64/console-startup-observer.exe"
)
Set-StrictMode -Version Latest
$ErrorActionPreference='Stop'
$repository=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith($repository+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)){throw 'Fresh build-owned evidence root required'}
$null=New-Item -ItemType Directory $build
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$runtime=$null;$scope=$null;$process=$null
$oldDesktop=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    & "$repository/tools/build-fixtures.cmd" x64 serial-port-mask *> "$build/serial-mask-build.log"
    if($LASTEXITCODE){throw 'Serial host oracle build failed'}
    $maskText=& "$repository/build/.ninja/fixtures-x64/serial-port-mask.exe"
    if($LASTEXITCODE -or $maskText -notmatch '^\d+$'){throw 'Serial namespace query failed'}
    $mask=[uint32]$maskText
    if($mask -gt 511){throw 'Serial mask outside COM1-COM9'}
    & "$repository/tools/build-fixtures.cmd" x86 serial-discovery $mask *> "$build/serial-build.log"
    if($LASTEXITCODE){throw 'Serial guest probe build failed'}
    $probe="$repository/build/.ninja/fixtures-x86/guest-probes/serial-discovery/S45VCD.COM"
    Get-FileHash $probe,"$repository/build/.ninja/fixtures-x64/serial-port-mask.exe",$PSCommandPath,$Observer|Export-Csv -NoTypeInformation "$build/inputs.csv"
    $runtime=New-PhysicalRuntimeStage $RuntimeRoot -FreshPackageIdentity
    $scope=New-IsolatedPackageScope $runtime
    Save-IsolatedPackageManifest $runtime "$build/manifest.json"
    $bin=Get-PackageBinaryRoot $runtime
    $null=New-Item -ItemType Directory "$runtime/tests" -Force
    Copy-Item $probe "$runtime/tests/S45VCD.COM"
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $route=0
    foreach($arguments in @(@('tests\S45VCD.COM'),@('COMMAND.COM','/c','COMMAND.COM','/c','tests\S45VCD.COM'))){
        ++$route
        $info=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $Observer).Path)
        $info.UseShellExecute=$false;$info.CreateNoWindow=$true
        foreach($arg in @("$bin/run16.exe",$runtime,"$build/observer-$route.txt",'--wait')+$arguments+@('--observation-timeout-ms','30000')){$info.ArgumentList.Add($arg)}
        $process=[Diagnostics.Process]::Start($info);$null=$process.Handle
        @{pid=$process.Id;start=$process.StartTime.ToUniversalTime().ToString('o');arguments=@($info.ArgumentList)}|ConvertTo-Json|Set-Content "$build/route-$route.json"
        if(!$process.WaitForExit(40000)){throw "Serial route $route deadline"}
        $process.WaitForExit()
        if($process.ExitCode -or (Get-Content "$build/observer-$route.txt" -Raw) -notmatch 'exit=0x00000000'){throw "Serial route $route failed"}
        $process.Dispose();$process=$null
    }
    @{mask=$mask;routes=$route;result='pass'}|ConvertTo-Json|Set-Content "$build/result.json"
} finally {
    if($process){if(!$process.HasExited){$process.Kill($true);$process.WaitForExit()};$process.Dispose()}
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldDesktop
    try{if($scope){Stop-IsolatedPackageScope $scope}}finally{if($runtime){Remove-PhysicalRuntimeStage $runtime}}
}
