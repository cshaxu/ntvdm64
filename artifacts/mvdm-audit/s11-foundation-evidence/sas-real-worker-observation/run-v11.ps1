$ErrorActionPreference='Stop'
. O:/repos.hobby/ntvdm64/tests/observation/isolated_package_cleanup.ps1
$runtime=$null;$scope=$null
try {
 $runtime=New-PhysicalRuntimeStage 'O:/repos.hobby/ntvdm64/build/t473-s11/trace-scalar-package/package' -FreshPackageIdentity
 $scope=New-IsolatedPackageScope $runtime
 Copy-Item "$runtime\product-system32-manifest.json" "O:\repos.hobby\ntvdm64\build\t473-s11\sas-real-startup\v11-manifest.json"
 Get-FileHash "$runtime\system32\ntvdm.exe","$runtime\system32\MEM.EXE" | Select-Object Path,Hash | Export-Csv "O:\repos.hobby\ntvdm64\build\t473-s11\sas-real-startup\v11-image-hashes.csv" -NoTypeInformation
 $env:TEMP=$runtime; $env:TMP=$runtime; $env:PATH="$runtime\system32;C:\Windows\System32"
 & O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/probe-v11.exe $runtime *> O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/result-v11.txt
 $code=$LASTEXITCODE
 Get-Content O:/repos.hobby/ntvdm64/build/t473-s11/sas-real-startup/result-v11.txt
 exit $code
} finally { if($scope){Stop-IsolatedPackageScope $scope};if($runtime){Remove-PhysicalRuntimeStage $runtime} }
