[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$ReportRoot
)
$ErrorActionPreference='Stop'
$build=(Resolve-Path -LiteralPath $BuildRoot).Path
$report=[IO.Path]::GetFullPath($ReportRoot)
$null=New-Item -ItemType Directory -Force -Path $report
$cases=@(
    @{Name='registry';Image='ntvdm-vm-frontend-test.exe';Arguments=@();Expected=0},
    @{Name='natural-exit';Image='console-client-test.exe';Arguments=@('--natural-exit');Expected=0},
    @{Name='natural-loss';Image='console-client-test.exe';Arguments=@('--natural-loss');Expected=0},
    @{Name='natural-console';Image='console-client-test.exe';Arguments=@('--natural-console');Expected=0},
    @{Name='lifecycle-console';Image='console-client-test.exe';Arguments=@('--lifecycle-console');Expected=73},
    @{Name='lifecycle-wow';Image='console-client-test.exe';Arguments=@('--lifecycle-wow');Expected=74},
    @{Name='lifecycle-loss';Image='console-client-test.exe';Arguments=@('--lifecycle-loss');Expected=74},
    @{Name='lifecycle-pending';Image='console-client-test.exe';Arguments=@('--lifecycle-pending');Expected=73}
)
$results=@()
foreach($case in $cases) {
    $image=Join-Path $build $case.Image
    $arguments=@{FilePath=$image;WorkingDirectory=$build;WindowStyle='Hidden';PassThru=$true
        RedirectStandardOutput=(Join-Path $report ($case.Name+'.stdout'))
        RedirectStandardError=(Join-Path $report ($case.Name+'.stderr'))}
    if($case.Arguments.Count){$arguments.ArgumentList=$case.Arguments}
    $process=Start-Process @arguments
    $row=[ordered]@{Case=$case.Name;Image=$image;SHA256=(Get-FileHash -LiteralPath $image).Hash
        PID=$process.Id;Started=$process.StartTime.ToUniversalTime().ToString('o');Expected=$case.Expected}
    $finished=$process.WaitForExit(60000) # Fixture budget, never a product timeout.
    if(!$finished){$process.Kill();$process.WaitForExit()}
    $row.Exit=$process.ExitCode;$row.TimedOut=!$finished
    $row.Passed=$finished -and $process.ExitCode -eq $case.Expected
    $results+=,[pscustomobject]$row
    $results | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $report 'results.json') -Encoding utf8
    if(!$row.Passed){throw "$($case.Name) failed: exit=$($process.ExitCode), timedOut=$(!$finished)"}
}
Write-Output "PASS $($results.Count) VM frontend registry and lifecycle cases"
