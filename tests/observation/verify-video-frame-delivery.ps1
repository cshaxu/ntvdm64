param(
    [Parameter(Mandatory)][string]$FormalBuild,
    [Parameter(Mandatory)][string]$WowBuild,
    [Parameter(Mandatory)][string]$Backup,
    [Parameter(Mandatory)][string]$GuestProbe,
    [Parameter(Mandatory)][string]$Prefix,
    [string]$DiagnosticWorker,
    [switch]$StandardFrontend,
    [string]$PackageRoot='O:\winnt'
)
$ErrorActionPreference='Stop'
$names=@('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe','WOW32.DLL','VDMREDIR.DLL')
$paths=@($names | ForEach-Object {Join-Path $PackageRoot $_})
function Copy-PackageFile([string]$Source,[string]$Destination) {
    # Process signalling can precede release of its image section. Retry only
    # sharing/lock violations, bounded to five seconds; other errors stay fatal.
    for($attempt=0; ; ++$attempt){
        try {
            Copy-Item -LiteralPath $Source -Destination $Destination -Force -ErrorAction Stop
            return
        } catch {
            $cause=$_.Exception
            while($cause.InnerException){$cause=$cause.InnerException}
            $code=$cause.HResult -band 0xffff
            if($code -notin @(32,33) -or $attempt -ge 25){throw}
            Start-Sleep -Milliseconds 200
        }
    }
}
function Stop-Package {
    Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe' OR Name='dtmgr.exe'" |
        Where-Object {$_.ExecutablePath -in $paths} | ForEach-Object {
            $p=Get-Process -Id $_.ProcessId -ErrorAction SilentlyContinue
            if($p){$p.Kill();if(!$p.WaitForExit(10000)){throw 'Package cleanup timeout'}}
        }
}
$report=Join-Path $PackageRoot ('logs\'+$Prefix+'-frames.txt')
$observation=Join-Path $PackageRoot ('logs\'+$Prefix+'-observer.txt')
if(Test-Path $report){throw 'Use a fresh evidence prefix'}
foreach($name in $names){
    if((Get-FileHash (Join-Path $Backup $name)).Hash -ne (Get-FileHash (Join-Path $PackageRoot $name)).Hash){throw "Backup differs: $name"}
}
Stop-Package
$oldReport=$env:MVDM_TEST_FRAME_REPORT
$oldRoot=$env:MVDM_TEST_PACKAGE_ROOT
$oldDesktop=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
try {
    foreach($name in $names){
        $source=Join-Path $(if($name -eq 'WOW32.DLL'){$WowBuild}else{$FormalBuild}) $name
        if($name -eq 'ntvdm.exe' -and $DiagnosticWorker){$source=$DiagnosticWorker}
        Copy-PackageFile $source (Join-Path $PackageRoot $name)
    }
    $probe=Join-Path $PackageRoot 'tests\VFRAME.COM'
    Copy-Item -LiteralPath $GuestProbe -Destination $probe -Force
    $env:MVDM_TEST_FRAME_REPORT=$report
    $env:MVDM_TEST_PACKAGE_ROOT=$PackageRoot
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    $launcher=if($StandardFrontend){Join-Path $PackageRoot 'run16.exe'}else{Join-Path $FormalBuild 'run16-video-observer.exe'}
    & (Join-Path $FormalBuild 'console-startup-observer.exe') $launcher $PackageRoot $observation --observation-timeout-ms 20000 $probe
    if($LASTEXITCODE -or !(Select-String -LiteralPath $observation -Pattern '^exit=0x00000000$' -Quiet)){throw 'Guest did not complete'}
    if(!(Select-String -LiteralPath ($observation+'.console.txt') -SimpleMatch 'GRAPHICS_FRAME_RETURN_OK' -Quiet)){throw 'Guest return marker missing'}
    if($StandardFrontend){
        'PASS uninstrumented production launcher/worker: guest graphics-to-text exit and final Console marker; copied-pixel proof is the separate observed-frontend case.'
        return
    }
    $lines=Get-Content -LiteralPath $report
    $frame=$false;$text=$false
    foreach($line in $lines){
        if($line -match '^FRAME serial=\d+ width=(\d+) height=(\d+) depth=8 a=(\d+) b=(\d+) other=0 paletteA=([0-9a-f]+) paletteB=([0-9a-f]+)$'){
            $half=([long]$Matches[1]*[long]$Matches[2])/2
            if([long]$Matches[3] -eq $half -and [long]$Matches[4] -eq $half -and $Matches[5] -ne $Matches[6]){$frame=$true}
        }
        if($frame -and $line -match '^TEXT serial=\d+ result=0$'){$text=$true}
    }
    if(!$frame -or !$text){throw 'Known guest pixels/palette or subsequent TEXT not received'}
    'PASS real guest pixels/palette through original painter and production frame receiver; later text retirement. Test-only logging/launcher package discovery, no Window rendering claim.'
} finally {
    Stop-Package
    foreach($name in $names){Copy-PackageFile (Join-Path $Backup $name) (Join-Path $PackageRoot $name)}
    foreach($name in $names){if((Get-FileHash (Join-Path $Backup $name)).Hash -ne (Get-FileHash (Join-Path $PackageRoot $name)).Hash){throw "Restore differs: $name"}}
    $env:MVDM_TEST_FRAME_REPORT=$oldReport
    $env:MVDM_TEST_PACKAGE_ROOT=$oldRoot
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldDesktop
    'RESTORED coherent package'
}
