[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$RuntimeRoot='O:\winnt',
    [string]$Observer
)
$ErrorActionPreference='Stop'
$repository=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith($repository+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)){
    throw 'Require a fresh build-owned evidence directory'
}
$package=(Resolve-Path $RuntimeRoot).Path
New-Item -ItemType Directory $build|Out-Null
& "$repository/tools/build-fixtures.cmd" x86 command-keyboard-shadow *> "$build/build.log"
if($LASTEXITCODE){throw 'Keyboard fixtures failed to build'}
$reader=Join-Path $build 'system32/s45-cmdkeyb-shadow-fixture.exe'
if(!$Observer){$Observer="$repository/build/.ninja/fixtures-x64/console-startup-observer.exe"}
$observerPath=(Resolve-Path $Observer).Path
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null;$saved=@{}
foreach($name in 'PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','NTVDM_SHADOW_REGISTRY_TRACE'){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
try {
    $stage=New-PhysicalRuntimeStage $package -FreshPackageIdentity
    $scope=New-IsolatedPackageScope $stage
    $binary=Get-PackageBinaryRoot $stage
    $tests=Join-Path $binary 'KTEST'
    & "$repository/tools/prepare-keyboard-test.cmd" $stage $build
    if($LASTEXITCODE){throw 'CMD keyboard test preparation failed'}
    $trace=Join-Path $build 'registry.log'
    $report=Join-Path $build 'observer.txt'
    $env:PATH="$binary;$env:SystemRoot\System32;$env:SystemRoot"
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:NTVDM_SHADOW_REGISTRY_TRACE=$trace
    & $observerPath (Join-Path $binary 'run16.exe') $binary $report --wait (Join-Path $tests 'S45KEY.PIF') --observation-timeout-ms 30000
    if($LASTEXITCODE){throw 'Keyboard guest observer failed'}
    $screen=Get-Content "$report.console.txt" -Raw
    # The native Console layout query may legitimately fail: original COMMAND
    # then takes NoInstallkb16. Do not require a registry hit from this run.
    # Deterministic original-reader outcomes are checked separately below.
    if($screen -notmatch 'bytes total conventional mem' -or $screen -match 'Bad command or filename'){
        throw 'Keyboard PIF route lacks MEM output'
    }
    $env:NTVDM_SHADOW_REGISTRY_TRACE="$build/reader.trace"
    & $reader *> "$build/reader.txt"
    if($LASTEXITCODE){throw 'Original keyboard reader fixture failed'}
    $readerText=Get-Content "$build/reader.trace" -Raw
    foreach($marker in 'QUERY_OK','QUERY_MALFORMED','QUERY_ABSENT'){
        if($readerText -notmatch $marker){throw "Missing reader result: $marker"}
    }
    'S45_COMMAND_KEYBOARD_LAYERED_SHADOW_OK'|Set-Content "$build/result.txt"
} finally {
    try {if($scope){Stop-IsolatedPackageScope $scope}} finally {
        try {if($stage){Remove-PhysicalRuntimeStage $stage}} finally {
            foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
        }
    }
}
