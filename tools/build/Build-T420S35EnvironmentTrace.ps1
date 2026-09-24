[CmdletBinding()]
param([Parameter(Mandatory)][string]$FormalRoot,
      [Parameter(Mandatory)][string]$BuildRoot,
      [ValidateSet('environment','dpmi','exception','pm-allocation-failure','dpmi-pointer','dpmi-ioctl','dpmi-xmem-failure')][string]$Boundary = 'environment',
      [switch]$DpmiLifecycleOnly,
      [Parameter(Mandatory)][string]$EnvironmentTraceLog)
$ErrorActionPreference = 'Stop'
if ($DpmiLifecycleOnly -and $Boundary -ne 'dpmi') { throw 'Lifecycle filter requires DPMI boundary' }
$repo = (Resolve-Path "$PSScriptRoot\..\..").Path
$formal = (Resolve-Path $FormalRoot).Path
$build = [IO.Path]::GetFullPath($BuildRoot)
if (!$build.StartsWith("$repo\build\", [StringComparison]::OrdinalIgnoreCase) -or
    (Test-Path $build)) { throw 'Use a fresh repository build directory' }
New-Item -ItemType Directory $build | Out-Null
$graph = Get-Content (Join-Path $formal 'build.ninja')
$flags = ($graph | Where-Object { $_.StartsWith('cflags = ') }).Substring(9).Replace('$:', ':')
if ($Boundary -eq 'environment') {
    $traceLog = [IO.Path]::GetFullPath($EnvironmentTraceLog).Replace('\', '/')
    if ([IO.Path]::GetDirectoryName($traceLog) -eq '' -or
        [IO.Path]::GetFileName($traceLog) -notmatch '^[A-Za-z0-9_.-]+$') {
        throw 'Environment trace must name a simple file below an explicit log directory'
    }
    $flags += ' /DS35_ENVIRONMENT_TRACE_LOG=\"' + $traceLog + '\"'
}
$memberName = 'obj/command/cmdenv.obj'
$libraryName = 'original-mvdm-command.lib'
$traceSource = 'command_environment_trace.c'
if ($Boundary -in @('dpmi','pm-allocation-failure','dpmi-pointer','dpmi-ioctl','dpmi-xmem-failure')) {
    $memberName = 'obj/dpmi/dpmi32.obj'
    $libraryName = 'original-mvdm-dpmi32.lib'
    $traceSource = 'dpmi_dispatch_trace.c'
    if ($Boundary -eq 'dpmi-xmem-failure') {
        $memberName = 'obj/dpmi/xmem.obj'
        $traceSource = 'dpmi_xmem_failure.c'
    }
    if ($Boundary -eq 'dpmi-pointer') {
        $memberName = 'obj/dpmi/int21map.obj'
        $traceSource = 'dpmi_pointer_trace.c'
    }
    if ($Boundary -eq 'dpmi-ioctl') {
        $memberName = 'obj/dpmi/int21map.obj'
        $traceSource = 'dpmi_ioctl_boundary.c'
    }
    if ($Boundary -eq 'pm-allocation-failure') {
        $memberName = 'obj/dpmi/dpmiint.obj'
        $traceSource = 'dpmi_pm_allocation_failure.c'
    }
    $ownerRow = [Array]::FindIndex($graph, [Predicate[string]]{
        param($line) $line.StartsWith("build ${memberName}: ")
    })
    if ($ownerRow -lt 0 -or !$graph[$ownerRow + 1].StartsWith('  dpmi_cflags = ')) {
        throw 'Missing original DPMI owner compile flags'
    }
    $flags = $graph[$ownerRow + 1].Substring('  dpmi_cflags = '.Length).Replace('$:', ':')
    $traceLog = [IO.Path]::GetFullPath($EnvironmentTraceLog).Replace('\', '/')
    if ([IO.Path]::GetDirectoryName($traceLog) -eq '' -or
        [IO.Path]::GetFileName($traceLog) -notmatch '^[A-Za-z0-9_.-]+$') {
        throw 'DPMI trace must name a simple file below an explicit log directory'
    }
    $flags += ' /DDPMI_TRACE_LOG=\"' + $traceLog + '\"'
    if ($DpmiLifecycleOnly) { $flags += ' /DDPMI_TRACE_LIFECYCLE_ONLY' }
}
if ($Boundary -eq 'exception') {
    $flags = ($graph | Where-Object { $_.StartsWith('host_cflags = ') }).Substring(14).Replace('$:', ':')
    $memberName = 'obj/host/nt_inthk.obj'
    $libraryName = 'original-softpc-host-roots.lib'
    $traceSource = 'dpmi_exception_trace.c'
    $traceLog = [IO.Path]::GetFullPath($EnvironmentTraceLog).Replace('\', '/')
    if ([IO.Path]::GetDirectoryName($traceLog) -eq '' -or
        [IO.Path]::GetFileName($traceLog) -notmatch '^[A-Za-z0-9_.-]+$') {
        throw 'Exception trace must name a simple file below an explicit log directory'
    }
    $flags += ' /DDPMI_TRACE_LOG=\"' + $traceLog + '\"'
}
$object = Join-Path $build 'observed-owner.obj'
$library = Join-Path $build $libraryName
$exe = Join-Path $build 'ntvdm.exe'
$commandObjects = ($graph | Where-Object { $_.StartsWith("build ${libraryName}: lib ") }).Split(' ',4)[3].Split(' ')
$workerInputs = ($graph | Where-Object { $_.StartsWith('build ntvdm.exe | ntvdm.lib: worker_link ') }).Substring('build ntvdm.exe | ntvdm.lib: worker_link '.Length).Split(' ')
$inputs = @($commandObjects + $workerInputs | Sort-Object -Unique)
$inputs | ForEach-Object { Get-FileHash (Join-Path $formal $_) } |
    Select-Object Path,Hash | ConvertTo-Json | Set-Content "$build\reused-inputs.json"
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
Push-Location $formal
try {
    $compile = "call `"$vs`" -arch=x86 -host_arch=x86 >nul && cl $flags /Fo`"$object`" `"$repo\tests\observation\$traceSource`""
    cmd.exe /d /s /c $compile *> "$build\compile.log"
    if ($LASTEXITCODE) { throw "Compile failed; see $build\compile.log" }
    $dispatchObject = Join-Path $build 'observed-dispatch.obj'
    if ($Boundary -eq 'dpmi-xmem-failure') {
        $dispatchRow = [Array]::FindIndex($graph, [Predicate[string]]{
            param($line) $line.StartsWith('build obj/dpmi/dpmi32.obj: ')
        })
        if ($dispatchRow -lt 0) { throw 'Missing original dispatch owner' }
        $dispatchFlags = $graph[$dispatchRow + 1].Substring('  dpmi_cflags = '.Length).Replace('$:', ':')
        $dispatchFlags += ' /DDPMI_TRACE_XMEM_FAILURE /DDPMI_TRACE_LOG=\"' + $traceLog + '\"'
        cmd.exe /d /s /c "call `"$vs`" -arch=x86 -host_arch=x86 >nul && cl $dispatchFlags /Fo`"$dispatchObject`" `"$repo\tests\observation\dpmi_dispatch_trace.c`"" *> "$build\dispatch-compile.log"
        if ($LASTEXITCODE) { throw 'Dispatch trace compile failed' }
    }
    $members = ($commandObjects | ForEach-Object {
        if ($_ -eq $memberName) { "`"$object`"" }
        elseif ($Boundary -eq 'dpmi-xmem-failure' -and $_ -eq 'obj/dpmi/dpmi32.obj') { "`"$dispatchObject`"" }
        else { $_ }
    }) -join ' '
    cmd.exe /d /s /c "call `"$vs`" -arch=x86 -host_arch=x86 >nul && lib /nologo /out:`"$library`" $members" *> "$build\archive.log"
    if ($LASTEXITCODE) { throw 'Archive failed' }
    $members = ($workerInputs | ForEach-Object {
        if ($_ -eq $libraryName) { "`"$library`"" } else { $_ }
    }) -join ' '
    cmd.exe /d /s /c "call `"$vs`" -arch=x86 -host_arch=x86 >nul && link /nologo /subsystem:console /opt:ref /out:`"$exe`" /map:`"$exe.map`" /implib:`"$build\ntvdm.lib`" /def:generated/ntvdm-wow32-provider.def $members rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib" *> "$build\link.log"
    if ($LASTEXITCODE) { throw 'Link failed' }
} finally { Pop-Location }
Get-FileHash $exe
