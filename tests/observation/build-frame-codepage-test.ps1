param(
    [string]$Cache='build/M0-T427/S2/r001',
    [Parameter(Mandatory)][string]$BuildRoot,
    [string]$Environment='build/M0-T427/S4/r049/msvc-x86.cmd'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cachePath=(Resolve-Path $Cache).Path
$environmentPath=(Resolve-Path $Environment).Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase)){throw 'Output must remain under build/'}
if(Test-Path $root){throw 'Fresh fixture build required'}
$null=New-Item -ItemType Directory -Path $root
$object=Join-Path $root 'frontend_frame_codepage.obj'
$source=Join-Path $repo 'tests/app/frontend_frame_codepage_test.c'
& $environmentPath cl.exe /nologo /c /MT /W4 /WX /wd4201 /std:c11 /D_CRT_SECURE_NO_WARNINGS /TC "/I$repo/src" "/I$repo/src/ntcon-exe" "/I$cachePath/obj/basesrv" "/Fo$object" $source
if($LASTEXITCODE){throw 'Fixture compilation failed'}
$graph=Get-Content (Join-Path $cachePath 'build.ninja')
$entry=@($graph | Where-Object {$_ -match '^build console-frame-failure-test.exe: console_test_link '})
if($entry.Count -ne 1){throw 'Fixture link dependency graph missing/ambiguous'}
$inputs=@(($entry[0] -replace '^.*: console_test_link ','') -split ' ' |
    Where-Object {$_ -ne 'obj/tests/console_frame_failure.obj'} |
    ForEach-Object {(Resolve-Path (Join-Path $cachePath $_)).Path})
$inputs+=@('obj/frontend/console_frontend.obj','obj/frontend/console_video.obj' |
    ForEach-Object {(Resolve-Path (Join-Path $cachePath $_)).Path})
$rule=[Array]::IndexOf($graph,'rule console_test_link')
if($rule -lt 0){throw 'Fixture link rule missing'}
$libraries=($graph[$rule+1] -split '\$in ',2)[1] -split ' '
$output=Join-Path $root 'frontend-frame-codepage-test.exe'
& $environmentPath link.exe /nologo /subsystem:console /opt:ref "/out:$output" $object @inputs @libraries
if($LASTEXITCODE){throw 'Fixture link failed'}
[ordered]@{Source=(Get-FileHash $source).Hash;Importer=(Get-FileHash "$repo/src/ntcon-exe/frontend_session.c").Hash;Output=(Get-FileHash $output).Hash;Inputs=@($inputs|ForEach-Object{[ordered]@{Path=$_;Hash=(Get-FileHash $_).Hash}})}|ConvertTo-Json -Depth 5|Set-Content (Join-Path $root 'manifest.json')
Write-Output "PASS built $output"
