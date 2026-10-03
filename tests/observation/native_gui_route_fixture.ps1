param([Parameter(Mandatory)][string]$PackageRoot,
      [switch]$WaitWorkerRetirement)
$ErrorActionPreference='Stop'
$PackageRoot=(Resolve-Path $PackageRoot).Path
$stem='NATIVE-GUI-'+$PID
$ready=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\'+$stem+'-ready')
$release=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,'Local\'+$stem+'-release')
$launcher=$null;$target=$null;$worker=$null
try {
    $start=[Diagnostics.ProcessStartInfo]::new((Join-Path $PackageRoot 'run16.exe'))
    $start.UseShellExecute=$false
    $start.ArgumentList.Add((Join-Path $PackageRoot 'native-gui-startup-probe.exe'))
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
    if($WaitWorkerRetirement) {
        $worker=[Diagnostics.Process]::GetProcessById($matches[0].ParentProcessId)
        $null=$worker.Handle
        if($worker.ProcessName -ne 'ntvwm'){throw 'GUI was not created by NTVWM'}
        if(!$worker.WaitForExit(13000)){throw 'Frontendless idle worker did not retire by service deadline'}
        if($target.HasExited){throw 'Worker retirement killed live GUI'}
    }
    $null=$release.Set()
    if(!$target.WaitForExit(10000) -or $target.ExitCode -ne 37){throw "Wrong actual GUI exit: $($target.ExitCode)"}
    'PASS GUI survives launcher/request release; actual exit=37; worker-retirement='+[bool]$WaitWorkerRetirement
} finally {
    $null=$release.Set()
    if($worker){$worker.Dispose()}
    if($target){$target.Dispose()}
    if($launcher){$launcher.Dispose()}
    $ready.Dispose();$release.Dispose()
}
