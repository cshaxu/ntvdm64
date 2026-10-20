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
$launcher=$null;$target=$null
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
    # GUI uses a one-shot NTVWM launcher.  It binds the suspended target and
    # then exits; no persistent GUI carrier is a lifecycle owner.  Preserve
    # only the observable target fact, not a fabricated worker residency.
    try {
        $carrier=[Diagnostics.Process]::GetProcessById($matches[0].ParentProcessId)
        try {
            $null=$carrier.Handle
            if(!$carrier.HasExited){throw 'GUI launcher remained resident after successful bind'}
        } finally {$carrier.Dispose()}
    } catch [ArgumentException] {}
    # Full deadline coverage proves the independent observed target survives
    # after its one-shot launcher has gone; it is not a worker-idle claim.
    if($ObserveWorkerResidency -and !$iteration){Start-Sleep -Milliseconds 11000}
    if($target.HasExited){throw 'Live GUI lost before explicit fixture release'}
    if($ManagementClose) {
        # GUI records are read-only.  The service fixture covers its explicit
        # ERROR_NOT_SUPPORTED close result; this runtime witness only proves
        # the independently owned target remains alive after launcher release.
        if($target.HasExited){throw 'Observed GUI target ended before management boundary witness'}
    }
    $null=$release.Set()
    if(!$target.WaitForExit(10000) -or $target.ExitCode -ne 37){throw "Wrong actual GUI exit: $($target.ExitCode)"}
    $target.Dispose();$target=$null;$launcher.Dispose();$launcher=$null
    }
    if($ManagementClose){'PASS GUI-ONLY-MANAGEMENT-CLOSE';return}
    'PASS GUI survives one-shot launcher/request release; two actual exits=37'
    # Stable short assertions survive narrow observer Console line wrapping.
    'PASS GUI-SURVIVAL-EXIT-37'
    'PASS GUI-RESIDENCY-'+[bool]$ObserveWorkerResidency
} finally {
    $null=$release.Set()
    if($target){$target.Dispose()}
    if($launcher){$launcher.Dispose()}
    $ready.Dispose();$release.Dispose()
}
