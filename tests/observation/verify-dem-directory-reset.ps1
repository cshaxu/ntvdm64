[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
    [string]$BuildCache='build/M0-T427/S2/r001')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh build evidence directory'}
New-Item -ItemType Directory $out | Out-Null
$wrapper=Join-Path $out 'compiler.cmd'
@('@echo off','call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul',
    'if errorlevel 1 exit /b %errorlevel%','%*') | Set-Content $wrapper -Encoding ASCII
$graph=Get-Content (Join-Path $repo "$BuildCache/build.ninja") -Raw
$match=[regex]::Match($graph,'(?m)^build obj/dem/demsrch.obj:.*\r?\n\s+cflags = (.+)$')
if(!$match.Success){throw 'Actual production DEM compile contract absent'}
$flags=$match.Groups[1].Value.Replace('$:',':')
if($flags -notmatch '/DCPU_40_STYLE\b' -or $flags -match '/DCPU_30_STYLE\b'){throw 'Wrong CPU profile'}
$source=Join-Path $repo 'tests/mvdm-host/dem_directory_reset_test.c'
$object=Join-Path $out 'dem_directory_reset_test.obj'
$rsp=Join-Path $out 'compile.rsp'
($flags+' /Gy /Fo"'+$object+'" "'+$source+'"') | Set-Content $rsp -Encoding ASCII
& $wrapper cl "@$rsp" 2>&1 | Tee-Object (Join-Path $out 'build.txt')
if($LASTEXITCODE){throw 'Compile failed'}
$generator=Get-Content (Join-Path $repo 'tools/build/New-T310OriginalSoftpcNinja.ps1') -Raw
$closure=[regex]::Match($generator,'(?m)^\$fixtureHostLibraries = ''([^'']+)''')
if(!$closure.Success){throw 'Selected fixture closure absent'}
$dependencies=@('obj/tests/ccpu_bounded_execution_fixture_seams.obj','obj/tests/ccpu_host_fixture_seams.obj')+
    $closure.Groups[1].Value.Split(' ')+@('common-root.lib','common-rpc.lib',
    'common-transport.lib','common-codec.lib','common-console.lib')
$libraries=$dependencies | ForEach-Object {
    $p=Join-Path (Join-Path $repo $BuildCache) $_
    if(!(Test-Path $p)){throw "Missing cache input $_"}; $p
}
$exe=Join-Path $out 'dem-directory-reset-test.exe'
# Existing host-fixture seam overrides, not forced unresolved symbols.
& $wrapper link /nologo /OPT:REF /force:multiple /alternatename:_call_ica_hw_interrupt=_ica_hw_interrupt "/OUT:$exe" "/MAP:$exe.map" $object @libraries kernel32.lib ntdll.lib user32.lib gdi32.lib advapi32.lib rpcrt4.lib shell32.lib legacy_stdio_definitions.lib 2>&1 |
    Tee-Object (Join-Path $out 'build.txt') -Append
if($LASTEXITCODE){throw 'Link failed'}
$map=Get-Content "$exe.map" -Raw
foreach($symbol in @('FileFindOpen','FileFindReset','FileFindNext','FillFCBSrchBuf')){
    if($map -notmatch "(?m)^.*_$symbol\s+.*\bdem_directory_reset_test\.obj\s*$"){
        throw "Actual original function ownership missing: $symbol"
    }
}
$start=[Diagnostics.ProcessStartInfo]::new()
$start.FileName=$exe; $start.UseShellExecute=$false; $start.CreateNoWindow=$true
$start.RedirectStandardOutput=$true; $start.RedirectStandardError=$true
$start.ArgumentList.Add((Join-Path $out 'files'))
$p=[Diagnostics.Process]::new(); $p.StartInfo=$start
$clock=[Diagnostics.Stopwatch]::StartNew()
try{
    if(!$p.Start()){throw 'Fixture failed to start'}
    $stdout=$p.StandardOutput.ReadToEndAsync(); $stderr=$p.StandardError.ReadToEndAsync()
    if(!$p.WaitForExit(10000)){$p.Kill(); $p.WaitForExit(); throw 'Timeout, not a pass'}
    $result=$stdout.Result+$stderr.Result
    $result | Set-Content (Join-Path $out 'result.txt')
    [pscustomobject]@{exit_code=$p.ExitCode;elapsed_ms=$clock.ElapsedMilliseconds;
        original_sha256=(Get-FileHash (Join-Path $repo 'src/mvdm/dos/dem/demsrch.c')).Hash;
        executable_sha256=(Get-FileHash $exe).Hash;
        scope='Actual original DEM functions and real NT directory calls; provider fixture, not guest INT21 proof'
    } | ConvertTo-Json | Set-Content (Join-Path $out 'result.json')
    Write-Output $result
    if($p.ExitCode -or $result -notmatch 'assertions=47 failures=0'){throw 'DEM assertions failed'}
}finally{$p.Dispose()}
