param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned release cache required'
}
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
$sets=@(
 @{name='formal';arch='x86';cache='build/M0-T435/S2/r051-palette-repair';targets='ntvdm.exe nthook32.dll';fixtures='frontend-request-client-test.exe broker-frontend-bootstrap-test.exe native-gui-startup-probe.exe'},
 @{name='service';arch='x64';cache='build/M0-T434/S3/r002-service';targets='ntsrv.exe'},
 @{name='hook64';arch='x64';cache='build/M0-T434/S3/r003-hook64';targets='nthook64.dll'},
 @{name='worker';arch='x64';cache='build/M0-T434/S3/r004-worker';targets='ntvwm.exe'},
 @{name='frontend';arch='x64';cache='build/M0-T434/S3/r005-frontend';targets='ntcon.exe'},
 @{name='launcher';arch='x64';cache='build/M0-T434/S3/r006-launcher';targets='run16.exe'},
 @{name='monitor';arch='x64';cache='build/M0-T434/S5/r024-monitor';targets='ntmon.exe monitor-rpc-test.exe'}
)
New-Item -ItemType Directory -Path $root | Out-Null
foreach($set in $sets) {
    $cache=(Resolve-Path (Join-Path $repo $set.cache)).Path
    $destination=Join-Path $root $set.name
    New-Item -ItemType Directory -Path $destination | Out-Null
    Copy-Item -Path "$cache/*" -Destination $destination -Recurse
    foreach($name in @('.ninja_deps','.ninja_log')) {
        if(Test-Path "$cache/$name"){Copy-Item -LiteralPath "$cache/$name" -Destination "$destination/$name"}
    }
    $graph=(Get-Content "$destination/build.ninja" -Raw).Replace($cache.Replace('\','/').Replace(':','$:'),
        $destination.Replace('\','/').Replace(':','$:'))
    [IO.File]::WriteAllText("$destination/build.ninja",$graph,[Text.UTF8Encoding]::new($false))
    $commands=@('@echo off',
        ('call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch='+$set.arch+' -host_arch=x64 >nul'),
        'if errorlevel 1 exit /b %errorlevel%',('cd /d "'+$destination+'"'),
        ('"'+$ninja+'" '+$set.targets+' '+$set.fixtures),'exit /b %errorlevel%')
    [IO.File]::WriteAllLines("$destination/release-build.cmd",$commands,[Text.Encoding]::ASCII)
    & "$destination/release-build.cmd" *> "$destination/release-build.log"
    if($LASTEXITCODE){throw "Release cache build failed: $($set.name)"}
    Write-Output "PASS dependency-selected $($set.name) $($set.arch)"
}
$runtime=Join-Path $root 'runtime'
New-Item -ItemType Directory -Path $runtime | Out-Null
Copy-Item -Path "$repo/build/M0-T434/S5/r037-runtime/*" -Destination $runtime -Recurse
foreach($set in $sets) {
    foreach($image in ($set.targets -split ' ' | Where-Object {$_ -ne 'monitor-rpc-test.exe'})) {
        Copy-Item -LiteralPath "$root/$($set.name)/$image" -Destination "$runtime/system32/$image" -Force
    }
}
Get-ChildItem "$runtime/system32" -File | Where-Object Name -In @('run16.exe','ntsrv.exe','ntcon.exe','ntvdm.exe','ntvwm.exe','ntmon.exe','nthook32.dll','nthook64.dll','WOW32.DLL','VDMREDIR.DLL') |
    ForEach-Object {[ordered]@{name=$_.Name;sha256=(Get-FileHash $_.FullName).Hash}} |
    ConvertTo-Json | Set-Content "$root/release-manifest.json"
'Built coherent T435 candidate; runtime/publication gates still required'
