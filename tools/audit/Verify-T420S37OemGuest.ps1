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
$binaryRoot = Join-Path $PackageRoot 'system32'
if (!(Test-Path -LiteralPath (Join-Path $binaryRoot 'run16.exe'))) {
    # Historical staged packages placed images at their root; deployed
    # packages use SYSTEM32. This is a fixed layout choice for this test.
    $binaryRoot = $PackageRoot
}
if ((Get-ItemPropertyValue 'HKLM:\SYSTEM\CurrentControlSet\Control\Nls\CodePage' OEMCP) -ne '437') {
    throw 'This fixed-byte probe requires the recorded OEM 437 host profile'
}
$paths = @('run16.exe','ntvdm.exe','ntsrv.exe') | ForEach-Object { Join-Path $binaryRoot $_ }
function Processes {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='ntsrv.exe'" |
        Where-Object { $_.ExecutablePath -in $paths })
}
if ((Processes).Count) { throw 'Package in use' }
if (-not ('S37VolumeOracle' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class S37VolumeOracle {
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode, SetLastError=true)]
    public static extern bool GetVolumeInformationW(string root,
        StringBuilder label, uint labelSize, out uint serial,
        out uint maxComponent, out uint flags, StringBuilder filesystem,
        uint filesystemSize);
}
'@
}
$volumeLabel = New-Object Text.StringBuilder 261
$filesystem = New-Object Text.StringBuilder 261
[uint32]$serial = 0; [uint32]$maxComponent = 0; [uint32]$volumeFlags = 0
if (![S37VolumeOracle]::GetVolumeInformationW([IO.Path]::GetPathRoot($PackageRoot),
    $volumeLabel, 261, [ref]$serial, [ref]$maxComponent, [ref]$volumeFlags,
    $filesystem, 261)) { throw 'Host volume oracle failed' }
$volumeExpected = New-Object byte[] 25
[BitConverter]::GetBytes($serial).CopyTo($volumeExpected, 2)
for ($i=6; $i -lt 25; $i++) { $volumeExpected[$i] = 0x20 }
$oem = [Text.Encoding]::GetEncoding(437)
$labelBytes = $oem.GetBytes($volumeLabel.ToString())
$fsBytes = $oem.GetBytes($filesystem.ToString())
[Array]::Copy($labelBytes, 0, $volumeExpected, 6, [Math]::Min(11, $labelBytes.Length))
[Array]::Copy($fsBytes, 0, $volumeExpected, 17, [Math]::Min(8, $fsBytes.Length))
$volumeHex = [BitConverter]::ToString($volumeExpected).Replace('-', '')
$root = Join-Path $PackageRoot 'tests\O37G'
if (Test-Path -LiteralPath $root) { throw 'Previous test directory exists; inspect it' }
# Physical test staging deliberately omits historical logs. This probe owns
# only its fresh reports below the supplied package, so establish that output
# directory rather than depending on copied diagnostic history.
$null = New-Item -ItemType Directory -Force -Path (Join-Path $PackageRoot 'logs')
Copy-Item -LiteralPath $Probe -Destination "$PackageRoot\tests\O37.COM"
$previousPrivateDesktop = [Environment]::GetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP', 'Process')
$previousPath = [Environment]::GetEnvironmentVariable('PATH', 'Process')
# The observer's private desktop is the established noninteractive product
# test boundary.  It avoids depending on whichever interactive desktop
# happens to be available, while leaving the package and guest unchanged.
[Environment]::SetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP', '1', 'Process')
# Match the ordinary product-observation lookup environment.  This exposes
# the candidate's selected carrier binaries without inheriting unrelated
# developer-tool paths as a hidden launch dependency.
[Environment]::SetEnvironmentVariable('PATH', $binaryRoot + ';' + (Join-Path $env:SystemRoot 'System32'), 'Process')
$results = @()
try {
foreach ($route in @('direct','nested')) {
    $report = "$PackageRoot\logs\$LogPrefix-$route.txt"
    if (Test-Path $report) { throw 'Use fresh prefix' }
    New-Item -ItemType Directory -Path $root | Out-Null
    # The observer's second positional argument is the launched image's
    # working directory.  The selected runtime is in SYSTEM32, but this
    # original DOS probe deliberately uses package-root-relative tests\ paths.
    # Its absolute Run16 image and controlled PATH resolve binaries; keep the
    # probe's own working-directory contract at the package root.
    # This is a completion witness, not merely launch admission.  Without
    # Run16's explicit wait contract the observer may finish when the
    # launcher has handed work to the broker, before the DOS probe executes.
    $args = @($paths[0],$PackageRoot,$report,'--wait')
    if ($route -eq 'nested') { $args += @('COMMAND.COM','/c','COMMAND.COM','/c') }
    $args += @((Join-Path $PackageRoot 'tests\O37.COM'),'--observation-timeout-ms','15000')
    $launcher = 0
    try {
        $p = Start-Process -FilePath $Observer -ArgumentList $args -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
        if (!$p.WaitForExit(30000)) { throw 'Observer still active' }
        $record = Get-Content $report -Raw
        $launcher = [int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
        if ($record -notmatch 'result=exited' -or $record -notmatch 'exit=0x00000000') { throw 'Guest failed' }
        # The observer writes ReadConsoleOutputCharacterA bytes, not UTF-8.
        $screen = [Text.Encoding]::GetEncoding(437).GetString([IO.File]::ReadAllBytes("$report.console.txt"))
        if ($screen -notmatch 'S37_OEM_GUEST_CREATE_RENAME_ATTR_READ_OK' -or $screen -match 'S37_OEM_GUEST_FAIL|Bad command or filename') { throw 'Guest text failed' }
        if ($screen -notmatch 'S37_OEM_GUEST_DIRECTORY_DELETE_DISK_OK') { throw 'Directory/disk guest text failed' }
        if ($screen -notmatch ('S37_VOLUME=' + $volumeHex)) { throw "Guest volume mismatch; expected $volumeHex; see $report.console.txt" }
        if ($screen -notmatch 'S37_OEM_GUEST_FCB_COMPUTER_OK' -or
            $screen -notmatch ('S37_HOST=' + [regex]::Escape($env:COMPUTERNAME))) { throw 'FCB/computer-name guest text failed' }
        $dir = Join-Path $root ('D' + [char]0xa3)
        if ($screen -notmatch 'S37_OEM_GUEST_COMMAND_ENV_OK' -or
            $screen -notmatch ('S37_ENV_DIR=' + [regex]::Escape($dir))) { throw 'COMMAND environment path mismatch' }
        $file = Join-Path $dir ('B' + [char]0xa3 + '.DAT')
        if ([IO.File]::ReadAllText($file) -cne "S37-OEM-GUEST-BYTES`r`n") { throw 'Unicode file/content mismatch' }
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
} finally {
    [Environment]::SetEnvironmentVariable('MVDM_OBSERVER_PRIVATE_DESKTOP', $previousPrivateDesktop, 'Process')
    [Environment]::SetEnvironmentVariable('PATH', $previousPath, 'Process')
}
