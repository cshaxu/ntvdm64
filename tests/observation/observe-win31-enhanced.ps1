param([Parameter(Mandatory)][string]$Observer,
      [Parameter(Mandatory)][string]$PackageRoot,
      [Parameter(Mandatory)][string]$Pif,
      [Parameter(Mandatory)][string]$BuildRoot,[switch]$IgnoreDeniedDiskWarning,
      [string]$SurfaceObserver='', [string]$GuestObserver='', [string]$GuestMap='',
      [string]$Python='', [switch]$QuiescedGuestSnapshot,
      [switch]$EarlyGuestSnapshot,
      [string]$RequiredTempDirectory='',
      [string]$RunNotepadProbe='', [string]$DesktopReadyHash='',
      [string]$DesktopReadyRegion='',
      [string]$NotepadReadyHashes='',
      [string]$MouseProbeRoot='',
      [switch]$MouseExitSequence,
      [string]$ExitConfirmProbe='', [string]$ExitDialogRegion='', [string]$ExitDialogHash='',
      [ValidateRange(10000,60000)][int]$ObservationTimeoutMs=30000)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $root)){throw 'Fresh build-owned evidence required'}
$observerPath=(Resolve-Path $Observer).Path
function DesktopHash([string]$path,[string]$region=$DesktopReadyRegion) {
    if(!$region){return (Get-FileHash $path).Hash}
    $rect=@($region.Split(',')|ForEach-Object {[int]$_})
    if($rect.Count -ne 4){throw 'Desktop rectangle requires left,top,right,bottom'}
    $bytes=[IO.File]::ReadAllBytes($path)
    if($bytes.Length -lt 54 -or $bytes[0] -ne 0x42 -or $bytes[1] -ne 0x4d -or
       [BitConverter]::ToUInt16($bytes,28) -ne 32 -or [BitConverter]::ToInt32($bytes,30) -ne 0) {
        throw 'Readiness requires uncompressed32-bit BMP'
    }
    $offset=[BitConverter]::ToInt32($bytes,10);$width=[BitConverter]::ToInt32($bytes,18)
    $height=-[BitConverter]::ToInt32($bytes,22)
    if($width -lt 1 -or $height -lt 1 -or $rect[0] -lt 0 -or $rect[1] -lt 0 -or
       $rect[2] -le $rect[0] -or $rect[3] -le $rect[1] -or $rect[2] -gt $width -or
       $rect[3] -gt $height -or [int64]$offset+[int64]$width*$height*4 -gt $bytes.Length) {
        throw 'Invalid top-down BMP/crop extent'
    }
    $row=($rect[2]-$rect[0])*4;$crop=[byte[]]::new($row*($rect[3]-$rect[1]))
    for($y=$rect[1];$y -lt $rect[3];$y++) {
        [Buffer]::BlockCopy($bytes,$offset+($y*$width+$rect[0])*4,$crop,($y-$rect[1])*$row,$row)
    }
    $sha=[Security.Cryptography.SHA256]::Create()
    try{return ([BitConverter]::ToString($sha.ComputeHash($crop))).Replace('-','')}
    finally{$sha.Dispose()}
}
if($MouseProbeRoot -and ($RunNotepadProbe -or !$SurfaceObserver -or !$DesktopReadyHash)) {
    throw 'Mouse observation requires surface/desktop readiness and no concurrent keyboard sequence'
}
if($MouseExitSequence -and !$MouseProbeRoot){throw 'Exit mouse sequence requires mouse probe'}
if($ExitConfirmProbe -and (!$MouseExitSequence -or !$ExitDialogRegion -or !$ExitDialogHash)) {
    throw 'Exit confirmation requires an observed specific dialog rectangle/hash'
}
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
    $nextSnapshot=[DateTime]::UtcNow.AddSeconds(5);$sample=0;$inputPosted=$false;$textPosted=$false;$mousePosted=$false;$exitConfirmed=$false
    while(!$controller.WaitForExit(100)) {
        if([DateTime]::UtcNow -ge $nextDiscovery) {
            $rows=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -eq $observerPath -or $_.ExecutablePath -like "$binary\*"})
            for($round=0;$round -lt 5;$round++) {
                foreach($row in $rows) {
                    if($row.ParentProcessId -in $known -and $row.ProcessId -notin $known) {
                        $known+=$row.ProcessId
                        try {
                            $p=[Diagnostics.Process]::GetProcessById($row.ProcessId);$null=$p.Handle
                            if(!$p.HasExited -and $p.MainModule.FileName -eq $row.ExecutablePath){
                                $pins+=$p
                                if($EarlyGuestSnapshot -and $p.MainModule.FileName -eq "$binary\ntvdm.exe"){$nextSnapshot=[DateTime]::UtcNow}
                            }else{$p.Dispose()}
                        } catch [ArgumentException] {}
                    }
                }
            }
            $nextDiscovery=[DateTime]::UtcNow.AddMilliseconds($(if($EarlyGuestSnapshot){100}else{1000}))
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
                    if($mousePosted -and !$exitConfirmed -and $ExitConfirmProbe -and
                       (Test-Path "$root/surface-$sample.bmp") -and
                       (DesktopHash "$root/surface-$sample.bmp" $ExitDialogRegion) -eq $ExitDialogHash) {
                        & $ExitConfirmProbe $p.Id $desktop '--enter' |Set-Content "$root/exit-confirmation.txt"
                        if($LASTEXITCODE){throw 'Unable to post confirmed guest exit-dialog key'}
                        $exitConfirmed=$true
                    }
                    if(!$mousePosted -and $MouseProbeRoot -and (Test-Path "$root/surface-$sample.bmp") -and
                       (DesktopHash "$root/surface-$sample.bmp") -eq $DesktopReadyHash) {
                        $previousHash=(Get-FileHash "$root/surface-$sample.bmp").Hash
                        $actions=@(@{name='corner';dx=1000;dy=1000;buttons=0},
                            @{name='file';dx=-552;dy=-400;buttons=0},
                            @{name='down';dx=0;dy=0;buttons=1},@{name='up';dx=0;dy=0;buttons=0})
                        if($MouseExitSequence){$actions+=@(
                            @{name='exit-move';dx=25;dy=159;buttons=0},
                            @{name='exit-down';dx=0;dy=0;buttons=1},
                            @{name='exit-up';dx=0;dy=0;buttons=0})}
                        foreach($action in $actions) {
                            & "$MouseProbeRoot/probe.exe" $p.Id $desktop "$MouseProbeRoot/mouse-hook.dll" `
                                $action.dx $action.dy $action.buttons "$root/mouse-$($action.name).txt"
                            if($LASTEXITCODE){throw 'Private input sink did not acknowledge'}
                            $deadline=[DateTime]::UtcNow.AddSeconds(3);$attempt=0;$changed=$false
                            do {
                                $capture="$root/mouse-$($action.name)-$attempt.bmp"
                                & $SurfaceObserver $p.Id $desktop $capture | Set-Content "$capture.txt"
                                if(!$LASTEXITCODE -and (Test-Path $capture)) {
                                    $hash=(Get-FileHash $capture).Hash
                                    if($hash -ne $previousHash){$previousHash=$hash;$changed=$true;break}
                                }
                                $attempt++;Start-Sleep -Milliseconds 50
                            }while([DateTime]::UtcNow -lt $deadline)
                            [IO.File]::AppendAllText("$root/mouse-observations.txt",
                                "$($action.name) fresh-pixels=$changed capture=$capture`r`n")
                            # A changed surface is only transport evidence.  The
                            # authoritative guest-consumption assertion is below:
                            # after the complete move/down/up sequence it must
                            # expose the exact Exit Windows confirmation dialog.
                            # Program Manager does not reliably repaint its menu
                            # hover selection for this synthetic relative route.
                        }
                        $mousePosted=$true
                        if($MouseExitSequence){Write-Output "Exit sequence posted; inspect fresh guest confirmation before confirming: frontend=$($p.Id) desktop=$desktop"}
                    }
                    if(!$inputPosted -and $RunNotepadProbe -and $DesktopReadyHash -and
                       (Test-Path "$root/surface-$sample.bmp") -and
                       (DesktopHash "$root/surface-$sample.bmp") -eq $DesktopReadyHash) {
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
