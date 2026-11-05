[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$LogRoot,
    [ValidateSet('channel','service')][string[]]$Cases=@('channel')
)
$ErrorActionPreference='Stop'
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
$build=(Resolve-Path -LiteralPath $BuildRoot).Path
$logs=[IO.Path]::GetFullPath($LogRoot)
foreach($path in @($build,$logs)) {
    if(!$path.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
        throw 'Use this worktree build tree for tests and evidence'
    }
}
New-Item -ItemType Directory -Force -Path $logs | Out-Null
foreach($case in $Cases) {
    $exe=Join-Path $build "frontend-base-$case-test.exe"
    $stdout=Join-Path $logs "$case.stdout.log"
    $stderr=Join-Path $logs "$case.stderr.log"
    if((Test-Path $stdout) -or (Test-Path $stderr)){throw 'Use fresh test evidence paths'}
    $run=Start-Process -FilePath $exe -WorkingDirectory $build -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $record=[ordered]@{pid=$run.Id;startUtc=[DateTime]::UtcNow.ToString('o');image=$exe;sha256=(Get-FileHash $exe).Hash;timeoutSeconds=60;exitCode=$null}
    $record | ConvertTo-Json | Set-Content (Join-Path $logs "$case.process.json")
    if(!$run.WaitForExit(60000)) {
        Stop-Process -Id $run.Id -Force
        $run.WaitForExit()
        throw "Owned frontend-base $case test timed out"
    }
    $run.WaitForExit()
    $record.exitCode=$run.ExitCode
    $record | ConvertTo-Json | Set-Content (Join-Path $logs "$case.process.json")
    if($run.ExitCode -ne 0){Get-Content $stderr;throw "Frontend-base $case test failed"}
    if(!(Select-String -LiteralPath $stdout -SimpleMatch "PASS frontend-base $case" -Quiet)){throw 'Missing assertion witness'}
    Get-Content $stdout
}
