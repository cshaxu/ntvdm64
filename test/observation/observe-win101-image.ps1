param([Parameter(Mandatory)][string]$InstallationRoot,[string]$MouseProbeRoot='', [switch]$BatchReturn,[string]$TraceRoot='', [string]$PaletteSnapshotRoot='', [string]$WorkerRepairRoot='', [switch]$RequireNormalReturn,[switch]$Repeat)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=(Resolve-Path (Join-Path $repo $InstallationRoot)).Path
if(!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned image test required'}
if($RequireNormalReturn -and (!$BatchReturn -or !$MouseProbeRoot -or $TraceRoot)) {
    throw 'Normal-return acceptance requires real mouse, batch continuation and no instrumentation'
}
if($Repeat -and !$RequireNormalReturn){throw 'Repeat requires strict normal-return acceptance'}
if($Repeat -and (Test-Path "$root/runtime/TMP/FIRST.DONE")){throw 'Stale repeat completion marker'}
$proof=Get-Content "$root/inputs.json" -Raw | ConvertFrom-Json
if($proof.role -notin @('mouse-module-image-test','original-worker-patched-mouse-image-test')){throw 'Wrong image test'}
if(Test-Path Z:\){throw 'Z: occupied'}
$observer=(Resolve-Path "$repo/build/M0-T425/S9/r033/console-startup-observer.exe").Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$oldPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP;$oldTemp=$env:TEMP;$oldTmp=$env:TMP
$oldTrace=$env:WIN101_PALETTE_TRACE
$workerMap="$repo/build/M0-T434/S3/r001-formal/ntvdm.exe.map"
if($TraceRoot -and $WorkerRepairRoot){throw 'Do not mix diagnostic and repair workers'}
if($TraceRoot -or $WorkerRepairRoot){
    $selectedWorker=if($TraceRoot){$TraceRoot}else{$WorkerRepairRoot}
    $trace=(Resolve-Path (Join-Path $repo $selectedWorker)).Path
    if(!$trace.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Private trace must be build owned'}
    Copy-Item -LiteralPath "$trace/ntvdm.exe" -Destination "$($proof.runtime)/system32/ntvdm.exe" -Force
    $workerMap="$trace/ntvdm.exe.map"
}
[ordered]@{workerSha256=(Get-FileHash "$($proof.runtime)/system32/ntvdm.exe").Hash;
    map=$workerMap;diagnostic=[bool]$TraceRoot;repair=[bool]$WorkerRepairRoot} |
    ConvertTo-Json | Set-Content "$root/runtime-worker.json"
$scope=$null;$mapped=$false;$owner=$null
try {
    & subst.exe Z: $proof.runtime
    if($LASTEXITCODE){throw 'Z: mapping failed'}
    $mapped=$true
    $scope=New-IsolatedPackageScope $proof.runtime 'Z:\'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1';$env:TEMP='Z:\TMP';$env:TMP='Z:\TMP'
    # Keep the guest-inherited environment path within the same short-path
    # contract as TEMP; retain the trace outside the runtime after observation.
    if($TraceRoot){$env:WIN101_PALETTE_TRACE='Z:\TMP\palette-trace.txt'}
    $report=Join-Path $root 'windows.txt'
    $target=if($Repeat){'Z:\WIN101\REPEAT.BAT'}elseif($BatchReturn){'Z:\WIN101\RETWIN.BAT'}else{'Z:\WIN101\WIN.COM'}
    $proc=Start-Process -FilePath $observer -ArgumentList @('Z:\system32\run16.exe','Z:\WIN101',$report,
        $target,'--observation-timeout-ms',$(if($MouseProbeRoot){'60000'}else{'30000'})) -WindowStyle Hidden -PassThru
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(8)
        do {
            $row=Get-CimInstance Win32_Process -Filter "ParentProcessId=$($proc.Id)" |
                Where-Object {$_.ExecutablePath -eq $observer} | Select-Object -First 1
            if(!$row){Start-Sleep -Milliseconds 100}
        } while(!$row -and [DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
        if(!$row){throw 'Private observer child absent'}
        $owner=[Diagnostics.Process]::GetProcessById($row.ProcessId);$null=$owner.Handle
        if($owner.HasExited -or $owner.MainModule.FileName -ne $observer){throw 'Owner identity changed'}
        [ordered]@{pid=$owner.Id;parentPid=$proc.Id;desktop="NTVDMConsoleTest-$($proc.Id)";image=$observer;
            startUtc=$owner.StartTime.ToUniversalTime().ToString('o')} | ConvertTo-Json | Set-Content "$root/setup-context.json"
        $deadline=[DateTime]::UtcNow.AddSeconds(15)
        do {
            $peers=@(Get-CimInstance Win32_Process -Filter "Name='ntcon.exe' OR Name='ntvdm.exe'" |
                Where-Object {$_.ExecutablePath -in $scope.Paths})
            $front=$peers|Where-Object Name -eq 'ntcon.exe'|Select-Object -First 1
            $worker=$peers|Where-Object Name -eq 'ntvdm.exe'|Select-Object -First 1
            if($front -and $worker){
                & "$PSScriptRoot/win101_setup_visual_probe.ps1" -ObserverPid $owner.Id -OutputRoot $root -FrontendPid $front.ProcessId -WorkerPid $worker.ProcessId -NoToggle
                # Finite startup-frontier sampling, not a completion assertion.
                foreach($sample in 1..3) {
                    if($owner.HasExited){break}
                    Start-Sleep -Seconds 3
                    if($owner.HasExited){break}
                    $sampleRoot=Join-Path $root "capture-$sample"
                    New-Item -ItemType Directory -Path $sampleRoot -Force|Out-Null
                    Copy-Item -LiteralPath "$root/setup-context.json" -Destination "$sampleRoot/setup-context.json"
                    & "$PSScriptRoot/win101_setup_visual_probe.ps1" -ObserverPid $owner.Id -OutputRoot $sampleRoot -FrontendPid $front.ProcessId -WorkerPid $worker.ProcessId -NoToggle
                }
                if($MouseProbeRoot -and !$owner.HasExited) {
                    $mouse=(Resolve-Path (Join-Path $repo $MouseProbeRoot)).Path
                    if(!$mouse.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Build-owned mouse probe required'}
                    $frontPin=[Diagnostics.Process]::GetProcessById($front.ProcessId)
                    $workerPin=[Diagnostics.Process]::GetProcessById($worker.ProcessId)
                    try {
                        $null=$frontPin.Handle
                        $null=$workerPin.Handle
                        if($frontPin.HasExited -or $frontPin.MainModule.FileName -notin $scope.Paths){throw 'Frontend pin mismatch'}
                        foreach($cycle in 1..$(if($Repeat){2}else{1})) {
                        $cycleRoot=$root
                        if($cycle -eq 2) {
                            if($workerPin.HasExited -or $workerPin.MainModule.FileName -notin $scope.Paths) {
                                throw 'Repeat did not retain the original worker instance'
                            }
                            $returned=Join-Path $proof.runtime 'TMP/FIRST.DONE'
                            $until=[DateTime]::UtcNow.AddSeconds(10)
                            while(!(Test-Path $returned) -and !$owner.HasExited -and [DateTime]::UtcNow -lt $until) {
                                Start-Sleep -Milliseconds 100
                            }
                            if(!(Test-Path $returned) -or (Get-Content $returned -Raw).Trim() -ne 'MOUSE101-FIRST-RETURNED') {
                                throw 'First Windows/DOS completion was not observed'
                            }
                            $cycleRoot=Join-Path $root 'cycle-2'
                            New-Item -ItemType Directory -Path $cycleRoot | Out-Null
                            Copy-Item -LiteralPath "$root/setup-context.json" -Destination "$cycleRoot/setup-context.json"
                            # Observe the second startup frontier; captures do not
                            # substitute for the second completion assertions.
                            foreach($sample in 1..3) {
                                Start-Sleep -Seconds 3
                                & "$PSScriptRoot/win101_setup_visual_probe.ps1" -ObserverPid $owner.Id -OutputRoot $cycleRoot -FrontendPid $frontPin.Id -WorkerPid $worker.ProcessId -NoToggle
                            }
                        }
                        foreach($action in @(
                            @{name='corner';dx=1000;dy=1000;buttons=0},
                            @{name='file';dx=-629;dy=-325;buttons=0},
                            @{name='down';dx=0;dy=0;buttons=1},
                            @{name='up';dx=0;dy=0;buttons=0},
                            @{name='special';dx=115;dy=0;buttons=0},
                            @{name='special-down';dx=0;dy=0;buttons=1},
                            @{name='end-select';dx=0;dy=14;buttons=1},
                            @{name='end-release';dx=0;dy=0;buttons=0},
                            @{name='ok';dx=155;dy=173;buttons=0},
                            @{name='ok-down';dx=0;dy=0;buttons=1},
                            @{name='ok-up';dx=0;dy=0;buttons=0})) {
                            if($owner.HasExited -or $frontPin.HasExited){throw 'Image test ended before input'}
                            & "$mouse/probe.exe" $frontPin.Id "NTVDMConsoleTest-$($proc.Id)" "$mouse/mouse-hook.dll" $action.dx $action.dy $action.buttons "$cycleRoot/mouse-$($action.name).txt"
                            if($LASTEXITCODE){throw 'Frontend mouse sink did not acknowledge'}
                            Start-Sleep -Milliseconds 300
                            $capture=Join-Path $cycleRoot "mouse-$($action.name)"
                            New-Item -ItemType Directory -Path $capture|Out-Null
                            Copy-Item -LiteralPath "$root/setup-context.json" -Destination "$capture/setup-context.json"
                            if(!$owner.HasExited -and !$frontPin.HasExited) {
                                & "$PSScriptRoot/win101_setup_visual_probe.ps1" -ObserverPid $owner.Id -OutputRoot $capture -FrontendPid $frontPin.Id -WorkerPid $worker.ProcessId -NoToggle
                            }
                        }
                        }
                    } finally {$workerPin.Dispose();$frontPin.Dispose()}
                    # Acceptance waits for the receipt/output below. A late
                    # diagnostic memory read must not race a legitimately
                    # completed worker; keep it only in observation mode.
                    if(!$RequireNormalReturn -and !$owner.HasExited) {
                        & 'O:\Windows\_update\tools\guest-dump.exe' $worker.ProcessId $workerMap "$root/after-exit-memory.bin" |
                            Set-Content "$root/after-exit-cpu.txt"
                        if($LASTEXITCODE){throw 'Post-EndSession guest read failed'}
                        if($PaletteSnapshotRoot) {
                            if($TraceRoot -or (Get-FileHash "$($proof.runtime)/system32/ntvdm.exe").Hash -ne
                               'CA8C7AC27A406ABF612927F4C485D58E413D0A686462FA4EEA7130F8ACB6F212') {
                                throw 'Palette layout requires the exact unchanged T434 worker'
                            }
                            $reader=(Resolve-Path (Join-Path $repo $PaletteSnapshotRoot)).Path
                            if(!$reader.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
                                throw 'Build-owned palette reader required'
                            }
                            & "$reader/probe.exe" $worker.ProcessId "$($proof.runtime)\system32\ntvdm.exe" |
                                Set-Content "$root/palette-snapshot.txt"
                            if($LASTEXITCODE){throw 'Read-only palette snapshot failed'}
                        }
                    }
                }
                break
            }
            Start-Sleep -Milliseconds 100
        } while([DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
        if(!$proc.WaitForExit(60000)){
            if(!$proc.HasExited){$proc.Kill();$null=$proc.WaitForExit(5000)}
            throw 'Owned Windows observer did not finish'
        }
        if($proc.ExitCode -ne 0){throw "Observer exit $($proc.ExitCode)"}
    } finally {if($owner){$owner.Dispose()};$proc.Dispose()}
    Get-Content "$report.console.txt"
    if($RequireNormalReturn) {
        $result=Get-Content $report -Raw
        $output=Get-Content "$report.console.txt" -Raw
        if($result -notmatch '(?m)^result=exited\r?$' -or $result -notmatch '(?m)^exit=0x00000000\r?$') {
            throw 'Windows return did not complete successfully'
        }
        if($output -notmatch '(?s)MOUSE101-WINDOWS-RETURNED.*bytes total conventional memory.*MOUSE101-DOS-RETURN-COMPLETE') {
            throw 'Ordered Windows completion / actual DOS MEM / batch continuation evidence absent'
        }
        'PASS normal Windows completion, actual DOS execution and ordered batch continuation'
    } else {
        'Windows image observation only; inspect screenshots/report, no interaction pass claimed'
    }
} finally {
    if($TraceRoot -and (Test-Path "$($proof.runtime)/TMP/palette-trace.txt")) {
        Copy-Item -LiteralPath "$($proof.runtime)/TMP/palette-trace.txt" -Destination "$root/palette-trace.txt"
    }
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$oldPrivate;$env:TEMP=$oldTemp;$env:TMP=$oldTmp
    $env:WIN101_PALETTE_TRACE=$oldTrace
    try{if($scope){Stop-IsolatedPackageScope $scope}}finally{if($mapped){& subst.exe Z: /d}}
}
