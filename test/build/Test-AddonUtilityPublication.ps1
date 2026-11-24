param([Parameter(Mandatory)][string]$RunRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$root=[IO.Path]::GetFullPath($RunRoot)
if(!$root.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned test directory required'}
New-Item -ItemType Directory $root|Out-Null
$prior=Get-Content "$repo/assets/release/addon-manifest.json" -Raw|ConvertFrom-Json
Copy-Item "$repo/assets/release/addon-manifest.json" $root
foreach($driver in $prior.drivers){Copy-Item "$repo/assets/release/$($driver.file)" $root}
& "$repo/tools/publish-addon-utilities.cmd" $root *> "$root/publication.log"
if($LASTEXITCODE){throw 'CMD publication failed'}
$current=Get-Content "$root/addon-manifest.json" -Raw|ConvertFrom-Json
if($current.schema -ne 1 -or $current.utilities.Count -ne 9){throw 'Incomplete utility manifest'}
function Assert-Equivalent($expected,$actual){
 if($null -eq $expected){if($null -ne $actual){throw 'Unexpected metadata'};return}
 if($expected -is [pscustomobject]){
  $keys=@($expected.PSObject.Properties.Name);$actualKeys=@($actual.PSObject.Properties.Name)
  if((($keys|Sort-Object)-join ',') -cne (($actualKeys|Sort-Object)-join ',')){throw 'Metadata fields changed'}
  foreach($key in $keys){Assert-Equivalent $expected.$key $actual.$key}
 }elseif($expected -is [array]){
  if($expected.Count -ne $actual.Count){throw 'Metadata count changed'}
  for($i=0;$i -lt $expected.Count;$i++){Assert-Equivalent $expected[$i] $actual[$i]}
 }elseif($expected -cne $actual){throw 'Metadata value changed'}
}
Assert-Equivalent $prior.drivers $current.drivers
foreach($item in @($current.drivers)+@($current.utilities)){
 if((Get-FileHash "$root/$($item.file)").Hash -ne $item.sha256){throw "Artifact hash: $($item.file)"}
}
foreach($item in $current.utilities){
 if((Get-FileHash "$repo/$($item.source)").Hash -ne $item.sourceSha256){throw "Source hash: $($item.id)"}
 if((Get-FileHash "$repo/build/.ninja/addon/$($item.id)/$($item.id).exe").Hash -ne $item.sha256){throw 'Wrong build image'}
}
$manifestHash=(Get-FileHash "$root/addon-manifest.json").Hash
$utilityHashes=@($current.utilities|ForEach-Object {(Get-FileHash "$root/$($_.file)").Hash})
[IO.File]::WriteAllText("$root/$($prior.drivers[0].file)",'invalid driver')
& "$repo/build/.ninja/addon/publish-utilities.cmd" $root *> "$root/reject-driver.log"
if(!$LASTEXITCODE){throw 'Mismatched driver accepted'}
if((Get-FileHash "$root/addon-manifest.json").Hash -ne $manifestHash){throw 'Preflight changed manifest'}
$after=@($current.utilities|ForEach-Object {(Get-FileHash "$root/$($_.file)").Hash})
if(($after -join ',') -ne ($utilityHashes -join ',')){throw 'Preflight changed utilities'}
& "$repo/build/.ninja/addon/publish-utilities.cmd" "$root/absent" *> "$root/reject-missing.log"
if(!$LASTEXITCODE){throw 'Missing destination manifest accepted'}
Copy-Item "$repo/assets/release/$($prior.drivers[0].file)" $root -Force
$lock=[IO.File]::Open("$root/HASH.EXE",[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::Read)
try{
 & "$repo/build/.ninja/addon/publish-utilities.cmd" $root *> "$root/reject-copy.log"
 if(!$LASTEXITCODE){throw 'Locked destination reported success'}
 if(Test-Path "$root/addon-manifest.json"){throw 'Failed copy retained completion manifest'}
}finally{$lock.Dispose()}
@{utilities=9;drivers=5;sourceHashes=$true;driverMetadataPreserved=$true;invalidDriverRejected=$true;missingDestinationRejected=$true}|ConvertTo-Json|Set-Content "$root/result.json"
'PASS addon utility publication and preflight rejection'
