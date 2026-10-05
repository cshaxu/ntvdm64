[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [string]$PackageRoot = 'O:\winnt',
    [string]$ProcessPackageRoot,
    [string]$LogRoot = 'O:\winnt\logs',
    [string]$LogPrefix = 'm0-t412-s8-exit',
    [string[]]$Cases,
    [string]$GuestFixturePath,
    [string]$VideoGuestFixturePath,
    [string]$FrontendObserver,
    [string]$NativeSurvivorFixture,
    # Explicit caller-resolved native path, e.g. Sysnative for the x86 launcher.
    # Does not change any expected output, receipt, order or exit assertion.
    [string]$NativeApplication='cmd.exe',
    [ValidateRange(1000,60000)][int]$ObservationTimeoutMs = 20000,
    [switch]$OrdinaryFrontend
)
$ErrorActionPreference = 'Stop'
if ($env:MVDM_OBSERVER_WINDOW_INPUT -and $env:MVDM_OBSERVER_PRIVATE_DESKTOP -ne '1') {
    throw 'Window input tests require MVDM_OBSERVER_PRIVATE_DESKTOP=1; no product process was started.'
}
. (Join-Path $PSScriptRoot 'Merge-ConsoleTextSnapshots.ps1')
. (Join-Path $PSScriptRoot '../../tests/observation/typeahead_witness.ps1')
. (Join-Path $PSScriptRoot '../../tests/observation/isolated_package_cleanup.ps1')
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
$LogRoot = (Resolve-Path -LiteralPath $LogRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$runtimeFixtureRoot = Join-Path $PackageRoot 'tests'
if (!(Test-Path -LiteralPath $runtimeFixtureRoot)) {
    New-Item -ItemType Directory -Path $runtimeFixtureRoot -Force | Out-Null
}
$runtimeFixtureRoot = (Resolve-Path -LiteralPath $runtimeFixtureRoot).Path
if($Cases -contains 'native-surviving-client') {
    if(!$NativeSurvivorFixture){throw 'Surviving-client case requires its authored native fixture'}
    $survivor=(Resolve-Path -LiteralPath $NativeSurvivorFixture).Path
    $buildRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../build'))+'\'
    if(!$survivor.StartsWith($buildRoot,[StringComparison]::OrdinalIgnoreCase)){throw 'Native fixture must be under build'}
    Copy-Item -LiteralPath $survivor -Destination (Join-Path $runtimeFixtureRoot 'SURVIVE.EXE') -Force
}
if($Cases -contains 'graphics-return' -or $Cases -contains 'direct-graphics-return') {
    if(!$VideoGuestFixturePath){throw 'Graphics return requires an authored video fixture'}
    $video=(Resolve-Path -LiteralPath $VideoGuestFixturePath).Path
    $videoBuild=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../build'))+'\'
    if(!$video.StartsWith($videoBuild,[StringComparison]::OrdinalIgnoreCase)){throw 'Video fixture must be under repository build'}
    $manifest=Get-Content -LiteralPath (Join-Path (Split-Path $video) 'manifest.json') -Raw | ConvertFrom-Json
    if($manifest.route -ne 'graphics-vram' -or (Get-FileHash -LiteralPath $video).Hash -ne $manifest.sha256){throw 'Wrong video probe or hash'}
    if($env:MVDM_OBSERVER_GRAPHICS_RETURN -eq '1' -and
        (!$manifest.windowHandshake -or $Cases.Count -ne 1 -or $Cases[0] -ne 'direct-graphics-return')){
        throw 'Graphics Window handshake requires its held probe and only direct-graphics-return'
    }
    Copy-Item -LiteralPath $video -Destination (Join-Path $runtimeFixtureRoot 'VTGRAPH.COM') -Force
}
$generatedFixtures = @(
    (Join-Path $runtimeFixtureRoot 'G7.COM'),
    (Join-Path $runtimeFixtureRoot 'STREAM.CMD'),
    (Join-Path $runtimeFixtureRoot 'EOF.CMD'),
    (Join-Path $runtimeFixtureRoot 'D7.CMD')
)
$productNames = @('run16.exe','ntvdm.exe','ntsrv.exe','ntcon.exe')
$binaryRoot=Get-PackageBinaryRoot $PackageRoot
# Retain compatibility with sealed pre-NTVWM evidence packages.
foreach($workerName in @('ntvwm.exe','ntvwm32.exe','ntvwm64.exe')){
    if(Test-Path -LiteralPath (Join-Path $binaryRoot $workerName)){$productNames+=$workerName}
}
$productPaths = $productNames | ForEach-Object { Join-Path $binaryRoot $_ }
if($Cases -contains 'native-surviving-client'){
    $productPaths+=Join-Path $runtimeFixtureRoot 'SURVIVE.EXE'
}
if($ProcessPackageRoot){
    $ProcessPackageRoot=(Resolve-Path -LiteralPath $ProcessPackageRoot).Path
    $physicalBinary=Get-PackageBinaryRoot $ProcessPackageRoot
    foreach($name in $productNames){
        $physical=Join-Path $physicalBinary $name
        if((Get-FileHash $physical).Hash -ne (Get-FileHash (Join-Path $binaryRoot $name)).Hash){
            throw "Process package differs from launch package: $name"
        }
        $productPaths+= $physical
    }
}
function Get-PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='ntsrv.exe' OR Name='ntcon.exe' OR Name='ntvwm.exe' OR Name='ntvwm32.exe' OR Name='ntvwm64.exe' OR Name='SURVIVE.EXE'" |
        Where-Object { $_.ExecutablePath -in $productPaths })
}
function Test-ExactFileBytes {
    param([Parameter(Mandatory)][string]$Left, [Parameter(Mandatory)][string]$Right)
    if (!(Test-Path -LiteralPath $Left) -or !(Test-Path -LiteralPath $Right)) { return $false }
    $leftBytes = [IO.File]::ReadAllBytes($Left)
    $rightBytes = [IO.File]::ReadAllBytes($Right)
    if ($leftBytes.Length -ne $rightBytes.Length) { return $false }
    for ($index = 0; $index -lt $leftBytes.Length; ++$index) {
        if ($leftBytes[$index] -ne $rightBytes[$index]) { return $false }
    }
    return $true
}
$foreignBroker=@(Get-CimInstance Win32_Process -Filter "Name='ntsrv.exe'" |
    Where-Object {$_.ExecutablePath -notin $productPaths})
