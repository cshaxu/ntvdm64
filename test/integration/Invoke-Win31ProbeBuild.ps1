# Assemble only authored witnesses; runtime fields remain owned by the caller.
function Invoke-Win31ProbeBuild {
    param([Parameter(Mandatory)][string]$OutputRoot,[string]$WindowsProbeImage)
    $repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
    $target=if($WindowsProbeImage){'win31-dosonly'}else{'win31-matrix'}
    $command='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x64 '+$target+'"'
    $p=Start-Process $env:ComSpec -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$OutputRoot/assemble.txt" -RedirectStandardError "$OutputRoot/assemble.stderr.txt"
    try {
        $null=$p.Handle
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$command;timeoutSeconds=120}|ConvertTo-Json|Set-Content "$OutputRoot/producer.json"
        if(!$p.WaitForExit(120000)){$p.Kill($true);$p.WaitForExit();throw 'Win31 witness producer deadline'}
        $p.WaitForExit();$p.ExitCode|Set-Content "$OutputRoot/producer.exit.txt"
        if($p.ExitCode){throw 'Win31 witness producer failed'}
    } finally {$p.Dispose()}
    & "$repo/tools/build-fixtures.cmd" x64 win31-probe-paths | Out-Host
    if($LASTEXITCODE){throw 'Win31 path binder build failed'}
    $cache="$repo/build/.ninja/fixtures-x64/guest-probes/win31-matrix"
    if(!$WindowsProbeImage){$WindowsProbeImage="$cache/W31CHECK.EXE"}
    & "$repo/tools/prepare-test-inputs.cmd" $OutputRoot $WindowsProbeImage W31CHECK.EXE "$cache/W31DOS.COM" W31DOS.COM
    if($LASTEXITCODE){throw 'Win31 witness input preparation failed'}
}
