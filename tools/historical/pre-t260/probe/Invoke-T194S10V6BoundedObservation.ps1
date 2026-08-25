[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)][string]$Runner,
    [Parameter(Mandatory = $true)][string]$Shim,
    [Parameter(Mandatory = $true)][string]$BundleEngine,
    [Parameter(Mandatory = $true)][string]$Profile,
    [Parameter(Mandatory = $true)][string]$ByobRoot,
    [Parameter(Mandatory = $true)][string]$Target,
    [Parameter(Mandatory = $true)][string]$EvidenceRoot,
    [ValidateRange(1, 120)][int]$WatchdogSeconds = 30
)

$ErrorActionPreference = 'Stop'

function Resolve-Leaf([string]$Path, [string]$Name) {
    $resolved = (Resolve-Path -LiteralPath $Path -ErrorAction Stop).Path
    if (-not (Test-Path -LiteralPath $resolved -PathType Leaf)) { throw "$Name is not a file: $Path" }
    return $resolved
}
function Quote-CmdArgument([string]$Value) { return '"' + ($Value -replace '"', '""') + '"' }
function Try-Hash([string]$Path) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) { return $null }
    try { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256 -ErrorAction Stop).Hash }
    catch { return $null }
}

$runnerPath = Resolve-Leaf $Runner 'Runner'
$shimPath = Resolve-Leaf $Shim 'Shim'
$bundleEnginePath = Resolve-Leaf $BundleEngine 'Bundle engine'
$profilePath = Resolve-Leaf $Profile 'Profile'
$targetPath = Resolve-Leaf $Target 'Target'
$rootPath = (Resolve-Path -LiteralPath $ByobRoot -ErrorAction Stop).Path
if (-not (Test-Path -LiteralPath $rootPath -PathType Container)) { throw "BYOB root is not a directory: $ByobRoot" }
$profileText = Get-Content -LiteralPath $profilePath -Raw
if ($profileText -notmatch '"schema"\s*:\s*"runner-byob-profile-v6"' -or
    $profileText -notmatch '"profile"\s*:\s*"nt4-en-us-command-normal-return-v6"') {
    throw 'Profile is not the admitted T194 v6 normal-return profile.'
}
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing evidence directory: $evidence" }

New-Item -ItemType Directory -Path $evidence -ErrorAction Stop | Out-Null
$stdoutPath = Join-Path $evidence 'stdout.log'
$stderrPath = Join-Path $evidence 'stderr.log'
$exitPath = Join-Path $evidence 'child-exitcode.txt'
$wrapperPath = Join-Path $evidence 'run-v6-one-slot.cmd'
$inputsPath = Join-Path $evidence 'inputs.json'
$outcomePath = Join-Path $evidence 'observation.json'
$arguments = @('--engine', $shimPath, '--bochs', $bundleEnginePath,
    '--byob-profile', $profilePath, '--byob-root', $rootPath, $targetPath)
$inputRecord = [ordered]@{
    schema = 'runner.t194.v6-one-slot-observation-inputs.v1'
    runner = [ordered]@{ path = $runnerPath; sha256 = (Get-FileHash -LiteralPath $runnerPath -Algorithm SHA256).Hash }
    shim = [ordered]@{ path = $shimPath; sha256 = (Get-FileHash -LiteralPath $shimPath -Algorithm SHA256).Hash }
    bundleEngine = [ordered]@{ path = $bundleEnginePath; sha256 = (Get-FileHash -LiteralPath $bundleEnginePath -Algorithm SHA256).Hash }
    profile = [ordered]@{ path = $profilePath; sha256 = (Get-FileHash -LiteralPath $profilePath -Algorithm SHA256).Hash }
    byobRoot = $rootPath
    target = [ordered]@{ path = $targetPath; sha256 = (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash }
    arguments = $arguments
    watchdogSeconds = $WatchdogSeconds
    retryPolicy = 'none'
}
[IO.File]::WriteAllText($inputsPath, ($inputRecord | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))

$command = (Quote-CmdArgument $runnerPath) + ' ' + (($arguments | ForEach-Object { Quote-CmdArgument $_ }) -join ' ')
$wrapper = @"
@echo off
setlocal
$command > $(Quote-CmdArgument $stdoutPath) 2> $(Quote-CmdArgument $stderrPath)
set "RUNNER_EXITCODE=%ERRORLEVEL%"
> $(Quote-CmdArgument $exitPath) echo %RUNNER_EXITCODE%
exit /b %RUNNER_EXITCODE%
"@
[IO.File]::WriteAllText($wrapperPath, $wrapper, [Text.UTF8Encoding]::new($false))

$started = [DateTime]::UtcNow
$process = $null; $launchError = $null; $watchdogTerminated = $false
$treeKillExit = $null; $treeKillOutput = $null; $cleanupWaitTimedOut = $false
$exitCode = $null
try {
    $process = Start-Process -FilePath $env:ComSpec -ArgumentList @('/d', '/c', $wrapperPath) -WindowStyle Hidden -PassThru
    if (-not $process.WaitForExit($WatchdogSeconds * 1000)) {
        $watchdogTerminated = $true
        $treeKillOutput = (& taskkill.exe /T /F /PID $process.Id 2>&1 | Out-String).Trim()
        $treeKillExit = $LASTEXITCODE
        if (-not $process.WaitForExit(5000)) { $cleanupWaitTimedOut = $true }
    }
    $exitCode = if (Test-Path -LiteralPath $exitPath -PathType Leaf) {
        [int](Get-Content -LiteralPath $exitPath -Raw).Trim()
    } elseif ($process.HasExited) { $process.ExitCode } else { $null }
} catch { $launchError = $_.Exception.Message }
finally {
    $record = [ordered]@{
        schema = 'runner.t194.v6-one-slot-observation.v1'
        startedUtc = $started.ToString('o')
        elapsedMilliseconds = [int](([DateTime]::UtcNow - $started).TotalMilliseconds)
        wrapperPid = if ($null -ne $process) { $process.Id } else { $null }
        watchdogSeconds = $WatchdogSeconds; watchdogTerminated = $watchdogTerminated
        treeKillExit = $treeKillExit; treeKillOutput = $treeKillOutput
        cleanupWaitTimedOut = $cleanupWaitTimedOut; exitCode = $exitCode; launchError = $launchError
        stdoutSha256 = Try-Hash $stdoutPath
        stderrSha256 = Try-Hash $stderrPath
        interpretation = 'one mechanical process-tree observation; normal COMMAND return requires an observed 54:11 marker'
    }
    [IO.File]::WriteAllText($outcomePath, ($record | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
    if ($null -ne $process) { $process.Dispose() }
}
if ($null -ne $launchError) { throw "V6 observation launch failed; retained evidence: $outcomePath" }
Write-Host "V6 one-slot observation retained: $evidence"
