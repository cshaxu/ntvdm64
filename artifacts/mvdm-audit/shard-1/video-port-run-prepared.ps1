$ErrorActionPreference='Stop'
. (Join-Path (Get-Location) 'tests/observation/isolated_package_cleanup.ps1')
$root=(Resolve-Path 'build/t473-vport-audit').Path
$source=(Resolve-Path 'build/t473-s9/package-r10').Path
$observer=(Resolve-Path 'build/.ninja/console-observers-x64/console-startup-observer.exe').Path
$report=Join-Path $root 'observer2.txt'
if(Test-Path $report){throw 'Fresh report required'}
if(Get-Process ntsrv,ntvdm,ntvwm,ntcon,run16 -ErrorAction SilentlyContinue){throw 'Product process exists'}
$stage=$null;$scope=$null;$old=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
 $stage=New-PhysicalRuntimeStage $source
 $scope=New-IsolatedPackageScope $stage
 $bin=Get-PackageBinaryRoot $stage
 Copy-Item (Join-Path $root 'VPORT.COM') (Join-Path $stage 'VPORT.COM')
 $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
 & $observer (Join-Path $bin 'run16.exe') $stage $report '--observation-timeout-ms' '35000' '--observe-console-input-marker' 'Microsoft(R) Windows NT DOS' '--observe-console-input-escaped' 'VPORT\r' '--observe-console-input-next-marker' 'VPORT=' '--observe-console-input-next-escaped' 'exit\r' (Join-Path $bin 'COMMAND.COM')
 $result=$LASTEXITCODE
 "observer exit=$result"
 if(Test-Path $report){Get-Content $report}
 if(Test-Path ($report+'.console.txt')){Get-Content ($report+'.console.txt')}
 if($result){throw "observer failed $result"}
} finally { $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$old;if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage} }
