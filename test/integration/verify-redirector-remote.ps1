param(
 [Parameter(Mandatory)][string]$PackageRoot,
 [Parameter(Mandatory)][string]$BuildRoot,
 [Parameter(Mandatory)][string]$Observer,
 [uint32]$ExpectedEnumStatus
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Use a fresh repository-build evidence directory'}
$Observer=(Resolve-Path $Observer).Path
$null=New-Item -ItemType Directory $root
& "$repo/tools/build-fixtures.cmd" x86 redirector-server-enum *> "$root/provider-build.log"
if($LASTEXITCODE){throw 'Native remote provider build failed'}
$nativeImage="$repo/build/.ninja/msvc-x86-release/worker/remote-provider-test.exe"
$provider=Start-Process $nativeImage -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/provider.txt" -RedirectStandardError "$root/provider.err"
try {
 $null=$provider.Handle
 @{pid=$provider.Id;start=$provider.StartTime.ToUniversalTime().ToString('o');image=$nativeImage;deadlineSeconds=120}|ConvertTo-Json|Set-Content "$root/provider-process.json"
 if(!$provider.WaitForExit(120000)){$provider.Kill();$provider.WaitForExit();throw 'Native enumeration deadline'}
 $provider.WaitForExit()
 if($provider.ExitCode){throw 'Native remote provider contract failed'}
} finally {$provider.Dispose()}
$native=Get-Content "$root/provider.txt"
$status=[regex]::Match(($native -join "`n"),'(?m)^LOCAL_ENUM_STATUS=(\d+)\r?$')
if(!$status.Success){throw 'Native enumeration status missing'}
$actual=[uint32]$status.Groups[1].Value
if($PSBoundParameters.ContainsKey('ExpectedEnumStatus') -and $ExpectedEnumStatus -ne $actual){throw 'Requested enumeration status differs from current native provider'}
$ExpectedEnumStatus=$actual
& "$repo/tools/build-fixtures.cmd" x86 guest-remote $ExpectedEnumStatus
if($LASTEXITCODE){throw 'Remote probe build failed'}
$probe="$repo/build/.ninja/fixtures-x86/guest-probes/remote/RAPTEST.COM"
. "$PSScriptRoot/../observation/isolated_package_cleanup.ps1"
$runtime=New-PhysicalRuntimeStage $PackageRoot -TestFiles @($probe)
$scope=New-IsolatedPackageScope $runtime
$saved=@{}
foreach($n in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP')){$saved[$n]=[Environment]::GetEnvironmentVariable($n)}
try {
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
