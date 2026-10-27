$ErrorActionPreference='Stop'
$repo='O:\repos.hobby\ntvdm64'
$root=Join-Path $repo 'build/t473-s10/sol-claim-watch'
if(Test-Path -LiteralPath $root){throw 'Fresh diagnostic directory required'}
$null=New-Item -ItemType Directory $root
$text=[IO.File]::ReadAllText("$repo/tests/observation/wow_sol_options_probe.c").Replace("`r`n","`n")
$helper=[IO.File]::ReadAllText("$repo/build/t473-s10/sol-claim-watch.inc")
$needle='/* Capture an actual first-chance AV'
if(!$text.Contains($needle)){throw 'Expected original diagnostic boundary'}
$text=$text.Replace($needle,$helper+"`n"+$needle)
$replacement=@'
    BOOL attached = FALSE;
    error_watch watch = {0};
    watch.thread_id = GetWindowThreadProcessId(window, NULL);
    watch.address = remote_error_entry(pid);
    if (!watch.thread_id || !watch.address) { puts("SOL error-watch: entry/owner resolution failed"); return FALSE; }
'@
$text=$text.Replace('    BOOL attached = FALSE;',$replacement)
$text=$text.Replace('        if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {',@'
        if (event.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            (void)arm_error_watch(&watch, event.dwThreadId);
            if (event.u.CreateProcessInfo.hFile) CloseHandle(event.u.CreateProcessInfo.hFile);
            CloseHandle(event.u.CreateProcessInfo.hThread); CloseHandle(event.u.CreateProcessInfo.hProcess);
        } else if (event.dwDebugEventCode == CREATE_THREAD_DEBUG_EVENT) {
            (void)arm_error_watch(&watch, event.dwThreadId);
            CloseHandle(event.u.CreateThread.hThread);
        } else if (event.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT && event.u.LoadDll.hFile) {
            CloseHandle(event.u.LoadDll.hFile);
        }
        if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
'@)
$text=$text.Replace('            if (code == EXCEPTION_ACCESS_VIOLATION) {',@'
            if (code == EXCEPTION_SINGLE_STEP && event.dwThreadId == watch.thread_id && watch.armed) {
                int captured = observe_error_watch(&watch, pid, event.dwThreadId);
                if (captured > 0) {
                    clear_error_watch(&watch);
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                    DebugActiveProcessStop(pid);
                    return TRUE;
                }
                if (captured == 0) {
                    ContinueDebugEvent(event.dwProcessId, event.dwThreadId, DBG_CONTINUE);
                    continue;
                }
            }
            if (code != EXCEPTION_BREAKPOINT) disposition = DBG_EXCEPTION_NOT_HANDLED;
            if (code == EXCEPTION_ACCESS_VIOLATION) {
'@)
$text=$text.Replace("`n                DebugActiveProcessStop(pid);","`n                clear_error_watch(&watch);`n                DebugActiveProcessStop(pid);")
$replacement=@'
    printf("SOL error-watch: completed armed=%d visits=%lu no-claim-failure\n", watch.armed, watch.visits);
    clear_error_watch(&watch);
    if (attached) DebugActiveProcessStop(pid);
'@
$text=$text.Replace('    if (attached) DebugActiveProcessStop(pid);',$replacement)
[IO.File]::WriteAllText("$root/probe.c",$text)
# Reuse the checked-in private-stage, environment, compiler and cleanup owner.
$runner=[IO.File]::ReadAllText("$repo/tests/observation/verify-wow-sol-options.ps1")
$runner=$runner.Replace('$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot ''../..''))',('$repo='''+$repo+''''))
$runner=$runner.Replace('. "$PSScriptRoot\isolated_package_cleanup.ps1"',('. "'+$repo+'\tests\observation\isolated_package_cleanup.ps1"'))
$runner=$runner.Replace('$source="$PSScriptRoot\wow_sol_options_probe.c"',('$source="'+$root+'\probe.c"'))
[IO.File]::WriteAllText("$root/verify.ps1",$runner)
& "$root/verify.ps1" -PackageRoot "$repo/build/t473-s10/candidate-validation/package" -CompilerDriver "$repo/build/.ninja/msvc-x86-release/wow32/msvc-x86.cmd" -BuildRoot "$root/run-r1" -CaptureFault
