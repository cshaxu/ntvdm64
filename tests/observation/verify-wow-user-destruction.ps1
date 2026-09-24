param(
    [Parameter(Mandatory = $true)] [string]$BuildRoot,
    [Parameter(Mandatory = $true)] [string]$RuntimeRoot,
    [Parameter(Mandatory = $true)] [string]$Wow32Provider,
    [ValidateRange(1, 120)] [int]$WindowTimeoutSeconds = 12,
    [ValidateRange(1, 3)] [int]$LaunchCount = 1,
    [switch]$OverlapFirst,
    [switch]$ReactivateFirst,
    [string]$ThreadSnapshotObserver,
    [string]$DiagnosticObserver,
    [string]$GitExecutable = 'git.exe'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
function Read-TraceSuffix([string]$Path, [int]$Offset = 0) {
    if (!(Test-Path -LiteralPath $Path)) {
        if ($Offset -ne 0) { throw "Trace disappeared after checkpoint: $Path" }
        return ''
    }
    # Keep one shared read handle: the producer appends while observation runs.
    # A short/replaced trace must fail acceptance, never replay an earlier task.
    $stream = [IO.File]::Open($Path, [IO.FileMode]::Open,
        [IO.FileAccess]::Read, [IO.FileShare]::ReadWrite)
    $reader = [IO.StreamReader]::new($stream)
    try { $text = $reader.ReadToEnd() }
    finally { $reader.Dispose() }
    if ($Offset -lt 0 -or $Offset -gt $text.Length) {
        throw "Trace checkpoint invalid: offset=$Offset length=$($text.Length) path=$Path"
    }
    return $text.Substring($Offset)
}
$build = (Resolve-Path -LiteralPath $BuildRoot).Path
$runtime = (Resolve-Path -LiteralPath $RuntimeRoot).Path
$provider = (Resolve-Path -LiteralPath $Wow32Provider).Path
if ($ThreadSnapshotObserver) {
    $ThreadSnapshotObserver = (Resolve-Path -LiteralPath $ThreadSnapshotObserver).Path
}
if (($OverlapFirst -or $ReactivateFirst) -and ($LaunchCount -ne 2 -or $DiagnosticObserver)) {
    throw 'Concurrent observation requires exactly two direct run16 launches.'
}
if ($OverlapFirst -and $ReactivateFirst) { throw 'Select two-window or single-instance observation, not both.' }
if ($DiagnosticObserver -and $LaunchCount -ne 1) {
    throw 'Worker-reuse observation requires direct run16 launches, not the debugger wrapper.'
}
$artifacts = @('run16.exe','basesrv.exe','ntvdm.exe','dtmgr.exe','VDMREDIR.dll')
# Reject incomplete historical/build artifacts before touching the runtime.
foreach ($artifact in @($artifacts | ForEach-Object { Join-Path $build $_ }) + @($provider)) {
    $stream = [IO.File]::OpenRead($artifact)
    try {
        $reader = [IO.BinaryReader]::new($stream)
        if ($stream.Length -lt 64 -or $reader.ReadUInt16() -ne 0x5a4d) {
            throw "Invalid PE artifact: $artifact"
        }
        $stream.Position = 0x3c
        $peOffset = $reader.ReadUInt32()
        if ($peOffset -gt $stream.Length - 6) { throw "Invalid PE offset: $artifact" }
        $stream.Position = $peOffset
        if ($reader.ReadUInt32() -ne 0x4550 -or $reader.ReadUInt16() -ne 0x14c) {
            throw "Expected x86 PE artifact: $artifact"
        }
    }
    finally { $stream.Dispose() }
}
$profile = Join-Path $runtime 'SYSTEM.INI'
$runId = 't422-s2-' + [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssfffZ') + '-' +
    [Guid]::NewGuid().ToString('N').Substring(0,8) + '-window-lifecycle'
$logs = Join-Path (Join-Path $runtime 'logs') $runId
if (Test-Path -LiteralPath $logs) { throw "Run ID already exists: $runId" }
$registrationTrace = Join-Path $logs 'registration.log'
$windowTrace = Join-Path $logs 't422-s2-wow-destroy-window.log'
$callbackTrace = Join-Path $logs 't422-s2-wow-destroy-callback.log'
$frameTrace = Join-Path $logs 't422-s2-wow-destroy-frame.log'
$descriptorTrace = Join-Path $logs 't422-s2-wow-destroy-set-descriptor.log'
$descriptorEntryTrace = Join-Path $logs 't422-s2-wow-destroy-set-descriptor-entry.log'
$dpmiTrace = Join-Path $logs 't422-s2-wow-destroy-dpmi-bop.log'
$swintTrace = Join-Path $logs 't422-s2-wow-destroy-dpmi-swint.log'
$iretTrace = Join-Path $logs 't422-s2-wow-destroy-dpmi-iret16.log'
$iretStateTrace = Join-Path $logs 't422-s2-wow-destroy-dpmi-iret16-state.log'
$allocselTrace = Join-Path $logs 't422-s2-wow-destroy-dpmi-allocsel.log'
$setDescriptorInstructionTrace = Join-Path $logs 't422-s2-wow-destroy-dpmi-setdescriptor-instruction.log'
$getSelTrace = Join-Path $logs 't422-s2-wow-destroy-krnl-getsel.log'
$longptraddTrace = Join-Path $logs 't422-s2-wow-destroy-longptradd.log'
$cpuTrace = Join-Path $logs 't422-s2-wow-destroy-cpu.log'
$classTrace = Join-Path $logs 't422-s2-wow-destroy-class-publication.log'
$vxdTrace = Join-Path $logs 't422-s2-wow-destroy-vxd.log'
$wowBopTrace = Join-Path $logs 't422-s2-wow-destroy-wow-bop.log'
$callbackCpuSelector = if ($env:MVDM_S2_CALLBACK_CPU_SELECTOR) {
    $env:MVDM_S2_CALLBACK_CPU_SELECTOR
} else {
    '83B7'
}

Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class T422NativeWindow {
  [DllImport("kernel32.dll")] public static extern uint GetACP();
  public delegate bool EnumChild(IntPtr window, IntPtr parameter);
  [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr window, EnumChild callback, IntPtr parameter);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr window, System.Text.StringBuilder text, int count);
  [DllImport("user32.dll", SetLastError=true)] public static extern bool IsWindow(IntPtr window);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr window, out uint process);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr window);
  [DllImport("user32.dll")] public static extern bool IsHungAppWindow(IntPtr window);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr window, System.Text.StringBuilder text, int count);
  [DllImport("user32.dll", SetLastError=true)] public static extern IntPtr SendMessageTimeout(
    IntPtr window, uint message, IntPtr wparam, IntPtr lparam, uint flags,
    uint timeout, out IntPtr result);
}
'@

