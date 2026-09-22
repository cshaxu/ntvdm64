[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [string]$RuntimeRoot = 'O:\winnt',
    [string]$Observer = 'O:\winnt\tests\console-startup-observer.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$observer = (Resolve-Path -LiteralPath $Observer).Path
$repository = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$testRoot = Join-Path $runtime 'tests'
$logRoot = Join-Path $runtime 'logs'
$shadow = Join-Path $runtime 'NTVDM.REG'
$testVdd = Join-Path $testRoot 's44-entry-hook-vdd.dll'
$trace = Join-Path $logRoot 's44-vdd-worker-loss-vdd.log'
$report = Join-Path $logRoot 's44-vdd-worker-loss-observer.txt'
$gateName = 'Local\MvdmS44VddWorkerLoss'
$paths = @('run16.exe', 'basesrv.exe', 'ntvdm.exe') |
    ForEach-Object { Join-Path $runtime $_ }

function PackageProcesses {
    @(
        Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='basesrv.exe' OR Name='ntvdm.exe'" |
            Where-Object { $_.ExecutablePath -in $paths }
    )
}

function Stop-ExactPackageProcesses {
    param([int[]]$Ids)
    foreach ($process in PackageProcesses | Where-Object { $_.ProcessId -in $Ids }) {
        Stop-Process -Id $process.ProcessId -Force -ErrorAction SilentlyContinue
    }
}

if (@(PackageProcesses).Count -ne 0) {
    throw 'S44 worker-loss test requires no pre-existing package session.'
}

New-Item -ItemType Directory -Force -Path $testRoot, $logRoot | Out-Null
foreach ($name in @('run16.exe', 'basesrv.exe', 'ntvdm.exe', 'dtmgr.exe')) {
    Copy-Item -LiteralPath (Join-Path $build $name) -Destination (Join-Path $runtime $name) -Force
}
Copy-Item -LiteralPath (Join-Path $build 's44-entry-hook-vdd.dll') -Destination $testVdd -Force
Copy-Item -LiteralPath (Join-Path $PSScriptRoot '..\mvdm-host\fixtures\S44-ENTRY-HOOK-NTVDM.REG') -Destination $shadow -Force
Remove-Item -LiteralPath $trace, $report, "$report.pre-input-console.txt", "$report.pre-input-console.txt.console.txt" -Force -ErrorAction SilentlyContinue

$previousTrace = [Environment]::GetEnvironmentVariable('S44_VDD_TRACE_PATH')
$previousGate = [Environment]::GetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE')
$gate = $null
$observation = $null
$owned = [Collections.Generic.HashSet[int]]::new()
try {
    $gate = [Threading.EventWaitHandle]::new($false, [Threading.EventResetMode]::ManualReset, $gateName)
    [Environment]::SetEnvironmentVariable('S44_VDD_TRACE_PATH', $trace)
    [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE', $gateName)
    $inputText = "exit`r"
    $args = @(
        (Join-Path $runtime 'run16.exe'), $runtime, $report, 'COMMAND.COM',
        '--observation-timeout-ms', '20000', '--observe-console-input-text', ('"' + $inputText + '"')
    )
    $observation = Start-Process -FilePath $observer -ArgumentList $args -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    [void]$owned.Add($observation.Id)
    $snapshot = "$report.pre-input-console.txt.console.txt"
    $deadline = [DateTime]::UtcNow.AddSeconds(25)
    do {
        $console = if (Test-Path -LiteralPath $snapshot) { Get-Content -LiteralPath $snapshot -Raw } else { '' }
        $promptReady = $console -match 'O:\\WINNT>'
        if (!$promptReady) { Start-Sleep -Milliseconds 100 }
    } while (!$promptReady -and [DateTime]::UtcNow -lt $deadline)
    if (!$promptReady) { throw 'Original COMMAND prompt was not observed before worker-loss injection.' }

    $package = PackageProcesses
    foreach ($process in $package) { [void]$owned.Add([int]$process.ProcessId) }
    $worker = @($package | Where-Object { $_.Name -eq 'ntvdm.exe' })
    $broker = @($package | Where-Object { $_.Name -eq 'basesrv.exe' })
    if ($worker.Count -ne 1 -or $broker.Count -ne 1) {
        throw "Expected one owned worker and broker; found workers=$($worker.Count) brokers=$($broker.Count)."
    }
    $workerId = [int]$worker[0].ProcessId
    $brokerId = [int]$broker[0].ProcessId
    Stop-ExactPackageProcesses @($workerId)
    [void]$gate.Set()
    if (!$observation.WaitForExit(30000)) { throw 'Observer did not receive bounded worker-loss completion.' }
    $result = Get-Content -LiteralPath $report -Raw
    if ($result -notmatch '(?m)^result=exited' -or $result -match '(?m)^exit=0x00000000') {
        throw "Worker loss was not reported as an explicit parent failure:`n$result"
    }
    if (@(PackageProcesses | Where-Object { $_.ProcessId -eq $brokerId }).Count -eq 0) {
        throw 'Worker loss unexpectedly terminated the still-healthy broker.'
    }

    $memOut = Join-Path $logRoot 's44-vdd-worker-loss-restart.log'
    $memErr = Join-Path $logRoot 's44-vdd-worker-loss-restart.stderr.log'
    Remove-Item -LiteralPath $memOut, $memErr -Force -ErrorAction SilentlyContinue
    $restart = Start-Process -FilePath (Join-Path $runtime 'run16.exe') -ArgumentList 'MEM.EXE' -WorkingDirectory $runtime `
        -RedirectStandardOutput $memOut -RedirectStandardError $memErr -WindowStyle Hidden -PassThru
    [void]$owned.Add($restart.Id)
    if (!$restart.WaitForExit(30000)) { throw 'Post-loss MEM restart timed out.' }
    $deadline = [DateTime]::UtcNow.AddSeconds(10)
    do {
        $mem = if (Test-Path -LiteralPath $memOut) { Get-Content -LiteralPath $memOut -Raw } else { '' }
        if ($mem -notmatch 'bytes total conventional memory') { Start-Sleep -Milliseconds 100 }
    } while ($mem -notmatch 'bytes total conventional memory' -and [DateTime]::UtcNow -lt $deadline)
    if ($mem -notmatch 'bytes total conventional memory' -or $mem -match 'Bad command or filename') {
        throw 'A fresh worker did not recover original MEM execution after loss.'
    }
    $markers = if (Test-Path -LiteralPath $trace) { @(Get-Content -LiteralPath $trace) } else { @() }
    if (@(@('CREATE', 'TERMINATE', 'BLOCK') | Where-Object { $markers -notcontains $_ }).Count -ne 0) {
        throw 'VDD callbacks did not complete after worker-loss restart.'
    }
    Write-Output 'S44_VDD_WORKER_LOSS_END_TO_END_OK'
}
finally {
    if ($gate) { [void]$gate.Set() }
    Stop-ExactPackageProcesses @($owned)
    if ($observation -and !$observation.HasExited) { Stop-Process -Id $observation.Id -Force -ErrorAction SilentlyContinue }
    [Environment]::SetEnvironmentVariable('S44_VDD_TRACE_PATH', $previousTrace)
    [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_INPUT_GATE', $previousGate)
    if ($gate) { $gate.Dispose() }
    Remove-Item -LiteralPath $shadow, $testVdd -Force -ErrorAction SilentlyContinue
}
