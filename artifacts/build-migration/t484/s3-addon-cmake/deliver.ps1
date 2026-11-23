$ErrorActionPreference='Stop'
Set-Location O:/repos.hobby/ntvdm64
$result=@{pid=$PID;started=[DateTimeOffset]::UtcNow.ToString('o');exit=1}
try {
 if((Get-Content "$PSScriptRoot/product-build-result.json" -Raw|ConvertFrom-Json).exit){throw 'Full cached build failed'}
 & 'C:/Program Files/Python313/python.exe' "$PSScriptRoot/compare.py"
 if($LASTEXITCODE){throw 'Addon byte comparison failed'}
 $rows=Get-Content assets/release/release-manifest.json -Raw|ConvertFrom-Json
 if($rows.Count -ne 11){throw 'Missing host images'}
 $paths=@{'WOW32.DLL'='wow32/wow32.dll';'ntvdm.exe'='worker/ntvdm.exe';'VCDEX.DLL'='worker/VCDEX.DLL';'VDMREDIR.DLL'='worker/VDMREDIR.dll';'nthook32.dll'='worker/nthook32.dll'}
 foreach($row in $rows){
  if($paths.ContainsKey($row.Name)){$file='build/.ninja/msvc-x86-release/'+$paths[$row.Name]}
  else{$pair=@{'ntvwm.exe'='worker';'ntcon.exe'='frontend';'ntmon.exe'='monitor';'run16.exe'='launcher';'ntsrv.exe'='service';'nthook64.dll'='hook'};$file='build/.ninja/msvc-x64-release/'+$pair[$row.Name]+'/'+$row.Name}
  if((Get-FileHash $file).Hash -ne $row.InputSha256){throw "Raw host changed: $file"}
  foreach($dst in @('assets/release','O:/winnt/system32')){if((Get-FileHash "$dst/$($row.Name)").Hash -ne $row.Sha256){throw 'Published host drift'}}
 }
 $args=@{}
 foreach($id in @('pif','hash','grp','lnk','szdd','patch31','path31','patch95','path95')){$args[$id+'Build']="$PWD/build/.ninja/addon/$id"}
 & tools/build/Stage-AddonUtilityRelease.ps1 @args
 $addons=Get-Content assets/release/addon-manifest.json -Raw|ConvertFrom-Json
 foreach($row in @($addons.drivers)+@($addons.utilities)){if((Get-FileHash "assets/release/$($row.file)").Hash -ne $row.sha256){throw "Addon publication mismatch: $($row.file)"}}
 $result.hostImages=11;$result.hostPublishedHashes=22;$result.addonImages=14
 & test/integration/pif_hash_tool_test.ps1 -Pif "$PWD/assets/release/PIF.EXE" -Hash "$PWD/assets/release/HASH.EXE" -BuildRoot "$PSScriptRoot/pif-hash" -ReleaseRoot "$PWD/assets/release"
 & test/integration/pif_root_update_test.ps1 -Pif "$PWD/assets/release/PIF.EXE" -BuildRoot "$PSScriptRoot/pif-root"
 & test/integration/szdd_roundtrip_test.ps1 -BuildRoot build/t484-s3-addon/szdd-proof
 & 'C:/Program Files/Python313/python.exe' test/integration/lnk_tool_test.py --lnk assets/release/LNK.EXE --output "$PSScriptRoot/lnk-proof"
 if($LASTEXITCODE){throw 'LNK test failed'}
 $result.exit=0
} catch {$result.error="$_";Write-Output $_}
$result.finished=[DateTimeOffset]::UtcNow.ToString('o')
$result|ConvertTo-Json|Set-Content "$PSScriptRoot/delivery-result.json"
exit $result.exit
