$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$evidence=Join-Path $PSScriptRoot 'latest-source-release'
foreach($row in (Get-Content "$evidence/manifest.json" -Raw|ConvertFrom-Json)){
    $file=Get-Item -LiteralPath (Join-Path $evidence $row.path)
    if($file.Length -ne $row.bytes -or (Get-FileHash $file.FullName).Hash -ne $row.sha256){throw "Evidence drift: $($row.path)"}
}
$rows=@(Get-Content "$evidence/ten-component-manifest.json" -Raw|ConvertFrom-Json)
if($rows.Count -ne 10 -or @($rows.Name|Sort-Object -Unique).Count -ne 10){throw 'Incomplete historical ten-image manifest'}
$current=@(Get-Content "$repo/assets/release/release-manifest.json" -Raw|ConvertFrom-Json)
if($current.Count -ne 10 -or @($current.Name|Sort-Object -Unique).Count -ne 10){throw 'Incomplete current ten-image manifest'}
foreach($root in @("$repo/assets/release",'O:/winnt/system32')){
    foreach($row in $current){if((Get-FileHash (Join-Path $root $row.Name)).Hash -ne $row.Sha256){throw "Deployment drift: $root/$($row.Name)"}}
}
$full=Get-Content "$evidence/final/forward.completion.json" -Raw|ConvertFrom-Json
if($full.exit -ne 0 -or !(Select-String -LiteralPath "$evidence/final/run-full.log" -SimpleMatch 'PASS Full selected gates; identical 10-file package' -Quiet)){throw 'Full suite not proven'}
$tools=Get-Content "$evidence/final/tools-result.json" -Raw|ConvertFrom-Json
if($tools.result -ne 'pass' -or @($tools.runs).Count -ne 5 -or @($tools.runs|Where-Object exit -ne 0).Count){throw 'Setup groups not proven'}
$utilities=Get-Content "$evidence/final/utilities-r4.completion.json" -Raw|ConvertFrom-Json
if($utilities.exit -ne 0){throw 'Final utility delivery not proven'}
$addon=Get-Content "$repo/assets/release/addon-manifest.json" -Raw|ConvertFrom-Json
foreach($name in @('SZDD.EXE','patch31.exe')){
    $entries=@($addon.utilities|Where-Object file -eq $name)
    if($entries.Count -ne 1){throw "Missing utility record: $name"}
    foreach($root in @("$repo/assets/release",'O:/winnt/system32')){if((Get-FileHash "$root/$name").Hash -ne $entries[0].sha256){throw "Utility hash drift: $name"}}
}
foreach($path in @('src/addon/win31-setup/patchset.c','tools/win31-setup/PATCHSET.EXE','tools/win31-setup/SZDDPACK.EXE','tools/win31-setup/MEDIAPATCH.EXE','assets/release/PATCHSET.EXE')){
    if(Test-Path (Join-Path $repo $path)){throw "Retired helper remains: $path"}
}
'PASS historical Full/Setup evidence, current coherent ten-image deployment and independent SZDD/patch31'
