param(
    [Parameter(Mandatory)][string]$PackageRoot,
    [Parameter(Mandatory)][string]$InstallRoot,
    [Parameter(Mandatory)][string]$Observer,
    [Parameter(Mandatory)][string]$OutputRoot,
    [string]$WindowsProbeImage,
    [string]$RetiredNativeCommandMarker,
    [string[]]$ServiceLossMarkers,
    [string]$WorkerCloseMonitor,
    [string]$RegistrySnapshotMonitor,
    [string]$RegistrySnapshotMarker,
    [switch]$ConsoleClose,
    [switch]$DirectRun16,
    [switch]$DirectShortPath,
    [switch]$DirectPif,
    [ValidateRange(10000,600000)][int]$TimeoutMs = 60000,
    [ValidateSet('std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1')]
    [string[]]$Cases = @('std-dosonly0','std-dosonly1','386-dosonly0','386-dosonly1'),
    [switch]$AllowUserInstallation
)
$ErrorActionPreference = 'Stop'
if([bool]$RegistrySnapshotMonitor -ne [bool]$RegistrySnapshotMarker){throw 'Registry snapshot requires both monitor and marker'}
if($ConsoleClose -and !$WorkerCloseMonitor){throw 'Console close requires management monitor'}
if($WorkerCloseMonitor -and !$ServiceLossMarkers){throw 'Worker close requires native readiness markers'}
if($DirectShortPath -and !$DirectRun16){throw 'Short PATH comparison requires direct Run16'}
if($DirectPif -and !$DirectRun16){throw 'PIF comparison requires direct Run16'}
if (@($Cases | Select-Object -Unique).Count -ne $Cases.Count) { throw 'Duplicate matrix cases' }
$repo = (Resolve-Path "$PSScriptRoot/../..").Path
$install = (Resolve-Path -LiteralPath $InstallRoot).Path.TrimEnd('\')
$package = (Resolve-Path -LiteralPath $PackageRoot).Path
$observer = (Resolve-Path -LiteralPath $Observer).Path
$output = [IO.Path]::GetFullPath($OutputRoot)
if (!$install.StartsWith('O:\tmp\',[StringComparison]::OrdinalIgnoreCase) -and
    !($AllowUserInstallation -and $install -in @('O:\w31','O:\w31t'))) {
    throw 'A dedicated Windows installation copy under O:\tmp is required'
}
if ($install -match '[^\x20-\x7e]' -or $install -match '\s') {
    throw 'The isolated guest path must be ASCII and contain no whitespace'
}
if (!$output.StartsWith((Join-Path $repo 'build')+'\',[StringComparison]::OrdinalIgnoreCase) -or (Test-Path $output)) {
    throw 'A fresh build-owned output directory is required'
}
if ($output -match '\s') { throw 'The observer output path must not contain whitespace' }
$patch = Join-Path $install 'PATCH'
$winIni = Join-Path $install 'WIN.INI'
$config = Join-Path $patch 'CONFIG.NT'
$autoexec = Join-Path $patch 'AUTOEXEC.NT'
$systemIni = Join-Path $install 'SYSTEM.INI'
$probe = Join-Path $patch 'W31CHECK.EXE'
$dosProbe = Join-Path $patch 'W31DOS.COM'
$dosResult = Join-Path $patch 'W31DOS.BIN'
$boot = Join-Path $patch 'W31BOOT.BIN'
$ready = Join-Path $patch 'W31READY.BIN'
foreach ($path in @($probe,$dosProbe,$dosResult,$boot,$ready)) {
    if (Test-Path -LiteralPath $path) { throw "Test input/output already exists: $path" }
}
foreach ($name in @('WINSTD.PIF','WIN386.PIF','WINSTD.CMD','WIN386.CMD')) {
    if (!(Test-Path -LiteralPath (Join-Path $patch $name) -PathType Leaf)) { throw "Missing $name" }
}
if ($Cases -contains '386-dosonly0' -or $Cases -contains '386-dosonly1') {
    $systemText = Get-Content -LiteralPath $systemIni -Raw
    if ($systemText -match '(?im)^\s*PermSwapDOSDrive\s*=' -or
        $systemText -match '(?im)^\s*32BitDiskAccess\s*=\s*(on|yes|true|1)\s*$') {
        throw 'Enhanced-mode test copy must not access a physical disk or permanent swap file'
    }
}
foreach ($case in @(@('WINSTD.PIF','/S'),@('WIN386.PIF','/3'))) {
    $pif = Join-Path $patch $case[0]
    $shown = & (Join-Path $repo 'assets/release/PIF.EXE') show $pif
    if ($LASTEXITCODE) { throw "Invalid test PIF: $pif" }
    $lines = @($shown)
    foreach ($required in @("PROGRAM=$install\WIN.COM", "DIRECTORY=$install",
            "ARGUMENTS=$($case[1])", "CONFIG=$patch\CONFIG.NT", "AUTOEXEC=$patch\AUTOEXEC.NT")) {
        if (-not ($lines | Where-Object { $_ -ieq $required })) { throw "PIF does not target isolated copy: $pif ($required)" }
    }
}
$nasm = (Get-Command nasm.exe -ErrorAction Stop).Source
New-Item -ItemType Directory -Path $output | Out-Null
# The private-desktop inspector only dismisses the identified, test-owned
# missing serial-port dialogs. Other errors remain visible to the observer.
. "$repo/tests/observation/inspect-session-windows.ps1" -ProcessIds @(0) -BuildRoot $output
if ($WindowsProbeImage) {
    # An extended witness must retain the boot/ready record contract below.
    Copy-Item -LiteralPath $WindowsProbeImage -Destination "$output/W31CHECK.EXE"
} else {
    & $nasm -f bin -o "$output/W31CHECK.EXE" "$repo/tests/observation/win31_startup_contract.asm"
    if ($LASTEXITCODE) { throw 'Failed to assemble Win16 startup witness' }
}
$probeBytes = [IO.File]::ReadAllBytes("$output/W31CHECK.EXE")
foreach ($field in @(@('@W31_BOOT_PATH@',$boot),@('@W31_READY_PATH@',$ready))) {
    $image = [Text.Encoding]::ASCII.GetString($probeBytes)
    $offset = $image.IndexOf($field[0],[StringComparison]::Ordinal)
    if ($offset -lt 0 -or $image.IndexOf($field[0],$offset+1,[StringComparison]::Ordinal) -ge 0) {
        throw 'Probe path placeholder must occur exactly once'
    }
    $bytes = [Text.Encoding]::ASCII.GetBytes($field[1])
    if ($bytes.Length -ge 128) { throw 'Probe output path exceeds fixed guest field' }
    [Array]::Clear($probeBytes,$offset,128)
    [Array]::Copy($bytes,0,$probeBytes,$offset,$bytes.Length)
}
[IO.File]::WriteAllBytes("$output/W31CHECK.EXE",$probeBytes)
& $nasm -f bin -o "$output/W31DOS.COM" "$repo/tests/observation/win31_dosonly_contract.asm"
if ($LASTEXITCODE) { throw 'Failed to assemble DOSONLY witness' }
$dosBytes = [IO.File]::ReadAllBytes("$output/W31DOS.COM")
$dosImage = [Text.Encoding]::ASCII.GetString($dosBytes)
$dosOffset = $dosImage.IndexOf('@W31_DOS_PATH@',[StringComparison]::Ordinal)
if ($dosOffset -lt 0 -or $dosImage.IndexOf('@W31_DOS_PATH@',$dosOffset+1,[StringComparison]::Ordinal) -ge 0) {
    throw 'DOS probe path placeholder must occur exactly once'
}
$dosPathBytes = [Text.Encoding]::ASCII.GetBytes($dosResult)
if ($dosPathBytes.Length -ge 128) { throw 'DOS result path exceeds fixed guest field' }
[Array]::Clear($dosBytes,$dosOffset,128)
[Array]::Copy($dosPathBytes,0,$dosBytes,$dosOffset,$dosPathBytes.Length)
[IO.File]::WriteAllBytes("$output/W31DOS.COM",$dosBytes)
. "$repo/tests/observation/isolated_package_cleanup.ps1"
$winBytes = [IO.File]::ReadAllBytes($winIni)
$configBytes = [IO.File]::ReadAllBytes($config)
$autoBytes = [IO.File]::ReadAllBytes($autoexec)
$encoding = [Text.Encoding]::GetEncoding(28591)
$ini = $encoding.GetString($winBytes)
if (@([regex]::Matches($ini,'(?im)^run=[^\r\n]*')).Count -ne 1) { throw 'Expected exactly one WIN.INI run entry' }
$baseConfig = $encoding.GetString($configBytes)
$baseConfig = [regex]::Replace($baseConfig,'(?im)^\s*dosonly\s*\r?\n','')
$results = [Collections.Generic.List[object]]::new()
$savedEnv = @{}
foreach ($name in @('PATH','TEMP','TMP','MVDM_OBSERVER_PRIVATE_DESKTOP','MVDM_OBSERVER_SHORT_HISTORY')) {
    $savedEnv[$name] = [Environment]::GetEnvironmentVariable($name,'Process')
}
try {
    Copy-Item -LiteralPath "$output/W31CHECK.EXE" -Destination $probe
    Copy-Item -LiteralPath "$output/W31DOS.COM" -Destination $dosProbe
    [IO.File]::WriteAllBytes($autoexec,$encoding.GetBytes($encoding.GetString($autoBytes)+"`r`n$dosProbe`r`n"))
    $ini = [regex]::Replace($ini,'(?im)^run=[^\r\n]*',"run=$probe")
    [IO.File]::WriteAllBytes($winIni,$encoding.GetBytes($ini))
    foreach ($case in @(
        @{Name='std-dosonly0'; Command='WINSTD.CMD'; Enhanced=$false; DosOnly=$false},
        @{Name='std-dosonly1'; Command='WINSTD.CMD'; Enhanced=$false; DosOnly=$true},
        @{Name='386-dosonly0'; Command='WIN386.CMD'; Enhanced=$true; DosOnly=$false},
        @{Name='386-dosonly1'; Command='WIN386.CMD'; Enhanced=$true; DosOnly=$true}
    ) | Where-Object {$_.Name -in $Cases}) {
        foreach ($path in @($boot,$ready,$dosResult)) { if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path} }
        $text = $baseConfig
        if ($case.DosOnly) { $text += "`r`ndosonly`r`n" }
        [IO.File]::WriteAllBytes($config,$encoding.GetBytes($text))
        $stage=$null; $scope=$null; $process=$null; $errorText=$null; $serviceLossPassed=$false; $registryCaptured=$false
        $report=Join-Path $output "$($case.Name).observer.txt"
        try {
            $stage=New-PhysicalRuntimeStage $package -FreshPackageIdentity
            $scope=New-IsolatedPackageScope $stage
            if($WorkerCloseMonitor -or $RegistrySnapshotMonitor) {
                $monitor=Join-Path $stage 'system32/monitor-rpc-test.exe'
                Copy-Item -LiteralPath $(if($WorkerCloseMonitor){$WorkerCloseMonitor}else{$RegistrySnapshotMonitor}) -Destination $monitor
                & "$repo/tools/build/Set-PackageIdentity.ps1" -Image $monitor -PackageId ([guid]::new([Convert]::FromHexString((Get-IsolatedPackageIdentity "$stage/system32/ntsrv.exe"))))
            }
            $pifName=if($case.Enhanced){'WIN386.PIF'}else{'WINSTD.PIF'}
            [ordered]@{case=$case.Name; package=$package; packageId=(Get-IsolatedPackageIdentity "$stage\system32\ntvdm.exe");
                ntvdmSha256=(Get-FileHash "$stage\system32\ntvdm.exe").Hash;
                pifSha256=(Get-FileHash (Join-Path $patch $pifName)).Hash;
                configSha256=(Get-FileHash $config).Hash; autoexecSha256=(Get-FileHash $autoexec).Hash;
                systemIniSha256=(Get-FileHash $systemIni).Hash; winIniSha256=(Get-FileHash $winIni).Hash;
                dosProbeSha256=(Get-FileHash $dosProbe).Hash; windowsProbeSha256=(Get-FileHash $probe).Hash} |
                ConvertTo-Json | Set-Content -LiteralPath (Join-Path $output "$($case.Name).inputs.json") -Encoding UTF8
            $env:PATH=if($DirectRun16 -and !$DirectShortPath){"$stage\system32;$($savedEnv.PATH)"}else{"$stage\system32;C:\Windows\System32;C:\Windows"}
            $env:TEMP='O:\tmp'; $env:TMP='O:\tmp'
            $env:MVDM_OBSERVER_PRIVATE_DESKTOP='1'; $env:MVDM_OBSERVER_SHORT_HISTORY='1'
            $observerArgs=if($DirectRun16){
                @("$stage\system32\run16.exe",$install,$report,
                  (Join-Path $patch $(if($DirectPif){$pifName}else{$case.Command})),'--observation-timeout-ms',"$TimeoutMs")
            }else{
                @('C:\Windows\System32\cmd.exe',$install,$report,
                  '/d','/c',(Join-Path $patch $case.Command),'--observation-timeout-ms',"$TimeoutMs")
            }
            # Console-close legitimately interrupts the outer test batch too.
            # Answer only its observed CMD confirmation, never inject into a VM.
            if($ConsoleClose){$observerArgs+=@('--observe-console-input-marker','(Y/N)?','--observe-console-input-escaped','y\r')}
            $process=Start-Process $observer -ArgumentList $observerArgs -WindowStyle Hidden -PassThru
            $deadline = [DateTime]::UtcNow.AddMilliseconds($TimeoutMs+15000)
            $known = @($process.Id)
            $desktop = 'NTVDMConsoleTest-' + $process.Id
            while (!$process.WaitForExit(500)) {
                if ([DateTime]::UtcNow -ge $deadline) { throw 'Observer did not complete its bounded observation' }
                if($RegistrySnapshotMarker -and !$registryCaptured -and (Test-Path -LiteralPath $RegistrySnapshotMarker)) {
                    $snapshot=Join-Path $output "$($case.Name).registry.json"
                    & $monitor --tree-json 2> "$snapshot.err" | Set-Content -LiteralPath $snapshot
                    if($LASTEXITCODE){throw 'Read-only registry snapshot failed'}
                    $registryCaptured=$true
                }
                if($ServiceLossMarkers -and !$serviceLossPassed -and
                    @($ServiceLossMarkers | Where-Object {!(Test-Path -LiteralPath $_)}).Count -eq 0) {
                    $pinned=[Collections.Generic.List[Diagnostics.Process]]::new()
                    try {
                        $owned=@(Get-CimInstance Win32_Process | Where-Object {
                            $_.ExecutablePath -and $_.ExecutablePath.StartsWith($stage+'\system32\',[StringComparison]::OrdinalIgnoreCase)
                        })
                        $service=@($owned | Where-Object Name -eq 'ntsrv.exe')
                        if($service.Count -ne 1 -or !($owned | Where-Object Name -eq 'ntvdm.exe') -or
                            !($owned | Where-Object Name -eq 'ntvwm.exe')){throw 'Missing isolated guest/native service chain'}
                        foreach($row in $owned){
                            $item=[Diagnostics.Process]::GetProcessById($row.ProcessId)
                            $null=$item.Handle;$pinned.Add($item)
                        }
                        foreach($marker in $ServiceLossMarkers){
                            $targetId=[int](Get-Content -LiteralPath $marker -Raw).Trim()
                            $item=[Diagnostics.Process]::GetProcessById($targetId)
                            $null=$item.Handle
                            if($item.MainModule.FileName -ine (Join-Path (Split-Path $marker) 'ORDER.EXE')){
                                $item.Dispose();throw 'Native readiness identity mismatch'
                            }
                            $pinned.Add($item)
                        }
                        $victim=$pinned | Where-Object Id -eq $service[0].ProcessId
                        if($WorkerCloseMonitor) {
                            $closeName=if($ConsoleClose){'ntcon.exe'}else{'ntvdm.exe'}
                            $workers=@($owned | Where-Object Name -eq $closeName)
                            if($workers.Count -ne 1){throw 'Expected one exact-stage guest worker'}
                            $closeArgs=if($ConsoleClose){@('--close-node','1',"$($workers[0].ProcessId)")}else{@('--terminate',"$($workers[0].ProcessId)")}
                            $close=Start-Process $monitor -ArgumentList $closeArgs -WindowStyle Hidden -PassThru -RedirectStandardOutput "$output/worker-close.stdout" -RedirectStandardError "$output/worker-close.stderr"
                            try {
                                if(!$close.WaitForExit(18000)){throw 'Worker-close RPC did not finish'}
                                if($close.ExitCode){throw 'Worker-close RPC rejected'}
                                if($victim.HasExited){throw 'Service died before management acknowledgement'}
                            } finally {
                                if(!$close.HasExited){$close.Kill();$close.WaitForExit()}
                                $close.Dispose()
                            }
                        } else {$victim.Kill()}
                        foreach($item in $pinned){
                            $remaining=[Math]::Max(0,($deadline-[DateTime]::UtcNow).TotalMilliseconds)
                            if(!$item.WaitForExit([int]$remaining)){throw "Lifecycle survivor: $($item.Id)"}
                        }
                        [ordered]@{beforeScopeCleanup=$true;service=$victim.Id;managedWorkerClose=([bool]$WorkerCloseMonitor -and !$ConsoleClose);consoleClose=[bool]$ConsoleClose;
                            exited=@($pinned | ForEach-Object {[ordered]@{pid=$_.Id;exit=$_.ExitCode}})} |
                            ConvertTo-Json -Depth 5 | Set-Content (Join-Path $output "$($case.Name).service-loss.json")
                        $serviceLossPassed=$true
                    } finally {
                        foreach($item in $pinned){
                            try {if(!$serviceLossPassed -and !$item.HasExited){$item.Kill();$item.WaitForExit()}}
                            finally {$item.Dispose()}
                        }
                    }
                }
                if (!$case.Enhanced) { continue }
                # Include the system CMD intermediary, then follow only the
                # observer's descendants; never act on unrelated processes.
                $rows = @(Get-CimInstance Win32_Process)
                for ($round=0; $round -lt 5; $round++) {
                    foreach ($row in $rows) {
                        if ($row.ParentProcessId -in $known -and $row.ProcessId -notin $known) { $known += $row.ProcessId }
                    }
                }
                try { $observation = [WindowInspector]::Read([int[]]$known,$desktop,$true,$false,$false) }
                catch [System.ComponentModel.Win32Exception] {
                    if ($_.Exception.NativeErrorCode -ne 2) { throw }
                    continue # The observer has not created its desktop yet.
                }
                if ($observation -match 'serial-ignore=True') {
                    [IO.File]::AppendAllText((Join-Path $output "$($case.Name).serial-dialogs.txt"),$observation)
                }
            }
            if((Get-Content -LiteralPath $report -Raw) -match '(?m)^result=timeout\r?$') {
                throw 'Runtime observer timed out'
            }
            if($RegistrySnapshotMarker -and !$registryCaptured){throw 'Registry readiness marker was not observed'}
            if($RetiredNativeCommandMarker) {
                # Observe before scope cleanup: cleanup must not manufacture
                # successful native-command retirement for the guest witness.
                $commands=@(Get-CimInstance Win32_Process -Filter "Name='cmd.exe'")
                $remaining=@($commands | Where-Object {
                    $_.CommandLine -and $_.CommandLine.IndexOf($RetiredNativeCommandMarker,[StringComparison]::OrdinalIgnoreCase) -ge 0
                })
                [ordered]@{marker=$RetiredNativeCommandMarker;beforeScopeCleanup=$true;
                    remaining=@($remaining | Select-Object ProcessId,ParentProcessId,CommandLine)} |
                    ConvertTo-Json -Depth 5 | Set-Content (Join-Path $output "$($case.Name).native-retirement.json")
                if($remaining.Count){throw 'Native command remains alive before scope cleanup'}
            }
        } catch {
            $errorText=$_.ToString()
            # Preserve the live service tree before cleanup destroys a failed
            # chain. A diagnostic failure must not replace the primary result.
            if($RegistrySnapshotMonitor -and $stage -and (Test-Path -LiteralPath $monitor)) {
                $snapshot=Join-Path $output "$($case.Name).failure-registry.json"
                try {
                    & $monitor --tree-json 2> "$snapshot.err" | Set-Content -LiteralPath $snapshot
                    "exit=$LASTEXITCODE" | Set-Content "$snapshot.exit"
                } catch { $_.ToString() | Set-Content "$snapshot.capture-error.txt" }
            }
        }
        finally {
            if($process){if(!$process.HasExited){$process.Kill();[void]$process.WaitForExit(5000)};$process.Dispose()}
            if($scope){Stop-IsolatedPackageScope $scope}
            if($stage){Remove-PhysicalRuntimeStage $stage}
        }
        $started=Test-Path -LiteralPath $boot
        $found=Test-Path -LiteralPath $ready
        $flags=$null; $hwnd=$null; $observedDosOnly=$null; $valid=$false
        if(Test-Path -LiteralPath $dosResult) {
            $dosData=[IO.File]::ReadAllBytes($dosResult)
            if($dosData.Length -eq 5 -and [Text.Encoding]::ASCII.GetString($dosData,0,4) -eq 'D31C') {
                $observedDosOnly=$dosData[4]
                Copy-Item -LiteralPath $dosResult -Destination (Join-Path $output "$($case.Name).dosonly.bin")
            }
        }
        if($found) {
            $data=[IO.File]::ReadAllBytes($ready)
            if($data.Length -eq 10 -and [Text.Encoding]::ASCII.GetString($data,0,4) -eq 'W31C') {
                $flags=[BitConverter]::ToUInt32($data,4);$hwnd=[BitConverter]::ToUInt16($data,8)
                $valid=$hwnd -ne 0 -and ((($flags -band 0x20) -ne 0) -eq $case.Enhanced) -and
                    ($observedDosOnly -eq [int]$case.DosOnly)
                Copy-Item -LiteralPath $ready -Destination (Join-Path $output "$($case.Name).ready.bin")
            }
        }
        $observation=if(Test-Path -LiteralPath $report){Get-Content -LiteralPath $report -Raw}else{''}
        $exited=$observation -match '(?m)^result=exited\r?$' -and $observation -match '(?m)^exit=0x00000000\r?$'
        $terminal=if($ServiceLossMarkers){$serviceLossPassed -and $observation -match '(?m)^result=exited\r?$'}else{$exited}
        $pass=$started -and $valid -and $terminal -and !$errorText
        $results.Add([pscustomobject]@{case=$case.Name;dosonly=$case.DosOnly;enhanced=$case.Enhanced;
            probeStarted=$started;progmanAndMode=$valid;flags=$flags;hwnd=$hwnd;observedDosOnly=$observedDosOnly;normalExit=$exited;serviceLoss=($serviceLossPassed -and !$WorkerCloseMonitor);workerClose=($serviceLossPassed -and [bool]$WorkerCloseMonitor -and !$ConsoleClose);consoleClose=($serviceLossPassed -and [bool]$ConsoleClose);
            passed=$pass;error=$errorText;observer=$report})
        Write-Host "$($case.Name): started=$started desktop-and-mode=$valid normal-exit=$exited pass=$pass"
    }
} finally {
    [IO.File]::WriteAllBytes($winIni,$winBytes)
    [IO.File]::WriteAllBytes($config,$configBytes)
    [IO.File]::WriteAllBytes($autoexec,$autoBytes)
    foreach ($path in @($probe,$dosProbe,$dosResult,$boot,$ready)) { if(Test-Path -LiteralPath $path){Remove-Item -LiteralPath $path} }
    foreach ($name in $savedEnv.Keys) { [Environment]::SetEnvironmentVariable($name,$savedEnv[$name],'Process') }
    $results | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $output 'matrix-results.json') -Encoding UTF8
}
if($results.Count -ne $Cases.Count -or @($results | Where-Object {!$_.passed}).Count) { throw 'Win31 runtime matrix did not pass all selected cases' }
"PASS $($Cases.Count) selected Windows 3.1 mode/DOSONLY combinations"