# The pinned Chinese guest supplies CP936 bytes to the original ANSI class
# registration path. Identify exactly those bytes under the host ACP too;
# this is identity for destruction testing, not localization acceptance.
$nativeMineClass = [Text.Encoding]::GetEncoding([int][T422NativeWindow]::GetACP()).GetString(
    [byte[]]@(0xc9, 0xa8, 0xc0, 0xd7))

function Set-ControlledProfile([byte[]]$original) {
    $text = [Text.Encoding]::Default.GetString($original)
    $text = [regex]::Replace($text, '(?im)^network\.drv\s*=.*(?:\r?\n|$)', '')
    if ($text -notmatch '(?im)^\[boot\]\s*\r?$') { $text += "`r`n[boot]`r`n" }
    $boot = [regex]::new('(?im)^(\[boot\]\s*\r?\n)')
    $text = $boot.Replace($text,
        "`$1; T422 S2 controlled real-window destruction observation`r`nnetwork.drv=wfwnet.drv`r`n", 1)
    [IO.File]::WriteAllText($profile, $text, [Text.Encoding]::Default)
}

# The observer owns only this deployment, not same-named executables in other
# checkouts. Retain PID/start-time identity and report state before forced
# cleanup; launcher completion alone is not proof of worker retirement.
function Get-RuntimeProcesses {
    foreach ($candidate in @(Get-Process ntvdm,basesrv,run16 -ErrorAction SilentlyContinue)) {
        try {
            $expected = Join-Path $runtime ($candidate.ProcessName + '.exe')
            if ([string]::Equals($candidate.Path, $expected,
                    [StringComparison]::OrdinalIgnoreCase)) {
                $candidate
            }
        } catch {
            if (!$candidate.HasExited) { throw }
        }
    }
}

