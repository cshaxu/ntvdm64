[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ReportRoot)
$ErrorActionPreference='Stop'
$build=(Resolve-Path -LiteralPath $BuildRoot).Path
$report=[IO.Path]::GetFullPath($ReportRoot)
$null=New-Item -ItemType Directory -Force -Path $report
$results=@()
foreach($case in @('ntvdm-command-capture-test','ntvdm-command-launch-test')) {
    $image=Join-Path $build ($case+'.exe')
    $run=Start-Process $image -WorkingDirectory $build -WindowStyle Hidden -PassThru `
        -RedirectStandardOutput (Join-Path $report ($case+'.stdout')) `
        -RedirectStandardError (Join-Path $report ($case+'.stderr'))
    $row=[ordered]@{Case=$case;Image=$image;SHA256=(Get-FileHash -LiteralPath $image).Hash
        PID=$run.Id;Started=$run.StartTime.ToUniversalTime().ToString('o')}
    $finished=$run.WaitForExit(60000) # Fixture deadline, not a product timeout.
    if(!$finished){$run.Kill();$run.WaitForExit()}
    $row.Exit=$run.ExitCode;$row.TimedOut=!$finished
    $row.Passed=$finished -and $run.ExitCode -eq 0
    $results+=,[pscustomobject]$row
    $results | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $report 'results.json') -Encoding utf8
    if(!$row.Passed){throw "$case failed: exit=$($run.ExitCode), timedOut=$(!$finished)"}
}
Write-Output 'PASS independent command capture and launch'
