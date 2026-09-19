[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Probe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$PackageRoot = 'O:\winnt'
)
$ErrorActionPreference = 'Stop'
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$Probe = (Resolve-Path -LiteralPath $Probe).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$paths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $paths })
}
if ((PackageProcesses).Count) { throw 'Package already in use' }
Copy-Item -LiteralPath $Probe -Destination "$PackageRoot\tests\D38I.COM"
$results = @()
foreach ($route in @('direct','nested')) {
    if ((PackageProcesses).Count) { throw 'Package process remains' }
    $report = "$PackageRoot\logs\$LogPrefix-$route.txt"
    if (Test-Path -LiteralPath $report) { throw 'Use a fresh log prefix' }
    $arguments = @('tests\D38I.COM')
    if ($route -eq 'nested') { $arguments = @('COMMAND.COM','/c','COMMAND.COM','/c') + $arguments }
    $launcher = 0
    $observation = Start-Process -FilePath $Observer -ArgumentList (
        @($paths[0],$PackageRoot,$report) + $arguments + @('--observation-timeout-ms','15000')
    ) -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
    try {
        if (!$observation.WaitForExit(30000)) { throw 'Observer remains live; inspect before retry' }
        $record = Get-Content -LiteralPath $report -Raw
        $launcher = [int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
        if ($record -notmatch '(?m)^result=exited\r?$' -or
            $record -notmatch '(?m)^exit=0x00000000\r?$') { throw "Guest did not pass: $route" }
        $screen = Get-Content -LiteralPath "$report.console.txt" -Raw
        $text = ([regex]::Matches($screen, '(?m)^\[\d+\] (.*)\r?$') |
            ForEach-Object { $_.Groups[1].Value.TrimEnd("`r") }) -join ''
        if ($text -match 'S38_FAIL|Bad command or filename' -or
            $text -notmatch 'S38_INT16_RETURN_OK' -or
            $text -notmatch 'S38_FAULT16_RETURN_NEGATIVE_OK') { throw 'Missing guest assertions' }
        $results += @{Route=$route; Report=$report; ProbeSha256=(Get-FileHash $Probe).Hash}
        Write-Host "PASS S38 interrupt/fault return $route"
    } finally {
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
$results | ConvertTo-Json -Depth 4 | Set-Content "$PackageRoot\logs\$LogPrefix-summary.json" -Encoding utf8
