[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$WindowReader,
    [Parameter(Mandatory)][string]$PackageRoot,
    [string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$WindowReader=(Resolve-Path -LiteralPath $WindowReader).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
if(!$ProcessPackageRoot){$ProcessPackageRoot=$PackageRoot}
$ProcessPackageRoot=(Resolve-Path -LiteralPath $ProcessPackageRoot).Path
foreach($name in @('run16.exe','ntvdm.exe','ntkvm.exe','ntsrv.exe')){
    if((Get-FileHash (Join-Path $PackageRoot $name)).Hash -ne
        (Get-FileHash (Join-Path $ProcessPackageRoot $name)).Hash){throw 'Process package differs from launch package'}
}
$run=Join-Path $PackageRoot 'run16.exe'
$worker=@((Join-Path $PackageRoot 'ntvdm.exe'),(Join-Path $ProcessPackageRoot 'ntvdm.exe'))
$frontend=@((Join-Path $PackageRoot 'ntkvm.exe'),(Join-Path $ProcessPackageRoot 'ntkvm.exe'))
$report=Join-Path $PackageRoot "logs\$LogPrefix.txt"
if(Test-Path -LiteralPath $report){throw 'Use a fresh log prefix'}
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Broker must be stopped before isolated test'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$controller=$null
$pinned=@()
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $text="winmine`rmem`rexit`r"
    $arguments=@($run,$PackageRoot,$report,'--observation-timeout-ms','45000',
        '--observe-console-input-text',('"'+$text+'"'),
        '--observe-console-line-delay-ms','1000','COMMAND.COM')
    $controller=Start-Process -FilePath $Observer -ArgumentList $arguments -WindowStyle Hidden -PassThru
    $desktop="NTVDMConsoleTest-$($controller.Id)"
    $deadline=[DateTime]::UtcNow.AddSeconds(30)
    $wow=$null
    do {
        $workers=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" |
            Where-Object {$_.ExecutablePath -in $worker})
        foreach($entry in $workers){
            $windows=(& $WindowReader $entry.ProcessId $desktop 2>&1 | Out-String)
            if($windows -match 'NETWORK\.DRV'){
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
    if(!$wow){throw 'No actual WOW NETWORK.DRV frontier observed'}
    $others=@($workers | Where-Object {$_.ProcessId -ne $wow.Id})
    $fronts=@(Get-CimInstance Win32_Process -Filter "Name='ntkvm.exe'" |
        Where-Object {$_.ExecutablePath -in $frontend -and $_.CommandLine -match '--session'})
    if($others.Count -ne 1 -or $fronts.Count -ne 1){throw 'Expected separate DOS/WOW workers and one character frontend'}
    foreach($entry in @($others[0],$fronts[0])){
        $process=Get-Process -Id $entry.ProcessId
        $null=$process.Handle
        $pinned+=$process
    }
    # Deliberate failure of the identified test WOW worker, not a process tree.
    $wow.Kill()
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
    $paths=@('run16.exe','ntkvm.exe','ntvdm.exe','ntsrv.exe') |
        ForEach-Object {(Join-Path $PackageRoot $_);(Join-Path $ProcessPackageRoot $_)}
    Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths} |
        ForEach-Object {Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue}
    if($controller -and !$controller.HasExited){$controller.Kill()}
    foreach($process in $pinned){$process.Dispose()}
    if($controller){$controller.Dispose()}
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate
}
