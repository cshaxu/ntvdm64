param([Parameter(Mandatory)][string]$PackageRoot,[Parameter(Mandatory)][string]$Fixture,
 [Parameter(Mandatory)][string]$BuildRoot,[switch]$Negative)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build root required'}
$null=New-Item -ItemType Directory $root
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null;$scope=$null
try {
 $stage=New-PhysicalRuntimeStage $PackageRoot
 $scope=New-IsolatedPackageScope $stage
 $bin=Get-PackageBinaryRoot $stage
 $mode=if($Negative){'negative'}else{'candidate'}
 $p=Start-Process $Fixture -ArgumentList @('product',('"'+$bin+'\run16.exe"'),('"'+$root+'\request"'),$mode) -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/stdout.txt" -RedirectStandardError "$root/stderr.txt"
 $null=$p.Handle
 @{pid=$p.Id;start=$p.StartTime;deadlineSeconds=70;stage=$stage;workerHash=(Get-FileHash "$bin/ntvwm.exe").Hash}|ConvertTo-Json|Set-Content "$root/owner.json"
 if(!$p.WaitForExit(70000)){$p.Kill($true);throw 'Product fixture deadline'}
 if($p.ExitCode -ne 0){throw "Product guard failed $($p.ExitCode)"}
 if(!(Select-String -Path "$root/request-summary.txt" -SimpleMatch 'PASS product' -Quiet)){throw 'Missing product evidence'}
 '0'|Set-Content "$root/exit.txt"
}catch{$_|Out-String|Set-Content "$root/failure.txt";'1'|Set-Content "$root/exit.txt";throw}
finally {if($scope){Stop-IsolatedPackageScope $scope};if($stage){Remove-PhysicalRuntimeStage $stage}}
