[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [switch]$TraceHandoff
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=[IO.Path]::GetFullPath($BuildRoot)
if(!$build.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $build)){
    throw 'Require a fresh build-owned evidence root'
}
$observerPath=(Resolve-Path $Observer).Path
New-Item -ItemType Directory $build|Out-Null
& "$repo/tools/build-fixtures.cmd" x86 native-guest-output *> "$build/build.log"
if($LASTEXITCODE){throw 'Native output fixture compile failed'}
& "$repo/tools/prepare-test-inputs.cmd" $build "$repo/build/.ninja/fixtures-x86/native-guest-output.exe" CG.EXE "$repo/build/.ninja/fixtures-x86/native-guest-output.bat" CG.BAT
if($LASTEXITCODE){throw 'Native output input preparation failed'}
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null;$saved=@{}
foreach($name in 'PATH','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_S34_TRACE_PATH'){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
try {
    $stage=New-PhysicalRuntimeStage $PackageRoot -FreshPackageIdentity -TestInBinary -TestFiles @("$build/CG.EXE","$build/CG.BAT")
    $scope=New-IsolatedPackageScope $stage
    $binary=Get-PackageBinaryRoot $stage
    $tests=Join-Path $binary 'tests'
    Save-IsolatedPackageManifest $stage "$build/manifest.json"
    $report=Join-Path $build 'observer.txt'
    $env:PATH="$binary;$env:SystemRoot\System32;$env:SystemRoot"
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $env:MVDM_S34_TRACE_PATH=if($TraceHandoff){"$build/handoff.txt"}else{$null}
    & $observerPath "$binary/run16.exe" $binary $report --observation-timeout-ms 60000 "$tests/CG.EXE" "$build/native.txt"
    if($LASTEXITCODE){throw 'Observer failed'}
    $record=Get-Content $report -Raw
    $screen=Get-Content "$report.console.txt" -Raw
    if($record -notmatch '(?m)^result=exited\r?$' -or $record -notmatch '(?m)^exit=0x00000025\r?$'){
        throw 'Native/guest completion failed'
    }
    $text=($screen -replace '(?m)^\[\d+\] ','') -replace '\s',''
    foreach($marker in 'NATIVE-BASE','DOS-INTERVAL','NATIVE-DURING-DOS','DOS-RESUMED','NATIVE-AFTER-DOS','PASSREAL-GUEST-CONCURRENTresult=0'){
        if(!$text.Contains($marker)){throw "Missing actual visible output: $marker"}
    }
    'PASS real COMMAND batch plus concurrent native output; DOS=0 native=37'|Set-Content "$build/result.txt"
} finally {
    try {if($scope){Stop-IsolatedPackageScope $scope}} finally {
        try {
            if($stage){
                foreach($name in 'CGREADY','CGDONE'){
                    $path=Join-Path $tests $name
                    if(Test-Path $path){Copy-Item $path (Join-Path $build $name)}
                }
                Remove-PhysicalRuntimeStage $stage
            }
        } finally {
            foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
        }
    }
}
