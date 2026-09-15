[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Observer,
    [string]$PackageRoot = 'O:\winnt',
    [string]$LogPrefix = 'm0-t412-s8-exit',
    [string[]]$Cases,
    [string]$GuestFixturePath
)
$ErrorActionPreference = 'Stop'
$Observer = (Resolve-Path -LiteralPath $Observer).Path
$PackageRoot = (Resolve-Path -LiteralPath $PackageRoot).Path
if ($LogPrefix -notmatch '^[a-z0-9-]+$') { throw 'Invalid log prefix' }
$productPaths = @('run16.exe','ntvdm.exe','basesrv.exe') | ForEach-Object { Join-Path $PackageRoot $_ }
function Get-PackageProcesses {
    @(Get-CimInstance Win32_Process -Filter "Name='run16.exe' OR Name='ntvdm.exe' OR Name='basesrv.exe'" |
        Where-Object { $_.ExecutablePath -in $productPaths })
}
if ((Get-PackageProcesses).Count) { throw 'Package already in use; no existing process will be stopped.' }
if ((!$Cases -or 'guest-seven' -in $Cases -or 'command-guest-seven' -in $Cases) -and !$GuestFixturePath) {
    throw 'Guest cases require -GuestFixturePath with a verified DOS-accessible short path to the build fixture.'
}
$guest=Join-Path (Split-Path -Parent $Observer) 'G7.COM'
# Test-only DOS program: MOV AX,4C07h; INT 21h. Never replaces package media.
if ($guest -notlike '*\build\M0-T412\S8\*') { throw 'Guest fixture must stay in S8 build root' }
[IO.File]::WriteAllBytes($guest,[byte[]](0xb8,0x07,0x4c,0xcd,0x21))
if ($GuestFixturePath) {
    if (!(Test-Path -LiteralPath $GuestFixturePath) -or
        (Get-FileHash -LiteralPath $GuestFixturePath).Hash -ne (Get-FileHash -LiteralPath $guest).Hash) {
        throw 'Short-path fixture does not match the build artifact'
    }
    $guest=$GuestFixturePath
}
$matrix = @(
    @{ Name='empty'; Text="exit`r"; Code=0 },
    @{ Name='native-zero'; Text="ver`rexit`r"; Code=0 },
    @{ Name='missing'; Text="missing`rver`rexit`r"; Code=0 },
    @{ Name='native-seven'; Text="cmd /c exit 7`rexit`r"; Code=0 },
    @{ Name='mem'; Text="mem`rexit`r"; Code=1 },
    @{ Name='nested-empty'; Text="command`rexit`rexit`r"; Code=1 },
    @{ Name='nested-mem'; Text="command`rcommand`rmem`rexit`rmem`rexit`rmem`rexit`r"; Code=1 },
    @{ Name='mem-repeat'; Text="mem`rmem`rexit`r"; Code=1 },
    @{ Name='direct-mem'; Args=@('MEM.EXE'); Code=0 },
    @{ Name='command-c'; Args=@('COMMAND.COM','/c','ver'); Code=0 },
    # Original COMMAND::LodCom1 -> FatalRet2 uses AX=4C00, not RetCode.
    @{ Name='command-c-seven'; Args=@('COMMAND.COM','/c','cmd','/c','exit','7'); Code=0 },
    @{ Name='guest-seven'; Args=@($guest); Code=7 },
    @{ Name='command-guest-seven'; Args=@('COMMAND.COM','/c',$guest); Code=0 },
    @{ Name='direct-seven'; Args=@('cmd.exe','/c','exit','7'); Code=7 },
    @{ Name='edit'; Edit=$true; Code=1 }
)
foreach ($selected in $Cases) {
    if ($selected -notin $matrix.Name) { throw "Unknown case: $selected" }
}
$environmentNames = @('MVDM_BASESRV_TRACE_PATH','MVDM_NATIVE_CHILD_REPORT_PATH','MVDM_COMMAND_CONTINUATION_REPORT_PATH')
$previous = @{}
foreach ($name in $environmentNames) { $previous[$name]=[Environment]::GetEnvironmentVariable($name) }
$results = @()
try {
    foreach ($case in $matrix) {
        if ($Cases -and $case.Name -notin $Cases) { continue }
        $report=Join-Path $PackageRoot "logs\$LogPrefix-$($case.Name).txt"
        if (Test-Path -LiteralPath $report) { throw "Use a fresh log prefix: $report exists" }
        [Environment]::SetEnvironmentVariable($environmentNames[0],"$report.broker.log")
        [Environment]::SetEnvironmentVariable($environmentNames[1],"$report.child.log")
        [Environment]::SetEnvironmentVariable($environmentNames[2],"$report.command.log")
        $arguments=@((Join-Path $PackageRoot 'run16.exe'),$PackageRoot,$report)
        if ($case.Args) { $arguments += $case.Args } else { $arguments += 'COMMAND.COM' }
        if ($case.Text) { $arguments += @('--observe-console-input-text',('"'+$case.Text+'"')) }
        if ($case.Edit) { $arguments += '--observe-console-edit-return' }
        $arguments += @('--observation-timeout-ms','20000')
        $launcherId = 0
        $observation = Start-Process -FilePath $Observer -ArgumentList $arguments -WorkingDirectory $PackageRoot -WindowStyle Hidden -PassThru
        try {
            if (!$observation.WaitForExit(55000)) { throw "Observer timeout: $($case.Name)" }
            $record=Get-Content -LiteralPath $report -Raw
            $launcherId=[int]([regex]::Match($record,'(?m)^pid=(\d+)').Groups[1].Value)
            if (!$launcherId) { throw 'Missing test launcher identity' }
            $actual=[Convert]::ToUInt32([regex]::Match($record,'(?m)^exit=0x([0-9a-f]+)').Groups[1].Value,16)
            if ($record -notmatch '(?m)^result=exited' -or $actual -ne $case.Code) {
                throw "Unexpected $($case.Name) result: $actual; expected $($case.Code)"
            }
            if (($case.Text -or $case.Edit) -and $record -notmatch '(?m)^scripted-console-input=delivered') {
                throw "Input not delivered: $($case.Name)"
            }
            if ($case.Name -ne 'direct-seven') {
                $guestReport=Get-Content -LiteralPath "$report.command.log" -Raw
                $returns=[regex]::Matches($guestReport,'MVDM-CMD-GUEST-RETURN code=([0-9A-F]+) first=0 repeat=0')
                if (!$returns.Count -or [Convert]::ToUInt32($returns[$returns.Count-1].Groups[1].Value,16) -ne $actual) {
                    throw "Guest/launcher result mismatch: $($case.Name)"
                }
            }
            if ($case.Name -in @('native-seven','command-c-seven')) {
                $native=Get-Content -LiteralPath "$report.child.log" -Raw
                if ($native -notmatch 'phase=1 status=1 value=00000007') { throw 'Native child 7 not observed' }
            }
            if ($case.Name -in @('guest-seven','command-guest-seven')) {
                $opens=Get-Content -LiteralPath "$report.dem-open.txt" -Raw
                if ($opens -notmatch ('phase=1[^\r\n]*cf=0 path='+[regex]::Escape($GuestFixturePath))) {
                    throw 'DOS guest fixture was not successfully opened; not exit-code evidence'
                }
            }
            if ($case.Name -eq 'nested-mem') {
                $screen=Get-Content -LiteralPath "$report.line-07.console.txt" -Raw
                if ([regex]::Matches($screen,'bytes total conventional memory').Count -ne 3) {
                    throw 'Three distinct-depth MEM reports not observed'
                }
            }
            $results += [pscustomobject]@{ Case=$case.Name; Expected=$case.Code; Actual=$actual; Report=$report }
            Write-Output "PASS $($case.Name): $actual"
        } finally {
            # Only children of this recorded test launcher, with exact product paths.
            # Unrelated package processes are never killed by image name.
            if ($launcherId) {
                $owned=@($launcherId)
                for ($depth=0; $depth -lt 5; ++$depth) {
                    $children=@(Get-PackageProcesses | Where-Object { $_.ParentProcessId -in $owned -and $_.ProcessId -notin $owned })
                    if (!$children.Count) { break }
                    $owned += @($children.ProcessId)
                }
                foreach ($id in ($owned | Sort-Object -Descending)) {
                    $process=Get-PackageProcesses | Where-Object { $_.ProcessId -eq $id }
                    if ($process -and ($id -eq $launcherId -or $process.ParentProcessId -in $owned)) { Stop-Process -Id $id }
                }
            }
        }
        if ((Get-PackageProcesses).Count) { throw 'Unowned package process remains; stopping matrix.' }
    }
} finally {
    foreach ($name in $environmentNames) { [Environment]::SetEnvironmentVariable($name,$previous[$name]) }
    $results | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $PackageRoot "logs\$LogPrefix-summary.json") -Encoding utf8
}
