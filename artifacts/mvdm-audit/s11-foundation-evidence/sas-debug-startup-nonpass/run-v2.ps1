$ErrorActionPreference='Stop'
. O:/repos.hobby/ntvdm64/tests/observation/isolated_package_cleanup.ps1
$runtime=$null;$scope=$null
try {
 $runtime=New-PhysicalRuntimeStage 'O:/repos.hobby/ntvdm64/build/t473-s11/trace-scalar-package/package'
 $scope=New-IsolatedPackageScope $runtime
 & O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/probe-v2.exe $runtime *> O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/result-v2.txt
 $code=$LASTEXITCODE
 Get-Content O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/result-v2.txt
 exit $code
} finally { if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime} }
