[CmdletBinding()]
param(
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$EvidenceRoot = '',
    [string]$BinaryName = 'ntdos64-native-container.exe',
    [ValidateSet('r4-default', 'pc-reset-floor-v1')]
    [string]$ProfileMode = 'r4-default',
    [ValidateRange(1, 120)]
    [int]$WatchdogSeconds = 30
)

$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
}
$repository = [IO.Path]::GetFullPath($RepositoryRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-intact-msvc-r4'
}
if ([string]::IsNullOrWhiteSpace($EvidenceRoot)) {
    $EvidenceRoot = Join-Path $repository 'artifacts\analysis\t95-s6-native-reset-stop-observation-001-20260811-001'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$evidence = [IO.Path]::GetFullPath($EvidenceRoot)
if (Test-Path -LiteralPath $evidence) { throw "Refusing to overwrite existing observation directory: $evidence" }

$binary = Join-Path $build $BinaryName
$profile = Join-Path $build 'intact-native-runtime-profile.bochsrc'
$bios = Join-Path $build 'ROM\BIOS-bochs-latest'
$vga = Join-Path $build 'ROM\VGABIOS-lgpl-latest'
foreach ($path in @($binary, $bios, $vga)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing admitted r4 input: $path" }
}
if (($ProfileMode -eq 'r4-default') -and -not (Test-Path -LiteralPath $profile -PathType Leaf)) { throw "Missing admitted r4 profile: $profile" }

New-Item -ItemType Directory -Path (Join-Path $evidence 'ROM') -Force | Out-Null
$copiedProfile = Join-Path $evidence 'intact-native-runtime-profile.bochsrc'
if ($ProfileMode -eq 'r4-default') {
    Copy-Item -LiteralPath $profile -Destination $copiedProfile -ErrorAction Stop
} else {
    $profileText = @"
# T95 S6 pc-reset-floor-v1: original Bochs parser controls only.
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
    [IO.File]::WriteAllText($copiedProfile, $profileText, [Text.UTF8Encoding]::new($false))
}
Copy-Item -LiteralPath $bios -Destination (Join-Path $evidence 'ROM\BIOS-bochs-latest') -ErrorAction Stop
Copy-Item -LiteralPath $vga -Destination (Join-Path $evidence 'ROM\VGABIOS-lgpl-latest') -ErrorAction Stop

$stdoutPath = Join-Path $evidence 'stdout.log'
$stderrPath = Join-Path $evidence 'stderr.log'
$exitCodePath = Join-Path $evidence 'exitcode.txt'
$wrapperPath = Join-Path $evidence 'run-native-reset-observation.cmd'
$recordPath = Join-Path $evidence 'observation.json'
$command = "$BinaryName -q -f intact-native-runtime-profile.bochsrc -benchmark 1"

$wrapper = @"
@echo off
setlocal
set "NTDOS64_UD_TEST="
set "NTDOS64_ADAPTER_BRIDGE="
set "NTDOS64_ADAPTER_PROFILE="
set "NTDOS64_ADAPTER_ROOT="
cd /d "$evidence"
"$binary" -q -f intact-native-runtime-profile.bochsrc -benchmark 1 > "$stdoutPath" 2> "$stderrPath"
set "NTDOS64_EXITCODE=%ERRORLEVEL%"
> "$exitCodePath" echo %NTDOS64_EXITCODE%
exit /b %NTDOS64_EXITCODE%
"@
[IO.File]::WriteAllText($wrapperPath, $wrapper, [Text.UTF8Encoding]::new($false))
$startedUtc = [DateTime]::UtcNow
$process = Start-Process -FilePath $env:ComSpec -ArgumentList @('/d', '/c', $wrapperPath) -WindowStyle Hidden -PassThru
$completed = $process.WaitForExit($WatchdogSeconds * 1000)
$watchdogTerminated = $false
if (-not $completed) {
    $watchdogTerminated = $true
    $process.Kill()
    $process.WaitForExit()
}
$exitCode = if (Test-Path -LiteralPath $exitCodePath -PathType Leaf) { [int](Get-Content -LiteralPath $exitCodePath -Raw).Trim() } else { $process.ExitCode }

$logCandidates = @(Get-ChildItem -LiteralPath $evidence -File | Where-Object { $_.Name -notin @('stdout.log', 'stderr.log', 'exitcode.txt', 'run-native-reset-observation.cmd', 'intact-native-runtime-profile.bochsrc', 'observation.json') })
$logText = ((Get-Content -LiteralPath $stdoutPath -Raw), (Get-Content -LiteralPath $stderrPath -Raw), ($logCandidates | ForEach-Object { Get-Content -LiteralPath $_.FullName -Raw })) -join "`n"
$benchmarkObserved = $logText -match 'Bochs benchmark mode is ON'
$controlledStopObserved = $logText -match 'cpu loop quit, shutting down simulator'
$negativeTerms = @('NTDOS64_ADAPTER_PROFILE=', 'NTDOS64_ADAPTER_ROOT=', '--byob', 'ntio=', 'ntdos=', 'command=')
$commandHasForbiddenTerm = @($negativeTerms | Where-Object { $command -match [regex]::Escape($_) }).Count -ne 0
$record = [ordered]@{
    schema = 'ntdos64.t95-s6.native-reset-controlled-stop-observation.v1'
    buildRoot = $build
    evidenceRoot = $evidence
    profileMode = $ProfileMode
    command = $command
    processId = $process.Id
    startedUtc = $startedUtc.ToString('o')
    elapsedMilliseconds = [int](([DateTime]::UtcNow - $startedUtc).TotalMilliseconds)
    watchdogSeconds = $WatchdogSeconds
    watchdogTerminated = $watchdogTerminated
    exitCode = $exitCode
    binarySha256 = (Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash
    profileSha256 = (Get-FileHash -LiteralPath $copiedProfile -Algorithm SHA256).Hash
    biosSha256 = (Get-FileHash -LiteralPath (Join-Path $evidence 'ROM\BIOS-bochs-latest') -Algorithm SHA256).Hash
    vgaBiosSha256 = (Get-FileHash -LiteralPath (Join-Path $evidence 'ROM\VGABIOS-lgpl-latest') -Algorithm SHA256).Hash
    logFiles = @($logCandidates | ForEach-Object { $_.Name })
    benchmarkObserved = $benchmarkObserved
    controlledStopObserved = $controlledStopObserved
    commandHasForbiddenTerm = $commandHasForbiddenTerm
    removedEnvironmentVariables = @('NTDOS64_UD_TEST', 'NTDOS64_ADAPTER_BRIDGE', 'NTDOS64_ADAPTER_PROFILE', 'NTDOS64_ADAPTER_ROOT')
}
[IO.File]::WriteAllText($recordPath, ($record | ConvertTo-Json -Depth 4), [Text.UTF8Encoding]::new($false))
if ($watchdogTerminated -or -not $benchmarkObserved -or -not $controlledStopObserved -or $commandHasForbiddenTerm -or $exitCode -notin @(0, 1)) {
    throw "Native reset/controlled-stop observation did not meet its declared predicates: $recordPath"
}
Write-Host "Native reset/controlled-stop observation completed: $evidence"
