param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath((Join-Path $repo $BuildRoot))
$cache=Join-Path $repo 'build/M0-T434/S3/r001-formal'
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Fresh build-owned repair cache required'
}
if((Get-FileHash "$cache/ntvdm.exe").Hash -ne
   'CA8C7AC27A406ABF612927F4C485D58E413D0A686462FA4EEA7130F8ACB6F212') {
    throw 'Selected baseline changed'
}
$ninja=(Get-Command ninja.exe -ErrorAction Stop).Source
New-Item -ItemType Directory -Path $root | Out-Null
Copy-Item -Path "$cache/*" -Destination $root -Recurse
foreach($hidden in @('.ninja_log','.ninja_deps')) {
    if(Test-Path "$cache/$hidden"){Copy-Item -LiteralPath "$cache/$hidden" -Destination "$root/$hidden"}
}
$graph=(Get-Content "$root/build.ninja" -Raw).Replace($cache.Replace('\','/').Replace(':','$:'),
    $root.Replace('\','/').Replace(':','$:'))
[IO.File]::WriteAllText("$root/build.ninja",$graph,[Text.UTF8Encoding]::new($false))
$commands=@('@echo off',
    'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
    'if errorlevel 1 exit /b %errorlevel%',
    ('cd /d "'+$root+'"'),
    ('"'+$ninja+'" ntvdm.exe softpc-text-video-test.exe console-text-producer-test.exe'),
    'exit /b %errorlevel%')
[IO.File]::WriteAllLines("$root/build-repair.cmd",$commands,[Text.Encoding]::ASCII)
& "$root/build-repair.cmd" *> "$root/repair-build.log"
if($LASTEXITCODE){throw "Repair build failed: $root/repair-build.log"}
foreach($test in @('softpc-text-video-test.exe','console-text-producer-test.exe')) {
    & "$root/$test" *> "$root/$test.txt"
    if($LASTEXITCODE){throw "Repair fixture failed: $test"}
}
'PASS repair build and both affected production adapter fixtures'
