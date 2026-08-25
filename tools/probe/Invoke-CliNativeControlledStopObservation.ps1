[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$Runner,
    [Parameter(Mandatory = $true)] [string]$Engine,
    [Parameter(Mandatory = $true)] [string]$Profile,
    [Parameter(Mandatory = $true)] [string]$ByobRoot,
    [Parameter(Mandatory = $true)] [string]$Bochs,
    [Parameter(Mandatory = $true)] [string]$Target,
    [Parameter(Mandatory = $true)] [string]$EvidenceRoot,
    [ValidateRange(1, 120)] [int]$WatchdogSeconds = 45
)

$ErrorActionPreference = 'Stop'

function Resolve-Leaf([string]$Path, [string]$Name) {
    $resolved = (Resolve-Path -LiteralPath $Path -ErrorAction Stop).Path
    if (-not (Test-Path -LiteralPath $resolved -PathType Leaf)) {
        throw "$Name is not a file: $Path"
    }
    return $resolved
}

function Quote-WindowsArgument([string]$Value) {
    if ($Value -notmatch '[\s"]') { return $Value }
    $escaped = [regex]::Replace($Value, '(\\*)"', '$1$1\\"')
    $escaped = [regex]::Replace($escaped, '(\\+)$', '$1$1')
    return '"' + $escaped + '"'
}

$runnerPath = Resolve-Leaf $Runner 'Runner'
$enginePath = Resolve-Leaf $Engine 'Engine'
$profilePath = Resolve-Leaf $Profile 'Profile'
$bochsPath = Resolve-Leaf $Bochs 'Bochs'
$targetPath = Resolve-Leaf $Target 'Target'
$rootPath = (Resolve-Path -LiteralPath $ByobRoot -ErrorAction Stop).Path
if (-not (Test-Path -LiteralPath $rootPath -PathType Container)) { throw "BYOB root is not a directory: $ByobRoot" }
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing observation directory: $evidence" }

New-Item -ItemType Directory -Path $evidence -ErrorAction Stop | Out-Null
$stdoutPath = Join-Path $evidence 'stdout.log'
$stderrPath = Join-Path $evidence 'stderr.log'
$inputsPath = Join-Path $evidence 'inputs.json'
$resultPath = Join-Path $evidence 'observation.json'
$started = [DateTime]::UtcNow
$process = $null
$timedOut = $false
$launchError = $null
$exitCode = $null
$stdout = ''
$stderr = ''

$arguments = @('--engine', $enginePath, '--bochs', $bochsPath,
    '--byob-profile', $profilePath, '--byob-root', $rootPath, $targetPath)
$inputRecord = [ordered]@{
    schema = 'runner.cli-native-controlled-stop-inputs.v1'
    runner = [ordered]@{ path = $runnerPath; sha256 = (Get-FileHash -LiteralPath $runnerPath -Algorithm SHA256).Hash }
    engine = [ordered]@{ path = $enginePath; sha256 = (Get-FileHash -LiteralPath $enginePath -Algorithm SHA256).Hash }
    profile = [ordered]@{ path = $profilePath; sha256 = (Get-FileHash -LiteralPath $profilePath -Algorithm SHA256).Hash }
    byobRoot = $rootPath
    bochs = [ordered]@{ path = $bochsPath; sha256 = (Get-FileHash -LiteralPath $bochsPath -Algorithm SHA256).Hash }
    target = [ordered]@{ path = $targetPath; sha256 = (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash }
    arguments = $arguments
    watchdogSeconds = $WatchdogSeconds
}
[IO.File]::WriteAllText($inputsPath, ($inputRecord | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))

try {
    $startInfo = [Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $runnerPath
    $startInfo.Arguments = (($arguments | ForEach-Object { Quote-WindowsArgument $_ }) -join ' ')
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardInput = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    $process = [Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    if (-not $process.Start()) { throw 'Process start returned false.' }
    $process.StandardInput.Close()
    $stdoutTask = $process.StandardOutput.ReadToEndAsync()
    $stderrTask = $process.StandardError.ReadToEndAsync()
    if (-not $process.WaitForExit($WatchdogSeconds * 1000)) {
        $timedOut = $true
        $process.Kill()
        $process.WaitForExit()
    }
    [Threading.Tasks.Task]::WaitAll(@($stdoutTask, $stderrTask))
    $stdout = $stdoutTask.Result
    $stderr = $stderrTask.Result
    $exitCode = $process.ExitCode
} catch {
    $launchError = $_.Exception.Message
} finally {
    [IO.File]::WriteAllText($stdoutPath, $stdout, [Text.UTF8Encoding]::new($false))
    [IO.File]::WriteAllText($stderrPath, $stderr, [Text.UTF8Encoding]::new($false))
    if ($null -ne $process) { $process.Dispose() }
}

$combined = $stdout + "`n" + $stderr
$result = [ordered]@{
    schema = 'runner.cli-native-controlled-stop-observation.v1'
    startedUtc = $started.ToString('o')
    elapsedMilliseconds = [int](([DateTime]::UtcNow - $started).TotalMilliseconds)
    watchdogSeconds = $WatchdogSeconds
    watchdogTerminated = $timedOut
    launchError = $launchError
    exitCode = $exitCode
    stdoutSha256 = (Get-FileHash -LiteralPath $stdoutPath -Algorithm SHA256).Hash
    stderrSha256 = (Get-FileHash -LiteralPath $stderrPath -Algorithm SHA256).Hash
    observedMarkers = [ordered]@{
        adapterControlledStopAccepted = ($combined -match 'adapter controlled stop accepted')
        cpuLoopShutdown = ($combined -match 'cpu loop quit, shutting down simulator')
        nativePostHandoff = ($combined -match 'ca80.*eip=0003')
    }
    interpretation = 'mechanical process observation only; marker presence does not prove guest or CLI success'
}
[IO.File]::WriteAllText($resultPath, ($result | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
if ($null -ne $launchError) { throw "CLI observation launch failed; retained evidence: $resultPath" }
Write-Host "CLI native observation retained: $evidence"
