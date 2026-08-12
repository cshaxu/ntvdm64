[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$EvidenceRoot = '',
    [ValidateRange(1, 30)]
    [int]$WatchdogSeconds = 30
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent $PSScriptRoot
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-startup-transaction-msvc-r3'
}
if ([string]::IsNullOrWhiteSpace($EvidenceRoot)) {
    throw 'EvidenceRoot is required; choose one new immutable observation directory.'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing observation directory: $evidence" }

function Get-Sha256([string]$Path) { return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash }

$binary = Join-Path $build 'ntdos64-native-startup-transaction.exe'
$bios = Join-Path $build 'ROM\BIOS-bochs-latest'
$vga = Join-Path $build 'ROM\VGABIOS-lgpl-latest'
foreach ($path in @($binary, $bios, $vga)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing r3 observation input: $path" }
}

$components = @(
    [ordered]@{ role = 'ntio'; file = 'NTIO.SYS'; bytes = 33792; sha256 = 'cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937'; source = (Join-Path $repository 'artifacts\toolchain-runs\ntio-tools16-opennt-v1\base\mvdm\dos\v86\doskrnl\bios\NTIO.SYS') },
    [ordered]@{ role = 'ntdos'; file = 'NTDOS.SYS'; bytes = 27858; sha256 = '957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84'; source = (Join-Path $repository 'artifacts\toolchain-runs\ntdos-tools16-historical-v1\base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS') },
    [ordered]@{ role = 'command'; file = 'COMMAND.COM'; bytes = 50384; sha256 = '908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43'; source = (Join-Path $repository 'artifacts\toolchain-runs\command-tools16-opennt-v1\base\mvdm\dos\v86\cmd\command\COMMAND.COM') }
)
foreach ($component in $components) {
    if (-not (Test-Path -LiteralPath $component.source -PathType Leaf)) { throw "Missing source-built component: $($component.source)" }
    if ((Get-Item -LiteralPath $component.source).Length -ne $component.bytes -or (Get-Sha256 $component.source) -ne $component.sha256) {
        throw "Source-built component identity mismatch: $($component.file)"
    }
}

New-Item -ItemType Directory -Path (Join-Path $evidence 'ROM'), (Join-Path $evidence 'byob') -Force | Out-Null
Copy-Item -LiteralPath $bios -Destination (Join-Path $evidence 'ROM\BIOS-bochs-latest') -ErrorAction Stop
Copy-Item -LiteralPath $vga -Destination (Join-Path $evidence 'ROM\VGABIOS-lgpl-latest') -ErrorAction Stop
foreach ($component in $components) {
    $destination = Join-Path $evidence ('byob\' + $component.file)
    Copy-Item -LiteralPath $component.source -Destination $destination -ErrorAction Stop
    if ((Get-Item -LiteralPath $destination).Length -ne $component.bytes -or (Get-Sha256 $destination) -ne $component.sha256) {
        throw "Copied component identity mismatch: $($component.file)"
    }
}

$profilePath = Join-Path $evidence 'byob\profile.json'
$profile = [ordered]@{
    schema = 'ntdos64-byob-profile-v1'
    profile = 'nt4-en-us-command-smoke-v1'
    architecture = 'x86'
    locale = 'en-US'
    compatibility_group = 'source-built-ntio-startup-transaction-r3'
    components = @($components | ForEach-Object { [ordered]@{ role = $_.role; file_name = $_.file; required = $true; bytes = $_.bytes; sha256 = $_.sha256; version = $null } })
    features = @()
    owner_note = $null
    machine_startup_plan = $true
    machine_startup_entry = 'ntio-v0'
    host_drive_inventory = [ordered]@{ types = @(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0) }
}
[IO.File]::WriteAllText($profilePath, ($profile | ConvertTo-Json -Depth 6 -Compress), [Text.UTF8Encoding]::new($false))

$bochsrcPath = Join-Path $evidence 'real-ntio-startup.bochsrc'
$bochsrc = @"
config_interface: textconfig
display_library: nogui
cpu: count=1, ips=1000000, reset_on_triple_fault=0
megs: 4
romimage: file=ROM/BIOS-bochs-latest
vgaromimage: file=ROM/VGABIOS-lgpl-latest
pci: enabled=0
ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
ata1: enabled=0, ioaddr1=0x170, ioaddr2=0x370, irq=15
ata2: enabled=0, ioaddr1=0x1e8, ioaddr2=0x3e0, irq=11
ata3: enabled=0, ioaddr1=0x168, ioaddr2=0x360, irq=9
vga: extension=none, update_freq=5
floppy_bootsig_check: disabled=1
boot: disk
plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0, gameport=0
"@
[IO.File]::WriteAllText($bochsrcPath, $bochsrc, [Text.UTF8Encoding]::new($false))

$stdoutPath = Join-Path $evidence 'stdout.log'
$stderrPath = Join-Path $evidence 'stderr.log'
$bochsLogPath = Join-Path $evidence 'bochs.log'
$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $binary
$start.Arguments = '-q -f real-ntio-startup.bochsrc -benchmark 1 -log bochs.log'
$start.WorkingDirectory = $evidence
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$environmentChanges = [ordered]@{
    NTDOS64_UD_TEST = ''
    NTDOS64_ADAPTER_BRIDGE = ''
    NTDOS64_ADAPTER_PROFILE = $profilePath
    NTDOS64_ADAPTER_ROOT = (Join-Path $evidence 'byob')
}
$priorEnvironment = [ordered]@{}
foreach ($name in $environmentChanges.Keys) {
    $priorEnvironment[$name] = [Environment]::GetEnvironmentVariable($name, 'Process')
    [Environment]::SetEnvironmentVariable($name, $environmentChanges[$name], 'Process')
}
$process = [Diagnostics.Process]::new()
$process.StartInfo = $start
$started = [DateTime]::UtcNow
$watchdogTerminated = $false
$directKillAttempted = $false
try {
    if (-not $process.Start()) { throw 'Unable to start r3 real-NTIO observation.' }
    $completed = $process.WaitForExit($WatchdogSeconds * 1000)
    if (-not $completed) {
        if ($process.HasExited) { throw 'r3 process exited during watchdog handoff without retained completion.' }
        $watchdogTerminated = $true
        $directKillAttempted = $true
        $process.Kill()
        $process.WaitForExit()
    }
} finally {
    foreach ($name in $priorEnvironment.Keys) {
        [Environment]::SetEnvironmentVariable($name, $priorEnvironment[$name], 'Process')
    }
}
$bochsLogComplete = Test-Path -LiteralPath $bochsLogPath -PathType Leaf
$exitCode = $process.ExitCode
$log = if ($bochsLogComplete) { Get-Content -LiteralPath $bochsLogPath -Raw } else { '' }
$commitMatches = [regex]::Matches($log, 'guest transaction committed cs=([0-9a-f]+) eip=([0-9a-f]+) address=([0-9a-f]+) bytes=([0-9a-f]+) resume=([0-9a-f]+)')
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.real-ntio-startup-transaction-observation.v2'
    buildRoot = $build; evidenceRoot = $evidence
    command = 'ntdos64-native-startup-transaction.exe -q -f real-ntio-startup.bochsrc -benchmark 1 -log bochs.log'
    startedUtc = $started.ToString('o'); elapsedMilliseconds = [int](([DateTime]::UtcNow - $started).TotalMilliseconds)
    watchdogSeconds = $WatchdogSeconds; watchdogTerminated = $watchdogTerminated; exitCode = $exitCode; processId = $process.Id
    directKillAttempted = $directKillAttempted; bochsLogComplete = $bochsLogComplete
    binarySha256 = Get-Sha256 $binary; profileSha256 = Get-Sha256 $profilePath; bochsrcSha256 = Get-Sha256 $bochsrcPath
    biosSha256 = Get-Sha256 (Join-Path $evidence 'ROM\BIOS-bochs-latest'); vgaBiosSha256 = Get-Sha256 (Join-Path $evidence 'ROM\VGABIOS-lgpl-latest')
    componentSources = @($components | ForEach-Object { [ordered]@{ role = $_.role; file = $_.file; sha256 = $_.sha256; bytes = $_.bytes } })
    planApplied = $log -match 'requested execution plan applied'; benchmarkObserved = $log -match 'Bochs benchmark mode is ON'; controlledStopObserved = $log -match 'cpu loop quit, shutting down simulator'
    genericCommitCount = $commitMatches.Count; genericCommit = if ($commitMatches.Count -eq 1) { $commitMatches[0].Value } else { $null }
    # Passive catalogue observations deliberately use the "ntdos64 adapter bop
    # observed" prefix.  They do not consume, resume, or otherwise intercept
    # the exception, so do not conflate them with the retired broad interceptor.
    bopObservationCount = [regex]::Matches($log, 'ntdos64 adapter bop observed cpu=').Count
    broadInterceptorObserved = $log -match 'ntdos64 #UD test interceptor|ntdos64 broad (?:#UD )?interceptor'
    environmentNames = @($environmentChanges.Keys)
    environmentRestored = $true
}
$recordPath = Join-Path $evidence 'observation.json'
[IO.File]::WriteAllText($recordPath, ($record | ConvertTo-Json -Depth 6), [Text.UTF8Encoding]::new($false))
if ($watchdogTerminated -or -not $bochsLogComplete -or $exitCode -notin @(0,1) -or -not $record.planApplied -or -not $record.benchmarkObserved -or -not $record.controlledStopObserved -or $record.genericCommitCount -ne 1 -or $record.broadInterceptorObserved) {
    throw "r3 real-NTIO startup transaction predicates failed: $recordPath"
}
Write-Host "r3 real-NTIO startup transaction observation completed: $evidence"
