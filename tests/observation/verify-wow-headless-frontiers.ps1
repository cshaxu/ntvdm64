[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$WindowReader,
    [Parameter(Mandatory)][string]$PackageRoot,
    [string]$ProcessPackageRoot,
    [Parameter(Mandatory)][string]$LogPrefix,
    [string]$LogRoot='O:\winnt\logs',
    [switch]$PackageNetworkProfile
)
$ErrorActionPreference='Stop'
if($LogPrefix -notmatch '^[a-z0-9-]+$'){throw 'Invalid log prefix'}
$Observer=(Resolve-Path -LiteralPath $Observer).Path
$WindowReader=(Resolve-Path -LiteralPath $WindowReader).Path
$PackageRoot=(Resolve-Path -LiteralPath $PackageRoot).Path
$LogRoot=(Resolve-Path -LiteralPath $LogRoot).Path
$launcher=Join-Path $PackageRoot 'run16.exe'
if(!$ProcessPackageRoot){$ProcessPackageRoot=$PackageRoot}
$ProcessPackageRoot=(Resolve-Path -LiteralPath $ProcessPackageRoot).Path
foreach($name in @('run16.exe','ntvdm.exe','ntkvm.exe','ntsrv.exe')){
    if((Get-FileHash (Join-Path $PackageRoot $name)).Hash -ne
        (Get-FileHash (Join-Path $ProcessPackageRoot $name)).Hash){throw 'Process package differs from launch package'}
}
$paths=@('run16.exe','ntvdm.exe','ntkvm.exe','ntsrv.exe') |
    ForEach-Object {(Join-Path $ProcessPackageRoot $_);(Join-Path $PackageRoot $_)}
