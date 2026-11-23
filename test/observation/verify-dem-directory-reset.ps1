[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$out=[IO.Path]::GetFullPath((Join-Path $repo $OutputRoot))
if(!$out.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $out)){throw 'Require a fresh build evidence directory'}
New-Item -ItemType Directory $out | Out-Null
$cache=Join-Path $repo 'build/.ninja/msvc-x86-release/worker'
& "$repo/tools/build-fixtures.cmd" x86 dem-directory-reset *> "$out/build.txt"
if($LASTEXITCODE){throw 'DEM fixture build failed'}
$exe=Join-Path $out 'dem-directory-reset-test.exe'
Copy-Item (Join-Path $cache 'dem-directory-reset-test.exe') $exe
Copy-Item (Join-Path $cache 'dem-directory-reset-test.exe.map') "$exe.map"
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
        original_sha256=(Get-FileHash (Join-Path $repo 'src/mvdm-host/dos/dem/demsrch.c')).Hash;
        executable_sha256=(Get-FileHash $exe).Hash;
        scope='Actual original DEM functions and real NT directory calls; provider fixture, not guest INT21 proof'
    } | ConvertTo-Json | Set-Content (Join-Path $out 'result.json')
    Write-Output $result
    if($p.ExitCode -or $result -notmatch 'assertions=115 failures=0'){throw 'DEM assertions failed'}
}finally{$p.Dispose()}
