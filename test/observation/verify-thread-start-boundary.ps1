[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot, [ValidateSet('x86','x64')][string]$Architecture='x86')
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if (!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path $root)) { throw 'Require a fresh evidence directory under build' }
$null=New-Item -ItemType Directory -Path $root
function Run-Owned([string]$name,[string]$file,[string]$arguments,[int]$seconds) {
    $p=Start-Process $file -ArgumentList $arguments -WorkingDirectory $root -WindowStyle Hidden -PassThru -RedirectStandardOutput "$root/$name.out" -RedirectStandardError "$root/$name.err"
    $null=$p.Handle
    @{pid=$p.Id;start=$p.StartTime.ToUniversalTime().ToString('o');command=$arguments;
      executable=$file;deadline=$p.StartTime.AddSeconds($seconds).ToUniversalTime().ToString('o')} |
        ConvertTo-Json | Set-Content "$root/$name-process.json"
    if (!$p.WaitForExit($seconds*1000)) {
        & taskkill.exe /PID $p.Id /T /F *> "$root/$name-timeout.txt"
        throw "Timeout: $name"
    }
    $p.WaitForExit(); $p.Refresh()
    $p.ExitCode | Set-Content "$root/$name.exit"
    if ($p.ExitCode) { throw "Failed: $name ($($p.ExitCode))" }
}
try {
    & "$repo/tools/build-fixtures.cmd" $Architecture thread-lifecycle *> "$root/build.log"
    if ($LASTEXITCODE) { throw 'Thread fixture build failed' }
    $cache="$repo/build/.ninja/fixtures-$Architecture"
    foreach ($name in 'thread-start-session-fixture.exe','thread-module.dll',
            'nt-thread-alert-fixture.exe','softpc-event-thread-shutdown-fixture.exe') {
        Copy-Item "$cache/$name" "$root/$name"
    }
    $sources=@('src/compat/guest_memory_lease.c','src/ntvdm-exe/session/session.c',
        'src/compat/host_context.c','src/compat/thread_start.c','src/lib-win32/thread/thread.c',
        'test/adapter-mvdm-host-out/thread_start_session_fixture.c',
        'src/mvdm-host/softpc.new/host/src/nt_thred.c',
        'src/vdmredir-dll/source/mvdm_redirector_dll_entry.c',
        'test/adapter-mvdm-host-out/thread_start_module_fixture.c')
    Run-Owned 'fixture' "$root/thread-start-session-fixture.exe" ('--verify "'+$root+'/thread-module.dll"') 45
    Run-Owned 'alert' "$root/nt-thread-alert-fixture.exe" ' ' 10
    Run-Owned 'shutdown' "$root/softpc-event-thread-shutdown-fixture.exe" ' ' 10
    $output=Get-Content "$root/fixture.out" -Raw
    foreach ($marker in @('THREAD_MODULE actual_redirector_entry=1 shared_binding=1 same_owner=1',
            'THREAD_LIFECYCLE uninitialized_rejected=1 repeated_install=1',
            'THREAD_TERMINAL_UNBIND normal=5 once=1 rejected_exit=c0000420',
            'THREAD_TERMINAL_INTENT terminal=5 ordinary=1 rejected_reset=1 exception_reset=1',
            'THREAD_START_ROLLBACK rejected=4 owner_count=1 no_worker_entry=1',
            'THREAD_PRE_CCPU original_exit_body=1 unbind_before_cpu=1 once=1')) {
        if (!$output.Contains($marker)) { throw "Missing assertion: $marker" }
    }
    $sources | ForEach-Object { Get-FileHash "$repo/$_" -Algorithm SHA256 } |
        Select-Object Path,Hash | ConvertTo-Json | Set-Content "$root/inputs.json"
    '0' | Set-Content "$root/exit.txt"
} catch {
    $_ | Out-String | Set-Content "$root/error.txt"
    '1' | Set-Content "$root/exit.txt"
    throw
}
