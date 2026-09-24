param([string]$RuntimeRoot='O:/winnt', [ValidateRange(30,600)][int]$TimeoutSeconds=180)
$ErrorActionPreference='Stop'
$runtime=(Resolve-Path $RuntimeRoot).Path
$paths=@('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object {Join-Path $runtime $_}
if (@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths}).Count) {
    throw 'Package is in use; refusing to share the observation worker'
}
$run='t422-s2-'+[DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ')+'-'+[guid]::NewGuid().ToString('N').Substring(0,8)+'-visual'
$logs=Join-Path $runtime ('logs/'+$run)
if (Test-Path $logs) {throw 'Run already exists'}
New-Item -ItemType Directory $logs | Out-Null
$profile=Join-Path $runtime 'SYSTEM.INI'
$original=[IO.File]::ReadAllBytes($profile)
$profileHash=(Get-FileHash $profile).Hash
$launcher=$null
$exitCode=$null
$forced=$false
try {
    # Same diagnostic profile as the established window-lifecycle observer.
    $text=[Text.Encoding]::Default.GetString($original)
    $text=[regex]::Replace($text,'(?im)^network\.drv\s*=.*(?:\r?\n|$)','')
    if ($text -notmatch '(?im)^\[boot\]\s*\r?$') {$text+="`r`n[boot]`r`n"}
    $text=[regex]::Replace($text,'(?im)^(\[boot\]\s*\r?\n)',"`$1; T422 S2 temporary visual observation`r`nnetwork.drv=wfwnet.drv`r`n")
    [IO.File]::WriteAllText($profile,$text,[Text.Encoding]::Default)
    $keep=@('SystemRoot','windir','ComSpec','TEMP','TMP','USERPROFILE','APPDATA','LOCALAPPDATA','USERNAME','USERDOMAIN','COMPUTERNAME','SystemDrive','OS','PROCESSOR_ARCHITECTURE','NUMBER_OF_PROCESSORS')
    Get-ChildItem Env: | Where-Object {$_.Name -notin $keep} | ForEach-Object {Remove-Item -LiteralPath ('Env:'+$_.Name)}
    $env:PATH="$env:SystemRoot\System32"
    $env:MVDM_WOW_WINDOW_TRACE_PATH=Join-Path $logs 'window.log'
    [ordered]@{run_id=$run;created_utc=[DateTime]::UtcNow.ToString('o');
        question='Observe real immutable WINMINE rendering and input separately from HWND lifecycle';
        command='run16.exe WINMINE.EXE';cwd=$runtime;timeout_seconds=$TimeoutSeconds;
        environment='reduced allowlist; diagnostic only';profile_before=$profileHash;
        profile_test=(Get-FileHash $profile).Hash;evidence_complete=$false;
        limitation='No frozen source/toolchain inputs; UI observations collected separately through computer-use';
        artifacts=@(@('run16.exe','ntvdm.exe','basesrv.exe','WOW32.DLL','VDMREDIR.DLL','WINMINE.EXE') | ForEach-Object {Get-FileHash (Join-Path $runtime $_)})
    } | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $logs 'manifest.json')
    # This is the interactive app whose GUI must be visible to the observer.
    $launcher=Start-Process (Join-Path $runtime 'run16.exe') -ArgumentList 'WINMINE.EXE' -WorkingDirectory $runtime -WindowStyle Normal -PassThru
    Write-Output "run_id=$run launcher=$($launcher.Id) logs=$logs"
    if ($launcher.WaitForExit($TimeoutSeconds*1000)) {$exitCode=$launcher.ExitCode}
    else {$forced=$true}
} finally {
    # The preflight forbids existing package processes. Collect only this
    # launcher's exact-path descendant tree; leave dtmgr and other apps alone.
    if ($launcher) {
        $owned=@($launcher.Id)
        $all=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})
        for ($n=0;$n -lt 5;$n++) {
            $children=@($all | Where-Object {$_.ParentProcessId -in $owned -and $_.ProcessId -notin $owned})
            if (!$children.Count) {break}
            $owned+=@($children.ProcessId)
        }
        foreach ($p in $all) {if ($p.ProcessId -in $owned) {Stop-Process -Id $p.ProcessId -Force -ErrorAction SilentlyContinue}}
    }
    [IO.File]::WriteAllBytes($profile,$original)
    [ordered]@{run_id=$run;launcher_exit=$exitCode;timeout=$forced;
        profile_restored=((Get-FileHash $profile).Hash -eq $profileHash);
        verdict='INCOMPLETE: inspect independent visual observations';evidence_complete=$false
    } | ConvertTo-Json | Set-Content (Join-Path $logs 'result.json')
}
