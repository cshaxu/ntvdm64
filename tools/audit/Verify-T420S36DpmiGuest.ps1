[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$FixtureRoot,
    [string]$PackageRoot = 'O:\winnt',
    [Parameter(Mandatory)][string]$LogPrefix,
    [switch]$Stress
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
$results = @()
$images = if ($Stress) { @('D36R.COM') } else { @('D36N.COM','D36E.COM') }
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
            if ($screen -match 'S36_DPMI_FAIL|Bad command or filename' -or
                $screen -notmatch 'S36_DPMI_ALLOC_REALLOC_DATA_FREE_OK') { throw 'Missing successful guest transcript' }
            if ($Stress -and $screen -notmatch 'S36_DPMI_FORCED_MOVE_FAILED_GROW_DATA_FREE_OK') {
                throw 'Missing forced relocation/failure/data marker'
            }
            $survivors = @(PackageProcesses | Select-Object ProcessId,ParentProcessId,ExecutablePath)
            $results += @{Image=$image; Sha256=$hash; Route=$route; Report=$report; ProcessesBeforeCleanup=$survivors}
            Write-Host "PASS DPMI $image $route"
        } finally {
            # Case isolation is not evidence of natural worker teardown.
            if ($launcher) {
                foreach ($child in (PackageProcesses)) {
                    if ($child.ParentProcessId -eq $launcher) {
                        $process = Get-Process -Id $child.ProcessId -ErrorAction SilentlyContinue
                        if ($process) { Stop-Process -Id $process.Id -Force; [void]$process.WaitForExit(5000) }
                    }
                }
            }
        }
    }
}
$results | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-summary.json") -Encoding utf8