function Stop-RuntimeProcesses {
    foreach ($candidate in @(Get-RuntimeProcesses)) {
        if (!$candidate.HasExited) {
            # Use the observed Process object rather than selecting by name
            # again. Other projects' workers and brokers are never targets.
            try { Stop-Process -InputObject $candidate -Force -ErrorAction Stop }
            catch { if (!$candidate.HasExited) { throw } }
        }
    }
}

function Write-RuntimeState([string]$name) {
    $state = @(Get-RuntimeProcesses | ForEach-Object {
        [ordered]@{pid=$_.Id; started_utc=$_.StartTime.ToUniversalTime().ToString('o');
            path=$_.Path; cpu_seconds=$_.CPU}
    })
    ConvertTo-Json -InputObject $state -Depth 4 |
        Set-Content -LiteralPath (Join-Path $logs $name)
}

New-Item -ItemType Directory -Force -Path $logs | Out-Null
$originalProfile = [IO.File]::ReadAllBytes($profile)
$originalHash = (Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash
$process = $null
$verdict = 'INCOMPLETE'
Write-Output "run_id=$runId logs=$logs"
[ordered]@{
    run_id=$runId; created_utc=[DateTime]::UtcNow.ToString('o')
    question='Real WINMINE visible-window destruction and subsequent task cleanup'
    evidence_kind='diagnostic guest; complete build/source snapshot not yet sealed'
    source_head=(& $GitExecutable -C (Join-Path $PSScriptRoot '../..') rev-parse HEAD)
    observer_sha256=(Get-FileHash -LiteralPath $PSCommandPath).Hash
    original_profile_sha256=$originalHash
    command='run16.exe WINMINE.EXE'; cwd=$runtime
    debugger=[bool]$DiagnosticObserver; timeout_seconds=$WindowTimeoutSeconds
    launch_count=$LaunchCount; require_same_worker=($LaunchCount -gt 1)
    overlapping_tasks=[bool]$OverlapFirst
    single_instance_reactivation=[bool]$ReactivateFirst
    thread_snapshot_observer=$ThreadSnapshotObserver
    thread_snapshot_sha256=if ($ThreadSnapshotObserver) {
        (Get-FileHash -LiteralPath $ThreadSnapshotObserver -Algorithm SHA256).Hash
    } else { $null }
    build_identity='INCOMPLETE: incremental build cache, not a frozen input snapshot'
    source_identity='INCOMPLETE: dirty source input set not sealed'
} | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $logs 'manifest.json')
try {
    Stop-RuntimeProcesses
    foreach ($name in $artifacts) {
        $source = Join-Path $build $name
        $destination = Join-Path $runtime $name
        if (!(Test-Path -LiteralPath $destination) -or
                (Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $destination).Hash) {
            Copy-Item -LiteralPath $source -Destination $destination -Force
        }
    }
    Copy-Item -LiteralPath $provider -Destination (Join-Path $runtime 'WOW32.DLL') -Force
    @($artifacts + @('WOW32.DLL','WINMINE.EXE','SYSTEM.INI') | ForEach-Object {
        $path = Join-Path $runtime $_
        [ordered]@{path=$path; size=(Get-Item -LiteralPath $path).Length;
            sha256=(Get-FileHash -LiteralPath $path).Hash}
    }) | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $logs 'inputs-before.json')
    $env:MVDM_WOW_REGISTRATION_TRACE_PATH = $registrationTrace
    $env:MVDM_S34_TRACE_PATH = Join-Path $logs 'basesrv-command.log'
    $env:MVDM_WOW_DISPATCH_TRACE_PATH = Join-Path $logs 'dispatch.log'
    $env:MVDM_WOW_NT_TRACE_PATH = Join-Path $logs 'nt-transition.log'
    Set-ControlledProfile $originalProfile
    Remove-Item -LiteralPath $windowTrace,$callbackTrace,$frameTrace,$descriptorTrace,$descriptorEntryTrace,$dpmiTrace,$swintTrace,$iretTrace,$iretStateTrace,$allocselTrace,$setDescriptorInstructionTrace,$getSelTrace,$longptraddTrace,$cpuTrace,$classTrace,$vxdTrace,$wowBopTrace -Force -ErrorAction SilentlyContinue
    $env:MVDM_WOW_WINDOW_TRACE_PATH = $windowTrace
    $env:MVDM_WOW_CALLBACK_TRACE_PATH = $callbackTrace
    $env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH = $frameTrace
    # The original WOW 04F2h service writes its shared LDT entries through
    # BOP 53:00.  Keep its existing default-off witness enabled for this
    # controlled integration observation, so a failed callback still records
    # whether the selector publication actually reached the CPU provider.
    $env:MVDM_S42_DPMI_SET_DESCRIPTOR_REPORT_PATH = $descriptorTrace
    $env:MVDM_S42_DPMI_SET_DESCRIPTOR_ENTRY_REPORT_PATH = $descriptorEntryTrace
    $env:MVDM_S42_DPMI_BOP_REPORT_PATH = $dpmiTrace
    $env:MVDM_S42_DPMI_SWINT_REPORT_PATH = $swintTrace
    $env:MVDM_S42_DPMI_IRET16_REPORT_PATH = $iretTrace
    # Share the CPU trace with the return-frame witness so their file order
    # is the observed execution order rather than a cross-file inference.
    $env:MVDM_S42_DPMI_IRET16_STATE_REPORT_PATH = $cpuTrace
    $env:MVDM_S42_DPMI_ALLOCSEL_TRACE_PATH = $allocselTrace
    $env:MVDM_S42_DPMI_SETDESCRIPTOR_TRACE_PATH = $setDescriptorInstructionTrace
    $env:MVDM_S42_KRNL_GETSEL_TRACE_PATH = $getSelTrace
    $env:MVDM_S42_WOW_LONGPTRADD_TRACE_PATH = $longptraddTrace
    $env:MVDM_WOW_CALLBACK_CPU_TRACE_PATH = $cpuTrace
    $env:MVDM_WOW_CALLBACK_CPU_SELECTOR = $callbackCpuSelector
    # Class publication is the source-side witness for the exact 16:16
    # procedure that later reaches W32Win16WndProcEx.  It is observation only.
    $env:MVDM_WOW_CLASS_PUBLICATION_TRACE_PATH = $classTrace
    $env:MVDM_S42_VXD_BOP_REPORT_PATH = $vxdTrace
    $env:MVDM_S41_WOW_BOP_REPORT_PATH = $wowBopTrace
    $expectedWorkerId = 0
    $expectedWorkerStart = $null
    # Record only aggregate shape, never environment values or secrets. A
    # caller-supplied synthetic padding variable makes size experiments
    # distinguishable from ordinary inherited-environment runs.
    $environmentItems = @(Get-ChildItem Env:)
    $environmentLengths = @($environmentItems | ForEach-Object {
        $_.Name.Length + $_.Value.Length + 2
    })
    $paddingValue = [Environment]::GetEnvironmentVariable('NTVDM_ENV_SIZE_PROBE')
    [ordered]@{
        entry_count=$environmentItems.Count
        utf16_character_count=(1 + ($environmentLengths | Measure-Object -Sum).Sum)
        largest_entry_characters=($environmentLengths | Measure-Object -Maximum).Maximum
        synthetic_padding_characters=if ($null -eq $paddingValue) { 0 } else { $paddingValue.Length }
        limitation='Aggregate shape only; this does not identify or seal the environment values'
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $logs 'environment-shape.json')
    $heldWindow = [IntPtr]::Zero
    $heldLauncher = $null
    $holdFirst = $OverlapFirst -or $ReactivateFirst
    $roundCount = if ($holdFirst) { 3 } else { $LaunchCount }
    for ($launch = 1; $launch -le $roundCount; ++$launch) {
    # Each launch must supply new trace evidence. Prior destruction callbacks
    # cannot validate a subsequent application, even if HWND values are reused.
    $windowTraceOffset = (Read-TraceSuffix $windowTrace).Length
    $callbackTraceOffset = (Read-TraceSuffix $callbackTrace).Length
    if ($holdFirst -and $launch -eq 3) {
        $window = $heldWindow
        $process = $heldLauncher
        if ($process.HasExited -or ![T422NativeWindow]::IsWindow($window)) {
            throw 'The first task did not survive completion of the second task.'
        }
    } else {
    if ($DiagnosticObserver) {
        $observerLog = Join-Path $logs 't422-s2-wow-destroy-native-exception.log'
        $process = Start-Process -FilePath $DiagnosticObserver -ArgumentList @(
            ('"' + $runtime + '"'), 'WINMINE.EXE', ('"' + $observerLog + '"')) `
            -WorkingDirectory $runtime -WindowStyle Hidden -PassThru
    } else {
        $process = Start-Process -FilePath (Join-Path $runtime 'run16.exe') -ArgumentList 'WINMINE.EXE' `
            -WorkingDirectory $runtime -WindowStyle Normal -PassThru
    }
    if ($ReactivateFirst -and $launch -eq 2) {
        if (!$process.WaitForExit(20000) -or $process.ExitCode -ne 0) {
            throw 'The second single-instance invocation did not complete successfully.'
        }
        $heldLauncher.Refresh()
        if ($heldLauncher.HasExited -or ![T422NativeWindow]::IsWindowVisible($heldWindow)) {
            throw 'The original application did not survive single-instance reactivation.'
        }
        Write-RuntimeState 'processes-after-reactivation.json'
        Write-Output "WOW_REACTIVATION_RETURN_OK launcher=$($process.Id) held=$heldWindow"
        continue
    }
    $deadline = [DateTime]::UtcNow.AddSeconds($WindowTimeoutSeconds)
    $window = [IntPtr]::Zero
    $observedClasses = [Collections.Generic.HashSet[string]]::new()
    while ([DateTime]::UtcNow -lt $deadline) {
        if (Test-Path -LiteralPath $windowTrace) {
            $matches = [regex]::Matches((Read-TraceSuffix $windowTrace $windowTraceOffset),
                '(?m)WindowDispatch hwnd=([0-9A-F]+) message=0001')
            foreach ($match in $matches) {
                $candidate = [IntPtr]::new([Convert]::ToInt64($match.Groups[1].Value, 16))
                $candidateClass = [Text.StringBuilder]::new(256)
                [void][T422NativeWindow]::GetClassName($candidate, $candidateClass, $candidateClass.Capacity)
                if ($candidateClass.Length) {
                    [void]$observedClasses.Add(('hwnd={0:X8} class={1}' -f
                        $candidate.ToInt64(), $candidateClass.ToString()))
                }
                # WOWEXEC's startup and helper windows are not application
                # acceptance. Select the immutable WINMINE class explicitly.
                if ([T422NativeWindow]::IsWindow($candidate) -and
                        $candidateClass.ToString() -in @('Minesweeper', '扫雷', $nativeMineClass)) {
                    $window = $candidate
                    break
                }
            }
            if ($window -ne [IntPtr]::Zero) { break }
        }
        Start-Sleep -Milliseconds 100
    }
    if ($window -eq [IntPtr]::Zero -or ![T422NativeWindow]::IsWindow($window)) {
        $failure = [Collections.Generic.List[string]]::new()
        foreach ($observed in $observedClasses) { $failure.Add($observed) }
        $process.Refresh()
        $failure.Add("run16 exited=$($process.HasExited)")
        if ($process.HasExited) { $failure.Add("run16 exitCode=$($process.ExitCode)") }
        foreach ($worker in @(Get-Process ntvdm -ErrorAction SilentlyContinue)) {
            $failure.Add("worker pid=$($worker.Id) cpu=$($worker.CPU) title=$($worker.MainWindowTitle)")
            if ($worker.MainWindowHandle -ne [IntPtr]::Zero) {
                $callback = [T422NativeWindow+EnumChild]{
                    param($handle, $parameter)
                    $text = [Text.StringBuilder]::new(2048)
                    [void][T422NativeWindow]::GetWindowText($handle, $text, $text.Capacity)
                    if ($text.Length) { $failure.Add($text.ToString()) }
                    return $true
                }
                [void][T422NativeWindow]::EnumChildWindows($worker.MainWindowHandle, $callback, [IntPtr]::Zero)
            }
        }
        $failure | Set-Content -LiteralPath (Join-Path $logs 't422-s2-wow-destroy-failure.log')
        $failure | Write-Output
        throw 'No live real WOW window became available for controlled destruction.'
    }
    }
    $result = [IntPtr]::Zero
    $deadline = [DateTime]::UtcNow.AddSeconds(10)
    while ([DateTime]::UtcNow -lt $deadline -and [T422NativeWindow]::IsWindow($window) -and
            ![T422NativeWindow]::IsWindowVisible($window)) {
        Start-Sleep -Milliseconds 100
    }
    if (![T422NativeWindow]::IsWindowVisible($window)) {
        throw 'The real WINMINE window was created but did not become visible.'
    }
    $windowOwner = [uint32]0
    [void][T422NativeWindow]::GetWindowThreadProcessId($window, [ref]$windowOwner)
    $workerIdentity = Get-Process -Id $windowOwner -ErrorAction Stop
    if ($launch -eq 1) {
        $expectedWorkerId = $windowOwner
        $expectedWorkerStart = $workerIdentity.StartTime
    } elseif ($windowOwner -ne $expectedWorkerId -or
            $workerIdentity.StartTime -ne $expectedWorkerStart) {
        throw 'The next application used a different worker; shared-worker reuse is not proved.'
    }
    $windowTitle = [Text.StringBuilder]::new(1024)
    [void][T422NativeWindow]::GetWindowText($window, $windowTitle, $windowTitle.Capacity)
    $closeLog = Join-Path $logs ("t422-s2-wow-close-state-$launch.log")
    $windowClass = [Text.StringBuilder]::new(256)
    [void][T422NativeWindow]::GetClassName($window, $windowClass, $windowClass.Capacity)
    "before hwnd=$window owner=$windowOwner visible=$([T422NativeWindow]::IsWindowVisible($window)) hung=$([T422NativeWindow]::IsHungAppWindow($window)) class=$windowClass title=$windowTitle" | Set-Content -LiteralPath $closeLog
    if ($holdFirst -and $launch -eq 1) {
        $heldWindow = $window
        $heldLauncher = $process
        Write-Output "WOW_TASK_HELD worker=$windowOwner launcher=$($process.Id) hwnd=$window"
        continue
    }
    if ($OverlapFirst -and $launch -eq 2) {
        $heldLauncher.Refresh()
        if ($window -eq $heldWindow -or $heldLauncher.HasExited -or
                ![T422NativeWindow]::IsWindowVisible($heldWindow)) {
            throw 'Two distinct live tasks were not present simultaneously.'
        }
        Write-RuntimeState 'processes-with-two-live-tasks.json'
    }
    if ([T422NativeWindow]::SendMessageTimeout($window, 0x0010, [IntPtr]::Zero,
            [IntPtr]::Zero, 0x0002, 5000, [ref]$result) -eq [IntPtr]::Zero) {
        throw "WM_CLOSE delivery failed: $([Runtime.InteropServices.Marshal]::GetLastWin32Error())"
    }
    $deadline = [DateTime]::UtcNow.AddSeconds(8)
    while ([DateTime]::UtcNow -lt $deadline -and [T422NativeWindow]::IsWindow($window)) {
        Start-Sleep -Milliseconds 100
    }
    if ([T422NativeWindow]::IsWindow($window)) { throw 'WM_CLOSE left the real WOW window live.' }
    $callbacks = if (Test-Path -LiteralPath $callbackTrace) {
        Read-TraceSuffix $callbackTrace $callbackTraceOffset
    } else { '' }
    $process.Refresh()
    "after exists=$([T422NativeWindow]::IsWindow($window)) launcherExited=$($process.HasExited) workerAlive=$([bool](Get-Process -Id $windowOwner -ErrorAction SilentlyContinue)) result=$result" | Add-Content -LiteralPath $closeLog
    if ($process.HasExited) { "launcherExitCode=$($process.ExitCode)" | Add-Content -LiteralPath $closeLog }
    $completed = $process.WaitForExit(15000)
    Write-RuntimeState "processes-after-launcher-grace-$launch.json"
    "afterGrace launcherExited=$completed" | Add-Content -LiteralPath $closeLog
    if ($completed) { "launcherExitCode=$($process.ExitCode)" | Add-Content -LiteralPath $closeLog }
    foreach ($message in '0002','0082') {
        $nativeDispatch = Read-TraceSuffix $windowTrace $windowTraceOffset
        $identity = 'WindowDispatch hwnd=' + ('{0:X8}' -f $window.ToInt64()) + ' message=' + $message
        if (!$nativeDispatch.Contains($identity)) {
            throw "The selected WINMINE HWND did not complete destruction message $message."
        }
        if ($callbacks -notmatch ("W32WndProc message=" + $message + ' .* success=1')) {
            throw "Original CallBack16 did not complete WM_$message during real destruction."
        }
    }
    $faultLog = if (Test-Path -LiteralPath $cpuTrace) { Get-Content -LiteralPath $cpuTrace -Raw } else { '' }
    $bopLog = if (Test-Path -LiteralPath $dpmiTrace) { Get-Content -LiteralPath $dpmiTrace -Raw } else { '' }
    if ($faultLog -match 'dpmi-fault vector=0a\b' -and $bopLog -match 'dpmi-bop index=18\b') {
        $verdict = 'FAIL: window destruction completed but guest reached fatal DPMI exception 0A'
        throw 'Guest reached fatal DPMI exception 0A; window destruction is not normal task exit.'
    }
    if (!$completed -or $process.ExitCode -ne 0) {
        $verdict = 'FAIL: visible-window destruction did not produce successful launcher completion'
        throw 'WINMINE closed but run16 did not complete successfully.'
    }
    $taskState = Get-Content -LiteralPath $windowTrace -Raw
    if ($taskState -match '(?m)^\d+ TaskFailure ') {
        $verdict = 'FAIL: USER task failure observed despite successful launcher completion'
        throw 'A USER task failed during the observation; launcher zero is insufficient.'
    }
    Write-Output "WOW_LAUNCH_OK launch=$launch worker=$windowOwner launcher=$($process.Id)"
    }
    $verdict = if ($ReactivateFirst) {
        'PASS: first window survives second-invocation completion and subsequently closes; full activation/cleanup remains separately audited'
    } else {
        "PASS: $LaunchCount visible-window launch/close cycles and launcher completions; full resource cleanup remains separately audited"
    }
    Write-Output 'T422_S2_WOW_USER_REAL_DESTRUCTION_OK'
}
catch {
    $verdict = 'FAIL: ' + $_.Exception.Message
    if ($ThreadSnapshotObserver) {
        foreach ($worker in @(Get-Process ntvdm -ErrorAction SilentlyContinue)) {
            if ($worker.Path -eq (Join-Path $runtime 'ntvdm.exe')) {
                & $ThreadSnapshotObserver $worker.Id > (Join-Path $logs ("worker-$($worker.Id)-threads.txt"))
                if ($LASTEXITCODE) { Write-Warning "Thread snapshot failed: $LASTEXITCODE" }
            }
        }
    }
    throw
}
finally {
    if ($DiagnosticObserver -and $process -and !$process.HasExited) {
        Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
    }
    try {
        Write-RuntimeState 'processes-before-forced-cleanup.json'
        Stop-RuntimeProcesses
    } finally {
        [IO.File]::WriteAllBytes($profile, $originalProfile)
    }
    if ((Get-FileHash -LiteralPath $profile -Algorithm SHA256).Hash -ne $originalHash) {
        throw 'SYSTEM.INI baseline was not restored byte-identically.'
    }
    [ordered]@{run_id=$runId; ended_utc=[DateTime]::UtcNow.ToString('o');
        verdict=$verdict; evidence_complete=$false; profile_restored=$true;
        cleanup='exact runtime-path run16/ntvdm/basesrv force-stopped after observation; pre-cleanup state recorded';
        limitation='No sealed build/source snapshot or rendered-content verification'
    } | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $logs 'result.json')
    Get-ChildItem -LiteralPath $logs -File | ForEach-Object {
        '{0}  {1}' -f (Get-FileHash -LiteralPath $_.FullName).Hash, $_.Name
    } | Set-Content -LiteralPath (Join-Path $logs 'checksums.sha256')
    Remove-Item Env:MVDM_WOW_REGISTRATION_TRACE_PATH -ErrorAction SilentlyContinue
    Remove-Item Env:MVDM_S34_TRACE_PATH -ErrorAction SilentlyContinue
    Remove-Item Env:MVDM_WOW_DISPATCH_TRACE_PATH -ErrorAction SilentlyContinue
    Remove-Item Env:MVDM_WOW_NT_TRACE_PATH -ErrorAction SilentlyContinue
    Remove-Item Env:MVDM_WOW_WINDOW_TRACE_PATH,Env:MVDM_WOW_CALLBACK_TRACE_PATH,Env:MVDM_WOW_CALLBACK_FRAME_TRACE_PATH,Env:MVDM_S42_DPMI_SET_DESCRIPTOR_REPORT_PATH,Env:MVDM_S42_DPMI_SET_DESCRIPTOR_ENTRY_REPORT_PATH,Env:MVDM_S42_DPMI_BOP_REPORT_PATH,Env:MVDM_S42_DPMI_SWINT_REPORT_PATH,Env:MVDM_S42_DPMI_IRET16_REPORT_PATH,Env:MVDM_S42_DPMI_IRET16_STATE_REPORT_PATH,Env:MVDM_S42_DPMI_ALLOCSEL_TRACE_PATH,Env:MVDM_S42_DPMI_SETDESCRIPTOR_TRACE_PATH,Env:MVDM_S42_KRNL_GETSEL_TRACE_PATH,Env:MVDM_S42_WOW_LONGPTRADD_TRACE_PATH,Env:MVDM_WOW_CALLBACK_CPU_TRACE_PATH,Env:MVDM_WOW_CALLBACK_CPU_SELECTOR,Env:MVDM_WOW_CLASS_PUBLICATION_TRACE_PATH,Env:MVDM_S42_VXD_BOP_REPORT_PATH,Env:MVDM_S41_WOW_BOP_REPORT_PATH -ErrorAction SilentlyContinue
}
