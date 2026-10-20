[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$MonitorRpc,
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogRoot,
    [Parameter(Mandatory)][string]$Prefix
)
$ErrorActionPreference='Stop'
if($Prefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid report prefix'}
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$stage=$null
$scope=$null
# COMMAND's original real-mode startup has a bounded physical-path contract.
# Stage the already coherent candidate beneath the owned short test root; this
# is test containment only, never a drive alias or a published package path.
$stage=New-PhysicalRuntimeStage (Resolve-Path $ProcessPackageRoot).Path
$scope=New-IsolatedPackageScope $stage
$package=$stage
$binary=Get-PackageBinaryRoot $package
$report=Join-Path $LogRoot "$Prefix.txt"
$result="$report.handoff.txt"
$oldPostExit=$env:MVDM_OBSERVER_POST_EXIT_MS
$observerProcess=$null
$frontend=$null
$worker=$null
try {
    if((Test-Path $report) -or (Test-Path $result)){throw 'Use a fresh evidence prefix'}
    $env:MVDM_OBSERVER_POST_EXIT_MS='5000'
    $observerProcess=Start-Process -FilePath (Resolve-Path $Observer).Path -ArgumentList @(
        (Join-Path $binary 'run16.exe'),$package,$report,'command',
        '--observation-timeout-ms','30000') -WindowStyle Hidden -PassThru
    $deadline=[DateTime]::UtcNow.AddSeconds(15)
    do {
        $workers=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" | Where-Object {$_.ExecutablePath -eq (Join-Path $binary 'ntvdm.exe')})
        $frontends=@(Get-CimInstance Win32_Process -Filter "Name='ntcon.exe'" | Where-Object {$_.ExecutablePath -eq (Join-Path $binary 'ntcon.exe')})
        if($workers.Count -eq 1 -and $frontends.Count -eq 1){
            $worker=Get-Process -Id $workers[0].ProcessId
            $frontend=Get-Process -Id $frontends[0].ProcessId
            break
        }
        Start-Sleep -Milliseconds 50
    }while([DateTime]::UtcNow -lt $deadline)
    if(!$worker -or !$frontend){throw 'DOS worker/frontend did not reach the close boundary'}
    & (Resolve-Path $MonitorRpc).Path --close-node 1 $frontend.Id *> "$report.close.txt"
    if($LASTEXITCODE){throw 'Frontend close RPC failed'}
    if(!$worker.WaitForExit(12000) -or !$frontend.WaitForExit(12000)){throw 'Acknowledged frontend close left a DOS participant live'}
    # The observer remains the sole Console owner during its requested bounded
    # post-exit interval.  Attach only to that exact process to observe the
    # actual input mode; no input is sent and no system Console is touched.
    Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class DosConsoleHandoffNative {
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool FreeConsole();
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool AttachConsole(uint pid);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr GetStdHandle(int id);
  [DllImport("kernel32.dll", SetLastError=true)] public static extern bool GetConsoleMode(IntPtr handle, out uint mode);
}
'@
    [void][DosConsoleHandoffNative]::FreeConsole()
    if(![DosConsoleHandoffNative]::AttachConsole([uint32]$observerProcess.Id)){throw 'Cannot attach to exact observer Console'}
    [uint32]$after=0
    $ok=[DosConsoleHandoffNative]::GetConsoleMode([DosConsoleHandoffNative]::GetStdHandle(-10),[ref]$after)
    [void][DosConsoleHandoffNative]::FreeConsole()
    if(!$ok){throw 'Cannot read restored Console input mode'}
    # AllocConsole's canonical interactive mode has these three bits.  The
    # regression clears them in the worker and, before the ordering repair,
    # left them cleared after NTCON's competing restore.  Assert the actual
    # keyboard contract rather than a host-private full-mode bit pattern.
    $interactive=[uint32](0x0001 -bor 0x0002 -bor 0x0004)
    @(
      ("restored-input-mode=0x{0:x8}" -f $after),
      ("interactive-input=$((($after -band $interactive) -eq $interactive))"),
      ("worker=$($worker.Id)"),
      ("frontend=$($frontend.Id)")
    ) | Set-Content -LiteralPath $result
    if(($after -band $interactive) -ne $interactive){throw 'DOS frontend close did not restore processed, line and echo input'}
    if(!$observerProcess.WaitForExit(10000) -or $observerProcess.ExitCode){throw 'Launcher observer did not complete normally'}
    'PASS direct DOS COMMAND management-close preserves the exact parent Console input mode'
} finally {
    $env:MVDM_OBSERVER_POST_EXIT_MS=$oldPostExit
    if($observerProcess){if(!$observerProcess.HasExited){$observerProcess.Kill();$observerProcess.WaitForExit(5000)|Out-Null};$observerProcess.Dispose()}
    if($worker){$worker.Dispose()}; if($frontend){$frontend.Dispose()}
    if($scope){Stop-IsolatedPackageScope $scope}
    if($stage){Remove-PhysicalRuntimeStage $stage}
}
