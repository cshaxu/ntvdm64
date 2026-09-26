[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [string]$PackageRoot = 'O:\winnt',
    [string]$LogRoot = 'O:\winnt\logs',
    [string]$LogPrefix = 'm0-t412-s8-exit',
    [string[]]$Cases,
    [string]$GuestFixturePath,
    [string]$VideoGuestFixturePath
)
$ErrorActionPreference = 'Stop'
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
$LogRoot = (Resolve-Path -LiteralPath $LogRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$runtimeFixtureRoot = Join-Path $PackageRoot 'tests'
if (!(Test-Path -LiteralPath $runtimeFixtureRoot)) {
    New-Item -ItemType Directory -Path $runtimeFixtureRoot -Force | Out-Null
}
$runtimeFixtureRoot = (Resolve-Path -LiteralPath $runtimeFixtureRoot).Path
if($Cases -contains 'graphics-return' -or $Cases -contains 'direct-graphics-return') {
    if(!$VideoGuestFixturePath){throw 'Graphics return requires an authored video fixture'}
    $video=(Resolve-Path -LiteralPath $VideoGuestFixturePath).Path
    $videoBuild=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../build'))+'\'
    if(!$video.StartsWith($videoBuild,[StringComparison]::OrdinalIgnoreCase)){throw 'Video fixture must be under repository build'}
    $manifest=Get-Content -LiteralPath (Join-Path (Split-Path $video) 'manifest.json') -Raw | ConvertFrom-Json
    if($manifest.route -ne 'graphics-vram' -or (Get-FileHash -LiteralPath $video).Hash -ne $manifest.sha256){throw 'Wrong video probe or hash'}
    Copy-Item -LiteralPath $video -Destination (Join-Path $runtimeFixtureRoot 'VTGRAPH.COM') -Force
}
$generatedFixtures = @(
    (Join-Path $runtimeFixtureRoot 'G7.COM'),
    (Join-Path $runtimeFixtureRoot 'STREAM.CMD'),
    (Join-Path $runtimeFixtureRoot 'EOF.CMD'),
    (Join-Path $runtimeFixtureRoot 'D7.CMD')
)
$productPaths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function Get-PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
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
    @{ Name='missing'; Text="missing`rver`rexit`r"; Code=0; ConsoleMarkers=@('is not recognized as an internal or external command','Microsoft Windows [Version'); ExpectedGuestError=$true },
    @{ Name='native-seven'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'D7.CMD')); Code=0; ConsoleMarkers=@('S10_DIRECT_SEVEN') },
    @{ Name='native-streams'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'STREAM.CMD')); Code=0; ConsoleMarkers=@('S10_STDOUT','S10_STDERR') },
    @{ Name='native-eof'; Args=@('COMMAND.COM','/c','cmd','/c',(Join-Path $shortFixtureRoot 'EOF.CMD')); Code=0; ConsoleMarkers=@('S10_EOF') },
    @{ Name='mem'; Text="mem`rexit`r"; Code=1; ConsoleMarkers=@('bytes total conventional memory') },
    # A nested COMMAND consumes the preceding exit and recreates its input
    # loop.  Pace complete lines so the next key sequence is not offered while
    # the original keyboard queue is between those two owners.
    @{ Name='nested-empty'; Text="command`rexit`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('Microsoft(R) Windows NT DOS'); ConsoleMarkerCount=2 },
    @{ Name='nested-mem'; Text="command`rcommand`rmem`rexit`rmem`rexit`rmem`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('bytes total conventional memory'); ConsoleMarkerCount=3 },
    @{ Name='mem-repeat'; Text="mem`rmem`rexit`r"; Code=1; ConsoleMarkers=@('bytes total conventional memory') },
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
    @{ Name='native-cmd-dos'; Supplemental=$true; Args=@('cmd.exe','/d','/c',('"'+(Join-Path $PackageRoot 'run16.exe')+' MEM.EXE"')); Code=0; ConsoleMarkers=@('bytes total conventional memory') },
    @{ Name='native-root-frontend'; Supplemental=$true; RootFrontend=$true; Args=@('cmd.exe','/d','/c',('"'+(Join-Path $PackageRoot 'run16.exe')+' MEM.EXE"')); Code=0; ConsoleMarkers=@('bytes total conventional memory') },
    @{ Name='direct-graphics-return'; Supplemental=$true; Args=@((Join-Path $runtimeFixtureRoot 'VTGRAPH.COM')); Code=0; ConsoleMarkers=@('S23_GRAPHICS_VRAM_OK') },
    @{ Name='graphics-return'; Supplemental=$true; Text=((Join-Path $runtimeFixtureRoot 'VTGRAPH.COM')+"`rmem`rexit`r"); LineDelayMs=1000; Code=1; ConsoleMarkers=@('S23_GRAPHICS_VRAM_OK','bytes total conventional memory') },
    # Keep one root frontend alive across two sequential DOS submissions.
    @{ Name='native-cmd-dos-repeat'; Supplemental=$true; Args=@('cmd.exe','/d','/c',('""'+(Join-Path $PackageRoot 'run16.exe')+'" MEM.EXE & "'+(Join-Path $PackageRoot 'run16.exe')+'" MEM.EXE"')); Code=0; ConsoleMarkers=@('bytes total conventional memory'); ConsoleMarkerCount=2 },
    @{ Name='dos-native-dos'; Supplemental=$true; Text="cmd.exe /d`rrun16 mem`rexit`rmem`rexit`r"; LineDelayMs=1000; Code=1; ConsoleMarkers=@('bytes total conventional memory','Microsoft Windows [Version'); ConsoleMarkerCount=2 },
    # No line-level wait for an owner change: keep typing through DOS/native
    # handoff. The observer still emits ordinary paired key records.
    @{ Name='dos-native-typeahead'; Supplemental=$true; Text="cmd.exe /d`rrun16 mem`rexit`rmem`rexit`r"; LineDelayMs=0; Code=1; ConsoleMarkers=@('bytes total conventional memory','Microsoft Windows [Version'); ConsoleMarkerCount=2 },
    @{ Name='worker-version-rejection'; Args=@('MEM.EXE'); Code=1306; Negative=$true }
)
foreach ($case in $matrix) {
    if ($case.Name -in @('native-cmd-dos-repeat','dos-native-dos','dos-native-typeahead')) {
        $case.RootFrontend=$true
    }
}
foreach ($selected in $Cases) {
    if ($selected -notin $matrix.Name) { throw "Unknown case: $selected" }
}
$environmentNames = @('MVDM_BASESRV_TRACE_PATH','MVDM_S34_TRACE_PATH')
$previous = @{}
foreach ($name in $environmentNames) { $previous[$name]=[Environment]::GetEnvironmentVariable($name) }
$results = @()
try {
    foreach ($case in $matrix) {
        if (($case.Negative -or $case.Supplemental) -and !$Cases) { continue }
        if ($Cases -and $case.Name -notin $Cases) { continue }
        $report=Join-Path $LogRoot "$LogPrefix-$($case.Name).txt"
        if (Test-Path -LiteralPath $report) { throw "Use a fresh log prefix: $report exists" }
        [Environment]::SetEnvironmentVariable($environmentNames[0],"$report.broker.log")
        if($case.RootFrontend){[Environment]::SetEnvironmentVariable('MVDM_S34_TRACE_PATH',"$report.frontend.log")}
        else{[Environment]::SetEnvironmentVariable('MVDM_S34_TRACE_PATH',$previous['MVDM_S34_TRACE_PATH'])}
        $arguments=@((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report)
        if ($case.Args) { $arguments += $case.Args } else { $arguments += 'COMMAND.COM' }
        if ($case.Text) {
            $arguments += @('--observe-console-input-text',('"'+$case.Text+'"'))
            if ($case.ContainsKey('LineDelayMs')) {
                $arguments += @('--observe-console-line-delay-ms',$case.LineDelayMs)
            }
        }
        if ($case.Edit) { $arguments += '--observe-console-edit-return' }
        $arguments += @('--observation-timeout-ms','20000')
        $launcherId = 0
        $reportedChildren = @()
        $observedDescendants = [Collections.Generic.HashSet[int]]::new()
        $observation = Start-Process -FilePath $Observer -ArgumentList $arguments -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
        try {
            if($case.Supplemental){
                [void]$observedDescendants.Add($observation.Id)
                $treeDeadline=[DateTime]::UtcNow.AddSeconds(50)
                do {
                    # Include the native CMD relay while it is alive; after
                    # exit, a product-only snapshot cannot reconstruct it.
                    $tree=@(Get-CimInstance Win32_Process | Select-Object ProcessId,ParentProcessId)
                    for($depth=0;$depth -lt 8;++$depth){
                        $added=$false
                        foreach($node in $tree){
                            if($observedDescendants.Contains([int]$node.ParentProcessId)){
                                if($observedDescendants.Add([int]$node.ProcessId)){$added=$true}
                            }
                        }
                        if(!$added){break}
                    }
                    if($observation.WaitForExit(100)){break}
                } while([DateTime]::UtcNow -lt $treeDeadline)
            }
            if (!$observation.WaitForExit(55000)) { throw "Observer timeout: $($case.Name)" }
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
            $consolePath="$report.console.txt"
            if (!(Test-Path -LiteralPath $consolePath)) {
                throw "Missing captured guest Console text: $($case.Name)"
            }
            $screen=Get-Content -LiteralPath $consolePath -Raw
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
            if ($case.ConsoleMarkerCount -and
                [regex]::Matches($screenForMarkers,[regex]::Escape(($case.ConsoleMarkers[0] -replace '\s',''))).Count -ne $case.ConsoleMarkerCount) {
                throw "Unexpected guest Console marker count for $($case.Name): $($case.ConsoleMarkers[0])"
            }
            if($case.RootFrontend){
                $identity=Get-Content -LiteralPath "$report.frontend.log" -Raw
                $owners=@([regex]::Matches($identity,'(?m)^(\d+) run16-frontend-owner (\d+)\r?$'))
                if(!$owners.Count){throw 'No production frontend owner observed'}
                foreach($owner in $owners){
                    if([int]$owner.Groups[1].Value -ne $launcherId -or
                       [int]$owner.Groups[2].Value -ne $launcherId){
                        throw "Frontend ownership mismatch: root=$launcherId actual=$($owner.Groups[1].Value)"
                    }
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
            $results += [pscustomobject]@{ Case=$case.Name; Expected=$case.Code; Actual=$actual; Report=$report }
            Write-Output "PASS $($case.Name): $actual"
        } finally {
            # Only children of this recorded test launcher, with exact product paths.
            # Unrelated package processes are never killed by image name.
            if ($launcherId) {
                $owned=@($launcherId) + $reportedChildren + @($observedDescendants)
                for ($depth=0; $depth -lt 5; ++$depth) {
                    $children=@(Get-PackageProcesses | Where-Object { $_.ParentProcessId -in $owned -and $_.ProcessId -notin $owned })
                    if (!$children.Count) { break }
                    $owned += @($children.ProcessId)
                }
                foreach ($id in ($owned | Sort-Object -Descending)) {
                    $process=Get-PackageProcesses | Where-Object { $_.ProcessId -eq $id }
                    if ($process -and $id -in $owned) {
                        # COMMAND can hand its Console to a nested owner just
                        # before this observer regains control.  Terminate the
                        # exact recorded test tree, then tolerate a natural
                        # exit race; never expand cleanup by image name.
                        Stop-Process -Id $id -Force -ErrorAction SilentlyContinue
                    }
                }
                $deadline = [Environment]::TickCount64 + 5000
                do {
                    $remaining = @(Get-PackageProcesses | Where-Object {
                        $_.ProcessId -in $owned
                    })
                    if (!$remaining.Count) { break }
                    Start-Sleep -Milliseconds 100
                } while ([Environment]::TickCount64 -lt $deadline)
            }
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
