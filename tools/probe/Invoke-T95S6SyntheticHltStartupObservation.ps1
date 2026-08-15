[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$EvidenceRoot = '',
    [ValidateSet('synthetic-hlt', 'source-built-ntio')]
    [string]$PackageMode = 'synthetic-hlt',
    [string]$NtioSource = '',
    [string]$NtdosSource = '',
    [string]$CommandSource = '',
    [ValidateRange(1, 60)]
    [int]$WatchdogSeconds = 30
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-adapter-msvc-r2'
}
if ([string]::IsNullOrWhiteSpace($EvidenceRoot)) {
    $EvidenceRoot = Join-Path $repository 'artifacts\analysis\t95-s6-synthetic-hlt-startup-001-20260811-001'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing evidence root: $evidence" }
$binary = Join-Path $build 'ntdos64-native-adapter-observation.exe'
$bios = Join-Path $build 'ROM\BIOS-bochs-latest'
$vga = Join-Path $build 'ROM\VGABIOS-lgpl-latest'
foreach ($path in @($binary, $bios, $vga)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing admitted input: $path" }
}

New-Item -ItemType Directory -Path (Join-Path $evidence 'guest'), (Join-Path $evidence 'ROM') -Force | Out-Null
$guest = Join-Path $evidence 'guest'
$ntio = Join-Path $guest 'NTIO.SYS'
$ntdos = Join-Path $guest 'NTDOS.SYS'
$command = Join-Path $guest 'COMMAND.COM'
if ($PackageMode -eq 'synthetic-hlt') {
    [IO.File]::WriteAllBytes($ntio, [byte[]]@(0xf4))
    [IO.File]::WriteAllBytes($ntdos, [byte[]]@(0x00))
    [IO.File]::WriteAllBytes($command, [byte[]]@(0x00))
} else {
    foreach ($input in @($NtioSource, $NtdosSource, $CommandSource)) {
        if ([string]::IsNullOrWhiteSpace($input) -or -not (Test-Path -LiteralPath $input -PathType Leaf)) {
            throw 'source-built-ntio mode requires three existing declared input files.'
        }
    }
    Copy-Item -LiteralPath $NtioSource -Destination $ntio -ErrorAction Stop
    Copy-Item -LiteralPath $NtdosSource -Destination $ntdos -ErrorAction Stop
    Copy-Item -LiteralPath $CommandSource -Destination $command -ErrorAction Stop
}
$ntioHash = (Get-FileHash -LiteralPath $ntio -Algorithm SHA256).Hash.ToLowerInvariant()
$ntdosHash = (Get-FileHash -LiteralPath $ntdos -Algorithm SHA256).Hash.ToLowerInvariant()
$commandHash = (Get-FileHash -LiteralPath $command -Algorithm SHA256).Hash.ToLowerInvariant()
$ntioBytes = (Get-Item -LiteralPath $ntio).Length
$ntdosBytes = (Get-Item -LiteralPath $ntdos).Length
$commandBytes = (Get-Item -LiteralPath $command).Length
$profile = Join-Path $guest 'profile.json'
$profileObject = [ordered]@{
    schema = 'ntdos64-byob-profile-v1'
    profile = 'nt4-en-us-command-smoke-v1'
    architecture = 'x86'
    locale = 'en-US'
    compatibility_group = if ($PackageMode -eq 'synthetic-hlt') { 'synthetic-hlt-mechanics-only' } else { 'source-built-ntio-prefix-only' }
    components = @(
        [ordered]@{ role = 'ntio'; file_name = 'NTIO.SYS'; required = $true; bytes = $ntioBytes; sha256 = $ntioHash; version = $null },
        [ordered]@{ role = 'ntdos'; file_name = 'NTDOS.SYS'; required = $true; bytes = $ntdosBytes; sha256 = $ntdosHash; version = $null },
        [ordered]@{ role = 'command'; file_name = 'COMMAND.COM'; required = $true; bytes = $commandBytes; sha256 = $commandHash; version = $null }
    )
    features = @()
    owner_note = $null
    machine_startup_plan = $true
    machine_startup_entry = 'ntio-v0'
}
[IO.File]::WriteAllText($profile, ($profileObject | ConvertTo-Json -Depth 5), [Text.UTF8Encoding]::new($false))
$artifactManifest = [ordered]@{
    schema = 'ntdos64.synthetic-hlt-startup-package.v1'
    role = if ($PackageMode -eq 'synthetic-hlt') { 'internal generic startup-plan mechanics fixture; not a DOS or NTIO artifact' } else { 'source-built NTIO first-prefix research package; not a DOS runtime package' }
    files = @(
        [ordered]@{ name = 'NTIO.SYS'; bytes = $ntioBytes; sha256 = $ntioHash; meaning = if ($PackageMode -eq 'synthetic-hlt') { 'one-byte HLT, the only placed guest payload' } else { 'source-built input, the only placed guest payload' } },
        [ordered]@{ name = 'NTDOS.SYS'; bytes = $ntdosBytes; sha256 = $ntdosHash; meaning = 'identity-only input; never placed' },
        [ordered]@{ name = 'COMMAND.COM'; bytes = $commandBytes; sha256 = $commandHash; meaning = 'identity-only input; never placed' }
    )
}
[IO.File]::WriteAllText((Join-Path $guest 'artifact-manifest.json'), ($artifactManifest | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))

$runtimeProfile = Join-Path $evidence 'pc-reset-floor-v1.bochsrc'
$runtimeText = @"
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
[IO.File]::WriteAllText($runtimeProfile, $runtimeText, [Text.UTF8Encoding]::new($false))
Copy-Item -LiteralPath $bios -Destination (Join-Path $evidence 'ROM\BIOS-bochs-latest') -ErrorAction Stop
Copy-Item -LiteralPath $vga -Destination (Join-Path $evidence 'ROM\VGABIOS-lgpl-latest') -ErrorAction Stop

$stdout = Join-Path $evidence 'stdout.log'
$stderr = Join-Path $evidence 'stderr.log'
$exitCodePath = Join-Path $evidence 'exitcode.txt'
$wrapper = Join-Path $evidence 'run-synthetic-hlt.cmd'
$wrapperText = @"
@echo off
setlocal
set "NTDOS64_UD_TEST="
set "NTDOS64_ADAPTER_BRIDGE="
set "NTDOS64_HOST_EXCLUDE_DRIVES="
set "NTDOS64_HOST_INCLUDE_DRIVES="
set "NTDOS64_ADAPTER_PROFILE=$profile"
set "NTDOS64_ADAPTER_ROOT=$guest"
cd /d "$evidence"
"$binary" -q -f pc-reset-floor-v1.bochsrc -benchmark 1 > "$stdout" 2> "$stderr"
set "NTDOS64_EXITCODE=%ERRORLEVEL%"
> "$exitCodePath" echo %NTDOS64_EXITCODE%
exit /b %NTDOS64_EXITCODE%
"@
[IO.File]::WriteAllText($wrapper, $wrapperText, [Text.UTF8Encoding]::new($false))
$started = [DateTime]::UtcNow
$process = Start-Process -FilePath $env:ComSpec -ArgumentList @('/d', '/c', $wrapper) -WindowStyle Hidden -PassThru
$completed = $process.WaitForExit($WatchdogSeconds * 1000)
$watchdogTerminated = $false
if (-not $completed) { $watchdogTerminated = $true; $process.Kill(); $process.WaitForExit() }
$exitCode = if (Test-Path -LiteralPath $exitCodePath -PathType Leaf) { [int](Get-Content -LiteralPath $exitCodePath -Raw).Trim() } else { $process.ExitCode }
$logText = ((Get-Content -LiteralPath $stdout -Raw), (Get-Content -LiteralPath $stderr -Raw)) -join "`n"
$planApplied = $logText -match 'requested execution plan applied'
$controlledStop = $logText -match 'cpu loop quit, shutting down simulator'
$entryObserved = $logText -match 'CS:0070\(' -and $logText -match 'EIP=00000001'
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.synthetic-hlt-startup-observation.v1'
    packageMode = $PackageMode
    buildRoot = $build
    evidenceRoot = $evidence
    binarySha256 = (Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash
    profileSha256 = (Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash
    runtimeProfileSha256 = (Get-FileHash -LiteralPath $runtimeProfile -Algorithm SHA256).Hash
    exitCode = $exitCode
    elapsedMilliseconds = [int](([DateTime]::UtcNow - $started).TotalMilliseconds)
    watchdogTerminated = $watchdogTerminated
    planApplied = $planApplied
    controlledStop = $controlledStop
    hltEntryObserved = $entryObserved
    ntioSha256 = $ntioHash
    ntdosSha256 = $ntdosHash
    commandSha256 = $commandHash
}
[IO.File]::WriteAllText((Join-Path $evidence 'observation.json'), ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
if ($PackageMode -eq 'synthetic-hlt' -and ($watchdogTerminated -or $exitCode -notin @(0, 1) -or -not $planApplied -or -not $controlledStop -or -not $entryObserved)) {
    throw "Synthetic HLT startup observation did not meet predicates: $evidence"
}
Write-Host "Synthetic HLT startup observation completed: $evidence"
