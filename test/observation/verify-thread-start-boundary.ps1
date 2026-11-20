[CmdletBinding()]
param([Parameter(Mandatory)][string]$BuildRoot)
$ErrorActionPreference='Stop'
$repo=(Resolve-Path "$PSScriptRoot/../..").Path
$root=[IO.Path]::GetFullPath($BuildRoot)
if (!$root.StartsWith($repo+'\build\',[StringComparison]::OrdinalIgnoreCase) -or
        (Test-Path $root)) { throw 'Require a fresh evidence directory under build' }
$null=New-Item -ItemType Directory -Path $root
$vs='C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
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
    & "$PSScriptRoot/extract-softpc-thread-exit-fixture.ps1" -RepositoryRoot $repo -OutputRoot $root
    $sources=@('src/compat/guest_memory_lease.c',
        'src/ntvdm-exe/session/session.c', 'src/compat/host_context.c',
        'src/compat/thread_start.c',
        'src/lib-win32/thread/thread.c',
        'test/adapter-mvdm-host-out/thread_start_session_fixture.c')
    $objects=@()
    foreach ($source in $sources) {
        $object=[IO.Path]::GetFileNameWithoutExtension($source)+'.obj'
        $objects+=('"'+$root+'\'+$object+'"')
        $argsLine='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /TC /c /O2 /MT /W4 /I "'+$repo+'\src" /I "'+$repo+'\src\compat\include" /I"'+$root+'" /Fo"'+$root+'\'+$object+'" "'+$repo+'\'+$source+'""'
        Run-Owned ('compile-'+[IO.Path]::GetFileNameWithoutExtension($source)) 'cmd.exe' $argsLine 120
    }
    $argsLine='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /out:"'+$root+'\fixture.exe" '+($objects -join ' ')+' /export:session_thread_current /export:opennt_create_cdecl_thread_named /implib:"'+$root+'\fixture.lib" kernel32.lib"'
    Run-Owned 'link' 'cmd.exe' $argsLine 120
    foreach ($source in @('src/vdmredir-dll/source/mvdm_redirector_dll_entry.c',
            'test/adapter-mvdm-host-out/thread_start_module_fixture.c')) {
        $object=[IO.Path]::GetFileNameWithoutExtension($source)+'.obj'
        $argsLine='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && cl.exe /nologo /TC /c /O2 /MT /W4 /I "'+$repo+'\src" /I"'+$root+'" /Fo"'+$root+'\'+$object+'" "'+$repo+'\'+$source+'""'
        Run-Owned ('compile-'+[IO.Path]::GetFileNameWithoutExtension($source)) 'cmd.exe' $argsLine 120
    }
    $moduleObjects=@('mvdm_redirector_dll_entry.obj','thread_start_module_fixture.obj',
        'fixture.lib') |
        ForEach-Object { '"'+$root+'\'+$_+'"' }
    $argsLine='/d /s /c "call "'+$vs+'" -arch=x86 -host_arch=x64 >nul && link.exe /nologo /dll /entry:mvdm_vdmredir_dll_entry@12 /export:thread_module_run /out:"'+$root+'\thread-module.dll" '+($moduleObjects -join ' ')+' kernel32.lib libcmt.lib libvcruntime.lib libucrt.lib"'
    Run-Owned 'link-module' 'cmd.exe' $argsLine 120
    Run-Owned 'fixture' "$root/fixture.exe" ('--verify "'+$root+'/thread-module.dll"') 45
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
