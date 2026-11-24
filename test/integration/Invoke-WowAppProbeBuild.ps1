# Runtime runners delegate production to the canonical CMD/native graph.
function Invoke-WowAppProbeBuild {
    param([Parameter(Mandatory)][string]$OutputRoot,
        [Parameter(Mandatory)][ValidateSet('sol-gameplay','sol-options','write-lifecycle')][string]$Name)
    $repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
    $command='/d /s /c ""'+$repo+'/tools/build-wow-app-probe.cmd" '+$Name+' "'+$OutputRoot+'""'
    $p=Start-Process $env:ComSpec -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$OutputRoot/compile.txt" -RedirectStandardError "$OutputRoot/compile.stderr.txt"
    try {
        $null=$p.Handle
        @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$command;timeoutSeconds=120}|ConvertTo-Json|Set-Content "$OutputRoot/producer.json"
        if(!$p.WaitForExit(120000)){$p.Kill($true);$p.WaitForExit();throw 'App probe producer deadline'}
        $p.WaitForExit();$p.ExitCode|Set-Content "$OutputRoot/producer.exit.txt"
        if($p.ExitCode){throw "App probe producer failed: $($p.ExitCode)"}
    } finally {$p.Dispose()}
}
