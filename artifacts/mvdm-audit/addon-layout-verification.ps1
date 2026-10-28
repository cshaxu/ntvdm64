$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$evidence=Join-Path $PSScriptRoot 'addon-layout'
foreach($row in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath (Join-Path $evidence $row.path)
    if($file.Length -ne $row.bytes -or (Get-FileHash $file.FullName).Hash -ne $row.sha256){throw "Evidence drift: $($row.path)"}
}
$build=Get-Content "$evidence/final/result.json" -Raw|ConvertFrom-Json
if($build.result -ne 'pass' -or @($build.runs).Count -ne 23 -or @($build.runs|Where-Object exit -ne 0).Count){throw 'Complete build/functional matrix missing'}
foreach($inputRecord in $build.sources){if((Get-FileHash $inputRecord.path).Hash -ne $inputRecord.sha256){throw "Built source drift: $($inputRecord.path)"}}
$supplement=Get-Content "$evidence/supplement/result.json" -Raw|ConvertFrom-Json
if($supplement.result -ne 'pass' -or @($supplement.runs).Count -ne 3 -or @($supplement.runs|Where-Object exit -ne 0).Count){throw 'Tagged/tagless GRP and SZDD controls missing'}
if(!(Select-String -LiteralPath "$evidence/supplement/grp-tagless.stdout.txt" -Pattern 'source-tag-bytes=0' -SimpleMatch -Quiet) -or
   !(Select-String -LiteralPath "$evidence/supplement/grp-tagged.stdout.txt" -Pattern 'source-tag-bytes=33' -SimpleMatch -Quiet)){throw 'GRP fixture capabilities not proven'}
$addon=Get-Content "$repo/assets/release/addon-manifest.json" -Raw|ConvertFrom-Json
if($addon.schema -ne 1 -or @($addon.drivers).Count -ne 3 -or @($addon.utilities).Count -ne 9){throw 'Expected three drivers and nine native utilities'}
$entries=@($addon.drivers)+@($addon.utilities)
if(@($entries.id|Sort-Object -Unique).Count -ne 12 -or @($entries.file|Sort-Object -Unique).Count -ne 12){throw 'Duplicate add-on owner/output'}
foreach($entry in $entries){
    if((Get-FileHash (Join-Path $repo $entry.source)).Hash -ne $entry.sourceSha256){throw "Source binding drift: $($entry.id)"}
    foreach($root in @("$repo/assets/release",'O:/winnt/system32')){if((Get-FileHash "$root/$($entry.file)").Hash -ne $entry.sha256){throw "Published add-on drift: $($entry.file)"}}
}
foreach($entry in $addon.drivers){$before=@($build.drivers_before|Where-Object id -eq $entry.id);if($before.Count -ne 1 -or $before[0].sha256 -ne $entry.sha256 -or $before[0].acceptance -ne $entry.acceptance){throw 'Driver bytes or inherited acceptance changed'}}
foreach($id in @('patch31','path31','patch95','path95')){
    $entry=@($addon.utilities|Where-Object id -eq $id)
    if($entry.Count -ne 1 -or $entry[0].file -cne "$id.exe"){throw 'Final utility naming mismatch'}
    foreach($root in @("$repo/assets/release",'O:/winnt/system32')){if((Get-Item "$root/$id.exe").Name -cne "$id.exe"){throw 'Physical file case mismatch'}}
}
foreach($name in @('W31PATCH.EXE','W31PATH.EXE','WIN95PATH.EXE')){foreach($root in @("$repo/assets/release",'O:/winnt/system32')){if(Test-Path "$root/$name"){throw "Retired alias remains: $name"}}}
foreach($path in @('tools/pif','tools/hash','tools/win31-path/WIN31PATH.EXE','tools/win95-path/WIN95PATH.EXE','tools/win95-launch/PATCH95.EXE','tools/win95-launch/compat95.sys','src/addon/win101-setup','src/addon/patch95/path95.c')){if(Test-Path (Join-Path $repo $path)){throw "Duplicate or old source home remains: $path"}}
$core=Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json
if(@($core).Count -ne 10 -or @($core.Name|Sort-Object -Unique).Count -ne 10){throw 'Incomplete core manifest'}
foreach($row in $core){foreach($root in @("$repo/assets/release",'O:/winnt/system32')){if((Get-FileHash "$root/$($row.Name)").Hash -ne $row.Sha256){throw 'Core package differs from its manifest'}}}
if(@(Get-ChildItem "$repo/src/addon" -Recurse -File|Where-Object Extension -in '.obj','.exe','.drv','.sys','.lst').Count){throw 'Generated outputs leaked into add-on source'}
'PASS 12 add-on components/outputs, 24 deployment hashes, final names, complete tool matrix, coherent core package and no duplicate binary/source homes'
