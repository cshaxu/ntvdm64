[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$Probe,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$PackageRoot = 'O:\winnt'
)
$ErrorActionPreference = 'Stop'
$Observer = (Resolve-Path $Observer).Path
$Probe = (Resolve-Path $Probe).Path
$PackageRoot = (Resolve-Path $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid prefix' }
if ((Get-ItemPropertyValue 'HKLM:\SYSTEM\CurrentControlSet\Control\Nls\CodePage' OEMCP) -ne '437') {
    throw 'This fixed-byte probe requires the recorded OEM 437 host profile'
}
if ((Get-ItemPropertyValue 'HKLM:\SYSTEM\CurrentControlSet\Control\Nls\CodePage' OEMCP) -ne '437') {
    throw 'This fixed-byte probe requires the recorded OEM 437 host profile'
}
$paths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function Processes {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $paths })
}
if ((Processes).Count) { throw 'Package in use' }
$root = Join-Path $PackageRoot 'tests\O37G'
if (Test-Path -LiteralPath $root) { throw 'Previous test directory exists; inspect it' }
Copy-Item -LiteralPath $Probe -Destination "$PackageRoot\tests\O37.COM"
$results = @()
foreach ($route in @('direct','nested')) {
    $report = "$PackageRoot\logs\$LogPrefix-$route.txt"
    if (Test-Path $report) { throw 'Use fresh prefix' }
    New-Item -ItemType Directory -Path $root | Out-Null
    $args = @($paths[0],$PackageRoot,$report)
    if ($route -eq 'nested') { $args += @('COMMAND.COM','/c','COMMAND.COM','/c') }
    $args += @('tests\O37.COM','--observation-timeout-ms','15000')
    $launcher = 0
    try {
        $p = Start-Process -FilePath $Observer -ArgumentList $args -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
        if (!$p.WaitForExit(30000)) { throw 'Observer still active' }
        $record = Get-Content $report -Raw
        $launcher = [int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
        if ($record -notmatch 'result=exited' -or $record -notmatch 'exit=0x00000000') { throw 'Guest failed' }
        $screen = Get-Content "$report.console.txt" -Raw
        if ($screen -notmatch 'S37_OEM_GUEST_CREATE_RENAME_ATTR_READ_OK' -or $screen -match 'S37_OEM_GUEST_FAIL|Bad command or filename') { throw 'Guest text failed' }
        $dir = Join-Path $root ('D' + [char]0xa3)
        $file = Join-Path $dir ('B' + [char]0xa3 + '.DAT')
        if ([IO.File]::ReadAllText($file) -cne "S37-OEM-GUEST-BYTES`r`n") { throw 'Unicode file/content mismatch' }
        if (@(Get-ChildItem -LiteralPath $root -Force).Count -ne 1 -or
            @(Get-ChildItem -LiteralPath $dir -Force).Count -ne 1) { throw 'Unexpected test files; preserve for inspection' }
        if (@(Get-ChildItem -LiteralPath $root -Force).Count -ne 1 -or
            @(Get-ChildItem -LiteralPath $dir -Force).Count -ne 1) { throw 'Unexpected test files; preserve for inspection' }
        $results += @{Route=$route; Report=$report; UnicodePath=$file; ProbeSha256=(Get-FileHash $Probe).Hash; FileSha256=(Get-FileHash $file).Hash}
        # Exact test-created objects only; no recursive deletion or guest media.
        Remove-Item -LiteralPath $file
        Remove-Item -LiteralPath $dir
        Remove-Item -LiteralPath $root
        Write-Host "PASS OEM guest $route"
    } finally {
        if ($launcher) {
            foreach ($child in (Processes)) {
                if ($child.ParentProcessId -eq $launcher) {
                    $p = Get-Process -Id $child.ProcessId -ErrorAction SilentlyContinue
                    if ($p) { Stop-Process -Id $p.Id -Force; [void]$p.WaitForExit(5000) }
                }
            }
        }
    }
}
$results | ConvertTo-Json -Depth 4 | Set-Content "$PackageRoot\logs\$LogPrefix-summary.json" -Encoding utf8
