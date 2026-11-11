param(
    [Parameter(Mandatory)][string]$InstallationRoot,
    [string]$Observer='build/M0-T425/S9/r033/console-startup-observer.exe',
    [switch]$CaptureWindow,
    [string]$SetupKeys='',
    [switch]$DirectoryStep,
    [switch]$HardwareStep,
    [switch]$MouseStep,
    [string]$ConsoleProbe='O:\Windows\_update\tools\console-install.exe'
)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=(Resolve-Path (Join-Path $repo $InstallationRoot)).Path
if (!$root.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase)) {
    throw 'Build-owned installation required'
}
$manifest=Get-Content "$root/inputs.json" -Raw | ConvertFrom-Json
if ($manifest.role -ne 'private-original-setup-installation-not-product') { throw 'Wrong test scope' }
if (Test-Path Z:\) { throw 'Z: occupied; leave untouched' }
$runtime=$manifest.runtime
$observerPath=(Resolve-Path (Join-Path $repo $Observer)).Path
. "$PSScriptRoot/isolated_package_cleanup.ps1"
$savedGuard=$env:WIN101_GUARD_ROOT
$savedPrivate=$env:MVDM_OBSERVER_PRIVATE_DESKTOP
$savedWindow=$env:MVDM_OBSERVER_WINDOW_INPUT
$savedTemp=$env:TEMP; $savedTmp=$env:TMP
$scope=$null; $mapped=$false
try {
    & subst.exe Z: $runtime
    if($LASTEXITCODE) { throw 'Z: mapping failed' }
    $mapped=$true
    $scope=New-IsolatedPackageScope $runtime 'Z:\'
    $env:WIN101_GUARD_ROOT=$root
    $temp=Join-Path $runtime 'TMP'
    New-Item -ItemType Directory -Path $temp -Force | Out-Null
    $env:TEMP='Z:\TMP'; $env:TMP='Z:\TMP'
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'
    Remove-Item Env:MVDM_OBSERVER_WINDOW_INPUT -ErrorAction SilentlyContinue
    $report=Join-Path $root 'setup-initial.txt'
    $proc=Start-Process -FilePath $observerPath -ArgumentList @(
        'Z:\system32\run16.exe','Z:\MEDIA',$report,'Z:\MEDIA\SETUP.EXE',
        '--observation-timeout-ms','60000') -WindowStyle Hidden -PassThru
    try {
        $deadline=[DateTime]::UtcNow.AddSeconds(8)
        $consoleRow=$null
        do {
            $consoleRow=Get-CimInstance Win32_Process -Filter "ParentProcessId=$($proc.Id)" |
                Where-Object {$_.ExecutablePath -eq $observerPath} | Select-Object -First 1
            if(!$consoleRow) { Start-Sleep -Milliseconds 100 }
        } while(!$consoleRow -and [DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
        if(!$consoleRow) { throw 'Private Console owner child not found' }
        $consoleOwner=[Diagnostics.Process]::GetProcessById($consoleRow.ProcessId)
        $null=$consoleOwner.Handle
        if($consoleOwner.HasExited -or $consoleOwner.MainModule.FileName -ne $observerPath) { throw 'Private owner identity changed' }
        [ordered]@{pid=$consoleOwner.Id;parentPid=$proc.Id;desktop="NTVDMConsoleTest-$($proc.Id)";
            image=$observerPath;startUtc=$consoleOwner.StartTime.ToUniversalTime().ToString('o')} |
            ConvertTo-Json | Set-Content "$root/setup-context.json"
        # Existing read-only guest-memory probe, no breakpoint or guest write.
        $deadline=[DateTime]::UtcNow.AddSeconds(8)
        $guest=$null
        do {
            $guest=Get-CimInstance Win32_Process -Filter "Name='ntvdm.exe'" |
                Where-Object {$_.ExecutablePath -in $scope.Paths} | Select-Object -First 1
            if(!$guest) { Start-Sleep -Milliseconds 100 }
        } while(!$guest -and [DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
        if($guest) {
            $map=if($manifest.guardMap){$manifest.guardMap}else{"$repo/build/M0-T435/S2/r012-guard/ntvdm.exe.map"}
            & 'O:\Windows\_update\tools\guest-dump.exe' $guest.ProcessId $map "$root/setup-memory.bin" |
                Set-Content "$root/setup-cpu.txt"
            if($LASTEXITCODE) { throw 'Read-only guest context probe failed' }
        }
        if($SetupKeys) {
            $probe=(Resolve-Path -LiteralPath $ConsoleProbe).Path
            $deadline=[DateTime]::UtcNow.AddSeconds(35)
            $before=Join-Path $root 'setup-before.txt'
            do {
                & $probe $consoleOwner.Id $before
                if($LASTEXITCODE) { throw 'Owned Console probe failed' }
                $page=Get-Content $before -Raw
                if($page -match 'Setup prepares Microsoft Windows') { break }
                Start-Sleep -Milliseconds 100
            } while([DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
            if($page -notmatch 'Setup prepares Microsoft Windows') { throw 'Setup welcome not observed; no input sent' }
            & $probe $consoleOwner.Id "$root/setup-after-key.txt" $SetupKeys
            if($LASTEXITCODE) { throw 'Owned Setup input probe failed' }
            if($DirectoryStep) {
                $page=Get-Content "$root/setup-after-key.txt" -Raw
                if($SetupKeys -ne 'C' -or $page -notmatch 'full pathname of the directory' -or
                    $page -notmatch 'C:\\WINDOWS' -or $manifest.expectedDestination -ne 'Z:\WIN101') {
                    throw 'Unexpected directory page; no path input sent'
                }
                $keys=([string][char]8)*10+'Z:\WIN101'+[char]13
                & $probe $consoleOwner.Id "$root/setup-after-directory.txt" $keys
                if($LASTEXITCODE) { throw 'Owned directory input failed' }
                if($HardwareStep) {
                    $page=Get-Content "$root/setup-after-directory.txt" -Raw
                    if($page -notmatch 'what kind of pointer device' -or $page -notmatch 'Continue\s+C') {
                        throw 'Hardware introduction not observed; no input sent'
                    }
                    & $probe $consoleOwner.Id "$root/setup-hardware.txt" C
                    if($LASTEXITCODE) { throw 'Owned hardware-menu input failed' }
                    if($MouseStep) {
                        $deadline=[DateTime]::UtcNow.AddSeconds(15)
                        do {
                            & $probe $consoleOwner.Id "$root/setup-mouse-before.txt"
                            if($LASTEXITCODE) { throw 'Mouse-menu read failed' }
                            $page=Get-Content "$root/setup-mouse-before.txt" -Raw
                            if($page -match '2: Microsoft Mouse' -and $page -match 'pointing device:\s*1') { break }
                            Start-Sleep -Milliseconds 100
                        } while([DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
                        if($page -notmatch '2: Microsoft Mouse' -or $page -notmatch 'pointing device:\s*1') {
                            throw 'Mouse selection node missing; no selection sent'
                        }
                        & $probe $consoleOwner.Id "$root/setup-after-mouse.txt" (([string][char]8)+'2'+[char]13)
                        if($LASTEXITCODE) { throw 'Mouse selection input failed' }
                    }
                }
            }
        }
        if($CaptureWindow) {
            $deadline=[DateTime]::UtcNow.AddSeconds(8)
            $front=$null; $worker=$null
            do {
                $rows=@(Get-CimInstance Win32_Process -Filter "Name='ntcon.exe' OR Name='ntvdm.exe'" |
                    Where-Object {$_.ExecutablePath -in $scope.Paths})
                $front=$rows | Where-Object Name -eq 'ntcon.exe' | Select-Object -First 1
                $worker=$rows | Where-Object Name -eq 'ntvdm.exe' | Select-Object -First 1
                if(!$front -or !$worker) { Start-Sleep -Milliseconds 100 }
            } while((!$front -or !$worker) -and [DateTime]::UtcNow -lt $deadline -and !$proc.HasExited)
            if(!$front -or !$worker) { throw 'Own frontend/worker not established' }
            & "$PSScriptRoot/win101_setup_visual_probe.ps1" -ObserverPid $consoleOwner.Id -OutputRoot $root -FrontendPid $front.ProcessId -WorkerPid $worker.ProcessId
        }
        if(!$proc.WaitForExit(60000)) {
            $proc.Kill(); $null=$proc.WaitForExit(5000)
            throw 'Owned setup observer did not finish'
        }
        if($proc.ExitCode -ne 0) { throw "Observer failed $($proc.ExitCode)" }
    } finally { if($consoleOwner){$consoleOwner.Dispose()}; $proc.Dispose() }
    if(!(Test-Path "$root/install-writes.txt")) { throw 'Guard evidence missing' }
    if(!(Test-Path $report) -or !(Test-Path "$report.console.txt")) { throw 'Setup report missing' }
    Get-Content "$report.console.txt"
    $audit=Get-Content "$root/install-writes.txt" -Raw -Encoding Unicode
    if($audit -match '(?m)^DENY ') { throw 'Guard refused out-of-scope mutation; inspect audit, no installation pass' }
    'Initial Setup observation only; no input or installation-success claim'
} finally {
    $env:WIN101_GUARD_ROOT=$savedGuard
    $env:MVDM_OBSERVER_PRIVATE_DESKTOP=$savedPrivate
    $env:MVDM_OBSERVER_WINDOW_INPUT=$savedWindow
    $env:TEMP=$savedTemp; $env:TMP=$savedTmp
    try { if($scope) { Stop-IsolatedPackageScope $scope } }
    finally { if($mapped) { & subst.exe Z: /d } }
}
