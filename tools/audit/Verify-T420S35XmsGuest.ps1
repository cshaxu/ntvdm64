[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [string]$PackageRoot = 'O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix
)
$ErrorActionPreference = 'Stop'
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$FixtureRoot = (Resolve-Path -LiteralPath $FixtureRoot).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$manifest = Get-Content -LiteralPath (Join-Path $FixtureRoot 'manifest.json') -Raw | ConvertFrom-Json
if ($manifest.int15ReservedKb -ne 0) { throw 'Reserved INT15 remains an investigative reproducer, not a passing gate' }
$productPaths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function Get-TestPackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $productPaths })
}
if ((Get-TestPackageProcesses).Count) { throw 'Package already in use' }
foreach ($item in $manifest.outputs) {
    if ($item.path -notin @('X35.COM','U35.COM','U35.PIF','U35.NT','U35AUTO.NT','X35UMB.SYS','R35.BAT')) { throw 'Unexpected fixture name' }
    $source = Join-Path $FixtureRoot $item.path
    if ((Get-FileHash -LiteralPath $source).Hash -ne $item.sha256) { throw "Fixture hash mismatch: $source" }
    Copy-Item -LiteralPath $source -Destination (Join-Path "$PackageRoot\tests" $item.path)
}
$success = 'S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK'
$cases = @(
    @{Name='direct'; Args=@('tests\X35.COM'); Code=0; Markers=@($success); Count=1},
    @{Name='profile'; Args=@('tests\U35.COM'); Code=0; Markers=@('S35_XMS_BOOT_UMB_ALLOC_RELEASE_DOUBLE_FREE_OK','S35_XMS_PROFILE_LOADED','S35_XMS_INT15_DEFAULT_ZERO_OK',$success); Count=1},
    @{Name='nested'; Args=@('COMMAND.COM','/c','COMMAND.COM','/c','tests\X35.COM'); Code=0; Markers=@($success); Count=1},
    @{Name='repeat'; Args=@('COMMAND.COM','/c','tests\R35.BAT'); Code=0; Markers=@($success,'S35_XMS_REPEAT_OK'); Count=2}
)
$results = @()
foreach ($case in $cases) {
    $report = Join-Path $PackageRoot "logs\$LogPrefix-$($case.Name).txt"
    if (Test-Path -LiteralPath $report) { throw 'Use a fresh log prefix' }
    $launcherId = 0
    $observation = Start-Process -FilePath $Observer -ArgumentList (@(
        (Join-Path $PackageRoot 'run16.exe'), $PackageRoot, $report
    ) + $case.Args + @('--observation-timeout-ms','20000')) -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
    try {
        if (!$observation.WaitForExit(45000)) { throw 'Observer timeout' }
        $record = Get-Content -LiteralPath $report -Raw
        $launcherId = [int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
        $code = [Convert]::ToUInt32([regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)').Groups[1].Value,16)
        if ($record -notmatch '(?m)^result=exited' -or $code -ne $case.Code) { throw "Failed $($case.Name): $code" }
        $screen = Get-Content -LiteralPath "$report.console.txt" -Raw
        $text = ($screen -replace '(?m)^\[\d+\]\s?','') -replace '\r?\n',''
        if ($text -match 'S35_XMS_.*FAIL|Bad command or filename|is not recognized') { throw "Guest failure: $($case.Name)" }
        foreach ($marker in $case.Markers) {
            if (!$text.Contains($marker)) { throw "Missing guest marker: $marker" }
        }
        if ([regex]::Matches($text,[regex]::Escape($success)).Count -ne $case.Count) { throw 'Unexpected success marker count' }
        # Record the authoritative pre-cleanup process state. Never infer
        # natural worker exit from the isolation cleanup in finally.
        $survivors = @(Get-TestPackageProcesses | Select-Object ProcessId,
            ParentProcessId, ExecutablePath)
        foreach ($remaining in $survivors) {
            if ($remaining.ParentProcessId -eq $launcherId -and
                $remaining.ExecutablePath -eq (Join-Path $PackageRoot 'ntvdm.exe')) {
                $worker = Get-Process -Id $remaining.ProcessId -ErrorAction SilentlyContinue
                if ($worker) { [void]$worker.WaitForExit(5000) }
            }
        }
        $settled = @(Get-TestPackageProcesses | Select-Object ProcessId,
            ParentProcessId, ExecutablePath)
        $results += @{Case=$case.Name; Exit=$code; Report=$report;
            ProcessesBeforeCleanup=$survivors; ProcessesAfterWorkerWait=$settled}
        Write-Host "PASS XMS $($case.Name)"
    } finally {
        # Isolate cases, not a claim of natural worker/lease teardown.
        # Only processes directly created by the recorded test launcher.
        if ($launcherId) {
            foreach ($child in (Get-TestPackageProcesses)) {
                if ($child.ParentProcessId -eq $launcherId) { Stop-Process -Id $child.ProcessId -Force -ErrorAction SilentlyContinue }
            }
        }
    }
    if ((Get-TestPackageProcesses).Count) { throw 'Unclassified package process remains; refusing next test' }
}
$results | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-summary.json") -Encoding utf8
