param([Parameter(Mandatory)][string]$RunRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$root=[IO.Path]::GetFullPath($RunRoot)
if(!$root.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned test directory required'}
New-Item -ItemType Directory $root|Out-Null
$prior=Get-Content "$repo/assets/release/addon-manifest.json" -Raw|ConvertFrom-Json
Copy-Item "$repo/assets/release/addon-manifest.json" $root
foreach($driver in $prior.drivers){Copy-Item "$repo/assets/release/$($driver.file)" $root}
& "$repo/tools/publish-addons.cmd" $root *> "$root/publication.log"
if($LASTEXITCODE){throw 'CMD publication failed'}
$current=Get-Content "$root/addon-manifest.json" -Raw|ConvertFrom-Json
if($current.schema -ne 1 -or $current.utilities.Count -ne 9){throw 'Incomplete addon manifest'}
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
foreach($before in $prior.drivers){
 $expected=$before|ConvertTo-Json -Depth 8|ConvertFrom-Json
 $actual=$current.drivers|Where-Object id -eq $before.id
 $hash=(Get-FileHash "$repo/$($before.source)").Hash
 $changed=$hash -ne $before.sourceSha256 -or $actual.sha256 -ne $before.sha256
 $expected.sourceSha256=$hash;$expected.sha256=$actual.sha256
 if($before.sources){
  $directory=Split-Path "$repo/$($before.source)"
  foreach($source in $expected.sources){
   $hash=(Get-FileHash "$directory/$($source.name)").Hash
   if($hash -ne $source.sha256){$changed=$true};$source.sha256=$hash
  }
 }
 if($changed){
  $expected.acceptance='candidate'
  $expected.PSObject.Properties.Remove('evidence');$expected.PSObject.Properties.Remove('verifiedScope')
 }
 Assert-Equivalent $expected $actual
}
foreach($item in @($current.drivers)+@($current.utilities)){
 if((Get-FileHash "$root/$($item.file)").Hash -ne $item.sha256){throw "Artifact hash: $($item.file)"}
}
foreach($item in $current.utilities){
 if((Get-FileHash "$repo/$($item.source)").Hash -ne $item.sourceSha256){throw "Source hash: $($item.id)"}
 if((Get-FileHash "$repo/build/.ninja/addon/$($item.id)/$($item.id).exe").Hash -ne $item.sha256){throw 'Wrong build image'}
}
$manifestHash=(Get-FileHash "$root/addon-manifest.json").Hash
$utilityHashes=@($current.utilities|ForEach-Object {(Get-FileHash "$root/$($_.file)").Hash})
$image="$repo/build/.ninja/addon/mouse31/MOUSE31.DRV"
$receipt="$repo/build/.ninja/addon/mouse31/build.sha256"
$imageBytes=[IO.File]::ReadAllBytes($image);$receiptBytes=[IO.File]::ReadAllBytes($receipt)
try{
 $changed=[byte[]]$imageBytes.Clone();$changed[$changed.Length-1]=$changed[$changed.Length-1] -bxor 1
 [IO.File]::WriteAllBytes($image,$changed)
 & "$repo/build/.ninja/addon/publish-addons.cmd" $root *> "$root/reject-receipt.log"
 if(!$LASTEXITCODE){throw 'Mismatched build receipt accepted'}
 if((Get-FileHash "$root/addon-manifest.json").Hash -ne $manifestHash){throw 'Preflight changed manifest'}
 $after=@($current.utilities|ForEach-Object {(Get-FileHash "$root/$($_.file)").Hash})
 if(($after -join ',') -ne ($utilityHashes -join ',')){throw 'Preflight changed utilities'}
 [IO.File]::WriteAllText($receipt,(Get-FileHash $image).Hash+"`r`n")
 & "$repo/build/.ninja/addon/publish-addons.cmd" $root *> "$root/changed-candidate.log"
 if($LASTEXITCODE){throw 'Candidate publication failed'}
 $candidate=(Get-Content "$root/addon-manifest.json" -Raw|ConvertFrom-Json).drivers|Where-Object id -eq win31
 if($candidate.acceptance -ne 'candidate' -or $candidate.evidence -or $candidate.verifiedScope){throw 'Stale qualification retained'}
 if($candidate.sha256 -ne (Get-FileHash $image).Hash){throw 'Candidate hash mismatch'}
}finally{
 [IO.File]::WriteAllBytes($image,$imageBytes);[IO.File]::WriteAllBytes($receipt,$receiptBytes)
}
& "$repo/build/.ninja/addon/publish-addons.cmd" "$root/absent" *> "$root/reject-missing.log"
if(!$LASTEXITCODE){throw 'Missing destination manifest accepted'}
Copy-Item "$repo/assets/release/$($prior.drivers[0].file)" $root -Force
$lock=[IO.File]::Open("$root/HASH.EXE",[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::Read)
try{
 & "$repo/build/.ninja/addon/publish-addons.cmd" $root *> "$root/reject-copy.log"
 if(!$LASTEXITCODE){throw 'Locked destination reported success'}
 if(Test-Path "$root/addon-manifest.json"){throw 'Failed copy retained completion manifest'}
}finally{$lock.Dispose()}
@{utilities=9;drivers=5;sourceHashes=$true;driverMetadataPreserved=$true;invalidReceiptRejected=$true;changedDriverCandidate=$true;missingDestinationRejected=$true}|ConvertTo-Json|Set-Content "$root/result.json"
'PASS addon publication and preflight rejection'
