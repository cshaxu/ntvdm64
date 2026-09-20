[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [string]$PackageRoot = 'O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix,
    [switch]$Stress,
    [switch]$Lifecycle
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path "$PSScriptRoot\..\..").Path
$originalRegisters = Get-Content "$repo\src\mvdm\softpc.new\base\ccpu386\c_reg.h" -Raw
$modeSwitch = Get-Content "$repo\src\mvdm\dpmi32\modesw.c" -Raw
foreach ($segment in @('ES','CS','SS','DS','FS','GS')) {
    $original = [regex]::Match($originalRegisters,"(?m)^#define\s+${segment}_REG\s+(\d+)")
    $binding = [regex]::Match($modeSwitch,"(?m)^#define\s+CPU40_${segment}_REG\s+(\d+)")
    if (!$original.Success -or !$binding.Success -or
        $original.Groups[1].Value -ne $binding.Groups[1].Value) { throw "Wrong CPU40 $segment index" }
}
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$FixtureRoot = (Resolve-Path -LiteralPath $FixtureRoot).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$paths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $paths })
}
if ((PackageProcesses).Count) { throw 'Package already in use' }
if ($Stress -and $Lifecycle) { throw 'Select one matrix' }
if ($Lifecycle) {
    foreach ($dependency in @('D36N.COM','D36L.COM')) {
        Copy-Item -LiteralPath (Join-Path $FixtureRoot $dependency) -Destination "$PackageRoot\tests\$dependency"
    }
}
$results = @()
$images = if ($Lifecycle) { @('D36T.COM') } elseif ($Stress) { @('D36R.COM') } else { @('D36N.COM','D36E.COM') }
foreach ($image in $images) {
    $source = Join-Path $FixtureRoot $image
    $hash = (Get-FileHash -LiteralPath $source).Hash
    Copy-Item -LiteralPath $source -Destination (Join-Path "$PackageRoot\tests" $image)
    foreach ($route in @('direct','nested')) {
        if ((PackageProcesses).Count) { throw 'Package process remains; refusing next case' }
        $report = Join-Path $PackageRoot "logs\$LogPrefix-$image-$route.txt"
        if (Test-Path -LiteralPath $report) { throw 'Use a fresh log prefix' }
        $arguments = @("tests\$image")
        if ($route -eq 'nested') { $arguments = @('COMMAND.COM','/c','COMMAND.COM','/c') + $arguments }
        $launcher = 0
        $observation = Start-Process -FilePath $Observer -ArgumentList (
            @($paths[0],$PackageRoot,$report) + $arguments + @('--observation-timeout-ms','15000')
        ) -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
        try {
            if (!$observation.WaitForExit(30000)) { throw 'Observer still active; inspect before retry' }
            $record = Get-Content -LiteralPath $report -Raw
            $launcher = [int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
            if ($record -notmatch '(?m)^result=exited\r?$' -or
                $record -notmatch '(?m)^exit=0x00000000\r?$') { throw "Failed $image $route" }
            $screen = Get-Content -LiteralPath "$report.console.txt" -Raw
            # The observer prefixes physical Console rows. Long guest markers
            # wrap in narrow terminals; join only those captured rows, retaining
            # every character (including spaces) rather than changing the guest.
            $screen = ([regex]::Matches($screen, '(?m)^\[\d+\] (.*)\r?$') |
                ForEach-Object { $_.Groups[1].Value.TrimEnd("`r") }) -join ''
            if ($screen -match 'S36_DPMI_FAIL|Bad command or filename' -or
                $screen -notmatch 'S36_DPMI_ALLOC_REALLOC_DATA_FREE_OK') { throw 'Missing successful guest transcript' }
            if ($Stress -and $screen -notmatch 'S36_DPMI_FORCED_MOVE_FAILED_GROW_DATA_FREE_OK') {
                throw 'Missing forced relocation/failure/data marker'
            }
            if ($Lifecycle -and ($screen -match 'S36_DPMI_TASK_EXIT_FAIL' -or
                $screen -notmatch 'S36_DPMI_TASK_EXIT_CAPACITY_RESTORED_OK')) { throw 'Task cleanup failed' }
            if ($Lifecycle -and [regex]::Matches($screen, 'S36_DPMI_EXIT_WITH_LIVE_ALLOCATION').Count -ne 4) {
                throw 'Expected four independently completed live-allocation children'
            }
            $survivors = @(PackageProcesses | Select-Object ProcessId,ParentProcessId,ExecutablePath)
            $brokerLossVerified = $false
            if ($Lifecycle) {
                $owned = @($survivors | Where-Object { $_.ParentProcessId -eq $launcher })
                $brokers = @($owned | Where-Object { $_.ExecutablePath -eq $paths[2] })
                $workers = @($owned | Where-Object { $_.ExecutablePath -eq $paths[1] })
                if ($brokers.Count -ne 1 -or $workers.Count -ne 1) { throw 'Missing test-owned resident pair' }
                $workerProcess = Get-Process -Id $workers[0].ProcessId
                Stop-Process -Id $brokers[0].ProcessId -Force
                if (!$workerProcess.WaitForExit(10000)) { throw 'Worker survived broker loss' }
                $brokerLossVerified = $true
            }
            $results += @{Image=$image; Sha256=$hash; Route=$route; Report=$report;
                ProcessesBeforeCleanup=$survivors; BrokerLossWorkerExit=$brokerLossVerified}
            Write-Host "PASS DPMI $image $route"
        } finally {
            # Case isolation is not evidence of natural worker teardown.
            if ($launcher) {
                foreach ($child in (PackageProcesses)) {
                    if ($child.ParentProcessId -eq $launcher) {
                        $process = Get-Process -Id $child.ProcessId -ErrorAction SilentlyContinue
                        if ($process) {
                            # A worker or broker can complete naturally between
                            # discovery and termination.  That is already the
                            # desired cleanup state, not an acceptance failure.
                            Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
                            [void]$process.WaitForExit(5000)
                        }
                    }
                }
            }
        }
    }
}
$results | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-summary.json") -Encoding utf8
