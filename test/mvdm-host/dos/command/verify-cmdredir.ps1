[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot,[Parameter(Mandatory)][string]$ReportRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../../../..").Path
$build=(Resolve-Path $BuildRoot).Path
$report=[IO.Path]::GetFullPath($ReportRoot)
$null=New-Item -ItemType Directory -Force $report
$command='/d /s /c ""'+$repo+'/tools/build-fixtures.cmd" x86 command-redirection "'+$build+'""'
$producer=Start-Process $env:ComSpec -ArgumentList $command -WorkingDirectory $repo -WindowStyle Hidden -PassThru -RedirectStandardOutput "$report/build.log" -RedirectStandardError "$report/build.stderr.log"
try {
    $null=$producer.Handle
    @{pid=$producer.Id;start=$producer.StartTime.ToUniversalTime().ToString('o');command=$command;timeoutSeconds=180}|ConvertTo-Json|Set-Content "$report/producer.json"
    if(!$producer.WaitForExit(180000)){$producer.Kill($true);$producer.WaitForExit();throw 'COMMAND producer deadline'}
    $producer.WaitForExit();$producer.ExitCode|Set-Content "$report/producer.exit.txt"
    if($producer.ExitCode){throw 'Pipe fixture build failed'}
} finally {$producer.Dispose()}
Copy-Item "$build/command-redirection-fixture.exe" "$report/pipe-fixture.exe"
$p=Start-Process "$report/pipe-fixture.exe" -WindowStyle Hidden -PassThru -RedirectStandardOutput "$report/test.stdout.txt" -RedirectStandardError "$report/test.stderr.txt"
$null=$p.Handle
$row=[ordered]@{PID=$p.Id;Started=$p.StartTime.ToUniversalTime().ToString('o');ObjectSHA256=(Get-FileHash "$build/obj/command/cmdredir.obj").Hash}
$done=$p.WaitForExit(15000) # Test deadline only.
if(!$done){$p.Kill();$p.WaitForExit()}
$p.WaitForExit();$row.Exit=$p.ExitCode;$row.TimedOut=!$done
$row|ConvertTo-Json|Set-Content "$report/result.json" -Encoding utf8
if(!$done -or $p.ExitCode -ne 0){throw 'Pipe byte/exit assertions failed'}
$p.Dispose()
'PASS original COMMAND pipe input/output bytes and thread exit'
