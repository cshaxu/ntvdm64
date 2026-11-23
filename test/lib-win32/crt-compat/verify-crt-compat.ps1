[CmdletBinding()]
param([Parameter(Mandatory)][string]$OutputRoot,
      [Parameter(Mandatory)][string]$WorkerBuildRoot,
      [Parameter(Mandatory)][string]$WowBuildRoot)
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$out = [IO.Path]::GetFullPath((Join-Path $root $OutputRoot))
if (!$out.StartsWith(([IO.Path]::GetFullPath((Join-Path $root 'build')) + '\'),
    [StringComparison]::OrdinalIgnoreCase) -or (Test-Path $out)) {
    throw 'Use a fresh directory below build/.'
}
New-Item -ItemType Directory -Path $out | Out-Null
function NativeBuild($target,$log) {
    $line='/d /s /c ""'+$root+'/tools/build-fixtures.cmd" x86 '+$target+' "'+$WowBuildRoot+'""'
    $p=Start-Process $env:ComSpec -ArgumentList $line -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$out/$log.log" -RedirectStandardError "$out/$log.stderr.log"
    try {
        $null=$p.Handle
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$line;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$out/$log.process.json"
        if(!$p.WaitForExit(180000)){$p.Kill($true);$p.WaitForExit();throw 'CRT producer deadline'}
        $p.WaitForExit();$p.ExitCode|Set-Content "$out/$log.exit.txt"
        return $p.ExitCode
    } finally {$p.Dispose()}
}
if((NativeBuild crt-contracts build) -ne 0){throw 'Native CRT producer failed'}
$native=Join-Path $WowBuildRoot 'tests/crt'
$worker = Get-Content (Join-Path $WorkerBuildRoot 'build.ninja') -Raw
$wow = Get-Content (Join-Path $WowBuildRoot 'build.ninja') -Raw
$wowCrtAbsolute=(Get-Content (Join-Path $native 'production-object.txt') -Raw).Trim()
$wowCrtObject=[IO.Path]::GetRelativePath((Resolve-Path $WowBuildRoot).Path,$wowCrtAbsolute).Replace('/','\')
$compile = & ninja -C $WowBuildRoot -t commands $wowCrtObject
if ($LASTEXITCODE -or !$compile -or $compile -match '(?i)/FI\s*"?.*?src[/\\]compat[/\\]') {
    throw 'WOW32 CRT must compile independently of the historical forced include.'
}
$headers = & ninja -C $WowBuildRoot -t deps $wowCrtObject
if ($LASTEXITCODE -or !($headers -match '\(VALID\)') -or
    $headers -match 'src[/\\](?:compat|mvdm-host|non-mvdm)[/\\]') {
    throw 'Missing WOW32 CRT dependency record or unexpected OpenNT header.'
}
if ($worker -notmatch 'src/lib-win32/crt-compat/crt_compat\.c' -or
    $worker -match 'src/ntvdm-exe/win32/crt_compat\.c' -or
    $worker -notmatch 'build ntvdm\.exe[^\r\n]*lib-crt-compat\.lib' -or
    $wow -notmatch 'src/lib-win32/crt-compat/crt_compat\.c' -or
    $wow -match 'src/ntvdm-exe/win32/crt_compat\.c') {
    throw 'Production CRT source ownership mismatch.'
}
$workerMap = Get-Content (Join-Path $WorkerBuildRoot 'ntvdm.exe.map') -Raw
$wowMap = Get-Content (Join-Path $WowBuildRoot 'wow32.dll.map') -Raw
foreach ($symbol in @('_sprintf','_sscanf')) {
    if ($workerMap -notmatch ("(?m)^.*\s" + $symbol + "\s+.*lib-crt-compat:crt_compat(?:\.c)?\.obj\s*$")) {
        throw "NTVDM CRT provider mismatch: $symbol"
    }
}
if ($wowMap -notmatch '(?m)^.*\s_sprintf\s+.*crt_compat(?:\.c)?\.obj\s*$') {
    throw 'WOW32 CRT provider mismatch: sprintf'
}
Copy-Item "$native/standalone.exe" "$out/crt-test.exe"
Copy-Item "$native/production.exe" "$out/wow-crt-test.exe"
& "$out/crt-test.exe" *> "$out/result.log"
if($LASTEXITCODE){throw 'Formatting behavior failed'}
& "$out/wow-crt-test.exe" *> "$out/result-wow.log"
if($LASTEXITCODE){throw 'Production WOW32 CRT formatting behavior failed'}
$missing=NativeBuild crt-missing missing
if(!$missing -or (Get-Content "$out/missing.log" -Raw) -notmatch 'LNK2019') {
    throw 'Missing provider was not rejected'
}
$duplicate=NativeBuild crt-duplicate duplicate
if(!$duplicate -or (Get-Content "$out/duplicate.log" -Raw) -notmatch 'LNK2005') {
    throw 'Duplicate provider was not rejected'
}
Write-Output 'PASS CRT source ownership, WOW32 header isolation, production object formatting and missing/duplicate provider controls'
