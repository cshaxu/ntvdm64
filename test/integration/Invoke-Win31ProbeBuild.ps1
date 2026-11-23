# Assemble only authored witnesses; runtime fields remain owned by the caller.
function Invoke-Win31ProbeBuild {
    param([Parameter(Mandatory)][string]$OutputRoot,[string]$WindowsProbeImage)
    $repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
    $target=if($WindowsProbeImage){'win31-dosonly'}else{'win31-matrix'}
    $command='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 '+$target+'"'
    $p=Start-Process $env:ComSpec -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$OutputRoot/assemble.txt" -RedirectStandardError "$OutputRoot/assemble.stderr.txt"
    try {
        $null=$p.Handle
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$command;timeoutSeconds=120}|ConvertTo-Json|Set-Content "$OutputRoot/producer.json"
        if(!$p.WaitForExit(120000)){$p.Kill($true);$p.WaitForExit();throw 'Win31 witness producer deadline'}
        $p.WaitForExit();$p.ExitCode|Set-Content "$OutputRoot/producer.exit.txt"
        if($p.ExitCode){throw 'Win31 witness producer failed'}
    } finally {$p.Dispose()}
    $cache="$repo/build/.ninja/fixtures-x86/guest-probes/win31-matrix"
    if(!$WindowsProbeImage){$WindowsProbeImage="$cache/W31CHECK.EXE"}
    Copy-Item -LiteralPath $WindowsProbeImage -Destination "$OutputRoot/W31CHECK.EXE"
    Copy-Item -LiteralPath "$cache/W31DOS.COM" -Destination "$OutputRoot/W31DOS.COM"
}
