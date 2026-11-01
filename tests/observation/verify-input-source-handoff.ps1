[CmdletBinding()]
param([Parameter(Mandatory)][string]$LogRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$build=(Resolve-Path "$repo/build").Path+'\'
$root=[IO.Path]::GetFullPath($LogRoot)
if(!$root.StartsWith($build,[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)) {
    throw 'Require a fresh repository-build evidence directory'
}
$exe=Join-Path $repo 'build/.ninja/msvc-x64-release/frontend/console-channel-lifetime-test.exe'
if(!(Test-Path -LiteralPath $exe)){throw 'Build console-channel-lifetime-test.exe first'}
New-Item -ItemType Directory -Path $root | Out-Null
$flags=@('MVDM_TEST_SOURCE_HANDOFF','MVDM_TEST_SOURCE_UNREAD','MVDM_TEST_SOURCE_OWNER',
    'MVDM_TEST_SOURCE_RETURNED','MVDM_TEST_SOURCE_REPEAT_RETURN','MVDM_TEST_SOURCE_LATE_RETURN',
    'MVDM_TEST_SOURCE_LATE_UNREAD','MVDM_TEST_SOURCE_LATE_NEUTRAL','MVDM_TEST_SOURCE_WINDOW_OWNER',
    'MVDM_TEST_SOURCE_NO_RETURN','MVDM_TEST_SOURCE_LIVE_HISTORY','MVDM_TEST_INPUT_RECEIPT',
    'MVDM_TEST_INPUT_COMPACTION','MVDM_TEST_INPUT_MIXED_SOURCES')
$cases=[ordered]@{
    consumed=@('MVDM_TEST_SOURCE_HANDOFF');unread=@('MVDM_TEST_SOURCE_HANDOFF','MVDM_TEST_SOURCE_UNREAD')
    owner=@('MVDM_TEST_SOURCE_OWNER');returned=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_RETURNED')
    repeat=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_REPEAT_RETURN')
    late=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_LATE_RETURN')
    late_unread=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_LATE_UNREAD')
    late_neutral=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_LATE_NEUTRAL')
    window_owner=@('MVDM_TEST_SOURCE_WINDOW_OWNER');no_return=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_NO_RETURN')
    live_repeat=@('MVDM_TEST_SOURCE_OWNER','MVDM_TEST_SOURCE_LIVE_HISTORY')
    receipt=@('MVDM_TEST_INPUT_RECEIPT');compact=@('MVDM_TEST_INPUT_COMPACTION')
    mixed=@('MVDM_TEST_INPUT_MIXED_SOURCES')
}
$previous=@{};foreach($flag in $flags){$previous[$flag]=[Environment]::GetEnvironmentVariable($flag)}
$rows=@()
try {
    foreach($case in $cases.GetEnumerator()) {
        foreach($flag in $flags){Remove-Item "Env:$flag" -ErrorAction SilentlyContinue}
        foreach($flag in $case.Value){Set-Item "Env:$flag" '1'}
        $report=Join-Path $root "$($case.Key).txt"
        $p=Start-Process $exe -ArgumentList @('--private-desktop-full',('"'+$report+'"')) -WindowStyle Hidden -PassThru
        Write-Output "$($case.Key) pid=$($p.Id)"
        $p.WaitForExit()
        $rows+=@{name=$case.Key;pid=$p.Id;exit=$p.ExitCode;report=$report}
        if($p.ExitCode){Get-Content $report -Tail 6;break}
    }
} finally {
    foreach($flag in $flags){
        if($null -eq $previous[$flag]){Remove-Item "Env:$flag" -ErrorAction SilentlyContinue}
        else{Set-Item "Env:$flag" $previous[$flag]}
    }
    @{runner=$PID;image=(Get-FileHash $exe).Hash;cases=$rows}|
        ConvertTo-Json -Depth 5|Set-Content (Join-Path $root 'result.json')
}
if($rows.Count -ne $cases.Count -or @($rows|Where-Object exit -ne 0).Count){throw 'Input-source handoff regression failed'}
'PASS input-source handoff, returns and retirement controls'
