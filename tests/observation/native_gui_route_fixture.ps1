param([Parameter(Mandatory)][string]$PackageRoot,
      [switch]$ObserveWorkerResidency,
      [switch]$ManagementClose,
      [string]$MonitorRpc='')
$ErrorActionPreference='Stop'
$PackageRoot=(Resolve-Path $PackageRoot).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$binaryRoot=Get-PackageBinaryRoot $PackageRoot
$stem='NATIVE-GUI-'+$PID
$ready=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\'+$stem+'-ready')
$release=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\'+$stem+'-release')
$launcher=$null;$target=$null;$worker=$null
try {
    $iterations=if($ManagementClose){1}else{2}
    for($iteration=0;$iteration -lt $iterations;$iteration++) {
    $null=$ready.Reset();$null=$release.Reset()
    $start=[Diagnostics.ProcessStartInfo]::new((Join-Path $binaryRoot 'run16.exe'))
    $start.UseShellExecute=$false
    $start.ArgumentList.Add((Join-Path $binaryRoot 'native-gui-startup-probe.exe'))
    $start.ArgumentList.Add($stem)
    $launcher=[Diagnostics.Process]::Start($start)
    if(!$ready.WaitOne(10000)){throw 'GUI target never ready'}
    if(!$launcher.WaitForExit(10000) -or $launcher.ExitCode){throw 'Startup-only launcher did not return zero'}
    # Test-only observation: production monitor does not enumerate target trees.
    $matches=@(Get-CimInstance Win32_Process -Filter "Name='native-gui-startup-probe.exe'" | Where-Object {$_.CommandLine -like "*$stem*"})
    if($matches.Count -ne 1){throw 'No unique controlled GUI target'}
    $target=[Diagnostics.Process]::GetProcessById($matches[0].ProcessId)
    $null=$target.Handle
    if($target.HasExited){throw 'GUI terminated with launcher/request release'}
    if(!$worker) {
        $worker=[Diagnostics.Process]::GetProcessById($matches[0].ParentProcessId)
        $null=$worker.Handle
        if($worker.ProcessName -ne 'ntvwm'){throw 'GUI was not created by NTVWM'}
    }
    if($worker.HasExited){throw 'First shared GUI worker did not remain resident'}
    # A deliberate longevity check, not input pacing or startup readiness.
    # Normal coverage uses exact-time policy fixtures; FullDeadlines also
    # proves real worker wiring survives beyond the old ten-second boundary.
    if($ObserveWorkerResidency -and !$iteration -and $worker.WaitForExit(11000)){
        throw 'Shared GUI worker retired at the obsolete idle deadline'
    }
    if($target.HasExited){throw 'Live GUI lost before explicit fixture release'}
    if($ManagementClose) {
        if(!$MonitorRpc){throw 'Management close requires the RPC monitor probe'}
        $monitor=[Diagnostics.ProcessStartInfo]::new((Resolve-Path $MonitorRpc).Path)
        $monitor.UseShellExecute=$false;$monitor.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
        $monitor.ArgumentList.Add('--terminate');$monitor.ArgumentList.Add([string]$worker.Id)
        $close=[Diagnostics.Process]::Start($monitor)
        try {
            if(!$close.WaitForExit(18000) -or $close.ExitCode){throw 'GUI-only worker close was not acknowledged'}
            if(!$worker.WaitForExit(5000) -or $worker.ExitCode -ne 1223){throw 'GUI-only carrier did not end after actual Console closure'}
            if($target.HasExited){throw 'Closing the carrier incorrectly killed its independent GUI'}
        } finally {$close.Dispose()}
    }
    $null=$release.Set()
    if(!$target.WaitForExit(10000) -or $target.ExitCode -ne 37){throw "Wrong actual GUI exit: $($target.ExitCode)"}
    if(!$ManagementClose -and $worker.HasExited){throw 'Shared carrier exited with its GUI target'}
    $target.Dispose();$target=$null;$launcher.Dispose();$launcher=$null
    }
    if($ManagementClose){'PASS GUI-ONLY-MANAGEMENT-CLOSE';return}
    'PASS GUI survives launcher/request release; two actual exits=37; first worker remains resident'
    # Stable short assertions survive narrow observer Console line wrapping.
    'PASS GUI-SURVIVAL-EXIT-37'
    'PASS GUI-RESIDENCY-'+[bool]$ObserveWorkerResidency
} finally {
    $null=$release.Set()
    if($worker){$worker.Dispose()}
    if($target){$target.Dispose()}
    if($launcher){$launcher.Dispose()}
    $ready.Dispose();$release.Dispose()
}
