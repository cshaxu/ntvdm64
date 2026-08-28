[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = ''
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function NinjaPath([string]$Path) {
    $value = $Path.Replace('\', '/')
    if ($value.Length -ge 2 -and $value[1] -eq ':') {
        return $value.Substring(0, 1) + '$' + ':' + $value.Substring(2)
    }
    return $value
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$build = Join-Path $root ("build/M0-T309/S2/cohorts/{0}" -f $Architecture)
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}

# These are the literal original `sources` C cohorts.  They are built into
# archives only: external references stay visible for S2's owner ledger, while
# a synthetic final link or a hand-written service body is prohibited.
$dem = @('dem','demfcb','demdata','demdir','demdisp','demerror','demfile',
         'demgset','demhndl','demioctl','demlock','demmisc','demmsg','demsrch',
         'demdasd','demlabel')
$command = @('cmd','cmddata','cmddisp','cmdexec','cmdexit','cmdmisc','cmdpif',
             'cmdredir','cmdconf','cmdkeyb','cmdenv')
$bindings = @(
    'src/session/mapping_manager.c',
    'src/session/guest_memory_lease.c',
    'src/session/session.c',
    'src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c',
    'src/adapter-mvdm-host-out/basesrv/source/base_vdm_client.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_command_registers.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_sas.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_host_identity.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_command_redirection.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_redirector_pointer_scope.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_vdd_sft_shadow.c',
    'src/adapter-mvdm-host-out/softpc/mvdm_vdm_stack.c',
    'src/adapter-mvdm-host-out/monitor/mvdm_vdm_tib.c',
    'src/adapter-mvdm-host-out/monitor/source/host_idle.c',
    'src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c',
    'src/adapter-mvdm-host-out/win32/source/ntioapi_facade.c',
    'src/adapter-mvdm-host-out/redir/mvdm_redirector_handle.c',
    'src/adapter-mvdm-host-out/redir/mvdm_redirector_mailslot.c'
)
foreach ($unit in $dem) {
    $path = Join-Path $root "src/mvdm-host/dos/dem/$unit.c"
    if (!(Test-Path -LiteralPath $path)) { throw "Missing original DEM source: $path" }
}
foreach ($unit in $command) {
    $path = Join-Path $root "src/mvdm-host/dos/command/$unit.c"
    if (!(Test-Path -LiteralPath $path)) { throw "Missing original COMMAND source: $path" }
}
foreach ($binding in $bindings) {
    if (!(Test-Path -LiteralPath (Join-Path $root $binding))) {
        throw "Missing selected source-shaped binding: $binding"
    }
}
New-Item -ItemType Directory -Force $build, (Join-Path $build 'obj/dem'), (Join-Path $build 'obj/command'), (Join-Path $build 'obj/binding') | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T309_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T309_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$includes = @(
    'src',
    'src/adapter-mvdm-host-out/win32/include',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/monitor/include',
    'src/adapter-mvdm-host-out/basesrv/include',
    'src/adapter-mvdm-host-out/redir/include',
    'src/session',
    'src/mvdm-support/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/mvdm-host/dos/dem',
    'src/mvdm-host/dos/command',
    'src/mvdm-host/softpc.new/host/inc',
    'src/mvdm-host/softpc.new/base/inc'
) | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$flags = '/nologo /TC /c /std:c11 /MT /W4 /showIncludes /DWIN_32 /DDEVL /Di386 /DNTVDM ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/softpc/include/error_abi.h')) + '" ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/include/vdm.h')) + '" ' +
    ($includes -join ' ')

$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('build_root = ' + (NinjaPath $build))
$graph.Add('cflags = ' + $flags)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' lib.exe /nologo /out:$out $in')
$graph.Add('rule audit')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe @$out.rsp')
$demObjects = foreach ($unit in $dem) {
    $object = "obj/dem/$unit.obj"
    $graph.Add("build ${object}: cc " + (NinjaPath (Join-Path $root "src/mvdm-host/dos/dem/$unit.c")))
    $object
}
$commandObjects = foreach ($unit in $command) {
    $object = "obj/command/$unit.obj"
    $graph.Add("build ${object}: cc " + (NinjaPath (Join-Path $root "src/mvdm-host/dos/command/$unit.c")))
    $object
}
$bindingObjects = foreach ($binding in $bindings) {
    $unit = [IO.Path]::GetFileNameWithoutExtension($binding)
    $object = "obj/binding/$unit.obj"
    $graph.Add("build ${object}: cc " + (NinjaPath (Join-Path $root $binding)))
    $object
}
$graph.Add('build original-dem-provider-cohort.lib: lib ' + ($demObjects -join ' '))
$graph.Add('build original-command-provider-cohort.lib: lib ' + ($commandObjects -join ' '))
$graph.Add('build source-shaped-bindings.lib: lib ' + ($bindingObjects -join ' '))
$auditResponse = @(
    '/nologo', '/dll', '/noentry', '/force:unresolved',
    '/out:external-link-audit.dll',
    '/wholearchive:original-dem-provider-cohort.lib',
    '/wholearchive:original-command-provider-cohort.lib',
    '/wholearchive:source-shaped-bindings.lib',
    'kernel32.lib', 'advapi32.lib'
)
[IO.File]::WriteAllLines((Join-Path $build 'external-link-audit.dll.rsp'), $auditResponse,
    [Text.UTF8Encoding]::new($false))
$graph.Add('build external-link-audit.dll: audit original-dem-provider-cohort.lib original-command-provider-cohort.lib source-shaped-bindings.lib')
$graph.Add('build cohorts: phony original-dem-provider-cohort.lib original-command-provider-cohort.lib source-shaped-bindings.lib')
$graph.Add('default cohorts external-link-audit.dll')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'),
    (($graph -join [Environment]::NewLine) + [Environment]::NewLine),
    [Text.UTF8Encoding]::new($false))
Write-Host "Generated T309 S2 original provider-cohort graph: $build"
