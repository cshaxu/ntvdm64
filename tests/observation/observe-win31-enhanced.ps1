param([Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Pif,
      [Parameter(Mandatory)][string]$BuildRoot,[switch]$IgnoreDeniedDiskWarning,
      [string]$SurfaceObserver='', [string]$GuestObserver='', [string]$GuestMap='',
      [string]$Python='', [switch]$QuiescedGuestSnapshot,
      [string]$RequiredTempDirectory='',
      [string]$RunNotepadProbe='', [string]$DesktopReadyHash='',
      [string]$NotepadReadyHashes='',
      [ValidateRange(10000,60000)][int]$ObservationTimeoutMs=30000)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$observerPath=(Resolve-Path $Observer).Path
if($RequiredTempDirectory -and !(Test-Path -LiteralPath $RequiredTempDirectory -PathType Container)) {
    throw 'Missing launcher TEMP prerequisite; direct PIF observation does not run win.cmd mkdir'
}
$binary=Join-Path (Resolve-Path $PackageRoot).Path 'system32'
if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -eq "$binary\ntsrv.exe"}).Count){throw 'Preserve existing broker/session; trial requires idle endpoint'}
New-Item -ItemType Directory -Path $root|Out-Null
if($RequiredTempDirectory){
    [IO.File]::WriteAllText("$root/launch-precondition.txt",'existing-temp='+
        (Resolve-Path -LiteralPath $RequiredTempDirectory).Path+"`r`n")
}
# Load the bounded dialog inspector before the observation timer starts.
$allowDeniedDisk=[bool]$IgnoreDeniedDiskWarning
. "$PSScriptRoot/inspect-session-windows.ps1" -ProcessIds @(0) -BuildRoot $root
$savedDesktop=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
$pins=@();$controller=$null
try {
    $report=Join-Path $root 'observation.txt'
    $controller=Start-Process $observerPath -ArgumentList @("$binary\run16.exe",$PackageRoot,$report,$Pif,'--observation-timeout-ms',"$ObservationTimeoutMs") -WindowStyle Hidden -PassThru
    $desktop='NTVDMConsoleTest-'+$controller.Id
    $known=@($controller.Id);$last='';$nextDiscovery=[DateTime]::UtcNow
    $nextSnapshot=[DateTime]::UtcNow.AddSeconds(5);$sample=0;$inputPosted=$false;$textPosted=$false
    while(!$controller.WaitForExit(100)) {
        if([DateTime]::UtcNow -ge $nextDiscovery) {
            $rows=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -eq $observerPath -or $_.ExecutablePath -like "$binary\*"})
            for($round=0;$round -lt 5;$round++) {
                foreach($row in $rows) {
                    if($row.ParentProcessId -in $known -and $row.ProcessId -notin $known) {
                        $known+=$row.ProcessId
                        try {
                            $p=[Diagnostics.Process]::GetProcessById($row.ProcessId);$null=$p.Handle
                            if(!$p.HasExited -and $p.MainModule.FileName -eq $row.ExecutablePath){$pins+=$p}else{$p.Dispose()}
                        } catch [ArgumentException] {}
                    }
                }
            }
            $nextDiscovery=[DateTime]::UtcNow.AddSeconds(1)
        }
        try {$text=[WindowInspector]::Read([int[]]$known,$desktop,$true,$allowDeniedDisk)}catch {
            [IO.File]::AppendAllText("$root/probe-errors.txt",[DateTime]::UtcNow.ToString('o')+' '+$_.Exception.Message+"`r`n")
            continue
        }
        if($text -ne $last) {
            [IO.File]::AppendAllText("$root/transitions.txt",[DateTime]::UtcNow.ToString('o')+"`r`n"+$text)
            Write-Output $text;$last=$text
        }
        if([DateTime]::UtcNow -ge $nextSnapshot) {
            foreach($p in $pins) {
                if($p.HasExited){continue}
                if($SurfaceObserver -and $p.MainModule.FileName -eq "$binary\ntcon.exe") {
                    & $SurfaceObserver $p.Id $desktop "$root/surface-$sample.bmp" |
                        Set-Content "$root/surface-$sample.txt"
                    if(!$inputPosted -and $RunNotepadProbe -and $DesktopReadyHash -and
                       (Test-Path "$root/surface-$sample.bmp") -and
                       (Get-FileHash "$root/surface-$sample.bmp").Hash -eq $DesktopReadyHash) {
                        & $RunNotepadProbe $p.Id $desktop | Set-Content "$root/notepad-input.txt"
                        if($LASTEXITCODE){throw 'Failed to post owned private-desktop input'}
                        $inputPosted=$true
                    }
                    elseif($inputPosted -and !$textPosted -and $NotepadReadyHashes -and
                           (Test-Path "$root/surface-$sample.bmp") -and
                           (Get-FileHash "$root/surface-$sample.bmp").Hash -in $NotepadReadyHashes.Split(',')){
                        & $RunNotepadProbe $p.Id $desktop 'enhanced-ok' | Set-Content "$root/notepad-text-input.txt"
                        if($LASTEXITCODE){throw 'Failed to post owned Notepad text'}
                        $textPosted=$true
                    }
                }
                if($GuestObserver -and $GuestMap -and $p.MainModule.FileName -eq "$binary\ntvdm.exe") {
                    [IO.File]::AppendAllText("$root/cpu-times.txt",[DateTime]::UtcNow.ToString('o')+" sample=$sample pid=$($p.Id) cpu=$($p.TotalProcessorTime.TotalSeconds)`r`n")
                    & $GuestObserver $p.Id $GuestMap "$root/memory-$sample.bin" |
                        Set-Content "$root/guest-$sample.txt"
                    if($Python){
                        $snapshotOptions=@()
                        if($QuiescedGuestSnapshot){$snapshotOptions+='--freeze'}
                        & $Python "$PSScriptRoot/read-ccpu-paged.py" --pid $p.Id --image "$binary\ntvdm.exe" --map $GuestMap --output "$root/paged-$sample.json" @snapshotOptions |
                            Set-Content "$root/paged-$sample.txt"
                    }
                }
            }
            $sample++;$nextSnapshot=[DateTime]::UtcNow.AddSeconds(5)
        }
    }
    $controller.WaitForExit()
    [IO.File]::WriteAllText("$root/controller-exit.txt",[string]$controller.ExitCode)
    Get-Content $report
    if(Test-Path "$report.console.txt"){Get-Content "$report.console.txt"}
} finally {
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$savedDesktop
    # Only pinned descendants of this observer; no name-based or tree kill.
    foreach($p in @($pins|Sort-Object Id -Descending)) {
        try{if(!$p.HasExited){$p.Kill();$null=$p.WaitForExit(5000)}}finally{$p.Dispose()}
    }
    if($controller){try{if(!$controller.HasExited){$controller.Kill();$null=$controller.WaitForExit(5000)}}finally{$controller.Dispose()}}
}
