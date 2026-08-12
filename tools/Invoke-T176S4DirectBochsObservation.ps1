[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$Bochs,
    [Parameter(Mandatory = $true)] [string]$Profile,
    [Parameter(Mandatory = $true)] [string]$ByobRoot,
    [Parameter(Mandatory = $true)] [string]$Config,
    [Parameter(Mandatory = $true)] [string]$EvidenceRoot,
    [ValidateSet('com', 'exe')] [string]$LaunchKind,
    [string]$LaunchTail = '',
    [ValidateRange(1, 100)] [int]$BenchmarkMillions = 10,
    [ValidateRange(1, 120)] [int]$WatchdogSeconds = 30,
    [ValidateRange(16, 240)] [int]$OuterTimeoutSeconds = 50
)

$ErrorActionPreference = 'Stop'

function Resolve-Leaf([string]$Path, [string]$Name) {
    $resolved = (Resolve-Path -LiteralPath $Path -ErrorAction Stop).Path
    if (-not (Test-Path -LiteralPath $resolved -PathType Leaf)) {
        throw "$Name is not a file: $Path"
    }
    return $resolved
}

$graceSeconds = 15
if ($OuterTimeoutSeconds -lt ($WatchdogSeconds + $graceSeconds)) {
    throw 'Outer timeout must include watchdog plus 15-second evidence-collection grace.'
}

$bochsPath = Resolve-Leaf $Bochs 'Bochs'
$profilePath = Resolve-Leaf $Profile 'Profile'
$configPath = Resolve-Leaf $Config 'Bochs configuration'
$rootPath = (Resolve-Path -LiteralPath $ByobRoot -ErrorAction Stop).Path
if (-not (Test-Path -LiteralPath $rootPath -PathType Container)) {
    throw "BYOB root is not a directory: $ByobRoot"
}
$evidencePath = (Resolve-Path -LiteralPath $EvidenceRoot -ErrorAction Stop).Path
if (-not (Test-Path -LiteralPath $evidencePath -PathType Container)) {
    throw "Evidence root is not a directory: $EvidenceRoot"
}

$logPath = Join-Path $evidencePath 'bochs.log'
$resultPath = Join-Path $evidencePath 'observation.json'
if ((Test-Path -LiteralPath $logPath) -or (Test-Path -LiteralPath $resultPath)) {
    throw 'Evidence root already contains a runtime result; refusing to overwrite.'
}

$environmentValues = [ordered]@{
    NTDOS64_ADAPTER_PROFILE = $profilePath
    NTDOS64_ADAPTER_ROOT = $rootPath
    NTDOS64_ADAPTER_LAUNCH_KIND = $LaunchKind
    NTDOS64_ADAPTER_LAUNCH_TAIL = $LaunchTail
    NTDOS64_ADAPTER_BRIDGE = '1'
    NTDOS64_ADAPTER_TRACE = '1'
}

$process = $null
$timedOut = $false
$launchError = $null
$exitCode = $null
$directProcessId = $null
$started = [DateTime]::UtcNow

try {
    $startInfo = [Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $bochsPath
    $startInfo.Arguments = '-q -f "' + $configPath + '" -benchmark ' +
        $BenchmarkMillions + ' -log "' + $logPath + '"'
    $startInfo.WorkingDirectory = Split-Path -Parent $configPath
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    # Windows PowerShell 5.1 initializes this collection lazily: the first
    # property read primes it, while assigning that first return would retain
    # null. Subsequent property index access is the child-only collection.
    $null = $startInfo.EnvironmentVariables
    foreach ($name in $environmentValues.Keys) {
        # This creates the child environment block directly. In particular,
        # an empty DOS tail remains `NAME=` instead of being deleted from the
        # coordinator PowerShell process.
        $startInfo.EnvironmentVariables[$name] = [string]$environmentValues[$name]
    }

    $process = [Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    if (-not $process.Start()) { throw 'Direct Bochs process start returned false.' }
    $directProcessId = $process.Id
    if (-not $process.WaitForExit($WatchdogSeconds * 1000)) {
        $timedOut = $true
        $process.Kill()
        $process.WaitForExit()
    }
    $exitCode = $process.ExitCode
} catch {
    $launchError = $_.Exception.Message
} finally {
    if ($null -ne $process) { $process.Dispose() }
}

$record = [ordered]@{
    schema = 'ntdos64.direct-bochs-observation.v1'
    startedUtc = $started.ToString('o')
    elapsedMilliseconds = [int](([DateTime]::UtcNow - $started).TotalMilliseconds)
    bochs = [ordered]@{ path = $bochsPath; sha256 = (Get-FileHash -LiteralPath $bochsPath -Algorithm SHA256).Hash }
    profile = [ordered]@{ path = $profilePath; sha256 = (Get-FileHash -LiteralPath $profilePath -Algorithm SHA256).Hash }
    byobRoot = $rootPath
    config = [ordered]@{ path = $configPath; sha256 = (Get-FileHash -LiteralPath $configPath -Algorithm SHA256).Hash }
    log = $logPath
    launchKind = $LaunchKind
    launchTail = $LaunchTail
    benchmarkMillions = $BenchmarkMillions
    watchdogSeconds = $WatchdogSeconds
    outerTimeoutSeconds = $OuterTimeoutSeconds
    evidenceCollectionGraceSeconds = $graceSeconds
    directPid = $directProcessId
    watchdogTerminated = $timedOut
    launchError = $launchError
    exitCode = $exitCode
    logSha256 = if (Test-Path -LiteralPath $logPath -PathType Leaf) { (Get-FileHash -LiteralPath $logPath -Algorithm SHA256).Hash } else { $null }
    interpretation = 'direct process control only; runtime markers require a separately admitted observation task'
}
[IO.File]::WriteAllText($resultPath, ($record | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
if ($null -ne $launchError) {
    throw "Direct Bochs observation launch failed; retained evidence: $resultPath"
}
Write-Host "Direct Bochs observation retained: $evidencePath"
