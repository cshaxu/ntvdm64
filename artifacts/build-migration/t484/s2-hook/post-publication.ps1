$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$r=@{pid=$PID;started=[DateTimeOffset]::UtcNow.ToString('o');exit=1}
try {
 if((Get-Content "$PSScriptRoot/delivery-result.json" -Raw|ConvertFrom-Json).exit){throw 'Publication failed'}
 & 'C:/Program Files/Python313/python.exe' tools/audit/verify-library-build-boundary.py
 if($LASTEXITCODE){throw 'Boundary failed'}
 & 'C:/Program Files/Python313/python.exe' test/build/test-authored-softpc-profile.py --component hook
 if($LASTEXITCODE){throw 'Hook profile failed'}
 $fixture="$PSScriptRoot/published-hook-fixture"
 New-Item -ItemType Directory $fixture | Out-Null
 foreach($name in @('nthook64.dll','nthook32.dll','run16.exe')){Copy-Item "assets/release/$name" "$fixture/$name"}
 foreach($name in @('nthook-install-test.exe','nthook-gui-test.exe','process-image-machine-test.exe')){Copy-Item "build/.ninja/msvc-x64-release/hook/$name" "$fixture/$name"}
 & "$fixture/process-image-machine-test.exe" C:/Windows/SysWOW64/cmd.exe C:/Windows/System32/cmd.exe
 if($LASTEXITCODE){throw 'Image-machine test failed'}
 & "$fixture/nthook-install-test.exe"
 if($LASTEXITCODE){throw 'Published Hook failed'}
 foreach($row in (Get-Content "$PSScriptRoot/candidate-final/product-system32-manifest.json" -Raw|ConvertFrom-Json)){
  foreach($destination in @('assets/release','O:/winnt/system32')){
   if((Get-FileHash "$destination/$($row.Name)").Hash -ne $row.Sha256){throw 'Published hash drift'}
  }
 }
 $r.exit=0
}catch{$r.error="$_";Write-Output $_}
$r.finished=[DateTimeOffset]::UtcNow.ToString('o')
$r|ConvertTo-Json|Set-Content "$PSScriptRoot/post-publication-result.json"
exit $r.exit
