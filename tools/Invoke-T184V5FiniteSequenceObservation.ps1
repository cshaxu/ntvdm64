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
function Quote-CmdArgument([string]$Value) {
    return '"' + ($Value -replace '"', '""') + '"'
}

$runnerPath = Resolve-Leaf $Runner 'Runner'
$shimPath = Resolve-Leaf $Shim 'Shim'
$bundleEnginePath = Resolve-Leaf $BundleEngine 'Bundle engine'
$profilePath = Resolve-Leaf $Profile 'Profile'
$targetPath = Resolve-Leaf $Target 'Target'
$rootPath = (Resolve-Path -LiteralPath $ByobRoot -ErrorAction Stop).Path
if (-not (Test-Path -LiteralPath $rootPath -PathType Container)) { throw "BYOB root is not a directory: $ByobRoot" }
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing evidence directory: $evidence" }

New-Item -ItemType Directory -Path $evidence -ErrorAction Stop | Out-Null
$stdoutPath = Join-Path $evidence 'stdout.log'
$stderrPath = Join-Path $evidence 'stderr.log'
$exitPath = Join-Path $evidence 'child-exitcode.txt'
$wrapperPath = Join-Path $evidence 'run-v5-finite-sequence.cmd'
$inputsPath = Join-Path $evidence 'inputs.json'
$outcomePath = Join-Path $evidence 'observation.json'
$arguments = @('--engine', $shimPath, '--bochs', $bundleEnginePath,
    '--byob-profile', $profilePath, '--byob-root', $rootPath, $targetPath)
$inputRecord = [ordered]@{
    schema = 'ntdos64.t184.v5-finite-sequence-inputs.v1'
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
set "NTDOS64_EXITCODE=%ERRORLEVEL%"
> $(Quote-CmdArgument $exitPath) echo %NTDOS64_EXITCODE%
exit /b %NTDOS64_EXITCODE%
"@
[IO.File]::WriteAllText($wrapperPath, $wrapper, [Text.UTF8Encoding]::new($false))

$started = [DateTime]::UtcNow
$process = $null
$launchError = $null
$watchdogTerminated = $false
$treeKillExit = $null
$exitCode = $null
try {
    $process = Start-Process -FilePath $env:ComSpec -ArgumentList @('/d', '/c', $wrapperPath) -WindowStyle Hidden -PassThru
    if (-not $process.WaitForExit($WatchdogSeconds * 1000)) {
        $watchdogTerminated = $true
        & taskkill.exe /T /F /PID $process.Id | Out-Null
        $treeKillExit = $LASTEXITCODE
        $process.WaitForExit()
    }
    $exitCode = if (Test-Path -LiteralPath $exitPath -PathType Leaf) {
        [int](Get-Content -LiteralPath $exitPath -Raw).Trim()
    } else { $process.ExitCode }
} catch {
    $launchError = $_.Exception.Message
} finally {
    $record = [ordered]@{
        schema = 'ntdos64.t184.v5-finite-sequence-observation.v1'
        startedUtc = $started.ToString('o')
        elapsedMilliseconds = [int](([DateTime]::UtcNow - $started).TotalMilliseconds)
        wrapperPid = if ($null -ne $process) { $process.Id } else { $null }
        watchdogSeconds = $WatchdogSeconds
        watchdogTerminated = $watchdogTerminated
        treeKillExit = $treeKillExit
        exitCode = $exitCode
        launchError = $launchError
        stdoutSha256 = if (Test-Path -LiteralPath $stdoutPath) { (Get-FileHash -LiteralPath $stdoutPath -Algorithm SHA256).Hash } else { $null }
        stderrSha256 = if (Test-Path -LiteralPath $stderrPath) { (Get-FileHash -LiteralPath $stderrPath -Algorithm SHA256).Hash } else { $null }
        interpretation = 'one mechanical process-tree observation; no normal DOS result or CLI success is inferred'
    }
    [IO.File]::WriteAllText($outcomePath, ($record | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
    if ($null -ne $process) { $process.Dispose() }
}
if ($null -ne $launchError) { throw "V5 observation launch failed; retained evidence: $outcomePath" }
Write-Host "V5 finite-sequence observation retained: $evidence"
