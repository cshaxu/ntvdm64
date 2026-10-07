param([Parameter(Mandatory)][string]$Cache,[Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$cache=(Resolve-Path $Cache).Path;$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned output required'}
$graph=Get-Content "$cache/build.ninja"
$compile=[Array]::FindIndex($graph,[Predicate[string]]{param($line) $line.StartsWith('build obj/worker/rpc_client.obj: ')})
if($compile -lt 0 -or $graph[$compile+1] -notmatch '^  cflags = '){throw 'Missing actual worker client flags'}
$flags=$graph[$compile+1].Trim().Substring(9)
$edge=@($graph|Where-Object {$_ -match '^build ntvdm.exe \| ntvdm.lib: worker_link '})
if($edge.Count -ne 1){throw 'Missing original link edge'}
$rule=[Array]::IndexOf($graph,'rule worker_link')
if($rule -lt 0){throw 'Missing original link rule'}
$command=@($graph[($rule+1)..($rule+5)]|Where-Object {$_ -match '^  command = '}|Select-Object -First 1)
if($command.Count -ne 1){throw 'Missing original linker command'}
$command=$command[0].Trim().Substring(10)
$inputs=@(($edge[0] -replace '^.*: worker_link ','') -split ' ')
$null=New-Item -ItemType Directory -Path $root
$object="$root/diagnostic.obj"
$sources=@()
$inputs=@($inputs|ForEach-Object {
    if($_ -eq 'obj/worker/rpc_client.obj'){'"'+$object+'"'}
    else {$file=(Resolve-Path (Join-Path $cache $_)).Path;$sources+=@{path=$file;sha256=(Get-FileHash $file).Hash};'"'+$file+'"'}
})
$link=$command.Replace('$out',"$root/ntvdm.exe").Replace('$in',($inputs -join ' ')).Replace('$:',':')
$flags=$flags.Replace('$:',':')
$script=@('@echo off',
 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
 'if errorlevel 1 exit /b %errorlevel%',('cd /d "'+$cache+'"'),
 ('cl.exe '+$flags+' /I"'+$cache+'/obj/basesrv" /Fo"'+$object+'" "'+$PSScriptRoot+'/ntvdm_command_client_diagnostic.c"'),
 'if errorlevel 1 exit /b %errorlevel%',$link,'exit /b %errorlevel%')
[IO.File]::WriteAllLines("$root/build.cmd",$script,[Text.Encoding]::ASCII)
& "$root/build.cmd" *> "$root/build.log"
if($LASTEXITCODE){throw "Diagnostic build failed; inspect $root/build.log"}
[ordered]@{role='test-only-never-release';source=(Get-FileHash "$PSScriptRoot/ntvdm_command_client_diagnostic.c").Hash;
    image=(Get-FileHash "$root/ntvdm.exe").Hash;reusedInputs=$sources}|ConvertTo-Json -Depth 4|Set-Content "$root/build.json"
'PASS test-only original NTVDM link with status-observing client wrapper'
