param(
    [Parameter(Mandatory=$true)][string]$BuildRoot,
    [Parameter(Mandatory=$true)][string]$FrontendBuild,
    [Parameter(Mandatory=$true)][string]$Observer,
    [string]$RepositoryRoot=(Split-Path (Split-Path $PSScriptRoot)),
    [string]$VsDevCmd='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
# Invoke this script only in an owned background process. Dependencies are
# accepted S1 objects, not newly guessed stand-ins; refuse source drift.
$ErrorActionPreference='Stop'
$repo=(Resolve-Path $RepositoryRoot).Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith("$repo\build\",[StringComparison]::OrdinalIgnoreCase)){throw 'BuildRoot must be below repository build'}
New-Item -ItemType Directory $root -Force|Out-Null
$env:TEMP="$root\tmp";$env:TMP=$env:TEMP
New-Item -ItemType Directory $env:TEMP -Force|Out-Null
$timings=[Collections.Generic.List[object]]::new()
function Run([string]$name,[string]$exe,[string]$arguments,[int]$seconds) {
    $watch=[Diagnostics.Stopwatch]::StartNew()
    $p=Start-Process $exe -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root\$name.stdout.txt" -RedirectStandardError "$root\$name.stderr.txt"
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime();command="$exe $arguments";root=$root;timeoutSeconds=$seconds}|ConvertTo-Json|Set-Content "$root\$name.owner.json"
    if(!$p.WaitForExit($seconds*1000)){$p.Kill($true);$p.WaitForExit();throw "$name timeout"}
    $p.Refresh();$timings.Add(@{name=$name;elapsedMs=$watch.ElapsedMilliseconds;exitCode=$p.ExitCode})
    $timings|ConvertTo-Json|Set-Content "$root\timings.json"
    if($p.ExitCode){throw "$name exit $($p.ExitCode)"}
}
try {
    $drift=& git -C $repo diff fb7650ce0 -- src/ntcon-exe src/opennt-abi/host-compat/console_grid.c src/common
    if($LASTEXITCODE -or $drift){throw 'Accepted frontend dependencies have changed; rebuild their closure'}
    $dependency=@('obj/frontend/frontend_session.obj','obj/frontend/console_frontend.obj','obj/frontend/console_video.obj','obj/opennt-abi-host-compat/console_grid.obj','frontend-window.lib')
    $dependency|ForEach-Object{Get-FileHash (Join-Path $FrontendBuild $_)}|ConvertTo-Json|Set-Content "$root\dependencies.json"
    $source=@('tests/observation/native_vt_fidelity_test.c','src/ntvwm-exe/console_state.c','src/ntvwm-exe/text_frame.c','src/common/console/members.c','src/common/native_process.c','src/common/codec/native_launch.c')
    $source|ForEach-Object{Get-FileHash (Join-Path $repo $_)}|ConvertTo-Json|Set-Content "$root\sources.json"
    foreach($arch in @('x64','x86')) {
        $commands=@('@echo off',('call "'+$VsDevCmd+'" -no_logo -arch='+$arch+' -host_arch=x64'),'if errorlevel 1 exit /b %errorlevel%')
        $flags='/nologo /MT /O2 /W4 /we4013 /std:c11 /D_WIN32_WINNT=0x0A00 /DWIN32_LEAN_AND_MEAN /I "'+$repo+'\src" /I "'+$repo+'\src\ntcon-exe"'
        $commands+=('cl.exe '+$flags+' /DVT_PRODUCER_ONLY /Fo"'+$root+'\producer-'+$arch+'.obj" /Fe"'+$root+'\producer-'+$arch+'.exe" "'+$repo+'\tests\observation\native_vt_fidelity_test.c" /link /incremental:no')
        $commands+='if errorlevel 1 exit /b %errorlevel%'
        if($arch -eq 'x64') {
            $objects=@();$i=0
            foreach($s in $source){$obj="$root\unit-$i.obj";$objects+='"'+$obj+'"';$i++
                $commands+=('cl.exe '+$flags+' /c /Fo"'+$obj+'" "'+(Join-Path $repo $s)+'"');$commands+='if errorlevel 1 exit /b %errorlevel%'}
            $libraries=($dependency|ForEach-Object{'"'+(Join-Path $FrontendBuild $_)+'"'}) -join ' '
            $commands+=('link.exe /nologo /machine:x64 /subsystem:console /opt:ref /out:"'+$root+'\verify.exe" '+($objects -join ' ')+' '+$libraries+' kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib')
            $commands+='if errorlevel 1 exit /b %errorlevel%'
        }
        $commands+='exit /b 0';$commands|Set-Content "$root\build-$arch.cmd"
        Run "build-$arch" $env:ComSpec ('/d /c ""'+$root+'\build-'+$arch+'.cmd""') 120
    }
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($arch in @('x64','x86')) {
        $args='"'+$root+'\verify.exe" "'+$root+'" "'+$root+'\observer-'+$arch+'.txt" "'+$root+'\result-'+$arch+'.txt" "'+$root+'\producer-'+$arch+'.exe" --observation-timeout-ms 60000'
        Run "verify-$arch" $Observer $args 90
        if((Get-Content "$root\observer-$arch.txt" -Raw) -notmatch 'exit=0x00000000'){throw "$arch verifier failed"}
        if((Get-Content "$root\result-$arch.txt" -Raw) -notmatch 'RESULT checks=\d+ failures=0'){throw "$arch assertions failed"}
    }
    Get-ChildItem $root -Filter '*.exe'|Get-FileHash|ConvertTo-Json|Set-Content "$root\artifacts.json"
    'PASS x86/x64 writers, real Console conversion, production capture/frame/projection/pixels and raw file/pipe bytes'|Set-Content "$root\result.txt"
    0|Set-Content "$root\exitcode.txt"
}catch{$_|Out-String|Set-Content "$root\failure.txt";1|Set-Content "$root\exitcode.txt";exit 1}
