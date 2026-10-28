$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$out=Join-Path $repo 'build/t473-s10/latest-source-release-r3'
$worker=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
$native=Join-Path $repo 'build/.ninja/msvc-x64-release'
$package=Join-Path $out 'package'
$record=Get-Content "$out/build-result.json" -Raw|ConvertFrom-Json
if($record.result -ne 'pass'){throw 'Latest-source build has not passed'}
& git diff --quiet $record.source_commit -- src CMakeLists.txt CMakePresets.json
if($LASTEXITCODE){throw 'Production build inputs differ from committed build source'}
& tools/build/Stage-System32ProductPackage.ps1 -BaselineRoot "$repo/build/package-uuid/package-c" -BuildCache $worker -Wow32 "$repo/build/.ninja/msvc-x86-release/wow32/wow32.dll" -OutputRoot $package -NativeHook "$worker/nthook32.dll" -NativeHook64 "$native/hook/nthook64.dll" -NativeWorker "$native/worker/ntvwm.exe" -NativeFrontend "$native/frontend/ntcon.exe" -NativeMonitor "$native/monitor/ntmon.exe" -NativeLauncher "$native/launcher/run16.exe" -NativeService "$native/service/ntsrv.exe"
if($LASTEXITCODE){throw 'Latest-source staging failed'}
$manifest=Get-Content "$package/product-system32-manifest.json" -Raw|ConvertFrom-Json
$names=@('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','WOW32.DLL','VDMREDIR.DLL','nthook32.dll','nthook64.dll')
if(@($manifest).Count -ne 10 -or @($manifest.Name|Sort-Object -Unique).Count -ne 10){throw 'Whole ten-component package required'}
$inputs=@{'run16.exe'="$native/launcher/run16.exe";'ntsrv.exe'="$native/service/ntsrv.exe";'ntcon.exe'="$native/frontend/ntcon.exe";'ntvdm.exe'="$worker/ntvdm.exe";'ntvwm.exe'="$native/worker/ntvwm.exe";'ntmon.exe'="$native/monitor/ntmon.exe";'WOW32.DLL'="$repo/build/.ninja/msvc-x86-release/wow32/wow32.dll";'VDMREDIR.DLL'="$worker/VDMREDIR.dll";'nthook32.dll'="$worker/nthook32.dll";'nthook64.dll'="$native/hook/nthook64.dll"}
foreach($r in $manifest){
    if($r.Name -notin $names -or (Get-FileHash -LiteralPath $inputs[$r.Name]).Hash -ne $r.InputSha256){throw 'Component did not come from latest cached build'}
    if((Get-FileHash -LiteralPath (Join-Path "$package/system32" $r.Name)).Hash -ne $r.Sha256){throw 'Stage input drift'}
}
$installed=(Resolve-Path 'O:/winnt/system32').Path
if($installed -ine 'O:\winnt\system32'){throw 'Unexpected installed root'}
$processes=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($installed+'\',[StringComparison]::OrdinalIgnoreCase) -and $_.Name -in $names})
$processes|Select-Object ProcessId,ExecutablePath|ConvertTo-Json|Set-Content "$out/stopped-installed-processes.json"
foreach($p in $processes){Stop-Process -Id $p.ProcessId -Force; $owned=Get-Process -Id $p.ProcessId -ErrorAction SilentlyContinue;if($owned){$owned.WaitForExit();$owned.Dispose()}}
$targets=@((Join-Path $repo 'assets/release'),$installed)
foreach($index in 0..1){
    $backup=Join-Path $out "before-publication-$index";New-Item -ItemType Directory -Path $backup|Out-Null
    foreach($r in $manifest){$path=Join-Path $targets[$index] $r.Name;if(Test-Path -LiteralPath $path){Copy-Item -LiteralPath $path -Destination (Join-Path $backup $r.Name)}}
}
foreach($target in $targets){foreach($r in $manifest){Copy-Item -LiteralPath (Join-Path "$package/system32" $r.Name) -Destination (Join-Path $target $r.Name) -Force}}
Copy-Item -LiteralPath "$package/product-system32-manifest.json" -Destination "$repo/assets/release/release-manifest.json" -Force
foreach($target in $targets){Copy-Item -LiteralPath "$package/system32/nthook32-LICENSE.txt" -Destination (Join-Path $target 'nthook32-LICENSE.txt') -Force}
$verified=@()
foreach($target in $targets){foreach($r in $manifest){
    $path=Join-Path $target $r.Name;$hash=(Get-FileHash -LiteralPath $path).Hash
    if($hash -ne $r.Sha256){throw 'Whole-package publication hash mismatch'}
    $verified+=@{path=$path;sha256=$hash;package_id=$r.PackageId;producer=$inputs[$r.Name];producer_sha256=$r.InputSha256}
}}
@{result='pass';source_commit=$record.source_commit;head_at_publication=(& git rev-parse HEAD);published_utc=[DateTime]::UtcNow.ToString('o');scope='All ten components built from latest committed production code; owner rejects old-package restoration';package_root=$package;verified=$verified;guest_changed=$false}|ConvertTo-Json -Depth 6|Set-Content "$out/publication-result.json"
'PASS latest-source ten-component package published to assets/release and O:/winnt/system32; 20 destination hashes verified'
