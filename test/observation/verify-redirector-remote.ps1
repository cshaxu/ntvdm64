param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [Parameter(Mandatory)][uint32]$ExpectedEnumStatus
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Use a fresh repository-build evidence directory'}
$Observer=(Resolve-Path $Observer).Path
$null=New-Item -ItemType Directory $root
& nasm -f bin "-DEXPECT_ENUM_STATUS=$ExpectedEnumStatus" -o "$root/RAPTEST.COM" "$PSScriptRoot/vdmredir_remote_unavailable.asm"
if($LASTEXITCODE){throw 'Probe assembly failed'}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $PackageRoot
$scope=New-IsolatedPackageScope $runtime
$saved=@{}
foreach($n in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
try {
 $null=New-Item -ItemType Directory "$runtime/tests" -Force
 Copy-Item "$root/RAPTEST.COM" "$runtime/tests"
 $env:PATH="$runtime/system32;"+$env:SystemRoot+'\System32'
 $env:TEMP=$runtime;$env:TMP=$runtime;$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 & $Observer "$runtime/system32/run16.exe" "$runtime/tests" "$root/observer.txt" --wait "$runtime/tests/RAPTEST.COM" --observation-timeout-ms 120000
 if($LASTEXITCODE){throw 'Observer failed'}
 $report=Get-Content "$root/observer.txt" -Raw
 $screen=Get-Content "$root/observer.txt.console.txt" -Raw
 if($report -notmatch 'exit=0x00000000' -or $screen -notmatch 'RAP-UNAVAILABLE-OK'){throw "RAP probe failed: $screen"}
 'PASS actual DOS remote/transact/null-transact failure, enum counts and local workstation provider' | Set-Content "$root/result.txt"
 Get-Content "$root/result.txt"
} finally {
 try {Stop-IsolatedPackageScope $scope;Remove-PhysicalRuntimeStage $runtime}
 finally {foreach($n in $saved.Keys){[Environment]::SetEnvironmentVariable($n,$saved[$n])}}
}