$worker=@((Join-Path $ProcessPackageRoot 'ntvdm.exe'),(Join-Path $PackageRoot 'ntvdm.exe'))
$frontend=@((Join-Path $ProcessPackageRoot 'ntkvm.exe'),(Join-Path $PackageRoot 'ntkvm.exe'))
if(@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'").Count){throw 'Another broker is running'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths}).Count){throw 'Candidate already in use'}
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
if($PackageNetworkProfile){
    $profile=Get-Content -LiteralPath (Join-Path $PackageRoot 'system.ini') -Raw
    if($profile -notmatch '(?im)^network\.drv\s*=\s*wfwnet\.drv\s*$'){
        throw 'PackageNetworkProfile requires the existing WFWNET.DRV configuration; this test never edits it'
    }
    if((Get-FileHash (Join-Path $PackageRoot 'WFWNET.DRV')).Hash -ne
        '4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA'){
        throw 'Unexpected original WFWNET.DRV media'
    }
}
try {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    foreach($app in @('WINMINE','SOL','WRITE')){
        $controller=$null
        $verified=$false
        $report=Join-Path $LogRoot "$LogPrefix-$($app.ToLowerInvariant()).txt"
        if(Test-Path -LiteralPath $report){throw 'Use a fresh log prefix'}
        try {
            $arguments=@(('"'+$launcher+'"'),('"'+(Join-Path $PackageRoot '.')+'"'),('"'+$report+'"'),
                '--observation-timeout-ms','16000','--wait',"$app.EXE")
            # S8 makes default GUI launch asynchronous. This frontier gate
            # deliberately observes a live target, so request explicit waiting
            # rather than treating successful launcher exit as guest failure.
            $controller=Start-Process -FilePath $Observer -ArgumentList $arguments -WindowStyle Hidden -PassThru
            $desktop="NTVDMConsoleTest-$($controller.Id)"
            $deadline=[DateTime]::UtcNow.AddSeconds(14)
            $frontier=$false
            do {
                $workers=@(Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" |
                    Where-Object {$_.ExecutablePath -in $worker})
                foreach($entry in $workers){
                    $windows=(& $WindowReader $entry.ProcessId $desktop 2>&1 | Out-String)
                    $readerExit=$LASTEXITCODE
                    $windows | Set-Content -LiteralPath ($report+'.last-windows.txt')
                    if($readerExit -ne 0 -or $windows -notmatch ('(?m)^pid='+$entry.ProcessId+' image=')){
                        throw "Worker window reader failed or has incompatible output (exit $readerExit); inspect $report.last-windows.txt"
                    }
                    $reached=if(!$PackageNetworkProfile){$windows -match 'NETWORK\.DRV'}
                        # Retain the owner-accepted non-Chinese ACP rendering:
                        # original GBK bytes may appear as Latin-1 code units.
                        # Both exact forms identify these original media strings;
                        # an arbitrary visible window is not a frontier witness.
                        elseif($app -eq 'WINMINE'){$windows -match '(?m)^window-utf16=\S+ visible=1 class=(626B96F7|00C900A800C000D7) text=\1\r?$'}
                        elseif($app -eq 'SOL'){$windows -match '(?m)^window-utf16=\S+ visible=1 class=005300740061007400690063 text=(51855B584E0D591F|00C400DA00B400E600B200BB00B900BB)\r?$'}
                        else{$windows -match 'Not enough memory for Write to complete this operation\.'}
                    if($reached){
                        if($workers.Count -ne 1){throw 'Expected one WOW worker'}
                        $windows | Set-Content -LiteralPath ($report+'.windows.txt')
                        $frontier=$true
                        break
                    }
                }
                if($frontier){break}
                if($controller.HasExited){throw "$app exited before its retained frontier"}
                Start-Sleep -Milliseconds 200
            } while([DateTime]::UtcNow -lt $deadline)
            if(!$frontier){throw "$app did not reach its selected retained frontier"}
            if(@(Get-CimInstance Win32_Process -Filter "Name='ntkvm.exe'" |
                Where-Object {$_.ExecutablePath -in $frontend}).Count){throw 'GUI launch created a character frontend'}
            if(!$controller.WaitForExit(20000)){throw 'Headless observer did not finish'}
            $record=Get-Content -LiteralPath $report -Raw
            if($record -notmatch '(?m)^result=timeout\r?$' -or
                $record -notmatch '(?m)^exit=0x53504354\r?$'){
                throw "$app no longer remained at the selected observation baseline"
            }
            $description=if(!$PackageNetworkProfile){'NETWORK.DRV modal'}
                elseif($app -eq 'WINMINE'){'localized WINMINE main window; interaction not tested'}
                elseif($app -eq 'SOL'){'original out-of-memory modal; application still incomplete'}
                else{'original Write out-of-memory modal; application still incomplete'}
            $verified=$true
            Write-Output "BASELINE PRESERVED $app : $description, no character frontend; not full application acceptance"
        } finally {
            # Exact isolated package only; not descendant-tree termination.
            # Record the intervention boundary before stopping any target.
            # RPC/launcher failures produced by cleanup are not guest failures.
            $owned=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -in $paths})
            @("cleanup-start-utc=$([DateTime]::UtcNow.ToString('o'))","verification-complete=$verified",
                "observer-running=$($controller -and !$controller.HasExited)") |
                Set-Content -LiteralPath ($report+'.cleanup.txt')
            $owned | ForEach-Object {"pid=$($_.ProcessId) image=$($_.ExecutablePath)"} |
                Add-Content -LiteralPath ($report+'.cleanup.txt')
            $owned |
                ForEach-Object {
                    $process=Get-Process -Id $_.ProcessId -ErrorAction SilentlyContinue
                    if($process){try {$null=$process.Handle;$process.Kill();$null=$process.WaitForExit(5000)}finally{$process.Dispose()}}
                }
            if($controller){
                if(!$controller.HasExited){$controller.Kill();$null=$controller.WaitForExit(5000)}
                $controller.Dispose()
            }
        }
    }
} finally {$env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate}
