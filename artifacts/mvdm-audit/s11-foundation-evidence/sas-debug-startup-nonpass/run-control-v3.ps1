$ErrorActionPreference='Stop'
. O:/repos.hobby/ntvdm64/tests/observation/isolated_package_cleanup.ps1
$runtime=$null;$scope=$null
try {
 $runtime=New-PhysicalRuntimeStage 'O:/repos.hobby/ntvdm64/build/t473-s11/trace-scalar-package/package' -FreshPackageIdentity
 $scope=New-IsolatedPackageScope $runtime
 $env:TEMP=$runtime; $env:TMP=$runtime; $env:PATH="$runtime\system32;C:\Windows\System32"
 & O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/control-v2.exe $runtime *> O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/control-v3-result.txt
 $code=$LASTEXITCODE
 Get-Content O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/control-v3-result.txt
 exit $code
} finally { if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime} }