if($foreignBroker.Count){
    throw 'A different package owns the system BaseSrv; stop it before this isolated test. No process was changed.'
}
if ((Get-PackageProcesses).Count) { throw 'Package already in use; no existing process will be stopped.' }
if ((!$Cases -or 'guest-seven' -in $Cases) -and !$GuestFixturePath) {
    throw 'Guest cases require -GuestFixturePath with a verified build-root fixture.'
}
# Runtime observers belong under PackageRoot/tests; generated guest input
# remains in its separately supplied build root, not beside the observer.
$fixtureRoot=if ($GuestFixturePath) {
    Split-Path -Parent ([IO.Path]::GetFullPath($GuestFixturePath))
} else { Split-Path -Parent $Observer }
if ($GuestFixturePath) {
    # Test-only DOS program: write a guest-owned textual witness, then
    # MOV AX,4C07h; INT 21h.  The outer launcher exit alone is not proof that
    # DOS opened and executed this COM image.  Never replaces package media.
    $guest=Join-Path $fixtureRoot 'G7.COM'
    if ($guest -notmatch '\\build\\M[0-9]+-T[0-9]+\\S[0-9]+\\') { throw 'Guest fixture must stay in an admitted task S build root' }
    [IO.File]::WriteAllBytes($guest,[byte[]](
        0xba,0x0c,0x01,             # MOV DX,010Ch (COM message)
        0xb4,0x09,0xcd,0x21,         # MOV AH,09h; INT 21h
        0xb8,0x07,0x4c,0xcd,0x21,    # MOV AX,4C07h; INT 21h
        0x53,0x31,0x30,0x5f,0x47,0x55,0x45,0x53,0x54,0x5f,0x53,0x45,0x56,0x45,0x4e,0x24
    ))
    if (!(Test-ExactFileBytes -Left $GuestFixturePath -Right $guest)) {
        throw 'Short-path fixture does not match the build artifact'
    }
    # Guest fixture provenance remains the admitted build-root input.  A
    # short, package-local test path is required by DOS COMMAND, but mapping a
    # drive with SUBST leaks an OS-global device mapping into the spawned
    # worker and is not reliably visible across its startup boundary.  Copy
    # only this verified test fixture under the declared runtime test root.
    $runtimeGuest = Join-Path $runtimeFixtureRoot 'G7.COM'
    [IO.File]::Copy($guest, $runtimeGuest, $true)
    if (!(Test-ExactFileBytes -Left $guest -Right $runtimeGuest)) {
        throw 'Runtime test fixture does not match the admitted build artifact'
    }
    $guest = $runtimeGuest
}
[IO.File]::WriteAllText((Join-Path $runtimeFixtureRoot 'STREAM.CMD'),"@echo off`r`necho S10_STDOUT`r`necho S10_STDERR 1>&2`r`n",[Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $runtimeFixtureRoot 'EOF.CMD'),"@echo off`r`necho S10_EOF`r`nmore <nul`r`nexit /b 37`r`n",[Text.Encoding]::ASCII)
[IO.File]::WriteAllText((Join-Path $runtimeFixtureRoot 'D7.CMD'),"@echo off`r`necho S10_DIRECT_SEVEN`r`nexit /b 7`r`n",[Text.Encoding]::ASCII)
$shortFixtureRoot=$runtimeFixtureRoot
$matrix = @(
    # Interactive COMMAND delegates these native built-ins through the
    # original BOP 54:08 path, hence the modern cmd.exe banner is the actual
    # Console witness.  Direct COMMAND /c remains a DOS COMMAND witness.
    @{ Name='empty'; Text="exit`r"; Code=0; ConsoleMarkers=@('Microsoft(R) Windows NT DOS'); ConsoleMarkerCount=1 },
    @{ Name='native-zero'; Text="ver`rexit`r"; Code=0; ConsoleMarkers=@('Microsoft Windows [Version') },
    @{ Name='native-interactive-return'; Supplemental=$true; Text="cmd.exe /d`recho NATIVE-INTERACTIVE-OK`rexit`rmem`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('Microsoft Windows [Version','bytes total conventional memory'); ExactConsoleLines=@('NATIVE-INTERACTIVE-OK') },
    @{ Name='native-surviving-client'; Supplemental=$true; RootFrontend=$true; Args=@((Join-Path $runtimeFixtureRoot 'SURVIVE.EXE')); Text="survivor`r"; LineDelayMs=1000; Code=37; ConsoleMarkers=@('NATIVE-PARENT-EXIT-37','NATIVE-SURVIVOR-INPUT-OK') },
    @{ Name='missing'; Text="missing`rver`rexit`r"; Code=0; ConsoleMarkers=@('is not recognized as an internal or external command','Microsoft Windows [Version'); ExpectedGuestError=$true },
    @{ Name='native-seven'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'D7.CMD')); Code=0; ConsoleMarkers=@('S10_DIRECT_SEVEN') },
    @{ Name='native-streams'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'STREAM.CMD')); Code=0; ConsoleMarkers=@('S10_STDOUT','S10_STDERR') },
    @{ Name='native-eof'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'EOF.CMD')); Code=0; ConsoleMarkers=@('S10_EOF') },
    @{ Name='mem'; Text="mem`rexit`r"; Code=1; ConsoleMarkers=@('bytes total conventional memory') },
    # A nested COMMAND consumes the preceding exit and recreates its input
    # loop.  Pace complete lines so the next key sequence is not offered while
    # the original keyboard queue is between those two owners.
    @{ Name='nested-empty'; Text="command`rexit`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('Microsoft(R) Windows NT DOS'); ConsoleMarkerCount=2 },
    @{ Name='nested-mem'; Text="command`rcommand`rmem`rexit`rmem`rexit`rmem`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('bytes total conventional memory'); SequentialMemLines=@(3,5,7) },
    @{ Name='nested-mem-typeahead'; Supplemental=$true; Typeahead=$true; Text="command`rcommand`rmem`rexit`rmem`rexit`rmem`rexit`r"; LineDelayMs=0; Code=1; ConsoleMarkers=@('bytes total conventional memory'); WitnessCommands=@('mem','mem','mem'); WitnessLines=@(3,5,7) },
    @{ Name='interactive-native-dos-return'; Supplemental=$true; Text="cmd`rrun16 command`rmem`rexit`recho window-native-return`rexit`rmem`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('bytes total conventional memory'); ConsoleMarkerCount=2; ExactConsoleLines=@('window-native-return') },
    @{ Name='mem-repeat'; Text="mem`rmem`rexit`r"; Code=1; ConsoleMarkers=@('bytes total conventional memory'); SequentialMemLines=@(1,2) },
    @{ Name='direct-mem'; Args=@('MEM.EXE'); Code=0; ConsoleMarkers=@('bytes total conventional memory') },
    @{ Name='command-c'; Args=@('COMMAND.COM','/c','ver'); Code=0; ConsoleMarkers=@('MS-DOS Version') },
    # Original COMMAND::LodCom1 -> FatalRet2 uses AX=4C00, not RetCode.
    @{ Name='command-c-seven'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'D7.CMD')); Code=0; ConsoleMarkers=@('S10_DIRECT_SEVEN') },
    @{ Name='guest-seven'; Args=@($guest); Code=7; ConsoleMarkers=@('S10_GUEST_SEVEN') },
    # Verify a nested DOS COMMAND route with package-owned media.  A temporary
    # build-root drive is intentionally not a guest-visible DOS drive, so it
    # cannot be used as a meaningful COMMAND /c image contract.
    @{ Name='command-c-mem'; Args=@('COMMAND.COM','/c','MEM.EXE'); Code=0; ConsoleMarkers=@('bytes total conventional memory') },
    @{ Name='direct-seven'; Args=@('cmd.exe','/c',(Join-Path $shortFixtureRoot 'D7.CMD')); Code=7; ConsoleMarkers=@('S10_DIRECT_SEVEN') },
    @{ Name='edit'; Edit=$true; Code=1; ConsoleMarkers=@('bytes total conventional memory') }
    @{ Name='native-cmd-dos'; Supplemental=$true; Args=@('cmd.exe','/d','/c',('"'+(Join-Path $binaryRoot 'run16.exe')+' MEM.EXE"')); Code=0; ConsoleMarkers=@('bytes total conventional memory') },
    @{ Name='native-root-frontend'; Supplemental=$true; RootFrontend=$true; Args=@('cmd.exe','/d','/c',('"'+(Join-Path $binaryRoot 'run16.exe')+' MEM.EXE"')); Code=0; ConsoleMarkers=@('bytes total conventional memory') },
    @{ Name='direct-graphics-return'; Supplemental=$true; Args=@((Join-Path $runtimeFixtureRoot 'VTGRAPH.COM')); Code=0; ConsoleMarkers=@('S23_GRAPHICS_VRAM_OK') },
    @{ Name='graphics-return'; Supplemental=$true; Text=((Join-Path $runtimeFixtureRoot 'VTGRAPH.COM')+"`rmem`rexit`r"); LineDelayMs=1000; Code=1; ConsoleMarkers=@('S23_GRAPHICS_VRAM_OK','bytes total conventional memory') },
    # Keep one root frontend alive across two sequential DOS submissions.
    @{ Name='native-cmd-dos-repeat'; Supplemental=$true; Args=@('cmd.exe','/d','/c',('""'+(Join-Path $binaryRoot 'run16.exe')+'" MEM.EXE & "'+(Join-Path $binaryRoot 'run16.exe')+'" MEM.EXE"')); Code=0; ConsoleMarkers=@('bytes total conventional memory'); ConsoleMarkerCount=2 },
    @{ Name='dos-native-dos'; Supplemental=$true; Text="cmd.exe /d`rrun16 mem`rexit`rmem`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('bytes total conventional memory','Microsoft Windows [Version'); ConsoleMarkerCount=2 },
    # No line-level wait for an owner change: keep typing through DOS/native
    # handoff. The observer still emits ordinary paired key records.
    @{ Name='dos-native-typeahead'; Supplemental=$true; Typeahead=$true; Text="cmd.exe /d`rrun16 mem`rexit`rmem`rexit`r"; LineDelayMs=0; Code=1; ConsoleMarkers=@('bytes total conventional memory','Microsoft Windows [Version'); WitnessCommands=@('run16 mem','mem'); WitnessLines=@(2,4) },
    @{ Name='frontend-chain-a'; Supplemental=$true; Text="cmd.exe /d`rrun16 command.com`rcmd.exe /d`recho S3-A-NATIVE-INNER`rexit /b 37`rmem`rexit`recho S3-A-PARENT-RETURN-%errorlevel%`rexit /b 23`rmem`rexit`r"; LineDelayMs=1000; TimeoutMs=45000; Code=1; ConsoleMarkers=@('bytes total conventional memory','Microsoft Windows [Version'); ConsoleMarkerCount=2; ExactConsoleLines=@('S3-A-NATIVE-INNER','S3-A-PARENT-RETURN-1'); NativeExitCodes=@(37,23) },
    @{ Name='frontend-chain-b'; Supplemental=$true; Args=@('cmd.exe','/d'); Text="run16 command.com`rcmd.exe /d`rrun16 command.com`rmem`rexit`recho S3-B-INNER-RETURN-%errorlevel%`rexit /b 37`rmem`rexit`recho S3-B-ROOT-RETURN-%errorlevel%`rexit /b 23`r"; LineDelayMs=1000; TimeoutMs=45000; Code=23; ConsoleMarkers=@('bytes total conventional memory','Microsoft Windows [Version'); ConsoleMarkerCount=2; ExactConsoleLines=@('S3-B-INNER-RETURN-1','S3-B-ROOT-RETURN-1'); NativeExitCodes=@(37,23) },
    @{ Name='worker-version-rejection'; Args=@('MEM.EXE'); Code=1306; Negative=$true }
)
foreach ($case in $matrix) {
    if($case.Args -and $case.Args[0] -eq 'cmd.exe'){$case.Args[0]=$NativeApplication}
    if ($case.Name -in @('native-cmd-dos-repeat','dos-native-dos','dos-native-typeahead','frontend-chain-a','frontend-chain-b')) {
        $case.RootFrontend=$true
    }
}
foreach ($selected in $Cases) {
    if ($selected -notin $matrix.Name) { throw "Unknown case: $selected" }
}
if($OrdinaryFrontend -and $FrontendObserver){throw 'Select ordinary or instrumented frontend, not both'}
if(!$OrdinaryFrontend -and @($matrix | Where-Object {$_.RootFrontend -and $_.Name -in $Cases}).Count){
    if(!$FrontendObserver -or
       (Get-FileHash -LiteralPath $FrontendObserver).Hash -ne
       (Get-FileHash -LiteralPath (Join-Path $binaryRoot 'ntcon.exe')).Hash){
        throw 'Owner cases require the test-only frontend observer in the isolated test package'
    }
}
$environmentNames = @('MVDM_BASESRV_TRACE_PATH','MVDM_S34_TRACE_PATH','MVDM_TEST_FRAME_REPORT','PATH')
$previous = @{}
foreach ($name in $environmentNames) { $previous[$name]=[Environment]::GetEnvironmentVariable($name) }
$results = @()
try {
    # Original guest utilities live in system32. This explicit test PATH is
    # ordinary user search input, not a product-owned package fallback.
    [Environment]::SetEnvironmentVariable('PATH',
        (Join-Path $PackageRoot 'system32')+';'+$previous['PATH'])
    foreach ($case in $matrix) {
        if (($case.Negative -or $case.Supplemental) -and !$Cases) { continue }
        if ($Cases -and $case.Name -notin $Cases) { continue }
        $report=Join-Path $LogRoot "$LogPrefix-$($case.Name).txt"
        if (Test-Path -LiteralPath $report) { throw "Use a fresh log prefix: $report exists" }
        [Environment]::SetEnvironmentVariable($environmentNames[0],"$report.broker.log")
        if($OrdinaryFrontend){[Environment]::SetEnvironmentVariable('MVDM_TEST_FRAME_REPORT',$null)}
        elseif($case.RootFrontend){[Environment]::SetEnvironmentVariable('MVDM_TEST_FRAME_REPORT',"$report.frontend.log")}
        else{[Environment]::SetEnvironmentVariable('MVDM_TEST_FRAME_REPORT',$previous['MVDM_TEST_FRAME_REPORT'])}
        $arguments=@((Join-Path $binaryRoot 'run16.exe'),$PackageRoot,$report)
        if ($case.Args) { $arguments += $case.Args } else { $arguments += 'COMMAND.COM' }
        if ($case.Text) {
            $arguments += @('--observe-console-input-text',('"'+$case.Text+'"'))
            if ($case.ContainsKey('LineDelayMs')) {
                $arguments += @('--observe-console-line-delay-ms',$case.LineDelayMs)
            }
        }
        if ($case.Edit) { $arguments += '--observe-console-edit-return' }
        # A case can require a larger minimum, but must not silently shorten
        # the caller's explicit observation budget (Window typing takes longer).
        $caseTimeout=if($case.TimeoutMs){[Math]::Max($case.TimeoutMs,$ObservationTimeoutMs)}else{$ObservationTimeoutMs}
        $arguments += @('--observation-timeout-ms',$caseTimeout)
        $launcherId = 0
        $reportedChildren = @()
        $observedDescendants = [Collections.Generic.HashSet[int]]::new()
        $nativeWaiters=@{}
        $frontendWaiters=@{}
        $caseWatch=[Diagnostics.Stopwatch]::StartNew()
        $cleanupMs=0
        $observation = Start-Process -FilePath $Observer -ArgumentList $arguments -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
        try {
            if($case.Supplemental){
                [void]$observedDescendants.Add($observation.Id)
                $treeDeadline=[DateTime]::UtcNow.AddSeconds(50)
                do {
                    # Include the native CMD relay while it is alive; after
                    # exit, a product-only snapshot cannot reconstruct it.
                    $tree=@(Get-CimInstance Win32_Process | Select-Object ProcessId,ParentProcessId,Name,ExecutablePath,CommandLine)
                    for($depth=0;$depth -lt 8;++$depth){
                        $added=$false
                        foreach($node in $tree){
                            if($observedDescendants.Contains([int]$node.ParentProcessId)){
                                if($observedDescendants.Add([int]$node.ProcessId)){$added=$true}
                            }
                        }
                        if(!$added){break}
                    }
                    if($case.NativeExitCodes){
                        foreach($node in $tree){
                            $nativeId=[int]$node.ProcessId
                            if($node.Name -eq 'cmd.exe' -and $observedDescendants.Contains($nativeId) -and !$nativeWaiters.ContainsKey($nativeId)){
                                $probe=$null
                                try {
                                    $probe=Get-Process -Id $nativeId -ErrorAction Stop
                                    if($probe.ProcessName -ne 'cmd'){throw 'Process identity changed'}
                                    # Pin the actual process object while it is alive;
                                    # its exit code remains observable after PID reuse.
                                    $null=$probe.Handle
                                    $nativeWaiters[$nativeId]=$probe
                                } catch {if($probe){$probe.Dispose()}}
                            }
                        }
                    }
                    if($OrdinaryFrontend -and $case.RootFrontend){
                        foreach($node in $tree){
                            $frontendId=[int]$node.ProcessId
                            if($node.ExecutablePath -eq (Join-Path $binaryRoot 'ntcon.exe') -and
                                $node.CommandLine -match '--session\s' -and
                                $observedDescendants.Contains($frontendId) -and !$frontendWaiters.ContainsKey($frontendId)){
                                $probe=$null
                                try {
                                    $probe=Get-Process -Id $frontendId -ErrorAction Stop
                                    $null=$probe.Handle
                                    if($probe.Path -ne $node.ExecutablePath){throw 'Frontend identity changed'}
                                    $frontendWaiters[$frontendId]=$probe
                                } catch {if($probe){$probe.Dispose()}}
                            }
                        }
                    }
                    if($observation.WaitForExit(100)){break}
                } while([DateTime]::UtcNow -lt $treeDeadline)
            }
            if (!$observation.WaitForExit([Math]::Max(55000,$caseTimeout+15000))) { throw "Observer timeout: $($case.Name)" }
            $record=Get-Content -LiteralPath $report -Raw
            $launcherId=[int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
            if (!$launcherId) { throw 'Missing test launcher identity' }
            # Once run16 has exited, Windows can reparent its broker/worker
            # before the finally block queries them.  The observer recorded
            # only direct children while the launcher still existed; retain
            # those exact-path identities for isolated cleanup below.
            $reportedChildren=@([regex]::Matches($record,
                '(?m)^direct-child pid=(\d+) .* path=(.+)\r?$') |
                ForEach-Object {
                    $path=$_.Groups[2].Value.Trim()
                    if ($path -in $productPaths) { [int]$_.Groups[1].Value }
                })
            $actual=[Convert]::ToUInt32([regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)').Groups[1].Value,16)
            if ($record -notmatch '(?m)^result=exited' -or $actual -ne $case.Code) {
                throw "Unexpected $($case.Name) result: $actual; expected $($case.Code)"
            }
            if (($case.Text -or $case.Edit) -and $record -notmatch '(?m)^scripted-console-input=delivered') {
                throw "Input not delivered: $($case.Name)"
            }
            if($env:MVDM_OBSERVER_GRAPHICS_RETURN -eq '1' -and
                $record -notmatch '(?m)^graphics-handshake=pass'){
                throw "Graphics Window/text handoff failed: $($case.Name)"
            }
            $consolePath="$report.console.txt"
            if (!(Test-Path -LiteralPath $consolePath)) {
                throw "Missing captured guest Console text: $($case.Name)"
            }
            $screen=Get-Content -LiteralPath $consolePath -Raw
            if($case.Text -and !$case.Typeahead -and !$case.SequentialMemLines -and ($env:MVDM_OBSERVER_WINDOW_INPUT -eq '1' -or
                $case.Name -in @('dos-native-dos','dos-native-typeahead',
                    'interactive-native-dos-return'))) {
                # Window text is finite, and a Console/native/DOS transition
                # can also shrink the final buffer after a proven CMD banner.
                # Require contiguous overlap; never count the same MEM result
                # once per captured frame or accept an unrelated repaint.
                $snapshots=@(Get-ChildItem -LiteralPath (Split-Path $report) -Filter ((Split-Path $report -Leaf)+'.line-*.console.txt') |
                    Sort-Object Name | ForEach-Object { Get-Content -LiteralPath $_.FullName -Raw })
                if(!$snapshots.Count){throw "Missing Window command snapshots: $($case.Name)"}
                $screen=Merge-ConsoleTextSnapshots ($snapshots+@($screen))
                $screen | Set-Content -LiteralPath "$report.transcript.txt" -Encoding UTF8
            }
            if($case.Typeahead){
                if($record -notmatch 'milestone-waits=0'){
                    throw 'Typeahead must retain continuous input with no consumption waits'
                }
                $before=Get-Content -LiteralPath "$report.pre-input-console.txt.console.txt" -Raw
                $lineCount=($case.Text.ToCharArray() | Where-Object {$_ -eq "`r"}).Count
                $observations=@(foreach($number in 1..$lineCount){
                    $path='{0}.line-{1:d2}.console.txt' -f $report,$number
                    [pscustomobject]@{Line=$number;Text=(Get-Content -LiteralPath $path -Raw)}
                })+@([pscustomobject]@{Line=($lineCount+1);Text=$screen})
                $witness=@(Assert-TypeaheadWitness $before $observations $case.WitnessCommands $case.WitnessLines)
                $witness | ConvertTo-Json | Set-Content -LiteralPath "$report.execution.json" -Encoding UTF8
                # Marker presence is observation across pages, not a merged
                # history or execution count. Counts/order use the witnesses.
                $screen+=(@(Get-ChildItem -LiteralPath (Split-Path $report) -Filter ((Split-Path $report -Leaf)+'.line-*.console.txt') |
                    Sort-Object Name | ForEach-Object {Get-Content -LiteralPath $_.FullName -Raw}) -join "`n")
            }
            # SequentialMemLines already requires each real MEM result after
            # that command in its own ordered snapshot below. A merged scroll
            # history is neither necessary proof nor a product guarantee; do
            # not impose contiguous-frame history on these repainting pages.
            if($case.NativeExitCodes){
                $nativeResults=@(foreach($pair in $nativeWaiters.GetEnumerator()){
                    [pscustomobject]@{ProcessId=$pair.Key;Ended=$pair.Value.HasExited;Code=$(if($pair.Value.HasExited){$pair.Value.ExitCode}else{$null})}
                })
                $nativeResults | ConvertTo-Json | Set-Content -LiteralPath "$report.native-results.json" -Encoding UTF8
                foreach($code in $case.NativeExitCodes){
                    if(!@($nativeResults | Where-Object {$_.Ended -and $_.Code -eq $code}).Count){
                        throw "Missing actual native completion $code in $($case.Name)"
                    }
                }
            }
            foreach($line in $case.ExactConsoleLines) {
                # An echoed command is not evidence that the inner shell ran
                # it or observed its direct target's actual completion code.
                if($screen -notmatch ('(?m)^\[\d+\]\s*'+[regex]::Escape($line)+'\s*$')) {
                    throw "Missing executed output line for $($case.Name): $line"
                }
            }
            # The Console observer preserves physical rows.  A narrow remote
            # viewport may split one guest sentence across rows, so assertions
            # must consume the same display text with row separators removed.
            # A renderer can either hard-wrap a word or word-wrap at a space;
            # the snapshot also trims row-end spaces. Normalize whitespace on
            # BOTH sides so a 55-column Console does not turn "external
            # command" into a false missing-marker failure.
            $screenForMarkers=($screen -replace '(?m)^\[\d+\]\s?','') -replace '\s',''
            if ($screenForMarkers -match '(?im)(badcommandorfilename|isnotrecognizedasaninternalorexternalcommand)' -and
                !$case.ExpectedGuestError) {
                throw "Guest Console reported an unexpected command-resolution failure: $($case.Name)"
            }
            foreach($marker in $case.ConsoleMarkers) {
                if ($screenForMarkers -notmatch [regex]::Escape(($marker -replace '\s',''))) {
                    throw "Missing guest Console marker for $($case.Name): $marker"
                }
            }
            foreach($lineNumber in $case.SequentialMemLines) {
                $linePath='{0}.line-{1:d2}.console.txt' -f $report,$lineNumber
                if(!(Test-Path -LiteralPath $linePath)) {
                    throw "Missing per-command Console snapshot for $($case.Name): line $lineNumber"
                }
                $step=Get-Content -LiteralPath $linePath -Raw
                $plain=($step -replace '(?m)^\[\d+\]\s?','') -replace '\s',''
                $commandAt=$plain.LastIndexOf('>mem',[StringComparison]::OrdinalIgnoreCase)
                if($commandAt -lt 0 -or
                    $plain.IndexOf('bytestotalconventionalmemory',$commandAt,[StringComparison]::OrdinalIgnoreCase) -lt 0) {
                    throw "MEM output did not follow its own command in $($case.Name): line $lineNumber"
                }
            }
            if ($case.ConsoleMarkerCount -and
                [regex]::Matches($screenForMarkers,[regex]::Escape(($case.ConsoleMarkers[0] -replace '\s',''))).Count -ne $case.ConsoleMarkerCount) {
                throw "Unexpected guest Console marker count for $($case.Name): $($case.ConsoleMarkers[0])"
            }
            if($case.RootFrontend -and $OrdinaryFrontend){
                @($frontendWaiters.Keys) | ConvertTo-Json | Set-Content -LiteralPath "$report.frontend-pids.json" -Encoding UTF8
                if($frontendWaiters.Count -ne 1 -or $frontendWaiters.ContainsKey($launcherId)){
                    throw 'Expected one observed independent frontend session in the ordinary package'
                }
                if($case.Name -eq 'native-surviving-client') {
                    foreach($frontend in $frontendWaiters.Values) {
                        if(!$frontend.WaitForExit(10000)){
                            throw 'Frontend did not retire after the surviving Console client completed'
                        }
                    }
                }
            }elseif($case.RootFrontend){
                $identity=Get-Content -LiteralPath "$report.frontend.log" -Raw
                $owners=@([regex]::Matches($identity,'(?m)^RECEIVER pid=(\d+)\r?$'))
                if($owners.Count -ne 1 -or [int]$owners[0].Groups[1].Value -eq $launcherId){
                    throw 'Expected one independent frontend receiver across nested DOS/native work'
                }
            }
            # The product deliberately has no native-child report hook.  A
            # successful observed COMMAND session is the regression contract:
            # original COMMAND consumes the native child result and returns
            # through its own AX=4C00 path.
            if ($case.Name -eq 'native-streams') {
                foreach($marker in @('S10_STDOUT','S10_STDERR')) {
                    if([regex]::Matches($screen,$marker).Count -ne 1){throw "Missing or duplicate emitted stream marker: $marker"}
                }
            }
            # The COM image itself emits S10_GUEST_SEVEN before INT 21h/4C.
            # Together with the captured Console row this is direct guest
            # execution evidence.  Do not depend on the retired default-off
            # DEM-open observer: it is no longer part of the production graph.
            if ($case.Name -eq 'worker-version-rejection') {
                $broker=Get-Content -LiteralPath "$report.broker.log" -Raw
                if ($broker -notmatch 'phase=prepare' -or
                    [regex]::Matches($broker,'phase=empty-grace').Count -lt 2) {
                    throw 'Version-rejected worker did not release the prepared launch back to an empty broker'
                }
            }
            $results += [pscustomobject]@{ Case=$case.Name; Expected=$case.Code; Actual=$actual; Report=$report;
                BodyMs=$caseWatch.ElapsedMilliseconds;CleanupMs=0 }
            Write-Output "PASS $($case.Name): $actual"
        } finally {
            $cleanupWatch=[Diagnostics.Stopwatch]::StartNew()
            foreach($probe in $nativeWaiters.Values){$probe.Dispose()}
            foreach($probe in $frontendWaiters.Values){$probe.Dispose()}
            # A runner timeout can precede normal result parsing. Recover only
            # this run's recorded launcher identity so its path-checked children
            # (including the declared drive alias) do not escape test cleanup.
            if(!$launcherId -and (Test-Path -LiteralPath $report)) {
                $partial=Get-Content -LiteralPath $report -Raw
                $match=[regex]::Match($partial,'(?m)^pid=(\d+)\r?$')
                if($match.Success){$launcherId=[int]$match.Groups[1].Value}
            }
            # Only children of this recorded test launcher, with exact product paths.
            # Unrelated package processes are never killed by image name.
            if ($launcherId) {
                $owned=@($launcherId) + $reportedChildren + @($observedDescendants)
                $packageRows=@(Get-PackageProcesses)
                for ($depth=0; $depth -lt 5; ++$depth) {
                    $children=@($packageRows | Where-Object { $_.ParentProcessId -in $owned -and $_.ProcessId -notin $owned })
                    if (!$children.Count) { break }
                    $owned += @($children.ProcessId)
                }
                Stop-IdentityCheckedProcesses @($packageRows | Where-Object {$_.ProcessId -in $owned}) $productPaths
            }
            $cleanupMs=$cleanupWatch.ElapsedMilliseconds
            if($results.Count -and $results[-1].Case -eq $case.Name){$results[-1].CleanupMs=$cleanupMs}
        }
        if ((Get-PackageProcesses).Count) { throw 'Unowned package process remains; stopping matrix.' }
    }
} finally {
    foreach ($name in $environmentNames) { [Environment]::SetEnvironmentVariable($name,$previous[$name]) }
    # These names were written by this verifier under the declared runtime
    # test root; never retain test guest input beside the product package.
    Remove-Item -LiteralPath $generatedFixtures -Force -ErrorAction SilentlyContinue
    $results | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $LogRoot "$LogPrefix-summary.json") -Encoding utf8
}
