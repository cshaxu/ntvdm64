param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath("$PSScriptRoot/../..")
$out=[IO.Path]::GetFullPath($BuildRoot)
if(!$out.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)){throw 'Fresh build-owned output required'}
New-Item -ItemType Directory $out|Out-Null
$tool="$repo/build/.ninja/build-tools/ntvdm-package-id.exe"
$id=(& $tool --new).Trim();if($LASTEXITCODE -or ([guid]$id) -eq [guid]::Empty){throw 'Invalid generated identity'}
foreach($case in @(@('x86','ntvdm.exe'),@('x64','run16.exe'))){
 $input="$repo/assets/release/$($case[1])";$before=(Get-FileHash $input).Hash
 & $tool --machine $case[0] $input;if($LASTEXITCODE){throw 'Valid machine rejected'}
 $wrong=if($case[0] -eq 'x86'){'x64'}else{'x86'}
 & $tool --machine $wrong $input;if(!$LASTEXITCODE){throw 'Wrong machine accepted'}
 $copy=Join-Path $out $case[1];Copy-Item $input $copy
 & $tool $id $copy;if($LASTEXITCODE){throw 'Stamp failed'}
 $read=(& $tool --read $copy).Trim()
 if($LASTEXITCODE -or $read -ne [Convert]::ToHexString(([guid]$id).ToByteArray())){throw 'Stamp/read mismatch'}
 & $tool --machine $case[0] $copy;if($LASTEXITCODE){throw 'Stamped machine rejected'}
 if((Get-FileHash $input).Hash -ne $before){throw 'Source changed'}
}
$file=Join-Path $out 'invalid.exe'
foreach($kind in 'empty','signature','offset','magic'){
 $b=[IO.File]::ReadAllBytes("$repo/assets/release/ntvdm.exe")
 switch($kind){
  empty {$b=[byte[]]@()}
  signature {$b[0]=0}
  offset {[BitConverter]::GetBytes([int]::MaxValue).CopyTo($b,60)}
  magic {$pe=[BitConverter]::ToInt32($b,60);$b[$pe+24]=0;$b[$pe+25]=0}
 }
 [IO.File]::WriteAllBytes($file,$b)
 & $tool --machine x86 $file;if(!$LASTEXITCODE){throw "Malformed PE accepted: $kind"}
}
& $tool --machine arm64 $file;if(!$LASTEXITCODE){throw 'Unknown architecture accepted'}
& $tool --machine x86 "$out/missing.exe";if(!$LASTEXITCODE){throw 'Absent image accepted'}
& $tool '00000000-0000-0000-0000-000000000000' "$out/run16.exe";if(!$LASTEXITCODE){throw 'Empty identity accepted'}
'PASS UUID, x86/x64, stamp/read, source preservation, malformed/missing/machine rejection'
$manifest="$repo/assets/release/release-manifest.json"
$baseline=[IO.File]::ReadAllText($manifest)
$records=@($baseline|ConvertFrom-Json)
$lines=@(& $tool --manifest $manifest)
if($LASTEXITCODE -or $lines.Count -ne 11){throw 'Complete manifest rejected'}
foreach($record in $records){
 $expected="$($record.Name) $($record.Machine) $($record.PackageId) $($record.Sha256) $($record.InputSha256)"
 if($expected -cnotin $lines){throw 'Manifest fields changed'}
}
$caseFile="$out/manifest.json"
$ten=@($records|Where-Object Name -ne 'VCDEX.DLL')|ConvertTo-Json
[IO.File]::WriteAllText($caseFile,$ten)
$lines=@(& $tool --manifest $caseFile)
if($LASTEXITCODE -or $lines.Count -ne 10){throw 'Supported ten-image manifest rejected'}
[IO.File]::WriteAllText($caseFile,$baseline,[Text.UTF8Encoding]::new($true))
$lines=@(& $tool --manifest $caseFile)
if($LASTEXITCODE -or $lines.Count -ne 11){throw 'UTF8 BOM rejected'}
$cases=@{
 duplicate=$baseline.Replace('"Name":','"Name":"run16.exe","Name":')
 unknown=$baseline.Replace('"Name":','"Unknown":')
 path=$baseline.Replace('system32','..')
 hash=$baseline.Replace($records[0].Sha256,'not-a-hash')
 uuid=$baseline.Replace($records[0].PackageId,'00000000-0000-0000-0000-000000000000')
 mixed=$baseline.Replace('"PackageId": "'+$records[0].PackageId+'"','"PackageId": "'+$id+'"')
 trailing=$baseline+'x'
 truncated=$baseline.Substring(0,$baseline.LastIndexOf(']'))
 duplicateImage=(@($records)+@($records[0])|ConvertTo-Json)
 missing=(@($records|Select-Object -Skip 2)|ConvertTo-Json)
 embeddedNull=$baseline+[char]0
 oversized=' '*16385
}
# Change just one UUID to exercise package-level consistency, not UUID syntax.
$copy=$baseline|ConvertFrom-Json;$copy[0].PackageId=$id
$cases.mixed=$copy|ConvertTo-Json
foreach($case in $cases.GetEnumerator()){
 [IO.File]::WriteAllText($caseFile,[string]$case.Value)
 $lines=@(& $tool --manifest $caseFile)
 if(!$LASTEXITCODE -or $lines.Count){throw "Malformed manifest emitted rows: $($case.Key)"}
}
'PASS complete manifest fields, ten/eleven images, BOM and atomic malformed rejection'
$package="$out/package"
New-Item -ItemType Directory "$package/system32"|Out-Null
foreach($record in $records){Copy-Item "$repo/assets/release/$($record.Name)" "$package/system32/$($record.Name)"}
Copy-Item $manifest "$package/product-system32-manifest.json"
. "$repo/test/observation/isolated_package_cleanup.ps1"
$stages=@()
try{
 foreach($fresh in @($false,$true)){
  $stage=New-PhysicalRuntimeStage $package -FreshPackageIdentity:$fresh
  $stages+=,$stage
  $after=@(Get-Content "$stage/product-system32-manifest.json" -Raw|ConvertFrom-Json)
  $ids=@($after.PackageId|Select-Object -Unique)
  if($ids.Count -ne 1 -or (($ids[0] -eq $records[0].PackageId) -eq $fresh)){throw 'Wrong copy/refresh identity'}
  foreach($row in $after){
   if((Get-FileHash "$stage/$($row.RelativePath)").Hash -ne $row.Sha256){throw 'Refreshed hash mismatch'}
   $resource=(& $tool --read "$stage/$($row.RelativePath)").Trim()
   if($LASTEXITCODE -or $resource -ne [Convert]::ToHexString(([guid]$ids[0]).ToByteArray())){throw 'Refreshed resource mismatch'}
   $before=$records|Where-Object Name -eq $row.Name
   if($before.InputSha256 -ne $row.InputSha256 -or $before.Machine -ne $row.Machine){throw 'Refresh changed provenance'}
  }
 }
}finally{foreach($stage in $stages){Remove-PhysicalRuntimeStage $stage}}
foreach($row in $records){if((Get-FileHash "$package/$($row.RelativePath)").Hash -ne $row.Sha256){throw 'Refresh modified source package'}}
# Preflight must not stamp even the first image when a later hash is invalid.
$bad=$baseline|ConvertFrom-Json;$bad[-1].Sha256='0'*64
$bad|ConvertTo-Json|Set-Content "$package/product-system32-manifest.json"
& "$repo/build/.ninja/build-tools/refresh-package.cmd" $package
if(!$LASTEXITCODE){throw 'Refresh accepted bad hash'}
foreach($row in $records){if((Get-FileHash "$package/$($row.RelativePath)").Hash -ne $row.Sha256){throw 'Preflight partially stamped package'}}
if(!(Test-Path "$package/product-system32-manifest.json")){throw 'Preflight removed source manifest'}
'PASS actual shared copy/refresh provider, resource identities, source preservation and all-input preflight'
Copy-Item $manifest "$package/product-system32-manifest.json" -Force
foreach($name in @('builds','logs','logs2','empty')){New-Item -ItemType Directory "$package/$name"|Out-Null}
[IO.File]::WriteAllText("$package/hidden.txt",'hidden input')
(Get-Item "$package/hidden.txt").Attributes=[IO.FileAttributes]::Hidden
$stage=$null
try{
 $stage=New-PhysicalRuntimeStage $package
 foreach($name in @('builds','logs','logs2')){if(Test-Path "$stage/$name"){throw 'Historical logs copied'}}
 if(!(Test-Path "$stage/empty") -or [IO.File]::ReadAllText("$stage/hidden.txt") -ne 'hidden input'){throw 'Empty/hidden input lost'}
 if($stage -notmatch '^O:\\tmp\\n[0-9a-f]{3}$'){throw 'Physical path exceeds legacy budget'}
 $marker=Get-Content "$stage/.ntvdm64-test-stage.json" -Raw|ConvertFrom-Json
 if($marker.source -ne [IO.Path]::GetFullPath($package) -or $marker.schema -ne 1 -or @($marker.binaries).Count -ne 5){throw 'Ownership marker mismatch'}
 $null=[DateTimeOffset]::Parse($marker.createdUtc)
}finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
$stage=$null
try{
 $stage=New-PhysicalRuntimeStage $package -RuntimeImages @('ntsrv.exe') -TopLevelNames @('system32','system32','product-system32-manifest.json')
 if(Test-Path "$stage/hidden.txt"){throw 'Unselected input copied'}
 $marker=Get-Content "$stage/.ntvdm64-test-stage.json" -Raw|ConvertFrom-Json
 if(@($marker.binaries).Count -ne 1 -or $marker.binaries[0].name -ne 'ntsrv.exe'){throw 'Selected ownership lost'}
}finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
foreach($name in @('..','../system32','logs','*','absent')){
 $rejected=$false
 try{$unexpected=New-PhysicalRuntimeStage $package -TopLevelNames @($name)}catch{$rejected=$true}
 if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw "Invalid top-level accepted: $name"}
}
$flat="$out/flat";New-Item -ItemType Directory $flat|Out-Null
foreach($row in $records){Copy-Item "$package/system32/$($row.Name)" $flat}
Copy-Item $manifest "$flat/product-system32-manifest.json"
$stage=$null
try{
 $stage=New-PhysicalRuntimeStage $flat -FreshPackageIdentity
 if(Test-Path "$stage/system32"){throw 'Flat layout changed'}
 $marker=Get-Content "$stage/.ntvdm64-test-stage.json" -Raw|ConvertFrom-Json
 if(@($marker.binaries).Count -ne 5){throw 'Flat ownership lost'}
}finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
Copy-Item "$package/system32/run16.exe" "$package/run16.exe"
$rejected=$false
try{$unexpected=New-PhysicalRuntimeStage $package}catch{$rejected=$true}
if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw 'Mixed layout accepted'}
Remove-Item -LiteralPath "$package/run16.exe"
$unicode="$out/测试 source"
New-Item -ItemType Directory $unicode|Out-Null
Copy-Item "$package/system32/run16.exe" "$unicode/run16.exe"
$stage=$null
try{
 $stage=New-PhysicalRuntimeStage $unicode -RuntimeImages @('run16.exe')
 $marker=[IO.File]::ReadAllText("$stage/.ntvdm64-test-stage.json")|ConvertFrom-Json
 if($marker.source -cne [IO.Path]::GetFullPath($unicode)){throw 'UTF8 marker source changed'}
}finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
'PASS selected/hidden/empty copying, legacy flat refresh, exclusions, mixed rejection and UTF8 ownership'
$testInput="$out/probe.bin"
[IO.File]::WriteAllBytes($testInput,[byte[]](0,1,127,255))
$stage=$null
try{
 $stage=New-PhysicalRuntimeStage $package -TestFiles @($testInput)
 if((Get-FileHash "$stage/tests/probe.bin").Hash -ne (Get-FileHash $testInput).Hash){throw 'Independent test input changed'}
}finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
foreach($invalid in @("$out/missing.bin",$out,"$repo/assets/release/PIF.EXE")){
 $rejected=$false
 try{$unexpected=New-PhysicalRuntimeStage $package -TestFiles @($invalid)}catch{$rejected=$true}
 if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw "Invalid test input accepted: $invalid"}
}
'PASS build-owned test input copying and missing/directory/outside-build rejection'
$stage=$null
try{
 $stage=New-PhysicalRuntimeStage $package -TestFiles @($testInput) -TestSubdirectory DFCB01
 if((Get-FileHash "$stage/tests/DFCB01/probe.bin").Hash -ne (Get-FileHash $testInput).Hash){throw 'Test subdirectory input changed'}
 if(Test-Path "$stage/tests/probe.bin"){throw 'Subdirectory input leaked into tests root'}
}finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
foreach($invalid in @('.','..','../escape','a/b','a\b','a b','C:outside')){
 $rejected=$false
 try{$unexpected=New-PhysicalRuntimeStage $package -TestFiles @($testInput) -TestSubdirectory $invalid}catch{$rejected=$true}
 if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw "Invalid test subdirectory accepted: $invalid"}
}
$rejected=$false
try{$unexpected=New-PhysicalRuntimeStage $package -TestSubdirectory DFCB01}catch{$rejected=$true}
if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw 'Empty subdirectory input accepted'}
$null=New-Item -ItemType Directory "$package/tests/DFCB01" -Force
[IO.File]::WriteAllText("$package/tests/DFCB01/keep.txt",'preserve')
$rejected=$false
try{$unexpected=New-PhysicalRuntimeStage $package -TestFiles @($testInput) -TestSubdirectory DFCB01}catch{$rejected=$true}
if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw 'Existing test subdirectory accepted'}
if([IO.File]::ReadAllText("$package/tests/DFCB01/keep.txt") -ne 'preserve'){throw 'Source collision changed'}
'PASS isolated test subdirectory, traversal/empty/collision rejection and source preservation'
foreach($source in @($package,$flat)){
 $stage=$null
 try{
  $stage=New-PhysicalRuntimeStage $source -TestInBinary -TestSubdirectory INPUTS -TestFiles @($testInput)
  $binary=Get-PackageBinaryRoot $stage
  if((Get-FileHash "$binary/tests/INPUTS/probe.bin").Hash -ne (Get-FileHash $testInput).Hash){throw 'Binary-directory input changed'}
  if($binary -ne $stage -and (Test-Path "$stage/tests/INPUTS/probe.bin")){throw 'Binary input leaked to package root'}
 }finally{if($stage){Remove-PhysicalRuntimeStage $stage}}
}
$rejected=$false
try{$unexpected=New-PhysicalRuntimeStage $package -TestInBinary}catch{$rejected=$true}
if(!$rejected){Remove-PhysicalRuntimeStage $unexpected;throw 'Empty binary-directory input accepted'}
'PASS structured/flat binary-directory input placement and empty-input rejection'
$global:LASTEXITCODE=0
