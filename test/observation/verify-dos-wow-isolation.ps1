[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$WindowReader,
    [Parameter(Mandatory)][string]$PackageRoot,
    [string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$BuildRoot,
    [Parameter(Mandatory)][string]$LogPrefix
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$evidence=[IO.Path]::GetFullPath($BuildRoot)
if(!$evidence.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path -LiteralPath $evidence)){throw 'Use a fresh repository build evidence directory'}
$null=New-Item -ItemType Directory -Path $evidence
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$WindowReader=(Resolve-Path -LiteralPath $WindowReader).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
if(!$ProcessPackageRoot){$ProcessPackageRoot=$PackageRoot}
$ProcessPackageRoot=(Resolve-Path -LiteralPath $ProcessPackageRoot).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$sourceBinary=Get-PackageBinaryRoot $PackageRoot
$processBinary=Get-PackageBinaryRoot $ProcessPackageRoot
$networkProfile=(Get-Content -LiteralPath (Join-Path $PackageRoot 'system.ini') -Raw) -match
    '(?im)^network\.drv\s*=\s*wfwnet\.drv\s*$'
foreach($name in @('run16.exe','ntvdm.exe','ntcon.exe','ntsrv.exe')){
    if((Get-FileHash (Join-Path $sourceBinary $name)).Hash -ne
        (Get-FileHash (Join-Path $processBinary $name)).Hash){throw 'Process package differs from launch package'}
}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$saved=@{}
foreach($name in @('TEMP','TMP','PATH','MVDM_OBSERVER_AFTER_FIRST_LINE_EVENT')){
    $saved[$name]=[Environment]::GetEnvironmentVariable($name)
}
$runtime=$null;$scope=$null;$controller=$null;$firstLineGate=$null
$pinned=@()
$lastWindows=''
try {
    $runtime=New-PhysicalRuntimeStage $PackageRoot
    $scope=New-IsolatedPackageScope $runtime
    $binary=Get-PackageBinaryRoot $runtime
    $run=Join-Path $binary 'run16.exe'
    $worker=@((Join-Path $binary 'ntvdm.exe'))
    $frontend=@((Join-Path $binary 'ntcon.exe'))
    $report=Join-Path $evidence "$LogPrefix.txt"
    # The structured package keeps DOS system programs under SYSTEM32.  The
    # observer launches the public CLI by full path, but its bare COMMAND.COM
    # argument must resolve within this exact staged package, never through a
    # caller's installed product PATH.
    $env:TEMP=$runtime;$env:TMP=$runtime;$env:PATH="$binary;"+$env:SystemRoot+'\System32'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    # After WINMINE is entered, hold later DOS commands at an explicit test
    # gate.  The parent opens it only after it has identified and terminated
    # the actual modal WOW worker; the observer then waits for the fresh DOS
    # prompt before it submits MEM/EXIT.
    $gateName='Local\NTVDM64-DOS-WOW-'+[Guid]::NewGuid().ToString('N')
    $firstLineGate=[Threading.EventWaitHandle]::new($false,[Threading.EventResetMode]::ManualReset,$gateName)
    $env:MVDM_OBSERVER_AFTER_FIRST_LINE_EVENT=$gateName
    $text="winmine`rmem`rexit`r"
    $arguments=@($run,$runtime,$report,'--observation-timeout-ms','45000',
        # ArgumentList carries one complete argv element; adding shell-style
        # quotes here would make COMMAND receive a literal leading quote.
        '--observe-console-input-text',$text,
        '--observe-console-line-delay-ms','1000','COMMAND.COM')
    # ProcessStartInfo makes the inter-line gate part of this child's explicit
    # test contract.  Do not rely on Start-Process inheriting a mutated parent
    # environment while a test host may itself be an environment broker.
    $start=[Diagnostics.ProcessStartInfo]::new($Observer)
    $start.UseShellExecute=$false
    $start.WindowStyle=[Diagnostics.ProcessWindowStyle]::Hidden
    $start.EnvironmentVariables['TEMP']=$runtime
    $start.EnvironmentVariables['TMP']=$runtime
    $start.EnvironmentVariables['PATH']=$env:PATH
    $start.EnvironmentVariables['MVDM_OBSERVER_PRIVATE_DESKTOP']='1'
    $start.EnvironmentVariables['MVDM_OBSERVER_AFTER_FIRST_LINE_EVENT']=$gateName
    foreach($argument in $arguments){$start.ArgumentList.Add($argument)}
    $controller=[Diagnostics.Process]::Start($start)
    $desktop="NTVDMConsoleTest-$($controller.Id)"
    $deadline=[DateTime]::UtcNow.AddSeconds(30)
    $wow=$null
    do {
        $workers=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" |
            Where-Object {$_.ExecutablePath -in $worker})
        foreach($entry in $workers){
            $windows=(& $WindowReader $entry.ProcessId $desktop 2>&1 | Out-String)
            $lastWindows=$windows
            # The immutable package's selected original network profile
            # controls WINMINE's first UI frontier.  A WFWNET profile reaches
            # its localized main window; the no-network baseline reaches the
            # original NETWORK.DRV dialog.  Neither is interchangeable.
            $frontier=if($networkProfile){
                $windows -match '(?m)^window=\S+ class=Minesweeper visible=1 text=Minesweeper\r?$' -or
                $windows -match '(?m)^window-utf16=\S+ visible=1 class=(626B96F7|00C900A800C000D7) text=\1\r?$'
            }else{$windows -match 'NETWORK\.DRV'}
            if($frontier){
                $windows | Set-Content -LiteralPath ($report+'.wow-windows.txt')
                $wow=Get-Process -Id $entry.ProcessId
                $null=$wow.Handle
                $pinned+=$wow
                break
            }
        }
        if($wow){break}
        if($controller.HasExited){throw 'DOS chain exited before WOW modal'}
        Start-Sleep -Milliseconds 250
    } while([DateTime]::UtcNow -lt $deadline)
    if(!$wow){
        $lastWindows | Set-Content -LiteralPath ($report+'.last-windows.txt')
        throw 'No actual WOW WINMINE frontier observed'
    }
    $others=@($workers | Where-Object {$_.ProcessId -ne $wow.Id})
    $fronts=@(Get-CimInstance Win32_Process -Filter "Name='ntcon.exe'" |
        Where-Object {$_.ExecutablePath -in $frontend -and $_.CommandLine -match '--session'})
    if($others.Count -ne 1 -or $fronts.Count -ne 1){throw 'Expected separate DOS/WOW workers and one character frontend'}
    foreach($entry in @($others[0],$fronts[0])){
        $process=Get-Process -Id $entry.ProcessId
        $null=$process.Handle
        $pinned+=$process
    }
    # Deliberate failure of the identified test WOW worker, not a process tree.
    $wow.Kill()
    if(!$wow.WaitForExit(5000)){throw 'Identified WOW worker did not exit after deliberate test termination'}
    $null=$firstLineGate.Set()
    if(!$controller.WaitForExit(20000)){throw 'DOS parent did not recover from WOW worker failure'}
    $record=Get-Content -LiteralPath $report -Raw
    $screen=Get-Content -LiteralPath ($report+'.console.txt') -Raw
    if($record -notmatch '(?m)^result=exited' -or $record -notmatch '(?m)^exit=0x00000001\r?$' -or
        $screen -notmatch 'bytes.*memory' -or $screen -match 'Bad command or filename'){
        throw 'No successful MEM/COMMAND return witness after WOW failure'
    }
    Write-Output 'PASS separate WOW worker failure returns to DOS MEM/COMMAND without a GUI-owned frontend'
} finally {
    # Exact candidate image paths only; never kill by ancestry or basename.
    # Restoration is unconditional: a cleanup diagnostic must not leak the
    # isolated environment into a subsequent product case.
    try {
        if($controller -and !$controller.HasExited){$controller.Kill()}
        foreach($process in $pinned){$process.Dispose()}
        if($controller){$controller.Dispose()}
        if($firstLineGate){$firstLineGate.Dispose()}
        if($scope){Stop-IsolatedPackageScope $scope}
        if($runtime){Remove-PhysicalRuntimeStage $runtime}
    } finally {
        $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
        foreach($name in $saved.Keys){[Environment]::SetEnvironmentVariable($name,$saved[$name])}
    }
}
