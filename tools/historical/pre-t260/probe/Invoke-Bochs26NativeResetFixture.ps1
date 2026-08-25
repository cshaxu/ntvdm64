[CmdletBinding()]
param(
    [string]$BuildRoot = '',
    # Stock Bochs firmware reaches the neutral 0000:7c00 probe at roughly
    # 13.2M ticks; the retained r18 evidence uses a 200M bounded stop. Keep
    # that proven ceiling available without turning this fixture into an
    # unbounded run.
    [ValidateRange(1, 200)]
    [int]$BenchmarkMillions = 1,
    [switch]$UdResumeFixture,
    [switch]$AdapterBridgeFixture,
    [switch]$AdapterRuntimeFixture,
    [switch]$AdapterMultiWriteFixture,
    [switch]$AdapterObservationFixture,
    [switch]$StockFirmwareBdaProbe,
    [switch]$StockFirmwareStartupSnapshotProbe,
    [switch]$StartupPlanFixture,
    [switch]$RealNtioStartupFixture
)

$ErrorActionPreference = 'Stop'
$repository = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $BuildRoot = Join-Path $repository 'artifacts\build\bochs-2.6-native-nogui-ucrt-r4'
}
$build = [IO.Path]::GetFullPath($BuildRoot)
$bochs = Join-Path $build $(if ($AdapterObservationFixture -or $AdapterMultiWriteFixture -or $StockFirmwareBdaProbe -or $StockFirmwareStartupSnapshotProbe -or $StartupPlanFixture -or $RealNtioStartupFixture) { 'runner-observation-fixture.exe' } else { 'bochs.exe' })
$bios = Join-Path $build 'bios\BIOS-bochs-latest'
$vgaBios = Join-Path $build 'bios\VGABIOS-lgpl-latest'
$fixtureDir = Join-Path $build 'runner-reset-fixture'
$config = Join-Path $fixtureDir 'bare-reset.bochsrc'
$log = Join-Path $fixtureDir 'bochs.log'
$stdout = Join-Path $fixtureDir 'stdout.log'
$stderr = Join-Path $fixtureDir 'stderr.log'

foreach ($path in @($bochs, $bios, $vgaBios)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing native-container fixture input: $path" }
}

New-Item -ItemType Directory -Path $fixtureDir -Force | Out-Null
$romForFixture = $bios
if ((@($UdResumeFixture, $AdapterBridgeFixture, $AdapterRuntimeFixture, $AdapterMultiWriteFixture, $AdapterObservationFixture, $StockFirmwareBdaProbe, $StockFirmwareStartupSnapshotProbe, $StartupPlanFixture, $RealNtioStartupFixture) | Where-Object { $_ }).Count -gt 1) {
    throw 'Choose only one #UD fixture mode.'
}
if ($UdResumeFixture -or $AdapterBridgeFixture -or $AdapterRuntimeFixture -or $AdapterMultiWriteFixture -or $AdapterObservationFixture) {
    $romForFixture = Join-Path $fixtureDir 'ud-resume-bios.bin'
    $rom = [byte[]]::new(131072)
    $resetVectorOffset = $rom.Length - 16
    if ($AdapterRuntimeFixture) {
        $rom[$resetVectorOffset + 0] = 0xbf # mov di, 08b0h
        $rom[$resetVectorOffset + 1] = 0xb0
        $rom[$resetVectorOffset + 2] = 0x08
        $rom[$resetVectorOffset + 3] = 0xc4
        $rom[$resetVectorOffset + 4] = 0xc4
        $rom[$resetVectorOffset + 5] = 0x50
        $rom[$resetVectorOffset + 6] = 0x11
        $rom[$resetVectorOffset + 7] = 0xf4
    } elseif ($AdapterMultiWriteFixture) {
        $rom[$resetVectorOffset + 0] = 0xb8 # mov ax, 0100h
        $rom[$resetVectorOffset + 1] = 0x00
        $rom[$resetVectorOffset + 2] = 0x01
        $rom[$resetVectorOffset + 3] = 0x8e # mov es, ax
        $rom[$resetVectorOffset + 4] = 0xc0
        $rom[$resetVectorOffset + 5] = 0xbd # mov bp, 0200h
        $rom[$resetVectorOffset + 6] = 0x00
        $rom[$resetVectorOffset + 7] = 0x02
        $rom[$resetVectorOffset + 8] = 0xc4
        $rom[$resetVectorOffset + 9] = 0xc4
        $rom[$resetVectorOffset + 10] = 0x50
        $rom[$resetVectorOffset + 11] = 0x46
        $rom[$resetVectorOffset + 12] = 0xf4
    } else {
        $rom[$resetVectorOffset + 0] = 0x0f
        $rom[$resetVectorOffset + 1] = 0x0b
        $rom[$resetVectorOffset + 2] = 0xf4
    }
[IO.File]::WriteAllBytes($romForFixture, $rom)
}
$bootDisk = ''
if ($StockFirmwareBdaProbe -or $StockFirmwareStartupSnapshotProbe) {
    $bootDisk = Join-Path $fixtureDir 'neutral-boot.img'
    $image = [byte[]]::new(1474560)
    $image[0] = 0x0f; $image[1] = 0x0b; $image[2] = 0xf4
    $image[510] = 0x55; $image[511] = 0xaa
    [IO.File]::WriteAllBytes($bootDisk, $image)
}
$pluginControl = if ($AdapterRuntimeFixture -or $AdapterMultiWriteFixture) { 'plugin_ctrl: unmapped=0, biosdev=0, speaker=0, extfpuirq=0, parallel=0, serial=0' } else { '' }
$floppyControl = if ($StockFirmwareBdaProbe -or $StockFirmwareStartupSnapshotProbe) { "floppya: 1_44=$($bootDisk.Replace('\', '/')), status=inserted" } else { '' }
$bootDevice = if ($StockFirmwareBdaProbe -or $StockFirmwareStartupSnapshotProbe) { 'floppy' } else { 'disk' }
$configText = @"
config_interface: textconfig
display_library: nogui
romimage: file=$($romForFixture.Replace('\', '/'))
vgaromimage: file=$($vgaBios.Replace('\', '/'))
megs: 4
boot: $bootDevice
log: $($log.Replace('\', '/'))
panic: action=fatal
error: action=report
info: action=report
debug: action=ignore
$pluginControl
$floppyControl
"@
[IO.File]::WriteAllText($config, $configText, [Text.UTF8Encoding]::new($false))

$start = [Diagnostics.ProcessStartInfo]::new()
$start.FileName = $bochs
$start.Arguments = "-q -f `"$config`" -benchmark $BenchmarkMillions"
$start.UseShellExecute = $false
$start.RedirectStandardOutput = $true
$start.RedirectStandardError = $true
$start.CreateNoWindow = $true
# Some inherited Windows environments contain both Path and PATH.  The legacy
# ProcessStartInfo collection rejects that duplicate on assignment; inheriting
# the existing environment is sufficient when this optional normalization fails.
try { $start.EnvironmentVariables['PATH'] = 'C:\msys64\ucrt64\bin;' + $env:PATH } catch { }
if ($UdResumeFixture) { $start.EnvironmentVariables['RUNNER_UD_TEST'] = '1' }
if ($AdapterBridgeFixture) { $start.EnvironmentVariables['RUNNER_ADAPTER_BRIDGE'] = '1' }
if ($AdapterRuntimeFixture -or $AdapterMultiWriteFixture -or $AdapterObservationFixture -or $StockFirmwareBdaProbe -or $StockFirmwareStartupSnapshotProbe -or $StartupPlanFixture -or $RealNtioStartupFixture) {
    $byobRoot = Join-Path $fixtureDir 'byob'
    New-Item -ItemType Directory -Path $byobRoot -Force | Out-Null
    foreach ($name in @('NTIO.SYS', 'NTDOS.SYS', 'COMMAND.COM')) {
        $bytes = if ($StartupPlanFixture -and $name -eq 'NTIO.SYS') { [byte[]](0xf4) } else { [byte[]](0x61, 0x62, 0x63) }
        [IO.File]::WriteAllBytes((Join-Path $byobRoot $name), $bytes)
    }
    $sha256 = 'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad'
    $profile = Join-Path $byobRoot 'profile.json'
    $profileText = "{`"schema`":`"runner-byob-profile-v1`",`"profile`":`"nt4-en-us-command-smoke-v1`",`"architecture`":`"x86`",`"locale`":`"en-US`",`"compatibility_group`":`"native-reset-fixture`",`"components`": [{`"role`":`"ntio`",`"file_name`":`"NTIO.SYS`",`"required`":true,`"bytes`":3,`"sha256`":`"$sha256`",`"version`":null},{`"role`":`"ntdos`",`"file_name`":`"NTDOS.SYS`",`"required`":true,`"bytes`":3,`"sha256`":`"$sha256`",`"version`":null},{`"role`":`"command`",`"file_name`":`"COMMAND.COM`",`"required`":true,`"bytes`":3,`"sha256`":`"$sha256`",`"version`":null}],`"features`":[],`"owner_note`":null}"
    if ($AdapterObservationFixture) {
        $profileText = $profileText.Substring(0, $profileText.Length - 1) + ',"machine_observations":[{"id":7,"physical_address":0,"byte_count":4}],"machine_observation_trigger":{"observation_id":7,"exception_vector":6,"instruction_bytes":"0f0b","resume_bytes":2}}'
    }
    if ($AdapterMultiWriteFixture) {
        $profileText = $profileText.Substring(0, $profileText.Length - 1) + ',"host_drive_inventory":{"types":[2,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}}'
    }
    if ($StockFirmwareBdaProbe) {
        $profileText = $profileText.Substring(0, $profileText.Length - 1) + ',"machine_observations":[{"id":7,"physical_address":1174,"byte_count":1}],"machine_observation_trigger":{"observation_id":7,"exception_vector":6,"instruction_bytes":"0f0b","resume_bytes":2}}'
    }
    if ($StockFirmwareStartupSnapshotProbe) {
        $profileText = $profileText.Substring(0, $profileText.Length - 1) + ',"machine_observations":[{"id":11,"physical_address":64,"byte_count":48},{"id":12,"physical_address":1174,"byte_count":1},{"id":13,"physical_address":1812,"byte_count":4},{"id":14,"physical_address":1043,"byte_count":2}],"machine_startup_snapshot_trigger":{"observation_ids":[11,12,13,14],"exception_vector":6,"instruction_bytes":"0f0b","resume_bytes":2},"machine_startup_snapshot_evidence_file":"startup-snapshot-evidence.json"}'
    }
    if ($StartupPlanFixture) {
        $haltSha256 = '2017ff3461395672aa0aa4f64894fd2f95a4b120e2690e8951656d79adc2eed2'
        $profileText = "{`"schema`":`"runner-byob-profile-v1`",`"profile`":`"nt4-en-us-command-smoke-v1`",`"architecture`":`"x86`",`"locale`":`"en-US`",`"compatibility_group`":`"native-plan-fixture`",`"components`": [{`"role`":`"ntio`",`"file_name`":`"NTIO.SYS`",`"required`":true,`"bytes`":1,`"sha256`":`"$haltSha256`",`"version`":null},{`"role`":`"ntdos`",`"file_name`":`"NTDOS.SYS`",`"required`":true,`"bytes`":3,`"sha256`":`"$sha256`",`"version`":null},{`"role`":`"command`",`"file_name`":`"COMMAND.COM`",`"required`":true,`"bytes`":3,`"sha256`":`"$sha256`",`"version`":null}],`"features`":[],`"owner_note`":null,`"machine_startup_plan`":true,`"machine_startup_entry`":`"ntio-v0`"}"
    }
    if ($RealNtioStartupFixture) {
        $sources = @{
            'NTIO.SYS' = Join-Path $repository 'artifacts\toolchain-runs\ntio-tools16-opennt-v1\base\mvdm\dos\v86\doskrnl\bios\NTIO.SYS'
            'NTDOS.SYS' = Join-Path $repository 'artifacts\toolchain-runs\ntdos-tools16-historical-v1\base\mvdm\dos\v86\doskrnl\dos\NTDOS.SYS'
            'COMMAND.COM' = Join-Path $repository 'artifacts\toolchain-runs\command-tools16-opennt-v1\base\mvdm\dos\v86\cmd\command\COMMAND.COM'
        }
        foreach ($name in $sources.Keys) {
            if (-not (Test-Path -LiteralPath $sources[$name] -PathType Leaf)) { throw "Missing source-built fixture input: $($sources[$name])" }
            Copy-Item -LiteralPath $sources[$name] -Destination (Join-Path $byobRoot $name) -Force
        }
        $profileText = '{"schema":"runner-byob-profile-v1","profile":"nt4-en-us-command-smoke-v1","architecture":"x86","locale":"en-US","compatibility_group":"source-built-ntio-prefix-fixture","components":[{"role":"ntio","file_name":"NTIO.SYS","required":true,"bytes":33792,"sha256":"cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937","version":null},{"role":"ntdos","file_name":"NTDOS.SYS","required":true,"bytes":27858,"sha256":"957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84","version":null},{"role":"command","file_name":"COMMAND.COM","required":true,"bytes":50384,"sha256":"908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43","version":null}],"features":[],"owner_note":null,"machine_startup_plan":true,"machine_startup_entry":"ntio-v0","host_drive_inventory":{"types":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}}'
    }
    [IO.File]::WriteAllText($profile, $profileText, [Text.UTF8Encoding]::new($false))
    $start.EnvironmentVariables['RUNNER_ADAPTER_BRIDGE'] = '1'
    $start.EnvironmentVariables['RUNNER_ADAPTER_PROFILE'] = $profile
    $start.EnvironmentVariables['RUNNER_ADAPTER_ROOT'] = $byobRoot
}

$process = [Diagnostics.Process]::new()
$process.StartInfo = $start
if (-not $process.Start()) { throw "Unable to start native reset fixture: $bochs" }
$standardOutput = $process.StandardOutput.ReadToEnd()
$standardError = $process.StandardError.ReadToEnd()
$process.WaitForExit()
[IO.File]::WriteAllText($stdout, $standardOutput, [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($stderr, $standardError, [Text.UTF8Encoding]::new($false))
$exitCode = $process.ExitCode

$benchmarkObserved = Select-String -LiteralPath $log -Pattern 'Bochs benchmark mode is ON' -Quiet
$controlledStopObserved = Select-String -LiteralPath $log -Pattern 'cpu loop quit, shutting down simulator' -Quiet
if (-not $benchmarkObserved -or -not $controlledStopObserved) {
    throw "Native reset fixture did not reach its declared benchmark stop; see $log"
}
if ($UdResumeFixture -and -not (Select-String -LiteralPath $log -Pattern 'runner #UD test interceptor resumed execution' -Quiet)) {
    throw "#UD resume fixture did not record its neutral callback; see $log"
}
if ($AdapterBridgeFixture -and -not (Select-String -LiteralPath $log -Pattern 'runner adapter CPU state snapshot passed through' -Quiet)) {
    throw "Adapter bridge fixture did not record state snapshot pass-through; see $log"
}
if ($AdapterRuntimeFixture -and -not (Select-String -LiteralPath $log -Pattern 'runner adapter transaction committed' -Quiet)) {
    throw "Adapter runtime fixture did not record a generic committed transaction; see $log"
}
if ($AdapterMultiWriteFixture -and -not (Select-String -LiteralPath $log -Pattern 'runner adapter multi-write transaction committed writes=4 bytes=c' -Quiet)) {
    throw "Adapter multi-write fixture did not record the generic committed transaction; see $log"
}
if ($AdapterObservationFixture -and -not (Select-String -LiteralPath $log -Pattern 'runner adapter observation copied' -Quiet)) {
    throw "Adapter observation fixture did not record a generic ordinary-RAM copy; see $log"
}
if ($StockFirmwareBdaProbe -and -not (Select-String -LiteralPath $log -Pattern 'runner adapter observation copied digest=' -Quiet)) {
    throw "Stock-firmware BDA probe did not record a completed observation digest; see $log"
}
if ($StockFirmwareStartupSnapshotProbe -and -not (Select-String -LiteralPath $log -Pattern 'runner adapter snapshot copied digest=' -Quiet)) {
    throw "Stock-firmware startup snapshot probe did not record a completed snapshot digest; see $log"
}
if ($StockFirmwareStartupSnapshotProbe -and -not (Test-Path -LiteralPath (Join-Path $byobRoot 'startup-snapshot-evidence.json') -PathType Leaf)) {
    throw "Stock-firmware startup snapshot probe did not write its declared evidence record"
}
if ($StartupPlanFixture -and -not (Select-String -LiteralPath $log -Pattern 'requested execution plan applied' -Quiet)) {
    throw "Startup-plan fixture did not record the one pre-loop plan application; see $log"
}
# Bochs 2.6 nogui reports exit code 1 after its native GUI exit stub even after
# the benchmark timer has stopped the CPU loop. The log predicates above make
# that result distinguishable from a startup or configuration failure.
if ($exitCode -notin @(0, 1)) { throw "Native reset fixture failed with exit code $exitCode; see $stdout and $stderr" }

Write-Host "Native reset fixture completed: $fixtureDir"
