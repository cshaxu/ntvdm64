[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86', 'x64')] [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$NodeExecutable = '',
    [string]$NinjaExecutable = '',
    [ValidateRange(0, 64)] [int]$ParallelJobs = 0
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

function Get-OriginalSources([string]$Manifest) {
    $raw = Get-Content -LiteralPath $Manifest -Raw
    # Original OpenNT manifests do not use one universal terminator after
    # SOURCES.  Most continue into architecture-specific lists; VDMREDIR
    # instead follows it with C_DEFINES.  Stop at either form (or the next
    # uppercase assignment), so the selected source list remains the original
    # manifest list rather than a hand-maintained build subset.
    $match = [regex]::Match($raw, '(?s)SOURCES\s*=\s*(.*?)(?=\r?\n\s*(?:!INCLUDE|UMTYPE|i386_SOURCES|MIPS_SOURCES|ALPHA_SOURCES|C_DEFINES|[A-Z][A-Z0-9_]*=)|\z)')
    if (!$match.Success) { throw "Cannot isolate original SOURCES block: $Manifest" }
    # An OpenNT `sources` block may retain a disabled candidate as a `#`
    # comment (for example the pre-EVID C-video glue).  It is source evidence,
    # not a selected member of this build profile.  Remove complete comment
    # lines before extracting the selected original translation units.
    $sourceBlock = [regex]::Replace($match.Groups[1].Value, '(?m)^\s*#.*(?:\r?\n|$)', '')
    $names = @([regex]::Matches($sourceBlock, '\b([A-Za-z0-9_]+\.c)\b') |
        ForEach-Object { $_.Groups[1].Value } |
        Select-Object -Unique)
    if ($names.Count -eq 0) { throw "No C sources selected by original manifest: $Manifest" }
    return @($names)
}

if ([string]::IsNullOrWhiteSpace($RepositoryRoot)) {
    $RepositoryRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
}
$root = (Resolve-Path -LiteralPath $RepositoryRoot).Path
if ([string]::IsNullOrWhiteSpace($BuildRoot)) {
    $build = Join-Path $root ("build/M0-T310/S8/p1-machine-source/{0}" -f $Architecture)
} else {
    $build = [IO.Path]::GetFullPath($BuildRoot)
}
if ($ParallelJobs -eq 0) {
    # Historical SoftPC units have substantial include fan-out.  Cap the
    # default at 12 jobs: enough parallelism to keep a modern compiler busy
    # without turning a cold 400+ TU build into avoidable memory/IO contention.
    # The runner accepts an explicit MVDM_BUILD_JOBS override for hosts with a
    # different capacity.
    $ParallelJobs = [Math]::Min(12, [Environment]::ProcessorCount)
}
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
$nativeNinja = if ([string]::IsNullOrWhiteSpace($NinjaExecutable)) {
    Get-Command ninja.exe -All -ErrorAction SilentlyContinue |
        Where-Object { $_.Source -match 'Ninja-build\.Ninja' } |
        Select-Object -First 1 -ExpandProperty Source
} else {
    (Resolve-Path -LiteralPath $NinjaExecutable).Path
}
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or
    [string]::IsNullOrWhiteSpace($nativeNinja) -or
    !(Test-Path -LiteralPath $nativeNinja -PathType Leaf)) {
    throw 'MSVC Build Tools and Ninja are required.'
}

$ccpuRoot = Join-Path $root 'src/mvdm/softpc.new/base/ccpu386'
$biosRoot = Join-Path $root 'src/mvdm/softpc.new/base/bios'
$keymouseRoot = Join-Path $root 'src/mvdm/softpc.new/base/keymouse'
$systemRoot = Join-Path $root 'src/mvdm/softpc.new/base/system'
$disksRoot = Join-Path $root 'src/mvdm/softpc.new/base/disks'
$supportRoot = Join-Path $root 'src/mvdm/softpc.new/base/support'
$videoRoot = Join-Path $root 'src/mvdm/softpc.new/base/video'
$cvidcRoot = Join-Path $root 'src/mvdm/softpc.new/base/cvidc'
$commsRoot = Join-Path $root 'src/mvdm/softpc.new/base/comms'
$dosRoot = Join-Path $root 'src/mvdm/softpc.new/base/dos'
$demRoot = Join-Path $root 'src/mvdm/dos/dem'
$commandRoot = Join-Path $root 'src/mvdm/dos/command'
$redirRoot = Join-Path $root 'src/mvdm/vdmredir'
$openntNetlibRoot = Join-Path $root 'src/opennt-host/netapi/netlib'
$openntNetapiRoot = Join-Path $root 'src/opennt-host/netapi/api'
$openntXactSrvRoot = Join-Path $root 'src/opennt-host/netapi/xactsrv'
$openntBaseVdmRoot = Join-Path $root 'src/opennt-host/base/win32/client'
$openntRtlRoot = Join-Path $root 'src/opennt-host/base/ntos/rtl'
$openntRtlX86Root = Join-Path $root 'src/opennt-host/base/ntos/rtl/x86'
$xmsRoot = Join-Path $root 'src/mvdm/xms.486'
$dpmiRoot = Join-Path $root 'src/mvdm/dpmi32'
$suballocRoot = Join-Path $root 'src/mvdm/suballoc'
$oemuniRoot = Join-Path $root 'src/mvdm/oemuni'
$sessionRoot = Join-Path $root 'src/ntvdm-exe/session'
$adapterWowRoot = Join-Path $root 'src/ntvdm-exe/wow'
$wow32DllRoot = Join-Path $root 'src/wow32-dll'
$baseReservationTestSource = Join-Path $root 'tests/adapter-basesrv/base_reservation_test.c'
$baseServiceReservationTestSource = Join-Path $root 'tests/adapter-basesrv/base_service_reservation_test.c'
$cpu40DescriptorDomainFixtureSource = Join-Path $root 'tests/mvdm-host/dpmi/cpu40_descriptor_domain_fixture.c'
$rtlX86FixtureSource = Join-Path $root 'tests/opennt-host/rtl_x86_fixture.c'
$environmentProjectionFixtureSource = Join-Path $root 'tests/opennt-host/environment_projection_fixture.c'
$cvidcVectorBindingFixtureSource = Join-Path $root 'tests/mvdm-host/cvidc_vector_binding_fixture.c'
$x87LayoutFixtureSource = Join-Path $root 'tests/mvdm-host/x87_layout_fixture.c'
$ccpuThreadLifecycleFixtureSource = Join-Path $root 'tests/mvdm-host/ccpu_thread_lifecycle_test.c'
$cvidcVectorProviderStubGenerator = Join-Path $root 'tools/build/GenerateCvidcVectorProviderStubs.mjs'
$baseDebugRoot = Join-Path $root 'src/mvdm/softpc.new/base/debug'
$hostRoot = Join-Path $root 'src/mvdm/softpc.new/host/src'
$hostEntryRoot = Join-Path $root 'src/mvdm/softpc.new/obj.vdm'
$adapterSoftpcRoot = Join-Path $root 'src/ntvdm-exe/softpc'
$adapterWin32Root = Join-Path $root 'src/ntvdm-exe/win32'
$hostCompatSource = Join-Path $root 'src/opennt-abi/host-compat/opennt_support_rtl.c'
$hostCrtRedirect = Join-Path $root 'src/ntvdm-exe/win32/mvdm_crt_redirect.h'
$softpcSymbolCompat = Join-Path $root 'src/ntvdm-exe/softpc/include/mvdm_softpc_symbol_compat.h'
$run16Root = Join-Path $root 'src/run16-exe'
$frontendRoot = Join-Path $root 'src/ntcon-exe'
$frontendWindowSources = @()
$workerPackageRoot = Join-Path $root 'src/ntvdm-exe'
$adapterBaseSrvRoot = Join-Path $root 'src/ntvdm-exe/command/source'
$adapterMonitorRoot = Join-Path $root 'src/ntvdm-exe/monitor/source'
$kernelVdmPrinterSource = Join-Path $adapterMonitorRoot 'monitor_printer.c'
$adapterRedirRoot = Join-Path $root 'src/ntvdm-exe/redir'
$redirDllRoot = Join-Path $root 'src/vdmredir-dll'
$redirDllBindingRoot = Join-Path $redirDllRoot 'source'
$adapterVddRoot = Join-Path $root 'src/ntvdm-exe/vdd'
$ccpuFallbackSource = Join-Path $adapterSoftpcRoot 'mvdm_softpc_ccpu_fallback.c'
$ccpuProductIncludeRoot = Join-Path $adapterSoftpcRoot 'include/generated/x86/prod'
$ccpuManifest = Join-Path $ccpuRoot 'sources'
$biosManifest = Join-Path $biosRoot 'sources'
$keymouseManifest = Join-Path $keymouseRoot 'sources'
$systemManifest = Join-Path $systemRoot 'sources'
$disksManifest = Join-Path $disksRoot 'sources'
$supportManifest = Join-Path $supportRoot 'sources'
$videoManifest = Join-Path $videoRoot 'sources'
$cvidcManifest = Join-Path $cvidcRoot 'sources'
$commsManifest = Join-Path $commsRoot 'sources'
$dosManifest = Join-Path $dosRoot 'sources'
$demManifest = Join-Path $demRoot 'sources'
$commandManifest = Join-Path $commandRoot 'sources'
$redirManifest = Join-Path $redirRoot 'sources'
$redirDllBindingManifest = Join-Path $redirDllRoot 'sources'
$xmsManifest = Join-Path $xmsRoot 'sources'
$dpmiManifest = Join-Path $dpmiRoot 'sources'
$suballocManifest = Join-Path $suballocRoot 'sources'
$oemuniManifest = Join-Path $oemuniRoot 'sources'
$hostManifest = Join-Path $hostRoot 'sources'
$hostEntrySource = Join-Path $hostEntryRoot 'ntvdm.c'
$hostEntryResourceSource = Join-Path $hostEntryRoot 'resource.rc'
$embeddedRomResourceSource = Join-Path $build 'generated/softpc-embedded-roms.rc'
$embeddedRomSources = @(
    (Join-Path $root 'src/mvdm/softpc.new/roms/bios1.rom'),
    (Join-Path $root 'src/mvdm/softpc.new/roms/bios4.rom'),
    (Join-Path $root 'src/mvdm/softpc.new/roms/v7vga.rom')
)
$hostExportDefinition = Join-Path $hostEntryRoot 'obj/i386/ntvdm.def'
$redirResourceSource = Join-Path $redirRoot 'vdmredir.rc'
$redirExportDefinition = Join-Path $redirRoot 'vdmredir.def'
$ccpuNames = Get-OriginalSources $ccpuManifest
# The historical manifest carries the real FPU body and a host-profile stub
# carrier in the same lexical list.  The selected CPU_40/FPU profile is the
# real `fpu.c` body; selecting `ntstubs.c` beside it creates duplicate FPU and
# SAS fallback definitions during a complete-library forced link.  Retain the
# original stub as mirror evidence, but do not select the alternate body for
# this profile.  The separately selected NTVDMx64 CCPU-vector-default patch
# owns only its registered default symbols.
$ccpuNames = @($ccpuNames | Where-Object { $_ -ne 'ntstubs.c' })
# vglob's CCPU-local GDP profile is not the selected C-VID layout. Compile
# the identical shared original below with the C-VID header/profile instead.
$ccpuNames = @($ccpuNames + 'localfm.c') | Select-Object -Unique
$biosNames = Get-OriginalSources $biosManifest
$keymouseNames = Get-OriginalSources $keymouseManifest
$systemNames = Get-OriginalSources $systemManifest
$disksNames = Get-OriginalSources $disksManifest
$supportNames = Get-OriginalSources $supportManifest
$videoNames = Get-OriginalSources $videoManifest
# CCPU's original C-language video-memory access backend.  It complements
# ccpu386 instruction execution and base/video device state; it is not an
# alternate CPU. Select its complete manifest rather than substitute display
# or video-memory shims one entrypoint at a time.
$cvidcNames = @(Get-OriginalSources $cvidcManifest)
# The selected product is Win32/x86 CCPU40.  Compile the complete original
# C-VID manifest, including its generated table definitions, as the source
# owner.  The former typed-table generator existed solely for deferred x64
# function-pointer-width recovery and must not replace the original x86 table
# contract with per-slot wrappers.
# These are complete original library packages, selected because the current
# BIOS/reset workset calls their public controller algorithms.  Selecting them
# as packages preserves the original implementation rather than substituting
# one unresolved serial/print/EMS symbol at a time.
$commsNames = @(Get-OriginalSources $commsManifest)
$dosNames = @(Get-OriginalSources $dosManifest)
# The original SoftPC initialization root invokes these four MVDM provider
# packages directly.  Select their complete source manifests as one source
# closure rather than make `CMDInit`/`DemInit`/`XMSInit`/`DBGInit` adapters.
$demNames = @(Get-OriginalSources $demManifest)
$commandNames = @(Get-OriginalSources $commandManifest)
# The original loader (`LoadVdmRedir` in nt_bop.c) requires one VDMREDIR DLL,
# not a hand-selected static-provider subset.  Compile its full original
# manifest as one candidate library first; later T333 cohorts decide which
# source-owned behavior can enter the DLL's runtime profile.  Do not replace
# excluded provider bodies with adapter policy.
$redirNames = @(Get-OriginalSources $redirManifest)
foreach ($name in $redirNames) {
    if (!(Test-Path -LiteralPath (Join-Path $redirRoot $name))) {
        throw "Original Redirector S3 source missing: $name"
    }
}
$adapterRedirNames = @(Get-OriginalSources $redirDllBindingManifest)
$xmsNames = @(Get-OriginalSources $xmsManifest)
$xmsNames += 'xmsmemr.c' # Original SAS callbacks for the selected CCPU40 backing.
# `i386_SOURCES` is an NT4 kernel-VDM host variant: it writes the host LDT,
# queries VDM kernel feature bits and mutates the fixed NTVDM V86-state page.
# It is retained in the complete mirror but is not a CCPU40 source member.
# The product has no kernel VDM/V86 route; selecting the manifest's portable
# `SOURCES` block preserves the original CCPU-compatible DPMI owner package.
# The portable original fallback bodies supply the dispatch members that the
# CCPU40 profile reaches without taking the NT4 i386 kernel-VDM branch.
$dpmiNames = @((Get-OriginalSources $dpmiManifest) + 'dpmimemr.c' + 'dpmimscr.c' + 'i386/dpmi386.c' | Select-Object -Unique)
# XMS uses the complete original SubAlloc package.  Keeping it out of the
# machine graph made its allocator algorithms look like a collection of
# missing adapter symbols, rather than selecting their original owner.
$suballocNames = @(Get-OriginalSources $suballocManifest)
$oemuniNames = @(Get-OriginalSources $oemuniManifest)
$sessionNames = @('guest_memory_lease.c', 'session.c')
# One `ntvdm.exe` owns the worker-local USER runtime.  WOW32.DLL imports its
# current-thread operations from that parent; it must not acquire a second TLS
# copy of the same worker domain.
$adapterWowWorkerNames = @('wow_gdi_alias.c', 'wow_user_runtime.c', 'wow_user_session_binding.c')
# The complete original SoftPC base debug package remains a worker-local
# provider.  The separate MVDM `dbg` product is a debugger/CSR owner package
# and is not selected merely to satisfy these original debug edges.
$baseDebugNames = @(
    'debuggng.c',
    'ega_dump.c',
    'btrace.c',
    'trace.c',
    'profile.c',
    'pigyoda.c',
    'yoda.c',
    'dasm.c'
)
# Select the complete original SoftPC host source package before deciding which
# unresolved edges are genuine modern-boundary adapters.  Individual fixtures
# and hand-written stand-ins are not a source-selection mechanism.
$hostNames = @(Get-OriginalSources $hostManifest)
# `nt_cprgs.c` is a selected original host carrier.  The historical
# `nt_aorc.c` bridge does not cover the 32-bit CCPU register surface reached
# by DPMI; the complete original bridge is generated in CVIDC `accessfn.c`.
$hostNames = @($hostNames + 'nt_cprgs.c') | Select-Object -Unique
if (!(Test-Path -LiteralPath $hostEntrySource)) { throw "Original SoftPC host entry missing: $hostEntrySource" }
if (!(Test-Path -LiteralPath $hostEntryResourceSource)) { throw "Original SoftPC resource source missing: $hostEntryResourceSource" }
if (!(Test-Path -LiteralPath $hostExportDefinition)) { throw "Original SoftPC export definition missing: $hostExportDefinition" }
if (!(Test-Path -LiteralPath $redirResourceSource)) { throw "Original Redirector resource missing: $redirResourceSource" }
if (!(Test-Path -LiteralPath $redirExportDefinition)) { throw "Original Redirector export definition missing: $redirExportDefinition" }
$adapterWin32Names = @('ntioapi_facade.c', 'thread_start_compat.c',
                          'nt_thread_alert_compat.c',
                          'console_compat.c', 'console_client.c', 'console_bitmap.c', 'console_graphics.c', 'console_text.c', 'crt_compat.c',
                           'command_process_compat.c', 'wow_private_unavailable.c',
                           'wow_hard_error_dialog.c',
                           'mvdm_base_vdm_environment.c')
# ExitVDM is an original Base client call reached by selected SoftPC teardown
# sources.  It already has one same-shaped session-owned implementation; keep
# it in the formal closure instead of accepting an unresolved external edge.
# The reached COMMAND helpers are worker bindings.  The former local
# BaseVDM queue/client/broker was the retired single-process product and must
# never be rebuilt into an audit or production archive.
$adapterBaseSrvNames = @('mvdm_command_guest_state.c',
                          'mvdm_command_native_child.c')
$adapterVddNames = @('mvdm_softpc_vdd_unavailable.c')
$adapterMonitorNames = @('vdm_control.c', '../mvdm_vdm_tib.c')
$adapterDebuggerNames = @('dbg_registers.c', 'dbg_prompt.c')
$adapterRedirWorkerNames = @('mvdm_redirector_worker_copy.c',
                              'mvdm_redirector_async.c')
# `vrnetapi.c` directly consumes this original OpenNT status-to-LAN-Manager
# conversion algorithm.  Keep it in its separately mirrored non-MVDM owner
# rather than duplicating a status table in Redirector or an adapter.
$openntNetlibNames = @('ntstatus.c', 'copystr.c', 'allocstr.c', 'initoem.c', 'memalloc.c', 'time.c', 'timezone.c', 'xlatesvc.c', 'svcinfo.c', 'packstr.c')
$openntNetapiNames = @('apibuff.c')
$openntXactSrvNames = @('apiwksta.c', 'apiuse.c', 'apimsg.c', 'apisvc.c', 'xssubs.c', 'strucsiz.c', 'examine.c', 'ascii.c', 'pointer.c', 'convert.c', 'xsunicod.c', 'auxdata.c', 'parmnum.c')
$openntBaseVdmNames = @('vdm.c')
$openntRtlNames = @('environ.c', 'error.c', 'time.c')
$openntRtlX86Names = @('largeint-selected.asm', 'movemem-selected.asm')
$adapterSoftpcNames = @('mvdm_softpc_firmware.c', 'mvdm_shadow_registry.c', 'mvdm_xms_memory.c', 'mvdm_a20.c', 'mvdm_softpc_guest_memory.c', 'mvdm_softpc_physical_mapping.c',
                         'mvdm_guest_location.c', 'mvdm_softpc_execution.c', 'mvdm_softpc_termination.c',
                         'mvdm_softpc_fast_bop.c',
                         'mvdm_softpc_wow_page_domain.c',
                         'mvdm_standalone_worker.c',
                        'mvdm_softpc_event_thread.c',
                        'mvdm_softpc_presentation_font.c',
                        'mvdm_softpc_text_video.c',
                        'mvdm_keyboard_history.c',
                        'mvdm_keyboard_reset.c',
                        'mvdm_softpc_mouse_input.c', 'mvdm_softpc_mouse_bridge.c',
                        'mvdm_softpc_mouse_guest.c',
                        'mvdm_softpc_descriptor_fields.c')
$workerPackageNames = @('package_layout.c')
$effectiveAddressSource = Join-Path $adapterSoftpcRoot 'mvdm_softpc_effective_address.c'
$effectiveAddressObject = 'obj/adapter-softpc/mvdm_softpc_effective_address.obj'
foreach ($name in $ccpuNames) {
    if (!(Test-Path -LiteralPath (Join-Path $ccpuRoot $name))) { throw "Original CCPU source missing: $name" }
}
foreach ($name in $biosNames) {
    if (!(Test-Path -LiteralPath (Join-Path $biosRoot $name))) { throw "Original SoftPC BIOS source missing: $name" }
}
foreach ($name in $keymouseNames) {
    if (!(Test-Path -LiteralPath (Join-Path $keymouseRoot $name))) { throw "Original SoftPC keymouse source missing: $name" }
}
foreach ($name in $systemNames) {
    if (!(Test-Path -LiteralPath (Join-Path $systemRoot $name))) { throw "Original SoftPC system source missing: $name" }
}
foreach ($name in $disksNames) {
    if (!(Test-Path -LiteralPath (Join-Path $disksRoot $name))) { throw "Original SoftPC disks source missing: $name" }
}
foreach ($name in $supportNames) {
    if (!(Test-Path -LiteralPath (Join-Path $supportRoot $name))) { throw "Original SoftPC support source missing: $name" }
}
foreach ($name in $videoNames) {
    if (!(Test-Path -LiteralPath (Join-Path $videoRoot $name))) { throw "Original SoftPC video source missing: $name" }
}
foreach ($name in $cvidcNames) {
    if (!(Test-Path -LiteralPath (Join-Path $cvidcRoot $name))) { throw "Original CCPU CVIDC source missing: $name" }
}
foreach ($name in $commsNames) {
    if (!(Test-Path -LiteralPath (Join-Path $commsRoot $name))) { throw "Original SoftPC comms source missing: $name" }
}
foreach ($name in $dosNames) {
    if (!(Test-Path -LiteralPath (Join-Path $dosRoot $name))) { throw "Original SoftPC DOS source missing: $name" }
}
foreach ($name in $demNames) {
    if (!(Test-Path -LiteralPath (Join-Path $demRoot $name))) { throw "Original MVDM DEM source missing: $name" }
}
foreach ($name in $commandNames) {
    if (!(Test-Path -LiteralPath (Join-Path $commandRoot $name))) { throw "Original MVDM COMMAND source missing: $name" }
}
foreach ($name in $xmsNames) {
    if (!(Test-Path -LiteralPath (Join-Path $xmsRoot $name))) { throw "Original MVDM XMS source missing: $name" }
}
foreach ($name in $dpmiNames) {
    if (!(Test-Path -LiteralPath (Join-Path $dpmiRoot $name))) { throw "Original MVDM DPMI32 source missing: $name" }
}
foreach ($name in $suballocNames) {
    if (!(Test-Path -LiteralPath (Join-Path $suballocRoot $name))) { throw "Original MVDM SubAlloc source missing: $name" }
}
foreach ($name in $oemuniNames) {
    if (!(Test-Path -LiteralPath (Join-Path $oemuniRoot $name))) { throw "Original MVDM OEM/Unicode support source missing: $name" }
}
foreach ($name in $sessionNames) {
    if (!(Test-Path -LiteralPath (Join-Path $sessionRoot $name))) { throw "Required session source missing: $name" }
}
foreach ($name in $adapterWowWorkerNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterWowRoot $name))) { throw "Required WOW worker binding source missing: $name" }
}
foreach ($name in $baseDebugNames) {
    if (!(Test-Path -LiteralPath (Join-Path $baseDebugRoot $name))) { throw "Original SoftPC base debug source missing: $name" }
}
foreach ($name in $hostNames) {
    if (!(Test-Path -LiteralPath (Join-Path $hostRoot $name))) { throw "Original SoftPC host root missing: $name" }
}
foreach ($name in $adapterWin32Names) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterWin32Root $name))) { throw "Required Win32 adapter source missing: $name" }
}
if (!(Test-Path -LiteralPath $hostCompatSource)) {
    throw "Required OpenNT host-compat source missing: $hostCompatSource"
}
foreach ($name in $adapterSoftpcNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterSoftpcRoot $name))) { throw "Required SoftPC adapter source missing: $name" }
}
foreach ($name in $adapterRedirNames) {
    if (!(Test-Path -LiteralPath (Join-Path $redirDllBindingRoot $name))) { throw "Required VDMREDIR DLL binding source missing: $name" }
}
foreach ($name in $adapterRedirWorkerNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterRedirRoot $name))) { throw "Required Redirector worker source missing: $name" }
}
if (!(Test-Path -LiteralPath $rtlX86FixtureSource -PathType Leaf)) {
    throw "Required original x86 RTL fixture missing: $rtlX86FixtureSource"
}
if (!(Test-Path -LiteralPath $environmentProjectionFixtureSource -PathType Leaf)) {
    throw "Required environment projection fixture missing: $environmentProjectionFixtureSource"
}
if (!(Test-Path -LiteralPath $x87LayoutFixtureSource -PathType Leaf)) {
    throw "Required x87 layout fixture missing: $x87LayoutFixtureSource"
}
if (!(Test-Path -LiteralPath $ccpuThreadLifecycleFixtureSource -PathType Leaf)) {
    throw 'Required CCPU thread-lifecycle fixture source missing.'
}
foreach ($name in $openntRtlX86Names) {
    if (!(Test-Path -LiteralPath (Join-Path $openntRtlX86Root $name))) { throw "Selected original x86 RTL source missing: $name" }
}
foreach ($name in $openntRtlNames) {
    if (!(Test-Path -LiteralPath (Join-Path $openntRtlRoot $name))) { throw "Selected original RTL source missing: $name" }
}
foreach ($name in $adapterVddNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterVddRoot $name))) { throw "Required VDD adapter source missing: $name" }
}
foreach ($name in $openntNetlibNames) {
    if (!(Test-Path -LiteralPath (Join-Path $openntNetlibRoot $name))) { throw "Original OpenNT netlib source missing: $name" }
}
foreach ($name in $openntNetapiNames) {
    if (!(Test-Path -LiteralPath (Join-Path $openntNetapiRoot $name))) { throw "Original OpenNT NetAPI source missing: $name" }
}
foreach ($name in $openntXactSrvNames) {
    if (!(Test-Path -LiteralPath (Join-Path $openntXactSrvRoot $name))) { throw "Original OpenNT XACTSRV source missing: $name" }
}
foreach ($name in $openntBaseVdmNames) {
    if (!(Test-Path -LiteralPath (Join-Path $openntBaseVdmRoot $name))) { throw "Original OpenNT Base VDM source missing: $name" }
}
foreach ($name in $workerPackageNames) {
    if (!(Test-Path -LiteralPath (Join-Path $workerPackageRoot $name))) { throw "Required package source missing: $name" }
}
foreach ($name in $adapterMonitorNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterMonitorRoot $name))) { throw "Required monitor adapter source missing: $name" }
}
if (!(Test-Path -LiteralPath $kernelVdmPrinterSource)) { throw "Required kernel VDM printer carrier missing: $kernelVdmPrinterSource" }
foreach ($name in @('PigReg_c.h', 'sas4gen.h', 'gdpvar.h')) {
    if (!(Test-Path -LiteralPath (Join-Path $ccpuProductIncludeRoot $name))) { throw "Selected x86 CCPU product carrier missing: $name" }
}
if (!(Test-Path -LiteralPath $ccpuFallbackSource)) { throw "Selected CCPU fallback source missing: $ccpuFallbackSource" }

New-Item -ItemType Directory -Force $build, (Join-Path $build 'generated') | Out-Null

# OpenNT's WOW32 build imports the running NTVDM and OEMUNI owners rather
# than linking a second machine into WOW32.DLL.  Keep the original NTVDM
# export list intact and derive this additional, private provider-import
# surface only for the standalone composition.  Every entry below already has
# one selected source owner in the parent; this file introduces no body.
$wow32ProviderExportDefinition = Join-Path $build 'generated/ntvdm-wow32-provider.def'
$wow32ProviderExportLines = [Collections.Generic.List[string]]::new()
foreach ($line in Get-Content -LiteralPath $hostExportDefinition) {
    if ($line -match '^\s*NAME\s+') {
        # The selected three-program worker retains the original ntvdm.exe
        # module identity.  WOW32 is late-loaded by that worker, so its import
        # library must name the actual parent module, while retaining every
        # original export spelling below.
        $wow32ProviderExportLines.Add('NAME ntvdm.exe')
    } else {
        $wow32ProviderExportLines.Add($line)
    }
}
$wow32ProviderExportLines.Add('')
$wow32ProviderExportLines.Add('; private imports for the selected original WOW32 provider')
foreach ($entry in @(
    'opennt_support_current_teb=_opennt_support_current_teb@0',
    'NtCurrentPeb=_NtCurrentPeb@0', 'ExitVDM=_ExitVDM@8',
    'OpenNtBaseClientWowStarted=_OpenNtBaseClientWowStarted@4',
    'VdmAllocateVirtualMemory', 'VdmFreeVirtualMemory',
    'VdmAddVirtualMemory', 'VdmRemoveVirtualMemory',
    'VdmAddDescriptorMapping', 'SetWOWforceIncrAlloc',
    'GetNextVDMCommand=_GetNextVDMCommand@4',
    'RegisterWowExec=_RegisterWowExec@4',
    'GetCurrentDirectoryOem=_GetCurrentDirectoryOem@8',
    'SetCurrentDirectoryOem=_SetCurrentDirectoryOem@4',
    'GetEnvironmentVariableOem=_GetEnvironmentVariableOem@12',
    'SetEnvironmentVariableOem=_SetEnvironmentVariableOem@8',
    'SetFileAttributesOem=_SetFileAttributesOem@8',
    'GetFileAttributesOem=_GetFileAttributesOem@4',
    'DeleteFileOem=_DeleteFileOem@4',
    'MoveFileOem=_MoveFileOem@8', 'MoveFileExOem=_MoveFileExOem@12',
    'GetFullPathNameOem=_GetFullPathNameOem@16',
    'GetTempFileNameOem=_GetTempFileNameOem@16',
    'RemoveFontResourceOem=_RemoveFontResourceOem@4',
    'CreateFileOem=_CreateFileOem@28',
    'GetVolumeInformationOem=_GetVolumeInformationOem@32',
    'mvdm_softpc_guest_memory_acquire',
    'mvdm_softpc_guest_memory_release',
    'mvdm_redirector_worker_copy_from',
    'mvdm_redirector_worker_copy_to',
    'mvdm_redirector_async_prepare',
    'mvdm_redirector_async_complete',
    'mvdm_redirector_async_release',
    # WOW32's original task-lifecycle body converts NTSTATUS through the
    # selected parent RTL implementation.  Export the existing body; this is
    # an import-surface declaration, not a standalone replacement.
    'OpenNtRtlNtStatusToDosError=_OpenNtRtlNtStatusToDosError@4',
    # VDMREDIR's original asynchronous compatibility boundary still imports
    # these worker-thread helpers.  WOW32 no longer imports them directly;
    # it uses its narrow wow_user_worker_active query instead.
    'session_thread_current',
    'session_thread_bind_owned_source',
    'session_thread_unbind',
    # The late-loaded WOW32 provider borrows this one worker-local domain;
    # these exports are an ABI bridge only, not a second USER implementation.
    'wow_user_runtime_current=_wow_user_runtime_current@0',
    'wow_user_runtime_enter=_wow_user_runtime_enter@4',
    'wow_user_runtime_leave=_wow_user_runtime_leave@4',
    'wow_user_runtime_set_context=_wow_user_runtime_set_context@12',
    'wow_user_worker_active=_wow_user_worker_active@0',
    # Cdecl C object names are resolved by the linker from their undecorated
    # .def spelling.  Unlike the stdcall entries above, adding a leading
    # underscore here asks LINK for a second underscore and hides the body.
    'mvdm_wow_gdi_handle_from_native',
    'mvdm_wow_gdi_handle_to_native',
    'mvdm_wow_gdi_handle_retire',
      'mvdm_softpc_system_find_file',
      'mvdm_softpc_profile_shadow_system_ini',
      'mvdm_softpc_system_copy_root',
      'mvdm_softpc_system_copy_system_directory',
      'NtvdmGetWindowsDirectoryA',
      'NtvdmGetSystemDirectoryA',
      'NtvdmGetWindowsDirectoryW',
      'NtvdmGetSystemDirectoryW',
    'mvdm_softpc_wow_page_domain_guest_shared_info',
    'mvdm_softpc_fast_bop_dispatch_offset',
    'mvdm_softpc_fast_bop_callback_offset',
    'mvdm_softpc_wow_page_domain_flat_address_array',
    'mvdm_softpc_wow_page_domain_guest_csr_flag',
    'mvdm_softpc_wow_page_domain_set_expected_windows_version',
    'mvdm_softpc_wow_page_domain_publish_system_color',
    'mvdm_softpc_wow_page_domain_client_desktop_ready',
    'mvdm_softpc_wow_page_domain_set_client_desktop',
    'mvdm_softpc_wow_page_domain_clear_client_desktop',
    'mvdm_softpc_wow_page_domain_publish_handle',
    'mvdm_softpc_wow_page_domain_retire_handle',
    'mvdm_softpc_wow_page_domain_active',
    'mvdm_softpc_wow_page_domain_refresh_window',
    'mvdm_softpc_wow_page_domain_callback_window',
    'mvdm_softpc_wow_page_domain_restore_callback',
    'mvdm_softpc_wow_page_domain_publish_class',
    'mvdm_softpc_wow_page_domain_retire_class',
    'ntvdm_shadow_reg_open_key_ex',
    'ntvdm_shadow_reg_query_value_ex',
    'ntvdm_shadow_reg_query_info_key',
    'ntvdm_shadow_reg_enum_value',
    'ntvdm_shadow_reg_close_key',
    'opennt_exit_thread=_opennt_exit_thread@4',
    'opennt_support_current_teb=_opennt_support_current_teb@0',
    'mvdm_softpc_effective_address'
    'mvdm_softpc_protected_address'
)) {
    $wow32ProviderExportLines.Add('    ' + $entry)
}
$wow32ProviderExportText = ($wow32ProviderExportLines -join [Environment]::NewLine) + [Environment]::NewLine
if (!(Test-Path -LiteralPath $wow32ProviderExportDefinition) -or
    [IO.File]::ReadAllText($wow32ProviderExportDefinition) -cne $wow32ProviderExportText) {
    [IO.File]::WriteAllText($wow32ProviderExportDefinition,
        $wow32ProviderExportText, [Text.UTF8Encoding]::new($false))
}
$embeddedRomResource = Get-Content -LiteralPath $hostEntryResourceSource -Raw
foreach ($romSource in $embeddedRomSources) {
    if (!(Test-Path -LiteralPath $romSource -PathType Leaf)) {
        throw "Required embedded SoftPC ROM missing: $romSource"
    }
    $romName = [IO.Path]::GetFileName($romSource)
    $romPath = $romSource.Replace('\', '/')
    $romResourceName = switch ($romName) {
        'bios1.rom' { 'SOFTPC_BIOS1'; break }
        'bios4.rom' { 'SOFTPC_BIOS4'; break }
        'v7vga.rom' { 'SOFTPC_V7VGA'; break }
        default { throw "Unexpected embedded SoftPC ROM: $romName" }
    }
    # ROMS_REZ_ID remains SoftPC's internal API selector.  RCDATA is the
    # Win32 resource class; only the active CPU40 firmware triplet is emitted:
    # profile.spc, cmos.ram and bios2.rom do not belong to the EXE contract.
    $embeddedRomResource += "`r`n$romResourceName RCDATA `"$romPath`"`r`n"
}
# Generating a test target must not invalidate an unchanged production ROM.
if (!(Test-Path -LiteralPath $embeddedRomResourceSource) -or
    [IO.File]::ReadAllText($embeddedRomResourceSource, [Text.Encoding]::ASCII) -cne $embeddedRomResource) {
    [IO.File]::WriteAllText($embeddedRomResourceSource, $embeddedRomResource,
        [Text.Encoding]::ASCII)
}
if ([string]::IsNullOrWhiteSpace($NodeExecutable)) { $NodeExecutable = $env:MVDM_NODE22 }
if ([string]::IsNullOrWhiteSpace($NodeExecutable) -or !(Test-Path -LiteralPath $NodeExecutable -PathType Leaf)) {
    throw 'Node 22 is required for the GDP slot generator; pass -NodeExecutable or set MVDM_NODE22.'
}
function Get-NodeSha256([string]$Path) {
    # The supported desktop PowerShell host can lack Get-FileHash.  The
    # formal graph already requires this exact Node 22 runtime for generated
    # carriers, so use it for deterministic patch provenance as well.
    $hash = & $NodeExecutable -e "const fs=require('fs');const crypto=require('crypto');process.stdout.write(crypto.createHash('sha256').update(fs.readFileSync(process.argv[1])).digest('hex'));" $Path
    if ($LASTEXITCODE -ne 0 -or $hash -notmatch '^[0-9a-f]{64}$') {
        throw "Node SHA-256 failed for: $Path"
    }
    return $hash
}
# Ninja schedules translation units itself.  The runner initializes the Visual
# Studio environment exactly once before Ninja starts, so rules invoke the MSVC
# tools directly.  Do not add /MP: nesting it below Ninja's job scheduler would
# oversubscribe the host and make builds less predictable.
$parallelRunner = Join-Path $build 'run-ninja-parallel.cmd'
@('@echo off',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%',
  ('if "%MVDM_BUILD_JOBS%"=="" set "MVDM_BUILD_JOBS=' + $ParallelJobs + '"'),
  ('"' + $nativeNinja + '" -C "' + $build + '" -j %MVDM_BUILD_JOBS% %*')) |
    Set-Content -LiteralPath $parallelRunner -Encoding ascii

$includeRootPaths = @(
    'src',
    'src/vdmredir-dll/include',
    'src/ntvdm-exe/redir/include',
    'src/opennt-host/netapi/xactsrv',
    'src/ntvdm-exe/vdd/include',
    # The adapter owns the modern `nt.h` type binding. Original reached NT
    # public-header subsets are restored under opennt-host below, so source
    # files still resolve historical short names without an adapter copy.
    'src/opennt-abi/host-compat/include',
    # Reached original non-MVDM OpenNT declaration slices retain their
    # source identity under opennt-host. Keep this after the adapter's nt.h:
    # nt.h owns modern type binding while this directory owns original
    # public-header subsets such as ntexapi.h.
    'src/opennt-host/public/sdk/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    # VDMREDIR's original Net/RPC and DLC bodies consume these retained
    # declaration-only OpenNT ABI headers.  They are original include roots,
    # not replacement implementations.
    'src/opennt-abi/source/public/internal/ds/inc',
    'src/opennt-abi/source/public/internal/net/inc',
    'src/opennt-abi/source/private/inc',
    'src/opennt-abi/source/private/ds/netapi/rpcxlate',
    'src/opennt-abi/source/private/windows/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/mvdm/inc',
    # The selected x86 product generator output is absent from the source
    # union, so it remains a bounded SoftPC adapter carrier, not a new mirror
    # file under mvdm.
    'src/ntvdm-exe/softpc/include/generated/x86/prod',
    'src/mvdm/xms.486',
    # DPMI's original precompiled header owns dpmidata.h beside its source
    # bodies.  Select the original directory rather than copying the carrier
    # into an adapter or overlay.
    'src/mvdm/dpmi32',
    'src/mvdm/vdmredir',
    'src/mvdm/softpc.new/base/ccpu386',
    'src/mvdm/softpc.new/host/inc',
    # Original sas.h includes generated sas4gen.h. The selected mirror retains
    # the CVIDC generated carrier; the historical host/genPg output is absent.
    'src/mvdm/softpc.new/base/cvidc',
    'src/mvdm/dos/dem',
    'src/mvdm/softpc.new/base/inc',
    'src/ntvdm-exe/softpc/include',
    'src/ntvdm-exe/command/include',
    'src/ntvdm-exe/monitor/include',
    'src/wow32-dll/include',
    'src/ntvdm-exe/wow/include',
    'src/ntvdm-exe/session'
)
$includeRoots = $includeRootPaths | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$softpcIncludeRootPaths = [System.Collections.Generic.List[string]]::new()
foreach ($path in $includeRootPaths) {
    if ($path -eq 'src/mvdm/softpc.new/base/inc') { continue }
    $softpcIncludeRootPaths.Add($path)
    if ($path -eq 'src/opennt-abi/host-compat/include') {
        # SoftPC's original `config.h` must precede the unrelated OpenNT Net
        # `config.h`.  This is an original header-basename disambiguation,
        # not a source divergence.  Non-SoftPC MVDM packages retain the
        # ordinary OpenNT include order below.
        $softpcIncludeRootPaths.Add('src/mvdm/softpc.new/base/inc')
    }
}
$softpcIncludeRoots = $softpcIncludeRootPaths | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$cvidcGeneratedRoot = Join-Path $build 'generated'
$cvidcGeneratedInclude = '/I "' + (NinjaPath $cvidcGeneratedRoot) + '"'

# `base/inc/egacpu.h` deliberately includes one of two original generated
# `evidgen.h` variants.  The C-video and base/video translation units require
# the C-video variant; CCPU itself keeps its own shorter generated variant.
# This is original generated-header provenance, not a product-wide include
# preference or a replacement API.
$cvidcFirstRootPaths = [System.Collections.Generic.List[string]]::new()
foreach ($path in $softpcIncludeRootPaths) {
    if ($path -eq 'src/mvdm/softpc.new/base/cvidc') { continue }
    if ($path -eq 'src/mvdm/softpc.new/base/ccpu386') {
        $cvidcFirstRootPaths.Add('src/mvdm/softpc.new/base/cvidc')
    }
    $cvidcFirstRootPaths.Add($path)
}
$cvidcFirstIncludeRoots = $cvidcFirstRootPaths | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }

# `i386` is never a product-wide host switch. The current CCPU recovery build
# retains its original CPU40 compatibility carrier definitions; any unavoidable historical x86-only unit must carry
# its own registered, target-local compilation exception.
# `softpc.new/obj.vdm/cdefine.inc` selects CCPU's generated C video-memory
# backend and V7 VGA firmware profile for the non-V86 CPU40 configuration.
# The selected CCPU40 product has no hardware-V86 route, so it retains those
# original carriers on both host-width rows. This selects the byte-exact
# `v7vga.rom` package already retained by mvdm-softpc-firmware rather than
# fabricating an unavailable `vga.rom` alias.
$baseCommonFlags = '/nologo /TC /c /MT /W4 /showIncludes /D_NO_CRT_STDIO_INLINE /DWIN32 /DWINNT /DOPENNT_ADAPTER_NT_ALERT_THREAD /DMVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DC_VID /DSPC386 /DSIM32 /DV7VGA /DANSI /DPROD ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include/nt.h')) + '" ' +
    '/FI "' + (NinjaPath $softpcSymbolCompat) + '" ' +
    ''
$baseFlags = $baseCommonFlags + ($softpcIncludeRoots -join ' ')
$hostFlags = $baseFlags + ' /FI "' + (NinjaPath $hostCrtRedirect) + '"'
# XACTSRV's selected local-provider bodies retain the original RAP-to-native
# conversion.  On modern Windows the ANSI NetUse entry no longer preserves the
# `USE_INFO_1` contract, whereas its native Unicode counterpart does.  Select
# the historical native-TCHAR build profile only for this finite translator
# package; VDMREDIR's DOS/RAP input remains OEM/16-bit exactly as before.
$xactSrvFlags = $baseFlags + ' /DUNICODE /D_UNICODE'
# The selected original x86 OpenNT composition is not the generic CCPU40
# profile above.  `softpc.new/obj.vdm/cdefine.inc` selects MONITOR, C_VID and
# X86GFX together on 386.  CPU40 is nevertheless a distinct executor profile:
# applying MONITOR to its generic host units selects retired CPU30 monitor
# state.  The selected closure therefore applies these defines only to the
# source-proven NTIO/video consumers below; CPU_40_STYLE/CCPU remains the sole
# executor.  In particular, this restores the original NTIO.SYS BOP vector
# handoff used by video instead of the non-MONITOR host_simulate path.
# The original OpenNT ABI header exposes its user-mode VDM TIB under _X86_.
# This is a declaration gate for the selected Win32/x86 build, not the retired
# CPU_30_STYLE V86 monitor selection.  The x64 row must not impersonate that
# ABI: any needed host-width-neutral declaration belongs at the existing
# adapter boundary.  DPMI receives the original header from opennt-abi; the
# adapter supplies only session TLS storage.
$dpmiArchitectureFlags = if ($Architecture -eq 'x86') { ' /D_X86_ ' } else { ' ' }
$dpmiFlags = $baseCommonFlags + $dpmiArchitectureFlags + ($includeRoots -join ' ')
$cvidcFirstFlags = $baseCommonFlags + ($cvidcFirstIncludeRoots -join ' ') + ' ' + $cvidcGeneratedInclude
$cvidcRuleFlags = $cvidcFirstFlags
# `accessfn.c` is selected by the original C-VID manifest as the non-CCPU
# vector facade for host/DPMI callers.  It must keep its original non-PROD
# body, but must not inherit CCPU: under CCPU its macro spellings become a
# second `c_cpu_*` executor provider.  The one translation unit therefore
# retains the same original vector call shape without claiming CCPU ownership.
$cvidcAccessFlags = $cvidcRuleFlags.Replace('/DCCPU ', '').Replace('/DPROD ', '')
$patchVectorDefaultsFlags = $baseFlags + ' /DMVDM_CCPU_VECTOR_DEFAULTS_ONLY'
$patchActivityCheckFlags = $baseFlags + ' /DMVDM_CCPU_ACTIVITY_CHECK_ONLY'

$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('build_root = ' + (NinjaPath $build))
$graph.Add('cflags = ' + $baseFlags)
$graph.Add('host_cflags = ' + $hostFlags)
$graph.Add('rtl_cflags = ' + $baseFlags + ' /Gz')
$graph.Add('dpmi_cflags = ' + $dpmiFlags)
$graph.Add('cvidc_first_cflags = ' + $cvidcFirstFlags)
$graph.Add('cvidc_rule_cflags = ' + $cvidcRuleFlags)
$graph.Add('cvidc_access_cflags = ' + $cvidcAccessFlags)
$graph.Add('patch_vector_defaults_cflags = ' + $patchVectorDefaultsFlags)
$graph.Add('patch_activity_check_cflags = ' + $patchActivityCheckFlags)
$graph.Add('rcflags = ' + ($includeRoots -join ' '))
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cl.exe $cflags /Fo$out $in')
$graph.Add('  description = CC $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_host')
$graph.Add('  command = cl.exe $host_cflags /Fo$out $in')
$graph.Add('  description = CC-HOST $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_rtl')
$graph.Add('  command = cl.exe $rtl_cflags /Fo$out $in')
$graph.Add('  description = CC-RTL $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule asm_x86')
$graph.Add('  command = ml.exe /nologo /c /coff /Fo$out $in')
$graph.Add('  description = ASM-X86 $in')
$graph.Add('rule cc_cvidc')
$graph.Add('  command = cl.exe $cvidc_first_cflags /Fo$out $in')
$graph.Add('  description = CC-CVIDC $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_dpmi')
$graph.Add('  command = cl.exe $dpmi_cflags /Fo$out $in')
$graph.Add('  description = CC-DPMI $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_cvidc_rule')
$graph.Add('  command = cl.exe $cvidc_rule_cflags /Fo$out $in')
$graph.Add('  description = CC-CVIDC-RULE $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_cvidc_access')
$graph.Add('  command = cl.exe $cvidc_access_cflags /Fo$out $in')
$graph.Add('  description = CC-CVIDC-ACCESS $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_patch_vector_defaults')
$graph.Add('  command = cl.exe $patch_vector_defaults_cflags /Fo$out $in')
$graph.Add('  description = CC-PATCH $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_patch_activity_check')
$graph.Add('  command = cl.exe $patch_activity_check_cflags /Fo$out $in')
$graph.Add('  description = CC-PATCH $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule cc_mvdm_debugger')
$graph.Add('  command = cl.exe $cflags /D_NTDBG_ /Fo$out $in')
$graph.Add('  description = CC-MVDM-DEBUGGER $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('rule rc')
$graph.Add('  command = rc.exe /nologo $rcflags /fo$out $in')
$graph.Add('  description = RC $in')
$graph.Add('rule lib')
$graph.Add('  command = lib.exe /nologo /out:$out $in')
$graph.Add('  description = LIB $out')
$graph.Add('rule forced_link_audit')
# This deliberately produces a non-runnable DLL.  /WHOLEARCHIVE makes the
# candidate's complete original membership visible to LINK; /FORCE keeps the
# unresolved physical forms in the adjacent log for source-first ownership.
    $graph.Add('  command = link.exe /nologo /dll /noentry /force:unresolved /force:multiple /out:$out /implib:$out.lib /wholearchive:original-ccpu386.lib /wholearchive:original-softpc-bios.lib /wholearchive:original-softpc-keymouse.lib /wholearchive:original-softpc-system.lib /wholearchive:original-softpc-disks.lib /wholearchive:original-softpc-support.lib /wholearchive:original-softpc-video.lib /wholearchive:original-softpc-cvidc.lib /wholearchive:original-softpc-comms.lib /wholearchive:original-softpc-dos.lib /wholearchive:original-mvdm-dem.lib /wholearchive:original-mvdm-command.lib /wholearchive:original-mvdm-xms.lib /wholearchive:original-mvdm-dpmi32.lib /wholearchive:original-mvdm-host-suballoc.lib /wholearchive:original-mvdm-host-oemuni.lib /wholearchive:original-softpc-base-trace.lib /wholearchive:original-softpc-host-roots.lib /wholearchive:softpc-bindings.lib /wholearchive:softpc-win32-bindings.lib worker-base.lib /wholearchive:monitor-bindings.lib /wholearchive:debugger-bindings.lib /wholearchive:session.lib /wholearchive:mvdm-softpc-effective-address.lib softpc-ccpu-vector-defaults.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
$graph.Add('rule redir_dll_link')
$graph.Add('  restat = 1')
# Keep the original VDMREDIR DLL boundary and entry contract.  The OpenNT
# `sources` manifest selects DLLENTRY=VrDllInitialize; without that entry the
# redirector's process-attach initialization (including named-pipe queues and
# locks) never runs.  The parent import library is an implicit output of the
# original process link, so this rule cannot create a second CCPU/SoftPC
# executor inside the DLL.
$graph.Add('  command = link.exe /nologo /dll /entry:mvdm_vdmredir_dll_entry@12 /def:' + (NinjaPath $redirExportDefinition) + ' /implib:$out.lib /out:$out $in kernel32.lib advapi32.lib netapi32.lib rpcrt4.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
$graph.Add('rule broker_test_link')
$graph.Add('  command = link.exe /nologo /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib')
$graph.Add('rule rtl_fixture_link')
$graph.Add('  command = link.exe /nologo /out:$out $in kernel32.lib user32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib')

$ccpuObjects = foreach ($name in $ccpuNames) {
    $object = 'obj/ccpu/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    # Existing localfm.c retains this profile's original Gdp/Cpu/Video state
    # declarations while omitting the already-owned Sas vector.
    $source = Join-Path $ccpuRoot $name
$graph.Add('build ' + $object + ': cc ' + (NinjaPath $source))
    $object
}
$biosObjects = foreach ($name in $biosNames) {
    $object = 'obj/bios/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $biosRoot $name)))
    if ($name -eq 'bios.c' -and $Architecture -eq 'x86') {
        # The original BIOS BOP table publishes FD only in its monitor
        # product profile.  CPU40 uses the same original DOSX continuation
        # frame through dpmi32/modesw.c's bounded counterpart.
        $graph.Add('  cflags = ' + $baseFlags + ' /DMONITOR')
    }
    $object
}
$keymouseObjects = foreach ($name in $keymouseNames) {
    $object = 'obj/keymouse/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $keymouseRoot $name)))
    if ($name -eq 'keybd_io.c') {
        $graph.Add('  cflags = ' + $baseFlags + ' /DMONITOR')
    }
    $object
}
$systemObjects = foreach ($name in $systemNames) {
    $object = 'obj/system/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $systemRoot $name)))
    if ($name -eq 'rom.c') {
        # `rom.c` is the explicit kernel-product boundary within the original
        # x86 profile: X86GFX excludes its generic ROM scratch implementation
        # in favour of the kernel VDM's pre-resident-ROM shell.  That shell is
        # not a finite standalone ABI, so retain the selected original
        # resource/SAS-copy route.  The NTIO handoff is selected only in its
        # direct consumers, not by reviving the retired monitor product shell.
        $graph.Add('  cflags = ' + $baseFlags + ' /DMVDM_SOFTPC_RECOVER_ROM_RESIDENCY')
    }
    $object
}
$disksObjects = foreach ($name in $disksNames) {
    $object = 'obj/disks/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $disksRoot $name)))
    $object
}
$supportObjects = foreach ($name in $supportNames) {
    $object = 'obj/support/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $supportRoot $name)))
    $object
}
$videoObjects = foreach ($name in $videoNames) {
    $object = 'obj/video/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    # Compile every selected original base/video translation unit directly.
    # The removed generated source carrier existed only for deferred x64 ABI
    # repair and must not define x86 controller behavior.
    $graph.Add('build ' + $object + ': cc_cvidc_rule ' + (NinjaPath (Join-Path $videoRoot $name)))
    if ($name -eq 'video.c') {
        # The original x86 NTIO path is confined to this controller: it
        # performs INT 10 through the caller recorded by BOP 5F.  Applying
        # MONITOR/X86GFX to the other controller TUs would instead select NT4 native
        # ROM/fullscreen providers, outside the standalone Console boundary.
        # NT4's MONITOR profile directly writes the kernel-provided low
        # B8000 alias.  A standalone worker has no such alias: select the
        # original SAS write branch while retaining the MONITOR-only NTIO
        # vector setup above.
        $graph.Add('  cvidc_rule_cflags = ' + $cvidcRuleFlags + ' /DMONITOR /DX86GFX /DMVDM_STANDALONE_SAS_VIDEO')
    }
    $object
}
$cvidcObjects = foreach ($name in $cvidcNames) {
    $object = 'obj/cvidc/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $rule = if ($name -eq 'accessfn.c') { 'cc_cvidc_access' } else { 'cc_cvidc_rule' }
    $graph.Add('build ' + $object + ': ' + $rule + ' ' + (NinjaPath (Join-Path $cvidcRoot $name)))
    $object
}
$cvidcVectorBindingSource = Join-Path $adapterSoftpcRoot 'mvdm_cvidc_vector_binding.c'
$cvidcCpuBindingInclude = Join-Path $build 'generated/cvidc_cpu_binding.inc'
$cvidcCpuBindingGenerator = Join-Path $root 'tools/build/GenerateCvidcCpuBinding.mjs'
& $NodeExecutable $cvidcCpuBindingGenerator $cvidcCpuBindingInclude (Split-Path $cvidcRoot -Parent)
if ($LASTEXITCODE -ne 0) { throw 'C-VID CPU binding generation failed.' }
$graph.Add('build obj/cvidc/mvdm_cvidc_vector_binding.obj: cc_cvidc_rule ' + (NinjaPath $cvidcVectorBindingSource) + ' | ' + (NinjaPath $cvidcCpuBindingInclude))
$cvidcObjects += 'obj/cvidc/mvdm_cvidc_vector_binding.obj'
$cvidcAccessorSource = Join-Path $root 'src/mvdm/softpc.new/base/cpu/src/evid/vglob.c'
$graph.Add('build obj/cvidc/vglob.obj: cc_cvidc_rule ' + (NinjaPath $cvidcAccessorSource))
$cvidcObjects += 'obj/cvidc/vglob.obj'
$commsObjects = foreach ($name in $commsNames) {
    $object = 'obj/comms/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $commsRoot $name)))
    if ($name -eq 'printer.c') {
        # Original NTIO.SYS vector setup initializes this original printer
        # status-table provider.  The function is gated independently of the
        # retired CPU30 execution shell.
        $graph.Add('  cflags = ' + $baseFlags + ' /DMONITOR')
    }
    $object
}
$dosObjects = foreach ($name in $dosNames) {
    $object = 'obj/dos/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $dosRoot $name)))
    if ($name -eq 'emm_mngr.c') {
        # The CCPU40 source and its selected `LIM` profile call the exact
        # original writeback bodies which the historical NTVDM gate excludes.
        # This flag opens only that unchanged source block (DIV-148).
        $graph.Add('  cflags = ' + $baseFlags + ' /DMVDM_SOFTPC_RECOVER_LIM_WRITEBACK')
    }
    $object
}
$demObjects = foreach ($name in $demNames) {
    $object = 'obj/dem/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $demRoot $name)))
    # Exact original `sources` contract: this package exposes its Win32 and
    # DEVL trace declarations through these compile definitions.  They do not
    # add a new product path; the source owns the zero-initialized trace mask.
    $graph.Add('  cflags = ' + $baseFlags + ' /DWIN_32 /DDEVL')
    $object
}
$commandObjects = foreach ($name in $commandNames) {
    $object = 'obj/command/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $commandRoot $name)))
    if ($name -in @('cmdmisc.c','cmdpif.c')) {
        $graph.Add('  cflags = ' + $baseFlags + ' /FI "' +
            (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include/console_title.h')) + '"')
    }
    $object
}
$redirThreadCompat = NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include/thread_start_compat.h')
$redirObjects = foreach ($name in $redirNames) {
    $object = 'obj/redir/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $redirRoot $name)))
    # Preserve original redirector source spelling.  Two original DLC/pipe
    # entries use the historical cdecl CreateThread callback contract; this
    # translation-unit-local forced facade is the finite modern ABI boundary.
    $redirFlags = $baseFlags + ' /DWIN_32 /DVDMREDIR_DLL /FI "' + $redirThreadCompat + '"'
    $graph.Add('  cflags = ' + $redirFlags)
    $object
}
$xmsObjects = foreach ($name in $xmsNames) {
    $object = 'obj/xms/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $xmsRoot $name)))
    # Exact original XMS manifest `C_DEFINES=-DWIN_32`.
    $graph.Add('  cflags = ' + $baseFlags + ' /DWIN_32')
    $object
}
$dpmiObjects = foreach ($name in $dpmiNames) {
    $object = 'obj/dpmi/' + ($name -replace '[\\/]', '_').Replace('.c', '.obj')
    $graph.Add('build ' + $object + ': cc_dpmi ' + (NinjaPath (Join-Path $dpmiRoot $name)))
    # This package needs the original monitor declaration carrier but does
    # not define the NT4 kernel-VDM `i386` branch globally.
    $graph.Add('  dpmi_cflags = ' + $dpmiFlags + ' /DLINKED_INTO_MONITOR')
    $object
}
$suballocObjects = foreach ($name in $suballocNames) {
    $object = 'obj/suballoc/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $suballocRoot $name)))
    $object
}
$oemuniObjects = foreach ($name in $oemuniNames) {
    $object = 'obj/oemuni/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $oemuniRoot $name)))
    $object
}
$sessionObjects = foreach ($name in $sessionNames) {
    $object = 'obj/ntvdm/session/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $sessionRoot $name)))
    $object
}
$adapterWowWorkerObjects = foreach ($name in $adapterWowWorkerNames) {
    $object = 'obj/adapter-wow-worker/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterWowRoot $name)))
    $object
}
$cpu40DescriptorDomainFixtureObject = 'obj/tests/cpu40_descriptor_domain_fixture.obj'
$graph.Add('build ' + $cpu40DescriptorDomainFixtureObject + ': cc_dpmi ' + (NinjaPath $cpu40DescriptorDomainFixtureSource))
$graph.Add('  dpmi_cflags = ' + $dpmiFlags + ' /DLINKED_INTO_MONITOR')
$rtlX86FixtureObject = 'obj/tests/rtl_x86_fixture.obj'
$graph.Add('build ' + $rtlX86FixtureObject + ': cc ' + (NinjaPath $rtlX86FixtureSource))
$environmentProjectionFixtureObject = 'obj/tests/environment_projection_fixture.obj'
$graph.Add('build ' + $environmentProjectionFixtureObject + ': cc ' + (NinjaPath $environmentProjectionFixtureSource))
$cvidcVectorBindingFixtureObject = 'obj/tests/cvidc_vector_binding_fixture.obj'
# This fixture must see C-VID's generated (non-CCPU) CpuVector view, just as
# accessfn.c does; it therefore validates the actual cross-profile ABI seam.
$graph.Add('build ' + $cvidcVectorBindingFixtureObject + ': cc_cvidc_access ' + (NinjaPath $cvidcVectorBindingFixtureSource))
$x87LayoutFixtureObject = 'obj/tests/x87_layout_fixture.obj'
$graph.Add('build ' + $x87LayoutFixtureObject + ': cc ' + (NinjaPath $x87LayoutFixtureSource))
$cvidcVectorProviderStubSource = 'generated/cvidc_vector_provider_stubs.c'
$cvidcVectorProviderStubObject = 'obj/tests/cvidc_vector_provider_stubs.obj'
$graph.Add('rule generate_cvidc_vector_provider_stubs')
$graph.Add('  command = "' + (NinjaPath $NodeExecutable) + '" "' + (NinjaPath $cvidcVectorProviderStubGenerator) + '" $out "' + (NinjaPath (Join-Path $build 'generated/cvidc_cpu_binding.inc')) + '"')
$graph.Add('  description = GEN-CVIDC-PROVIDER-STUBS $out')
$graph.Add('build ' + $cvidcVectorProviderStubSource + ': generate_cvidc_vector_provider_stubs generated/cvidc_cpu_binding.inc')
$graph.Add('build ' + $cvidcVectorProviderStubObject + ': cc ' + $cvidcVectorProviderStubSource)
$baseDebugObjects = foreach ($name in $baseDebugNames) {
    $object = 'obj/base-debug/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $baseDebugRoot $name)))
    $object
}
$hostObjects = foreach ($name in $hostNames) {
    $object = 'obj/host/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
$graph.Add('build ' + $object + ': cc_host ' + (NinjaPath (Join-Path $hostRoot $name)))
    if ($name -eq 'nt_lpt.c') {
        # The original NTIO printer-table handoff is a finite VDM_TIB data
        # registration path.  Its selected kernel semantic carrier is linked
        # separately; this does not select the CPU30 monitor executor.
        $graph.Add('  host_cflags = ' + $hostFlags + ' /DMONITOR')
    } elseif ($name -in @('nt_timer.c', 'nt_thred.c', 'nt_com.c', 'nt_event.c', 'nt_error.c')) {
        $threadCompat = NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include/thread_start_compat.h')
        # `nt.h` from the modern SDK can predefine the historical include
        # guard before the original source reaches <ntexapi.h>.  Force the
        # selected opennt-host subset after nt.h so the original timer's
        # declarations stay visible on both architectures.
        $ntexapiSubset = NinjaPath (Join-Path $root 'src/opennt-host/public/sdk/inc/ntexapi.h')
        $threadAbiFlag = if ($name -eq 'nt_thred.c') { ' /DOPENNT_HOST_CREATE_THREAD_ABI' } else { '' }
        $graph.Add('  host_cflags = ' + $hostFlags + $threadAbiFlag + ' /FI "' + $ntexapiSubset + '" /FI "' + $threadCompat + '"')
    }
    $object
}
# Focused host fixtures exercise the original CPU/host bodies with their own
# test main.  Keep that archive identical except for the product's original
# ntvdm.c process entry; it is neither a second worker entry nor a substitute
# host implementation.
$hostFixtureObjects = @($hostObjects)
$hostEntryObject = 'obj/host/ntvdm_entry.obj'
$graph.Add('build ' + $hostEntryObject + ': cc_host ' + (NinjaPath $hostEntrySource))
$graph.Add('build obj/host/softpc-resource.res: rc ' + (NinjaPath $embeddedRomResourceSource))
$hostObjects += $hostEntryObject
$adapterWin32Objects = foreach ($name in $adapterWin32Names) {
    $object = 'obj/adapter-win32/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterWin32Root $name)))
    $object
}
$hostCompatObject = 'obj/opennt-abi-host-compat/opennt_support_rtl.obj'
$graph.Add('build ' + $hostCompatObject + ': cc ' + (NinjaPath $hostCompatSource))
$adapterWin32Objects += $hostCompatObject
$consoleGridObject = 'obj/opennt-abi-host-compat/console_grid.obj'
$graph.Add('build ' + $consoleGridObject + ': cc ' + (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/console_grid.c')))
$graph.Add('  cflags = /nologo /c /MT /W4 /we4013 /showIncludes')
$adapterWin32Objects += $consoleGridObject
$adapterBaseSrvObjects = foreach ($name in $adapterBaseSrvNames) {
    $object = 'obj/adapter-basesrv/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterBaseSrvRoot $name)))
    $object
}
$adapterMonitorObjects = foreach ($name in $adapterMonitorNames) {
    $object = 'obj/adapter-monitor/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterMonitorRoot $name)))
    $object
}
$adapterDebuggerRoot = Join-Path $root 'src/ntvdm-exe/debugger/source'
$adapterDebuggerObjects = foreach ($name in $adapterDebuggerNames) {
    $object = 'obj/adapter-debugger/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterDebuggerRoot $name)))
    $object
}
$originalDebuggerObject = 'obj/mvdm-debugger/dbg.obj'
$graph.Add('build ' + $originalDebuggerObject + ': cc_mvdm_debugger ' +
    (NinjaPath (Join-Path $root 'src/mvdm/dbg/dbg.c')))
$adapterSoftpcObjects = foreach ($name in $adapterSoftpcNames) {
    $object = 'obj/adapter-softpc/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterSoftpcRoot $name)))
    $object
}
$adapterRedirObjects = foreach ($name in $adapterRedirNames) {
    $object = 'obj/vdmredir-dll/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $redirDllBindingRoot $name)))
    $object
}
$adapterRedirWorkerObjects = foreach ($name in $adapterRedirWorkerNames) {
    $object = 'obj/adapter-redir/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterRedirRoot $name)))
    $object
}
$adapterSoftpcFixtureObjects = @($adapterSoftpcObjects | Where-Object {
    $_ -ne 'obj/adapter-softpc/mvdm_standalone_worker.obj'
})
$openntRtlX86Objects = foreach ($name in $openntRtlX86Names) {
    $object = 'obj/opennt-rtl-x86/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': asm_x86 ' + (NinjaPath (Join-Path $openntRtlX86Root $name)))
    $object
}
$openntRtlObjects = foreach ($name in $openntRtlNames) {
    $object = 'obj/opennt-rtl/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc_rtl ' + (NinjaPath (Join-Path $openntRtlRoot $name)))
    if ($name -eq 'time.c') {
        # Modern user-mode headers no longer expose NT4's complete time-of-day
        # layout.  The selected original owner supplies only the conversion
        # actually needed by VDMREDIR.
        $graph.Add('  rtl_cflags = ' + $baseFlags + ' /DMVDM_STANDALONE_TIME1970_ONLY')
    }
    # error.c's non-kernel branch only emits historical DbgPrint diagnostics.
    # The selected standalone closure has no kernel debug provider, while the
    # original mapping and LastStatusValue logic is identical in this branch.
    if ($name -eq 'error.c') {
        $graph.Add('  rtl_cflags = ' + $baseFlags + ' /Gz /DNTOS_KERNEL_RUNTIME')
    }
    $object
}
$kernelVdmPrinterObject = 'obj/kernel-vdm/monitor_printer.obj'
$graph.Add('build ' + $kernelVdmPrinterObject + ': cc ' + (NinjaPath $kernelVdmPrinterSource))
$adapterVddObjects = foreach ($name in $adapterVddNames) {
    $object = 'obj/adapter-vdd/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterVddRoot $name)))
    $object
}
$openntNetlibObjects = foreach ($name in $openntNetlibNames) {
    $object = 'obj/opennt-netlib/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $openntNetlibRoot $name)))
    if ($name -eq 'allocstr.c') {
        # The original unit contains NetpAllocTStrFromString, a NetConfig
        # helper not reached by the selected Redirector slice.  Function
        # COMDATs let the final /OPT:REF link retain the byte-identical
        # directly reached allocation algorithms without importing the wider
        # historical RPC/MIDL NetAPI product shell solely for that dead edge.
        $graph.Add('  cflags = ' + $baseFlags + ' /Gy')
    }
    if ($name -eq 'packstr.c') {
        # ServiceControl reaches only the original fixed/variable string
        # packer.  Its unrelated RPC enum-buffer allocator is not admitted.
        $graph.Add('  cflags = ' + $baseFlags + ' /DMVDM_XACTSRV_PACKSTR_COPY_ONLY')
    }
    $object
}
$openntNetapiObjects = foreach ($name in $openntNetapiNames) {
    $object = 'obj/opennt-netapi-api/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $openntNetapiRoot $name)))
    $object
}
$openntXactSrvObjects = foreach ($name in $openntXactSrvNames) {
    $object = 'obj/opennt-xactsrv/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $openntXactSrvRoot $name)))
    $graph.Add('  cflags = ' + $xactSrvFlags)
    $object
}
$openntBaseVdmObjects = foreach ($name in $openntBaseVdmNames) {
    $object = 'obj/opennt-base-vdm/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $openntBaseVdmRoot $name)))
    $object
}
# T412 owner archives are explicit targets until the three-process links replace
# the legacy local provider. No fixture object or generated declaration enters
# these libraries; their process/transport imports remain composition edges.
$baseOwnerManifest = @()
$brokerTransportManifest = @()
if ($Architecture -eq 'x86') {
    $baseBindingInclude = NinjaPath (Join-Path $root 'src/ntsrv-exe/opennt/include')
    $baseOwnerFlags = $baseFlags + ' /we4013 /I "' + $baseBindingInclude + '"' +
        ' /I "' + (NinjaPath (Join-Path $root 'src/opennt-host/base/win32/inc')) + '"' +
        ' /I "' + (NinjaPath (Join-Path $root 'src/opennt-host/base/win32/server')) + '"'
    $baseServerFlags = $baseOwnerFlags + ' /D_CSRSRV_ /DOPENNT_BASE_VDM_SERVER /FI "' +
        $baseBindingInclude + '/base_server.h"'
    # Select the original configuration closure without importing client RPC
    # dispatch or CSR capture into the service. The function bodies are copied
    # verbatim from the tracked mirror; the generated carrier is build-only.
    $vdmConfigSource = Join-Path $root 'src/opennt-host/base/win32/client/vdm.c'
    $vdmConfigText = [IO.File]::ReadAllText($vdmConfigSource)
    $vdmConfigHelper = [regex]::Matches($vdmConfigText, '(?ms)^static VOID BaseSetLastNTError\(.*?^}')
    $vdmConfigFunction = [regex]::Matches($vdmConfigText, '(?ms)^BOOL\r?\nBaseGetVdmConfigInfo\(.*?^}')
    if ($vdmConfigHelper.Count -ne 1 -or $vdmConfigFunction.Count -ne 1) {
        throw 'Original VDM configuration closure is missing or ambiguous'
    }
    $vdmConfigCarrier = Join-Path $build 'generated/base_vdm_config.c'
    $vdmConfigPreamble = "#include <nt.h>`r`n#include <ntrtl.h>`r`n#include <wchar.h>`r`n#include <basevdm.h>`r`n#include <base_config.h>`r`n#define VDM_TAG 0`r`n#define MAKE_TAG(Tag) 0`r`n"
    [IO.File]::WriteAllText($vdmConfigCarrier, $vdmConfigPreamble +
        $vdmConfigHelper[0].Value + "`r`n" + $vdmConfigFunction[0].Value + "`r`n",
        [Text.UTF8Encoding]::new($false))
    $baseOwnerGroups = @{
        'opennt-base-client' = @(
            @('client', 'src/opennt-host/base/win32/client/vdm.c', ($baseOwnerFlags + ' /Gy /DOPENNT_BASE_CLIENT_VDM_COMMANDS /DOPENNT_BASE_CLIENT_VDM_ENVIRONMENT')),
            @('classifier', 'src/opennt-host/base/win32/client/vdm.c', ($baseOwnerFlags + ' /Gy /DOPENNT_BASE_CLIENT_CLASSIFIER')),
            @('capture', 'src/opennt-host/base/ntdll/csrutil.c', ($baseOwnerFlags + ' /Gz')))
        'opennt-base-server' = @(
            @('srvvdm', 'src/opennt-host/base/win32/server/srvvdm.c', $baseServerFlags),
            @('exports', 'src/opennt-host/windows/core/ntuser/server/exports.c', $baseServerFlags))
        'opennt-base-bindings' = @(
            @('service', 'src/ntsrv-exe/opennt/source/base_service.c', $baseServerFlags),
            @('service-core', 'src/ntsrv-exe/opennt/source/service_core.c', $baseServerFlags),
            @('worker-registry', 'src/ntsrv-exe/opennt/source/worker_registry.c', $baseServerFlags),
            @('frontend-registry', 'src/ntsrv-exe/opennt/source/frontend_registry.c', $baseServerFlags),
            @('native-commands', 'src/ntsrv-exe/opennt/source/native_commands.c', $baseServerFlags),
            @('service-lifecycle', 'src/ntsrv-exe/opennt/source/lifecycle.c', $baseServerFlags),
            @('service-management', 'src/ntsrv-exe/opennt/source/management.c', $baseServerFlags),
            @('worker-spawn', 'src/ntsrv-exe/transport/worker_spawn.c', ('/nologo /c /MT /W4 /we4013 /showIncludes /I "' + (NinjaPath (Join-Path $root 'src')) + '"')),
            @('command', 'src/ntsrv-exe/opennt/source/base_command.c', $baseServerFlags),
            @('values', 'src/ntsrv-exe/opennt/source/base_values.c', $baseServerFlags),
            @('payload', 'src/ntsrv-exe/opennt/source/base_payload.c', $baseServerFlags),
            @('startup', 'src/ntsrv-exe/opennt/source/base_startup.c', $baseOwnerFlags),
            @('dispatch', 'src/ntsrv-exe/opennt/source/base_dispatch.c', $baseServerFlags),
            @('resources', 'src/ntsrv-exe/opennt/source/base_resource.c', ($baseServerFlags + ' /DOPENNT_BASE_NATIVE_RESOURCES')),
            @('streams', 'src/ntsrv-exe/opennt/source/base_stream.c', $baseServerFlags),
            @('waits', 'src/ntsrv-exe/opennt/source/base_wait.c', ($baseServerFlags + ' /DOPENNT_BASE_NATIVE_RESOURCES')),
            @('registry', 'src/ntsrv-exe/opennt/source/base_process.c', $baseServerFlags),
            @('reservation', 'src/ntsrv-exe/opennt/source/base_reservation.c', $baseOwnerFlags),
            @('request', 'src/ntsrv-exe/opennt/source/base_request.c', $baseServerFlags),
            @('config', 'src/ntsrv-exe/opennt/source/base_config.c', $baseOwnerFlags),
            @('process', 'src/ntsrv-exe/opennt/source/base_client_process.c', $baseOwnerFlags),
            @('interactive', 'src/ntsrv-exe/opennt/source/base_interactive.c', $baseOwnerFlags),
            @('classifier-path', 'src/ntsrv-exe/opennt/source/base_classifier_path.c', $baseOwnerFlags))
    }
    foreach ($group in @('opennt-base-client', 'opennt-base-server', 'opennt-base-bindings')) {
        $members = foreach ($member in $baseOwnerGroups[$group]) {
            $object = 'obj/' + $group + '/' + $member[0] + '.obj'
            $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $member[1])))
            $graph.Add('  cflags = ' + $member[2])
            $baseOwnerManifest += [ordered]@{
                archive = $group + '.lib'
                path = $member[1]
                sha256 = Get-NodeSha256 (Join-Path $root $member[1])
                object = $object
                buildDisposition = 'explicit-x86-owner-archive; product selection determined by composition and link map'
            }
            $object
        }
        if ($group -eq 'opennt-base-bindings') {
            $configObject = 'obj/opennt-base-bindings/config-command.obj'
            $graph.Add('build ' + $configObject + ': cc ' + (NinjaPath $vdmConfigCarrier) + ' | ' + (NinjaPath $vdmConfigSource))
            $graph.Add('  cflags = ' + $baseOwnerFlags)
            $members = @($members) + $configObject
            $baseOwnerManifest += [ordered]@{
                archive = $group + '.lib'
                path = 'src/opennt-host/base/win32/client/vdm.c'
                sha256 = Get-NodeSha256 $vdmConfigSource
                object = $configObject
                buildDisposition = 'verbatim BaseGetVdmConfigInfo and BaseSetLastNTError closure; generated build-only carrier; no client RPC or CSR capture'
            }
        }
        $graph.Add('build ' + $group + '.lib: lib ' + ($members -join ' '))
    }
    $transportObjects = foreach ($unit in @('rpc_security', 'vdm_receipt', 'vdm_delivery', 'vdm_payload', 'vdm_message')) {
        $source = 'src/ntsrv-exe/transport/' + $unit + '.c'
        $object = 'obj/broker-transport/' + $unit + '.obj'
        $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source)))
        $graph.Add('  cflags = /nologo /c /MT /W4 /we4013 /showIncludes /I"' + (Join-Path $root 'src') + '"')
        $brokerTransportManifest += [ordered]@{
            archive = 'broker-transport.lib'
            path = $source
            sha256 = Get-NodeSha256 (Join-Path $root $source)
            object = $object
            buildDisposition = 'explicit-x86-native-transport; product-dispatch-integration-pending'
        }
        $object
    }
    $graph.Add('build broker-transport.lib: lib ' + ($transportObjects -join ' '))
    $graph.Add('build obj/tests/vdm_protocol_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/broker/vdm_payload.c')))
    $graph.Add('  cflags = /nologo /c /MT /W4 /we4013 /showIncludes /I"' + (Join-Path $root 'src') + '" /I"' + (Join-Path $root 'src/ntsrv-exe/transport') + '"')
    $graph.Add('build vdm-protocol-test.exe: broker_test_link obj/tests/vdm_protocol_test.obj broker-transport.lib')
    $graph.Add('build opennt-broker-owners: phony opennt-base-client.lib opennt-base-server.lib opennt-base-bindings.lib broker-transport.lib')
    $baseReservationTestObject = 'obj/tests/base_reservation_test.obj'
    $graph.Add('build ' + $baseReservationTestObject + ': cc ' + (NinjaPath $baseReservationTestSource))
    $graph.Add('  cflags = ' + $baseOwnerFlags)
    $graph.Add('build basesrv-reservation-test.exe: broker_test_link ' + $baseReservationTestObject + ' opennt-base-bindings.lib broker-transport.lib')
    $baseServiceReservationTestObject = 'obj/tests/base_service_reservation_test.obj'
    $graph.Add('build ' + $baseServiceReservationTestObject + ': cc ' + (NinjaPath $baseServiceReservationTestSource))
    $graph.Add('  cflags = ' + $baseServerFlags)
    $graph.Add('rule basesrv_service_test_link')
    $graph.Add('  command = link.exe /nologo /out:$out $in ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
    $graph.Add('build obj/tests/base_service_fixture.obj: cc ' + (NinjaPath (Join-Path $root 'tests/adapter-basesrv/base_service_fixture.c')))
    $graph.Add('  cflags = ' + $baseServerFlags)
    $graph.Add('build basesrv-service-reservation-test.exe: basesrv_service_test_link ' + $baseServiceReservationTestObject + ' obj/tests/base_service_fixture.obj obj/run16/support.obj opennt-base-server.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib')
    $graph.Add('build obj/tests/basesrv_idle_policy.obj: cc ' + (NinjaPath (Join-Path $root 'tests/adapter-basesrv/idle_policy_test.c')))
    $graph.Add('  cflags = /nologo /c /MT /W4 /WX /showIncludes /I"' + (Join-Path $root 'src') + '"')
    $graph.Add('build basesrv-idle-policy-test.exe: basesrv_service_test_link obj/tests/basesrv_idle_policy.obj')
    $graph.Add('build obj/tests/base_client_rpc_first.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/base_client_rpc_first_test.c')))
    $graph.Add('  cflags = ' + $baseOwnerFlags)
    $graph.Add('rule base_rpc_test_link')
    $graph.Add('  command = link.exe /nologo /subsystem:console /out:$out $in rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
    $graph.Add('build base-client-rpc-first-test.exe: base_rpc_test_link obj/tests/base_client_rpc_first.obj obj/ntvwm/next_command.obj worker-base.lib frontend-client.lib obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib | ntsrv.exe ntvwm.exe')
    $nativeServiceFlags = '/nologo /c /MT /W4 /we4013 /showIncludes /I obj/basesrv /I "' + (NinjaPath (Join-Path $root 'src')) + '"'
    # Frontend-only nxvm closure. Refuse source drift before emitting objects;
    # no dependency on the sibling repository at build or runtime.
    $windowManifest = Get-Content (Join-Path $frontendRoot 'nxvm-import.json') -Raw | ConvertFrom-Json
    $fontBuild = Join-Path $build 'frontend-font'
    & (Join-Path $root 'tools/build/Generate-FrontendFont.ps1') -OutputFile (Join-Path $fontBuild 'native_pc_font.h')
    if ((Get-NodeSha256 (Join-Path $frontendRoot 'lib/LICENSE.nxvm')) -ne $windowManifest.licenseSha256) {
        throw 'Frontend nxvm license hash mismatch'
    }
    foreach ($row in $windowManifest.files) {
        if ($row.path -notmatch '^lib/(types|base|kvm-base|kvm-window)/[A-Za-z0-9_./-]+$' -or $row.path.Contains('..')) {
            throw "Invalid frontend nxvm path: $($row.path)"
        }
        if ((Get-NodeSha256 (Join-Path $frontendRoot $row.path)) -ne $row.sha256) {
            throw "Frontend nxvm source hash mismatch: $($row.path)"
        }
        if ($row.compile) { $frontendWindowSources += 'src/ntcon-exe/' + $row.path }
    }
    $frontendWindowSources += @('window_frame.c','text_frame.c','window_controller.c','window_input_queue.c','window_keyboard.c','window_mouse.c') |
        ForEach-Object { 'src/ntcon-exe/' + $_ }
    $windowObjects = @()
    foreach ($source in $frontendWindowSources) {
        $object = 'obj/frontend/window/' + $source.Substring('src/ntcon-exe/'.Length).Replace('.c','.obj')
        $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $root $source)))
        $graph.Add('  cflags = ' + $nativeServiceFlags + ' /std:c11 /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0601 /I "' + (NinjaPath $frontendRoot) + '" /I "' + (NinjaPath $fontBuild) + '"')
        $windowObjects += $object
    }
    $graph.Add('build frontend-window.lib: lib ' + ($windowObjects -join ' '))
    $graph.Add('build obj/tests/console_title_native.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_title_native_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build console-title-native-test.exe: broker_test_link obj/tests/console_title_native.obj')
    $graph.Add('build obj/tests/console_keyboard_layout_native.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_keyboard_layout_native_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build console-keyboard-layout-native-test.exe: rtl_fixture_link obj/tests/console_keyboard_layout_native.obj')
    $graph.Add('build obj/tests/console_graphics_native.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_graphics_native_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags + ' /I"' + (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include')) + '"')
    $graph.Add('build console-graphics-native-test.exe: broker_test_link obj/tests/console_graphics_native.obj')
    $graph.Add('build obj/tests/console_bitmap.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_bitmap_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/console_bitmap_provider.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvdm-exe/win32/console_bitmap.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule bitmap_test_link')
    $graph.Add('  command = link.exe /nologo /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib gdi32.lib')
    $graph.Add('build console-bitmap-test.exe: bitmap_test_link obj/tests/console_bitmap.obj obj/tests/console_bitmap_provider.obj')
    $graph.Add('build obj/tests/console_graphics_provider.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvdm-exe/win32/console_graphics.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags + ' /I"' + (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/include')) + '"')
    $graph.Add('build obj/tests/frontend_scope_lifetime.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/frontend_scope_lifetime_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build frontend-scope-lifetime-test.exe: broker_test_link obj/tests/frontend_scope_lifetime.obj obj/run16/frontend_scope.obj obj/frontend/session_service.obj')
    $graph.Add('build obj/tests/native_lifetime_pair.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/native_lifetime_pair_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build native-lifetime-pair-test.exe: broker_test_link obj/tests/native_lifetime_pair.obj')
    $graph.Add('rule native_pair_gui_link')
    $graph.Add('  command = link.exe /nologo /subsystem:windows /entry:mainCRTStartup /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib')
    $graph.Add('build native-lifetime-pair-target.exe: native_pair_gui_link obj/tests/native_lifetime_pair.obj')
    $graph.Add('build obj/run16/entry.obj: cc ' + (NinjaPath (Join-Path $run16Root 'main.c')))
    $graph.Add('  cflags = ' + $baseOwnerFlags)
    $graph.Add('build obj/run16/application_search.obj: cc ' + (NinjaPath (Join-Path $run16Root 'application_search.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/application_search.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/application_search_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build application-search-test.exe: broker_test_link obj/tests/application_search.obj obj/run16/application_search.obj')
    $graph.Add('build obj/common/guest_environment.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/guest_environment.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/guest_environment.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/guest_environment_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build guest-environment-test.exe: broker_test_link obj/tests/guest_environment.obj obj/common/guest_environment.obj')
    $graph.Add('build obj/worker-base/connection.obj: cc ' + (NinjaPath (Join-Path $root 'src/worker-base/connection.c')))
    $graph.Add('build obj/worker-base/shutdown_close.obj: cc ' + (NinjaPath (Join-Path $root 'src/worker-base/shutdown_close.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/ntvwm/next_command.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvwm-exe/next_command.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build worker-base.lib: lib obj/worker-base/connection.obj obj/worker-base/shutdown_close.obj')
    $graph.Add('build obj/tests/worker_shutdown.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/worker_shutdown_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build worker-shutdown-test.exe: base_rpc_test_link obj/tests/worker_shutdown.obj worker-base.lib frontend-client.lib obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib')
    $graph.Add('build obj/common/pipe_transfer.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/transport/pipe_transfer.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-transport.lib: lib obj/common/pipe_transfer.obj')
    $graph.Add('build obj/common/system_root.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/system_root.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-root.lib: lib obj/common/system_root.obj obj/common/guest_environment.obj')
    $graph.Add('build obj/tests/common_system_root.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_system_root_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-system-root-test.exe: broker_test_link obj/tests/common_system_root.obj common-root.lib')
    $graph.Add('build obj/common/console_members.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/console/members.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-console.lib: lib obj/common/console_members.obj obj/common/console_client.obj')
    $graph.Add('build obj/common/local_binding.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/rpc/local_binding.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/common/native_command.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/rpc/native_command.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/common/frontend_control.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/rpc/frontend_control.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/common/worker_control.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/rpc/worker_control.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/common/management.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/rpc/management.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-rpc.lib: lib obj/common/local_binding.obj obj/common/native_command.obj obj/common/frontend_control.obj obj/common/worker_control.obj obj/common/management.obj')
    $graph.Add('build obj/tests/common_management.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_management_test.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-management-test.exe: base_rpc_test_link obj/tests/common_management.obj common-rpc.lib')
    $graph.Add('build obj/tests/common_worker_control.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_worker_control_test.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-worker-control-test.exe: base_rpc_test_link obj/tests/common_worker_control.obj common-rpc.lib')
    $graph.Add('build obj/tests/common_frontend_control.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_frontend_control_test.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-frontend-control-test.exe: base_rpc_test_link obj/tests/common_frontend_control.obj common-rpc.lib')
    $graph.Add('build obj/tests/common_native_command.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_native_command_test.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-native-command-test.exe: base_rpc_test_link obj/tests/common_native_command.obj common-rpc.lib')
    $graph.Add('build obj/tests/common_console_members.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_console_members_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-console-members-test.exe: broker_test_link obj/tests/common_console_members.obj')
    $graph.Add('build obj/tests/common_rpc_binding.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/common_rpc_binding_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-rpc-binding-test.exe: broker_test_link obj/tests/common_rpc_binding.obj')
    $graph.Add('build obj/run16/launch_options.obj: cc ' + (NinjaPath (Join-Path $run16Root 'launch_options.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/run16_launch_options.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/run16_launch_options_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build run16-launch-options-test.exe: broker_test_link obj/tests/run16_launch_options.obj obj/run16/launch_options.obj')
    $graph.Add('build obj/tests/run16_gui_wait.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/run16_gui_wait_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build run16-gui-wait-test.exe: broker_test_link obj/tests/run16_gui_wait.obj')
    $graph.Add('rule gui_wait_target_link')
    $graph.Add('  command = link.exe /nologo /subsystem:windows /entry:wmainCRTStartup /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib')
    $graph.Add('build run16-gui-wait-target.exe: gui_wait_target_link obj/tests/run16_gui_wait.obj')
    $graph.Add('build obj/run16/support.obj: cc ' + (NinjaPath (Join-Path $root 'src/opennt-abi/host-compat/opennt_support_rtl.c')))
    $graph.Add('  cflags = ' + $baseOwnerFlags + ' /Gy')
    $graph.Add('build obj/run16/rpc_client.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntsrv-exe/opennt/source/base_rpc_client.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $baseOwnerFlags + ' /I obj/basesrv')
    $graph.Add('build obj/run16/stub.obj: cc obj/basesrv/service_c.c | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule run16_link')
    $graph.Add('  command = link.exe /nologo /subsystem:console /entry:wWinMainCRTStartup /opt:ref /out:$out /map:$out.map $in rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib')
    $graph.Add('build obj/frontend/console_frontend.obj: cc ' + (NinjaPath (Join-Path $frontendRoot 'console_frontend.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/frontend/console_video.obj: cc ' + (NinjaPath (Join-Path $frontendRoot 'console_video.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/frontend/console_channel.obj: cc ' + (NinjaPath (Join-Path $frontendRoot 'console_channel.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/run16/frontend_scope.obj: cc ' + (NinjaPath (Join-Path $run16Root 'frontend_scope.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/frontend/session_service.obj: cc ' + (NinjaPath (Join-Path $frontendRoot 'session_service.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/console_frontend_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_frontend_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule console_test_link')
    $graph.Add('  command = link.exe /nologo /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib gdi32.lib')
    $graph.Add('build obj/tests/ntvwm_next_command.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_next_command_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-next-command-test.exe: console_test_link obj/tests/ntvwm_next_command.obj obj/ntvwm/next_command.obj')
    foreach ($kind in @('library','controller','keyboard','mouse')) {
        $source = Join-Path $root ('tests/component-integration/frontend_window_' + $kind + '_test.c')
        $object = 'obj/tests/frontend_window_' + $kind + '.obj'
        $graph.Add('build ' + $object + ': cc ' + (NinjaPath $source))
        $graph.Add('  cflags = ' + $nativeServiceFlags + ' /std:c11 /I "' + (NinjaPath $frontendRoot) + '" /I "' + (NinjaPath $fontBuild) + '"')
        $graph.Add('build frontend-window-' + $kind + '-test.exe: console_test_link ' + $object + ' frontend-window.lib obj/frontend/console_video.obj obj/ntvwm/text_frame.obj')
    }
    $graph.Add('build console-frontend-test.exe: console_test_link obj/tests/console_frontend_test.obj obj/frontend/console_frontend.obj obj/frontend/console_video.obj ' + $consoleGridObject)
    $graph.Add('build obj/tests/console_video.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_video_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build console-video-test.exe: console_test_link obj/tests/console_video.obj obj/frontend/console_frontend.obj obj/frontend/console_video.obj ' + $consoleGridObject)
    # Standalone carrier fixture; the same source is selected in the worker
    # adapter list above. Device/history and guest gates remain separate tests.
    $graph.Add('build obj/tests/keyboard_history_carrier.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvdm-exe/softpc/mvdm_keyboard_history.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/keyboard_origin.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/keyboard_origin_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build keyboard-origin-test.exe: rtl_fixture_link obj/tests/keyboard_origin.obj obj/tests/keyboard_history_carrier.obj')
    $graph.Add('build obj/common/native_launch.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/codec/native_launch.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build common-codec.lib: lib obj/common/native_launch.obj')
    $graph.Add('build obj/run16/native_launch.obj: cc ' + (NinjaPath (Join-Path $root 'src/run16-exe/native_launch.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/frontend/frontend_session.obj: cc ' + (NinjaPath (Join-Path $frontendRoot 'frontend_session.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags + ' /std:c11 /I "' + (NinjaPath $frontendRoot) + '"')
    $graph.Add('build obj/frontend/native_request_client.obj: cc ' + (NinjaPath (Join-Path $root 'src/run16-exe/native_request_client.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/frontend/bootstrap_client.obj: cc ' + (NinjaPath (Join-Path $root 'src/run16-exe/bootstrap_client.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/frontend/main.obj: cc ' + (NinjaPath (Join-Path $frontendRoot 'main.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/control_transfer.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/control_transfer_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build control-transfer-test.exe: console_test_link obj/tests/control_transfer.obj')
    $graph.Add('build obj/common/console_client.obj: cc ' + (NinjaPath (Join-Path $root 'src/common/console/client.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build frontend-client.lib: lib obj/frontend/native_request_client.obj obj/frontend/bootstrap_client.obj')
    $graph.Add('rule frontend_link')
    $graph.Add('  command = link.exe /nologo /subsystem:console /entry:wmainCRTStartup /opt:ref /out:$out /map:$out.map $in rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib gdi32.lib advapi32.lib legacy_stdio_definitions.lib')
    $graph.Add('build ntcon.exe: frontend_link obj/frontend/main.obj obj/frontend/session_service.obj obj/frontend/frontend_session.obj frontend-window.lib obj/frontend/console_frontend.obj obj/frontend/console_video.obj obj/frontend/console_channel.obj frontend-client.lib obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj ' + $consoleGridObject + ' opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib')
    foreach ($name in @('main','console_state','execution')) {
        $graph.Add('build obj/ntvwm/' + $name + '.obj: cc ' + (NinjaPath (Join-Path $root ('src/ntvwm-exe/' + $name + '.c'))))
        $graph.Add('  cflags = ' + $nativeServiceFlags + ' /I "' + (NinjaPath $fontBuild) + '"')
    }
    $nativeBinding = ' obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib'
    $graph.Add('build ntvwm.exe: frontend_link worker-base.lib obj/ntvwm/next_command.obj obj/ntvwm/main.obj obj/ntvwm/presentation.obj obj/ntvwm/text_frame.obj obj/ntvwm/console_state.obj obj/ntvwm/execution.obj obj/run16/native_launch.obj' + $nativeBinding)
    $graph.Add('build obj/run16/image_classification.obj: cc ' + (NinjaPath (Join-Path $run16Root 'image_classification.c')))
    $graph.Add('  cflags = ' + $baseOwnerFlags)
    $graph.Add('build obj/tests/run16_image_classification.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/run16_image_classification_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build run16-image-classification-test.exe: frontend_link obj/tests/run16_image_classification.obj obj/run16/image_classification.obj obj/run16/support.obj original-opennt-rtl-x86.lib')
    $graph.Add('build obj/tests/native_gui_startup_probe.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/native_gui_startup_probe.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule native_gui_probe_link')
    $graph.Add('  command = link.exe /nologo /subsystem:windows /entry:wWinMainCRTStartup /opt:ref /out:$out $in kernel32.lib user32.lib')
    $graph.Add('build native-gui-startup-probe.exe: native_gui_probe_link obj/tests/native_gui_startup_probe.obj')
    $graph.Add('build obj/ntvwm/text_frame.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvwm-exe/text_frame.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/ntvwm_text_frame.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_text_frame_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags + ' /std:c11')
    $graph.Add('build ntvwm-text-frame-test.exe: console_test_link obj/tests/ntvwm_text_frame.obj obj/ntvwm/text_frame.obj obj/frontend/console_video.obj')
    $graph.Add('build obj/tests/ntvwm_native_colour_probe.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_native_colour_probe.c')))
    $graph.Add('build ntvwm-native-colour-probe.exe: console_test_link obj/tests/ntvwm_native_colour_probe.obj obj/ntvwm/text_frame.obj')
    $graph.Add('build obj/ntvwm/presentation.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvwm-exe/presentation.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/ntvwm_presentation.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_presentation_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-presentation-test.exe: console_test_link obj/tests/ntvwm_presentation.obj obj/ntvwm/presentation.obj obj/ntvwm/console_state.obj obj/ntvwm/text_frame.obj obj/frontend/console_video.obj')
    $graph.Add('build obj/tests/frontend_text_handoff.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/frontend_text_handoff_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build frontend-text-handoff-test.exe: console_test_link obj/tests/frontend_text_handoff.obj obj/frontend/frontend_session.obj frontend-window.lib obj/frontend/console_frontend.obj obj/frontend/console_video.obj ' + $consoleGridObject)
    $graph.Add('build obj/tests/ntvwm_execution_lifetime.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_execution_lifetime_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-execution-lifetime-test.exe: frontend_link obj/tests/ntvwm_execution_lifetime.obj obj/ntvwm/execution.obj obj/run16/native_launch.obj common-codec.lib')
    $graph.Add('build obj/tests/ntsrv_native_job_tracker.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntsrv_native_job_tracker_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/native_job_tracker_research.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/native_job_tracker_research.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntsrv-native-job-tracker-test.exe: console_test_link obj/tests/ntsrv_native_job_tracker.obj obj/tests/native_job_tracker_research.obj')
    $graph.Add('build obj/tests/ntvwm_surviving_client.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_surviving_client_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-surviving-client-test.exe: frontend_link obj/tests/ntvwm_surviving_client.obj')
    $graph.Add('build obj/tests/ntvwm_close.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_close_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-close-test.exe: frontend_link obj/tests/ntvwm_close.obj obj/ntvwm/console_state.obj')
    $graph.Add('build obj/tests/frontend_request_client.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/frontend_request_client_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build frontend-request-client-test.exe: console_test_link obj/tests/frontend_request_client.obj frontend-client.lib obj/run16/native_launch.obj')
    $graph.Add('build obj/tests/frontend_gui_boundary.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/frontend_gui_boundary_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule gui_boundary_link')
    $graph.Add('  command = link.exe /nologo /subsystem:windows /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib')
    $graph.Add('build frontend-gui-boundary-test.exe: gui_boundary_link obj/tests/frontend_gui_boundary.obj')
    $graph.Add('build obj/tests/frontend_chain_target.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/frontend_chain_target.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $chainInputs = 'obj/tests/frontend_chain_target.obj obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib'
    $graph.Add('build frontend-chain-cui.exe: frontend_link ' + $chainInputs)
    $graph.Add('build obj/tests/frontend_chain_input.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/frontend_chain_input.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build frontend-chain-input.exe: frontend_link obj/tests/frontend_chain_input.obj')
    $graph.Add('rule frontend_chain_gui_link')
    $graph.Add('  command = link.exe /nologo /subsystem:windows /entry:wmainCRTStartup /opt:ref /out:$out $in rpcrt4.lib ntdll.lib kernel32.lib shell32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib')
    $graph.Add('build frontend-chain-gui.exe: frontend_chain_gui_link ' + $chainInputs)
    $graph.Add('build obj/tests/broker_frontend_bootstrap.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/broker_frontend_bootstrap_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build broker-frontend-bootstrap-test.exe: frontend_link obj/tests/broker_frontend_bootstrap.obj frontend-client.lib obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib | ntcon.exe ntsrv.exe')
    $graph.Add('build obj/tests/native_console_capture.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/native_console_capture_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build native-console-capture-test.exe: console_test_link obj/tests/native_console_capture.obj obj/ntvwm/console_state.obj')
    $graph.Add('build obj/tests/ntvwm_capture_race.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_capture_race_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-capture-race-test.exe: console_test_link obj/tests/ntvwm_capture_race.obj')
    $graph.Add('build obj/tests/ntvwm_geometry_handoff.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/ntvwm_geometry_handoff_probe.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build ntvwm-geometry-handoff-probe.exe: console_test_link obj/tests/ntvwm_geometry_handoff.obj')
    $graph.Add('build obj/tests/console_channel_lifetime.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_channel_lifetime_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build console-channel-lifetime-test.exe: console_test_link obj/tests/console_channel_lifetime.obj obj/frontend/frontend_session.obj frontend-window.lib common-transport.lib ' + $consoleGridObject)
    $graph.Add('build obj/tests/console_frame_failure.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_frame_failure_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags + ' /std:c11 /I "' + (NinjaPath (Join-Path $root 'src/ntcon-exe')) + '"')
    $graph.Add('build console-frame-failure-test.exe: console_test_link obj/tests/console_frame_failure.obj frontend-window.lib common-transport.lib ' + $consoleGridObject)
    $graph.Add('build obj/tests/console_pointer_dispatch_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_pointer_dispatch_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule pointer_test_link')
    $graph.Add('  command = link.exe /nologo /manifest:embed /manifestuac:"level=''asInvoker'' uiAccess=''false''" /out:$out $in libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib user32.lib')
    $graph.Add('build console-pointer-contract-test.exe: pointer_test_link obj/tests/console_pointer_dispatch_test.obj obj/frontend/console_video.obj ' + $consoleGridObject)
    $graph.Add('build obj/tests/console_client_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_client_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/console_client.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvdm-exe/win32/console_client.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build console-client-test.exe: console_test_link obj/tests/console_client_test.obj obj/tests/console_client.obj obj/tests/console_graphics_provider.obj obj/tests/console_bitmap_provider.obj obj/frontend/console_frontend.obj obj/frontend/console_video.obj worker-base.lib ' + $consoleGridObject)
    $graph.Add('build obj/tests/softpc_text_video.obj: cc ' + (NinjaPath (Join-Path $root 'tests/component-integration/softpc_text_video_test.c')))
    $graph.Add('build softpc-text-video-test.exe: console_test_link obj/tests/softpc_text_video.obj obj/adapter-softpc/mvdm_softpc_text_video.obj')
    $graph.Add('build obj/tests/console_text_producer.obj: cc ' + (NinjaPath (Join-Path $root 'tests/component-integration/console_text_producer_test.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/tests/console_text_provider.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntvdm-exe/win32/console_text.c')))
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build console-text-producer-test.exe: console_test_link obj/tests/console_text_producer.obj obj/tests/console_text_provider.obj')
    # The frontend EXE is a runtime prerequisite, never a launcher link input.
    $graph.Add('build run16.exe: run16_link obj/run16/entry.obj obj/run16/application_search.obj obj/run16/image_classification.obj obj/run16/launch_options.obj obj/run16/frontend_scope.obj frontend-client.lib obj/run16/native_launch.obj obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib || ntcon.exe')
    $graph.Add('build obj/tests/console_video_observed.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/console_video_observed.c')))
    $graph.Add('  cflags = /nologo /c /MT /std:c11 /W4 /we4013 /showIncludes /I obj/basesrv /I "' + (NinjaPath (Join-Path $root 'src')) + '"')
    $graph.Add('build obj/tests/run16_package_observed.obj: cc ' + (NinjaPath (Join-Path $root 'tests/app/run16_package_observed.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $baseOwnerFlags)
    $graph.Add('build frontend-video-observer.exe: frontend_link obj/frontend/main.obj obj/frontend/session_service.obj obj/frontend/frontend_session.obj frontend-window.lib frontend-client.lib obj/tests/console_video_observed.obj obj/frontend/console_channel.obj obj/run16/support.obj obj/run16/rpc_client.obj obj/run16/stub.obj ' + $consoleGridObject + ' opennt-base-client.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib')
    $graph.Add('rule basesrv_idl')
    $graph.Add('  command = midl.exe /nologo /env win32 /target NT100 /prefix client Client_ /prefix server Server_ /acf "' + (NinjaPath (Join-Path $root 'src/common/protocol/service.acf')) + '" /out obj/basesrv /h service.h /cstub service_c.c /sstub service_s.c $in')
    $graph.Add('build obj/basesrv/service_s.c | obj/basesrv/service_c.c obj/basesrv/service.h: basesrv_idl ' + (NinjaPath (Join-Path $root 'src/common/protocol/service.idl')) + ' | ' + (NinjaPath (Join-Path $root 'src/common/protocol/service.acf')))
    $nativeServiceFlags = '/nologo /c /MT /W4 /we4013 /showIncludes /I obj/basesrv /I "' + (NinjaPath (Join-Path $root 'src')) + '"'
    $graph.Add('build obj/basesrv/entry.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntsrv-exe/main.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/basesrv/stub.obj: cc obj/basesrv/service_s.c | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/worker/rpc_client.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntsrv-exe/opennt/source/base_rpc_client.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $baseOwnerFlags + ' /I obj/basesrv')
    $graph.Add('build obj/worker/stub.obj: cc obj/basesrv/service_c.c | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule basesrv_link')
    $graph.Add('  command = link.exe /nologo /subsystem:windows /entry:mainCRTStartup /opt:ref /out:$out /map:$out.map $in rpcrt4.lib ntdll.lib kernel32.lib user32.lib advapi32.lib legacy_stdio_definitions.lib')
    $graph.Add('build ntsrv.exe: basesrv_link obj/basesrv/entry.obj obj/basesrv/stub.obj obj/run16/support.obj opennt-base-server.lib opennt-base-bindings.lib broker-transport.lib original-opennt-rtl-x86.lib')
    $graph.Add('build obj/monitor/main.obj: cc ' + (NinjaPath (Join-Path $root 'src/ntmon-exe/main.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build obj/monitor/stub.obj: cc obj/basesrv/service_c.c | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('rule monitor_link')
    $graph.Add('  command = link.exe /nologo /subsystem:console /opt:ref /out:$out /map:$out.map $in rpcrt4.lib kernel32.lib advapi32.lib legacy_stdio_definitions.lib')
    # monitor is a client of the authenticated local BaseSrv endpoint.  It
    # shares only the native transport scope helper, never a BaseClient
    # registration or worker lifecycle library.
    $graph.Add('build ntmon.exe: monitor_link obj/monitor/main.obj obj/monitor/stub.obj broker-transport.lib')
    $monitorRpcTestObject = 'obj/tests/monitor_rpc_test.obj'
    $graph.Add('build ' + $monitorRpcTestObject + ': cc ' + (NinjaPath (Join-Path $root 'tests/adapter-basesrv/monitor_rpc_test.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build monitor-rpc-test.exe: monitor_link ' + $monitorRpcTestObject + ' obj/monitor/stub.obj broker-transport.lib common-rpc.lib')
    $graph.Add('build obj/tests/monitor_layout.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/monitor_layout_test.c')) + ' | obj/basesrv/service.h')
    $graph.Add('  cflags = ' + $nativeServiceFlags)
    $graph.Add('build monitor-layout-test.exe: monitor_link obj/tests/monitor_layout.obj obj/monitor/stub.obj broker-transport.lib common-rpc.lib')
    $graph.Add('rule worker_link')
    # MSVC leaves an unchanged import library's timestamp intact. Without
    # restat that implicit output makes the whole link dirty on every build.
    $graph.Add('  restat = 1')
    $graph.Add('  command = cmd.exe /d /s /c "link.exe /nologo /subsystem:console /opt:ref /out:$out /map:$out.map /def:generated/ntvdm-wow32-provider.def $in rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib && "' + (NinjaPath $NodeExecutable) + '" "' + (NinjaPath (Join-Path $root 'tools/audit/Verify-VdmTibStorage.mjs')) + '" $out.map obj/adapter-monitor/mvdm_vdm_tib.obj"')
    # CCPU40 and DPMI discover the original nt_inthk providers only after their
    # archives have been scanned. Retain the normal source-order occurrence,
    # then rescan this one original host archive at the tail for those hooks.
    # This import library is the one worker parent ABI.  Late-loaded original
    # providers import it rather than a retired second `original-softpc-process`
    # image with its own local BaseVDM queue.
$graph.Add('build ntvdm.exe | ntvdm.lib: worker_link worker-base.lib obj/worker/rpc_client.obj obj/worker/stub.obj ' + ($adapterRedirWorkerObjects -join ' ') + ' ' + ($adapterWowWorkerObjects -join ' ') + ' worker-shell.lib worker-command-bindings.lib original-softpc-host-roots.lib original-softpc-support.lib original-softpc-bios.lib original-softpc-keymouse.lib original-softpc-system.lib original-softpc-disks.lib original-softpc-video.lib original-softpc-cvidc.lib original-softpc-comms.lib original-softpc-dos.lib original-mvdm-dem.lib original-mvdm-command.lib original-mvdm-xms.lib original-mvdm-dpmi32.lib original-mvdm-host-suballoc.lib original-mvdm-host-oemuni.lib original-softpc-base-trace.lib original-opennt-base-vdm.lib opennt-base-client.lib opennt-base-bindings.lib original-opennt-rtl-x86.lib softpc-bindings.lib vdd-bindings.lib softpc-win32-bindings.lib monitor-bindings.lib kernel-vdm-printer.lib debugger-bindings.lib session.lib broker-transport.lib mvdm-softpc-effective-address.lib softpc-ccpu-vector-defaults.lib softpc-activity-check.lib original-ccpu386.lib original-softpc-host-roots.lib obj/host/softpc-resource.res')
}
# Test-only original keyboard-state observation; reuse the exact worker link
# inputs, substituting the keyba object before the original archive is scanned.
if ($Architecture -eq 'x86') {
    $workerEdge = $graph | Where-Object { $_.StartsWith('build ntvdm.exe | ntvdm.lib: worker_link ') }
    if (@($workerEdge).Count -ne 1) { throw 'Expected one formal worker edge' }
    $graph.Add('build obj/tests/keyba_observed.obj: cc ' + (NinjaPath (Join-Path $root 'tests/observation/keyba_observed.c')))
    $graph.Add('build obj/tests/nt_event_observed.obj: cc_host ' + (NinjaPath (Join-Path $root 'tests/observation/nt_event_observed.c')))
    $eventEdge = $graph | Where-Object { $_.StartsWith('build obj/host/nt_event.obj: ') }
    if (@($eventEdge).Count -ne 1) { throw 'Expected one original event-owner edge' }
    $eventFlags = $graph[$graph.IndexOf($eventEdge)+1]
    if (!$eventFlags.StartsWith('  host_cflags = ')) { throw 'Missing event-owner compiler flags' }
    $graph.Add($eventFlags)
    $graph.Add($workerEdge.Replace('build ntvdm.exe | ntvdm.lib:', 'build ntvdm-keyboard-observer.exe:').Replace(': worker_link ', ': worker_link obj/tests/keyba_observed.obj obj/tests/nt_event_observed.obj '))
    foreach($unit in @('console_client','console_channel')) {
        $graph.Add('build obj/tests/' + $unit + '_observed.obj: cc ' + (NinjaPath (Join-Path $root ('tests/observation/' + $unit + '_observed.c'))))
        $graph.Add('  cflags = ' + $nativeServiceFlags)
    }
    $graph.Add($workerEdge.Replace('build ntvdm.exe | ntvdm.lib:', 'build ntvdm-console-wire-observer.exe:').Replace(': worker_link ', ': worker_link obj/tests/console_client_observed.obj '))
    $frontendEdge = $graph | Where-Object { $_.StartsWith('build ntcon.exe: ') }
    if(@($frontendEdge).Count -ne 1){throw 'Expected one frontend edge'}
    $graph.Add($frontendEdge.Replace('build ntcon.exe:', 'build frontend-console-wire-observer.exe:').Replace('obj/frontend/console_channel.obj','obj/tests/console_channel_observed.obj'))
}
$workerPackageObjects = foreach ($name in $workerPackageNames) {
    $object = 'obj/worker/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $workerPackageRoot $name)))
    $object
}
$graph.Add('build ' + $effectiveAddressObject + ': cc ' + (NinjaPath $effectiveAddressSource))
$patchVectorDefaultsObject = 'obj/ccpu/fmstubs_vector_defaults.obj'
$graph.Add('build ' + $patchVectorDefaultsObject + ': cc_patch_vector_defaults ' + (NinjaPath $ccpuFallbackSource))
$patchActivityCheckObject = 'obj/ccpu/fmstubs_activity_check.obj'
$graph.Add('build ' + $patchActivityCheckObject + ': cc_patch_activity_check ' + (NinjaPath $ccpuFallbackSource))
$redirResourceObject = 'obj/redir/vdmredir.res'
$graph.Add('build ' + $redirResourceObject + ': rc ' + (NinjaPath $redirResourceSource))
$graph.Add('build original-ccpu386.lib: lib ' + ($ccpuObjects -join ' '))
$graph.Add('build original-softpc-bios.lib: lib ' + ($biosObjects -join ' '))
$graph.Add('build original-softpc-keymouse.lib: lib ' + ($keymouseObjects -join ' '))
$graph.Add('build original-softpc-system.lib: lib ' + ($systemObjects -join ' '))
$graph.Add('build original-softpc-disks.lib: lib ' + ($disksObjects -join ' '))
$graph.Add('build original-softpc-support.lib: lib ' + ($supportObjects -join ' '))
$graph.Add('build original-softpc-video.lib: lib ' + ($videoObjects -join ' '))
$graph.Add('build original-softpc-cvidc.lib: lib ' + ($cvidcObjects -join ' '))
$graph.Add('build original-softpc-comms.lib: lib ' + ($commsObjects -join ' '))
$graph.Add('build original-softpc-dos.lib: lib ' + ($dosObjects -join ' '))
$graph.Add('build original-mvdm-dem.lib: lib ' + ($demObjects -join ' '))
$graph.Add('build original-mvdm-command.lib: lib ' + ($commandObjects -join ' '))
$graph.Add('build original-mvdm-redir.lib: lib ' + ($redirObjects -join ' '))
$graph.Add('build original-mvdm-xms.lib: lib ' + ($xmsObjects -join ' '))
$graph.Add('build original-mvdm-dpmi32.lib: lib ' + ($dpmiObjects -join ' '))
$graph.Add('build original-mvdm-host-suballoc.lib: lib ' + ($suballocObjects -join ' '))
$graph.Add('build original-mvdm-host-oemuni.lib: lib ' + ($oemuniObjects -join ' '))
$graph.Add('build original-softpc-base-trace.lib: lib ' + ($baseDebugObjects -join ' '))
$graph.Add('build original-softpc-host-roots.lib: lib ' + ($hostObjects -join ' '))
$graph.Add('build original-softpc-host-fixture-roots.lib: lib ' + ($hostFixtureObjects -join ' '))
$graph.Add('build softpc-bindings.lib: lib ' + ($adapterSoftpcObjects -join ' '))
$graph.Add('build softpc-fixture-bindings.lib: lib ' + ($adapterSoftpcFixtureObjects -join ' '))
$graph.Add('build vdmredir-dll-bindings.lib: lib ' + ($adapterRedirObjects -join ' '))
$graph.Add('build vdd-bindings.lib: lib ' + ($adapterVddObjects -join ' '))
$graph.Add('build original-opennt-netlib.lib: lib ' + ($openntNetlibObjects -join ' '))
$graph.Add('build original-opennt-netapi-api.lib: lib ' + ($openntNetapiObjects -join ' '))
$graph.Add('build original-opennt-xactsrv.lib: lib ' + ($openntXactSrvObjects -join ' '))
$graph.Add('build original-opennt-base-vdm.lib: lib ' + ($openntBaseVdmObjects -join ' '))
$graph.Add('build original-opennt-rtl-x86.lib: lib ' + ((@($openntRtlObjects) + @($openntRtlX86Objects)) -join ' '))
$graph.Add('build worker-shell.lib: lib obj/worker/package_layout.obj')
$graph.Add('build session.lib: lib ' + ($sessionObjects -join ' '))
$graph.Add('build wow-worker-bindings.lib: lib ' + ($adapterWowWorkerObjects -join ' '))
$graph.Add('build rtl-x86-fixture.exe: rtl_fixture_link ' + $rtlX86FixtureObject + ' original-opennt-rtl-x86.lib')
$graph.Add('build environment-projection-fixture.exe: rtl_fixture_link ' + $environmentProjectionFixtureObject + ' original-opennt-base-vdm.lib original-opennt-rtl-x86.lib softpc-win32-bindings.lib')
$graph.Add('build mvdm-softpc-effective-address.lib: lib ' + $effectiveAddressObject)
$graph.Add('build softpc-win32-bindings.lib: lib ' + ($adapterWin32Objects -join ' '))
# Worker composition contains only reached COMMAND/BOP support.  It has no
# local BaseVDM producer, retry loop or single-process queue.
$graph.Add('build worker-command-bindings.lib: lib ' + ($adapterBaseSrvObjects -join ' '))
$graph.Add('build monitor-bindings.lib: lib ' + ($adapterMonitorObjects -join ' '))
$graph.Add('build kernel-vdm-printer.lib: lib ' + $kernelVdmPrinterObject)
$graph.Add('build debugger-bindings.lib: lib ' + ((@($adapterDebuggerObjects) + @($originalDebuggerObject)) -join ' '))
$graph.Add('build softpc-ccpu-vector-defaults.lib: lib ' + $patchVectorDefaultsObject)
$graph.Add('build softpc-activity-check.lib: lib ' + $patchActivityCheckObject)
$graph.Add('build original-softpc-candidate: phony original-ccpu386.lib original-softpc-bios.lib original-softpc-keymouse.lib original-softpc-system.lib original-softpc-disks.lib original-softpc-support.lib original-softpc-video.lib original-softpc-cvidc.lib original-softpc-comms.lib original-softpc-dos.lib original-mvdm-dem.lib original-mvdm-command.lib original-mvdm-redir.lib original-mvdm-xms.lib original-mvdm-dpmi32.lib original-mvdm-host-suballoc.lib original-mvdm-host-oemuni.lib original-softpc-base-trace.lib original-softpc-host-roots.lib original-opennt-netlib.lib original-opennt-netapi-api.lib original-opennt-xactsrv.lib original-opennt-rtl-x86.lib softpc-bindings.lib vdmredir-dll-bindings.lib worker-shell.lib worker-command-bindings.lib softpc-win32-bindings.lib monitor-bindings.lib kernel-vdm-printer.lib debugger-bindings.lib session.lib mvdm-softpc-effective-address.lib softpc-ccpu-vector-defaults.lib softpc-activity-check.lib')
$graph.Add('build product-programs: phony run16.exe ntsrv.exe ntvdm.exe ntvwm.exe ntcon.exe ntmon.exe VDMREDIR.dll')
$graph.Add('build obj/tests/ccpu_halt_reset_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/ccpu_halt_reset_test.c')))
$hostFixtureSeamsObject = 'obj/tests/ccpu_host_fixture_seams.obj'
$graph.Add('build ' + $hostFixtureSeamsObject + ': cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/ccpu_host_fixture_seams.c')))
$boundedExecutionFixtureSeamsObject = 'obj/tests/ccpu_bounded_execution_fixture_seams.obj'
$graph.Add('build ' + $boundedExecutionFixtureSeamsObject + ': cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/ccpu_bounded_execution_fixture_seams.c')))
# Same original owner libraries as the product, with a fixture main only.
$graph.Add('rule event_test_link')
$graph.Add('  command = link.exe /nologo /force:multiple /map:$out.map /out:$out $in kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib')
$fixtureHostLibraries = 'worker-base.lib worker-shell.lib worker-command-bindings.lib original-softpc-host-fixture-roots.lib original-softpc-support.lib original-softpc-bios.lib original-softpc-keymouse.lib original-softpc-system.lib original-softpc-disks.lib original-softpc-video.lib original-softpc-cvidc.lib original-softpc-comms.lib original-softpc-dos.lib original-mvdm-dem.lib original-mvdm-command.lib original-mvdm-xms.lib original-mvdm-dpmi32.lib original-mvdm-host-suballoc.lib original-mvdm-host-oemuni.lib original-softpc-base-trace.lib original-opennt-base-vdm.lib original-opennt-rtl-x86.lib softpc-fixture-bindings.lib vdmredir-dll-bindings.lib vdd-bindings.lib softpc-win32-bindings.lib monitor-bindings.lib kernel-vdm-printer.lib debugger-bindings.lib session.lib mvdm-softpc-effective-address.lib softpc-ccpu-vector-defaults.lib softpc-activity-check.lib original-ccpu386.lib obj/host/softpc-resource.res'
$graph.Add('build ccpu-halt-reset-test.exe: event_test_link obj/tests/ccpu_halt_reset_test.obj ' + $hostFixtureSeamsObject + ' ' + $fixtureHostLibraries)
# Test-only debug boundaries: original matching body and actual checked binding.
$graph.Add('rule debug_unit_link')
$graph.Add('  command = link.exe /nologo /machine:x86 /opt:ref /out:$out $in legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib')
$graph.Add('build obj/tests/ccpu_debug_match_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/ccpu_debug_match_test.c')))
$graph.Add('build ccpu-debug-match-test.exe: debug_unit_link obj/tests/ccpu_debug_match_test.obj obj/ccpu/c_debug.obj')
$graph.Add('build obj/tests/debug_register_binding_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/debug_register_binding_test.c')))
$graph.Add('build debug-register-binding-test.exe: debug_unit_link obj/tests/debug_register_binding_test.obj')
$graph.Add('build obj/tests/debugger_event_transport_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/debugger_event_transport_test.c')))
$graph.Add('build debugger-event-transport-test.exe: event_test_link obj/tests/debugger_event_transport_test.obj')
$graph.Add('build obj/tests/debugger_module_event_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/debugger_module_event_test.c')))
$graph.Add('build debugger-module-event-test.exe: event_test_link obj/tests/debugger_module_event_test.obj')
$graph.Add('build obj/tests/oem_command_uppercase_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/oem_command_uppercase_test.c')))
$graph.Add('build oem-command-uppercase-test.exe: debug_unit_link obj/tests/oem_command_uppercase_test.obj')
$graph.Add('build obj/tests/debugger_worker_module_observer.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/debugger_worker_module_observer.c')))
$graph.Add('build debugger-worker-module-observer.exe: event_test_link obj/tests/debugger_worker_module_observer.obj')
$graph.Add('rule s44_bop_vdd_cc')
$graph.Add('  command = cl.exe $cflags /DS44_BOP_PROVIDER /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('build obj/tests/s44_bop_vdd.obj: s44_bop_vdd_cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/s44_test_vdd.c')))
$graph.Add('rule s44_entry_hook_vdd_cc')
$graph.Add('  command = cl.exe $cflags /DS44_ENTRY_USER_HOOK /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('build obj/tests/s44_entry_hook_vdd.obj: s44_entry_hook_vdd_cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/s44_test_vdd.c')))
$graph.Add('rule s44_memory_vdd_cc')
$graph.Add('  command = cl.exe $cflags /DS44_MEMORY_PROVIDER /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('build obj/tests/s44_memory_vdd.obj: s44_memory_vdd_cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/s44_test_vdd.c')))
$graph.Add('rule s44_terminate_vdd_cc')
$graph.Add('  command = cl.exe $cflags /DS44_TERMINATE_PROVIDER /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file: ')
$graph.Add('build obj/tests/s44_terminate_vdd.obj: s44_terminate_vdd_cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/s44_test_vdd.c')))
$graph.Add('rule s44_test_vdd_link')
$graph.Add('  command = link.exe /nologo /dll /machine:x86 /entry:VDDInitialize@12 /export:VDDInitialize=_VDDInitialize@12 /export:S44BOPInit=_S44BOPInit@0 /export:S44BOPDispatch=_S44BOPDispatch@0 /out:$out $in kernel32.lib user32.lib')
$graph.Add('build s44-bop-vdd.dll: s44_test_vdd_link obj/tests/s44_bop_vdd.obj ntvdm.lib')
$graph.Add('build s44-entry-hook-vdd.dll: s44_test_vdd_link obj/tests/s44_entry_hook_vdd.obj ntvdm.lib')
$graph.Add('build s44-memory-vdd.dll: s44_test_vdd_link obj/tests/s44_memory_vdd.obj ntvdm.lib')
$graph.Add('build s44-terminate-vdd.dll: s44_test_vdd_link obj/tests/s44_terminate_vdd.obj ntvdm.lib')
$graph.Add('build debugger-event-tests: phony debugger-event-transport-test.exe debugger-module-event-test.exe debugger-worker-module-observer.exe')
$graph.Add('build dpmi-debug-tests: phony ccpu-debug-match-test.exe debug-register-binding-test.exe debugger-event-transport-test.exe debugger-module-event-test.exe ccpu-halt-reset-test.exe')
$ccpuThreadLifecycleFixtureObject = 'obj/tests/ccpu_thread_lifecycle_test.obj'
$graph.Add('build ' + $ccpuThreadLifecycleFixtureObject + ': cc ' + (NinjaPath $ccpuThreadLifecycleFixtureSource))
$graph.Add('build ccpu-thread-lifecycle-test.exe: event_test_link ' + $ccpuThreadLifecycleFixtureObject + ' ' + $hostFixtureSeamsObject + ' ' + $fixtureHostLibraries)
# Same production libraries; only the entry is a source-shaped memory test.
$graph.Add('rule memory_test_link')
$graph.Add('  command = link.exe /nologo /force:multiple /map:$out.map /out:$out $in kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib')
$graph.Add('build obj/tests/original_external_memory_test.obj: cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/original_external_memory_test.c')))
$graph.Add('build original-external-memory-test.exe: memory_test_link obj/tests/original_external_memory_test.obj ' + $hostFixtureSeamsObject + ' ' + $fixtureHostLibraries)
$highLinearPageFixtureObject = 'obj/tests/ccpu_bounded_execution_fixture.obj'
$graph.Add('build ' + $highLinearPageFixtureObject + ': cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/ccpu_bounded_execution_fixture.c')))
$graph.Add('build ccpu-high-linear-page-test.exe: memory_test_link ' + $highLinearPageFixtureObject + ' ' + $boundedExecutionFixtureSeamsObject + ' ' + $fixtureHostLibraries)
$wowPageDomainFixtureObject = 'obj/tests/wow_page_domain_fixture.obj'
$wowUserObjectFixtureObject = 'obj/adapter-wow/wow_user_object_bindings.obj'
$graph.Add('build ' + $wowPageDomainFixtureObject + ': cc ' + (NinjaPath (Join-Path $root 'tests/mvdm-host/wow_page_domain_fixture.c')))
$graph.Add('build ' + $wowUserObjectFixtureObject + ': cc ' + (NinjaPath (Join-Path $wow32DllRoot 'source/wow_user_object_bindings.c')))
$graph.Add('build wow-page-domain-test.exe: memory_test_link ' + $wowPageDomainFixtureObject + ' ' + $wowUserObjectFixtureObject + ' ' + $hostFixtureSeamsObject + ' ' + $fixtureHostLibraries)
# INT 2Ah publication now exercises the production native FlatAddress cache
# and its original DIB mapping owner, not just an isolated LDT word copy.
$graph.Add('build cpu40-descriptor-domain-fixture.exe: memory_test_link ' + $cpu40DescriptorDomainFixtureObject + ' ' + $hostFixtureSeamsObject + ' ' + $fixtureHostLibraries)
$graph.Add('build obj/tests/wow_user_client_view_layout_fixture.obj: cc ' + (NinjaPath (Join-Path $root 'tests/adapter-mvdm-host-out/wow/wow_user_client_view_layout_fixture.c')))
$graph.Add('  cflags = /nologo /c /MT /W3 /showIncludes /I "' + (NinjaPath (Join-Path $root 'src')) + '" /I "' + (NinjaPath (Join-Path $wow32DllRoot 'include')) + '"')
$graph.Add('build wow-user-client-view-layout-test.exe: memory_test_link obj/tests/wow_user_client_view_layout_fixture.obj')
$graph.Add('build cvidc-vector-binding-fixture.exe: memory_test_link ' + $cvidcVectorBindingFixtureObject + ' ' + $hostFixtureSeamsObject + ' ' + $fixtureHostLibraries)
$graph.Add('build x87-layout-fixture.exe: rtl_fixture_link ' + $x87LayoutFixtureObject)
$graph.Add('build VDMREDIR.dll | VDMREDIR.dll.lib: redir_dll_link ' + (($redirObjects + @($redirResourceObject)) -join ' ') + ' ntvdm.lib worker-base.lib vdmredir-dll-bindings.lib original-opennt-netlib.lib original-opennt-netapi-api.lib original-opennt-xactsrv.lib original-opennt-rtl-x86.lib softpc-bindings.lib softpc-win32-bindings.lib')
$graph.Add('build original-softpc-forced-closure.dll: forced_link_audit original-ccpu386.lib original-softpc-bios.lib original-softpc-keymouse.lib original-softpc-system.lib original-softpc-disks.lib original-softpc-support.lib original-softpc-video.lib original-softpc-cvidc.lib original-softpc-comms.lib original-softpc-dos.lib original-mvdm-dem.lib original-mvdm-command.lib original-mvdm-xms.lib original-mvdm-dpmi32.lib original-mvdm-host-suballoc.lib original-mvdm-host-oemuni.lib original-softpc-base-trace.lib original-softpc-host-roots.lib softpc-bindings.lib worker-shell.lib worker-command-bindings.lib softpc-win32-bindings.lib monitor-bindings.lib kernel-vdm-printer.lib debugger-bindings.lib session.lib mvdm-softpc-effective-address.lib softpc-ccpu-vector-defaults.lib')
$graph.Add('default original-softpc-candidate')
# Select neutral archives only through explicit provider dependencies. Do not
# embed common objects inside worker/EXE-private archives or reverse-link them.
for ($commonIndex = 0; $commonIndex -lt $graph.Count; ++$commonIndex) {
    $commonEdge = $graph[$commonIndex]
    $commonLink = [regex]::Match($commonEdge, '^build .+: \S*link ')
    if ($commonLink.Success -and $commonEdge -match '(opennt-base-bindings\.lib|ntvdm\.lib|worker-command-bindings\.lib|softpc-bindings\.lib|run16/entry\.obj)') {
        $rootBoundary = $commonEdge.IndexOf(' |', $commonLink.Length)
        if ($rootBoundary -ge 0) { $commonEdge = $commonEdge.Insert($rootBoundary, ' common-root.lib') }
        else { $commonEdge += ' common-root.lib' }
        $graph[$commonIndex] = $commonEdge
    }
    if ($commonLink.Success -and $commonEdge -match '(opennt-base-bindings\.lib|base_rpc_client\.obj|monitor/main\.obj)') {
        $rpcBoundary = $commonEdge.IndexOf(' |', $commonLink.Length)
        if ($rpcBoundary -ge 0) { $commonEdge = $commonEdge.Insert($rpcBoundary, ' common-rpc.lib') }
        else { $commonEdge += ' common-rpc.lib' }
        $graph[$commonIndex] = $commonEdge
    }
    if ($commonLink.Success -and
        $commonEdge -match '(worker-base\.lib|frontend-client\.lib|opennt-base-bindings\.lib|console_client\.obj|console_channel\.obj|native_request_io\.obj|session_service\.obj|frontend_scope\.obj|console_state\.obj)') {
        $commonBoundary = $commonEdge.IndexOf(' |', $commonLink.Length)
        if ($commonBoundary -ge 0) {
            $graph[$commonIndex] = $commonEdge.Insert($commonBoundary, ' common-transport.lib common-codec.lib common-console.lib')
        } else { $graph[$commonIndex] = $commonEdge + ' common-transport.lib common-codec.lib common-console.lib' }
    }
}
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))

# Some product objects live below a component-local namespace (for example
# obj/ntvdm/session).  Keeping a hand-maintained list of those leaves made a
# fresh graph depend on whichever target happened to be built first.  Derive
# every object-output parent from the graph just emitted, so all selected
# targets can compile independently in a new disposable root.
$objectOutputDirectories = @(
    [regex]::Matches(($graph -join [Environment]::NewLine), '(?m)^build\s+(obj/[^\s:|]+)') |
        ForEach-Object { Split-Path -Parent $_.Groups[1].Value } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) } |
        Select-Object -Unique |
        ForEach-Object { Join-Path $build $_ }
)
if ($objectOutputDirectories.Count -gt 0) {
    New-Item -ItemType Directory -Force $objectOutputDirectories | Out-Null
}

[ordered]@{
    schema = 'm0.t310.original-softpc-machine-candidate.v3'
    architecture = $Architecture
    cpuProfile = 'CCPU40'
    toolchain = 'MSVC /MT via VsDevCmd'
    ninjaParallelDefault = $ParallelJobs
    ninjaParallelOverride = 'MVDM_BUILD_JOBS'
    i386Define = $false
    originalCcpuManifest = 'src/mvdm/softpc.new/base/ccpu386/sources'
    originalHostManifest = 'src/mvdm/softpc.new/host/src/sources'
    ccpuSourceCount = @($ccpuNames).Count
    ccpuSources = @($ccpuNames)
    privateComponentSources = @()
    biosSources = @($biosNames)
    keymouseSources = @($keymouseNames)
    systemSources = @($systemNames)
    disksSources = @($disksNames)
    supportSources = @($supportNames)
    videoSources = @($videoNames)
    cvidcSources = @($cvidcNames)
    cvidcOriginalAccessor = [ordered]@{
        path = 'src/mvdm/softpc.new/base/cpu/src/evid/vglob.c'
        sha256 = Get-NodeSha256 $cvidcAccessorSource
        profile = 'C-VID GDP 1280; 38 original pairs; ten typed conversion pairs; original latch/selector exceptions'
    }
    commsSources = @($commsNames)
    dosSources = @($dosNames)
    demSources = @($demNames)
    commandSources = @($commandNames)
    redirectorSources = @($redirNames)
    xmsSources = @($xmsNames)
    dpmiSources = @($dpmiNames)
    privateOverlaySources = @()
    suballocSources = @($suballocNames)
    oemuniSources = @($oemuniNames)
    baseDebugSources = @($baseDebugNames)
    baseDebugBuildDisposition = 'selected-complete-original-softpc-debug; mvdm-debugger product excluded'
    hostRoots = @($hostNames)
    hostEntrySource = 'src/mvdm/softpc.new/obj.vdm/ntvdm.c'
    hostEntryResource = [ordered]@{
        path = 'src/mvdm/softpc.new/obj.vdm/resource.rc'
        sha256 = Get-NodeSha256 $hostEntryResourceSource
        buildDisposition = 'selected-original-resource-source; compiled unchanged per host-width row'
    }
    ntvdmSoftpcSources = @($adapterSoftpcNames)
    vdmRedirDllBindingSources = @($adapterRedirNames)
    ntvdmRedirectorWorkerSources = @($adapterRedirWorkerNames)
    workerPackageSources = @($workerPackageNames)
    ntvdmSessionSources = @($sessionNames)
    ntvdmWowWorkerSources = @($adapterWowWorkerNames)
    ntvdmWin32Sources = @($adapterWin32Names)
    sharedConsoleGridSource = 'src/opennt-abi/host-compat/console_grid.c'
    openntRtlX86Sources = @($openntRtlX86Names)
    openntRtlSources = @($openntRtlNames)
    openntBaseVdmSources = @($openntBaseVdmNames)
    openntBrokerOwnerArchives = @($baseOwnerManifest)
    brokerTransportArchive = @($brokerTransportManifest)
    brokerComposition = [ordered]@{
        target = 'ntsrv.exe'
        selected = ($Architecture -eq 'x86')
        disposition = 'selected x86 product broker; authenticated registration, command delivery and bounded death handling verified'
        sources = @('src/ntsrv-exe/main.c', 'src/common/protocol/service.idl', 'src/common/protocol/service.acf' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
        libraries = @('opennt-base-server.lib', 'opennt-base-bindings.lib', 'broker-transport.lib', 'original-opennt-rtl-x86.lib')
    }
    workerBaseComposition = [ordered]@{
        target = 'worker-base.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'project-owned worker connection initialization and disconnect; NTVDM and NTVWM only; common provides protocol clients'
        sources = @('connection.c', 'connection.h', 'shutdown_close.c' | ForEach-Object {
            $path = 'src/worker-base/' + $_
            [ordered]@{ path = $path; sha256 = Get-NodeSha256 (Join-Path $root $path) }
        })
    }
    commonRootComposition = [ordered]@{
        target = 'common-root.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'checked actual-own-EXE root and relative internal paths; no RPC peer validation, search or environment authority'
        sources = @('src/common/system_root.c', 'src/common/system_root.h', 'src/common/guest_environment.c', 'src/common/guest_environment.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    commonTransportComposition = [ordered]@{
        target = 'common-transport.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'project-added bounded pipe mechanics; explicit ordering; no protocol interpretation or ownership policy'
        sources = @('src/common/transport/pipe_transfer.c', 'src/common/transport/pipe_transfer.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    commonCodecComposition = [ordered]@{
        target = 'common-codec.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'copied native packet codec and validation only; no resource materialization or target creation'
        sources = @('src/common/codec/native_launch.c', 'src/common/codec/native_launch.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    commonConsoleComposition = [ordered]@{
        target = 'common-console.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'on-demand Console snapshot and ordered frontend protocol client; no task, identity, rendering or retirement decisions'
        sources = @('src/common/console/members.c', 'src/common/console/members.h', 'src/common/console/client.c', 'src/common/console/client.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    commonRpcComposition = [ordered]@{
        target = 'common-rpc.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'ncalrpc binding/rollback, native-command/frontend-control/worker-control clients; no endpoint selection or service policy'
        sources = @('src/common/rpc/local_binding.c', 'src/common/rpc/local_binding.h', 'src/common/rpc/connection.h', 'src/common/rpc/native_command.c', 'src/common/rpc/native_command.h', 'src/common/rpc/frontend_control.c', 'src/common/rpc/frontend_control.h', 'src/common/rpc/worker_control.c', 'src/common/rpc/worker_control.h', 'src/common/rpc/management.c', 'src/common/rpc/management.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    launcherComposition = [ordered]@{
        target = 'run16.exe'
        selected = ($Architecture -eq 'x86')
        disposition = 'client-only x86 launcher; native independent-frontend paths tested; migrated DOS/GUI grouping gates pending'
        sources = @('src/run16-exe/main.c', 'src/run16-exe/application_search.c', 'src/run16-exe/application_search.h', 'src/run16-exe/image_classification.c', 'src/run16-exe/image_classification.h', 'src/run16-exe/launch_options.c', 'src/run16-exe/launch_options.h', 'src/run16-exe/frontend_scope.c', 'src/run16-exe/frontend_scope.h', 'src/opennt-abi/host-compat/opennt_support_rtl.c' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
        libraries = @('frontend-client.lib', 'opennt-base-client.lib', 'opennt-base-bindings.lib', 'broker-transport.lib', 'original-opennt-rtl-x86.lib')
    }
    frontendClientComposition = [ordered]@{
        target = 'frontend-client.lib'
        selected = ($Architecture -eq 'x86')
        disposition = 'finite bootstrap/native request client and bounded presentation receipt; no renderer/input-pump dependency'
        sources = @('src/run16-exe/bootstrap_client.c', 'src/run16-exe/frontend_bootstrap.h', 'src/run16-exe/native_request_client.c', 'src/run16-exe/native_request_client.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    nativeLaunchComposition = [ordered]@{
        target = 'obj/run16/native_launch.obj'
        consumers = @('run16.exe','ntvwm.exe')
        disposition = 'launcher-owned restricted resource/CreateProcess primitive; no worker policy or frontend ownership'
        sources = @('src/run16-exe/native_launch.c','src/run16-exe/native_launch.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    frontendServiceComposition = [ordered]@{
        target = 'obj/frontend/session_service.obj'
        selected = ($Architecture -eq 'x86')
        disposition = 'independent frontend channel/request pump, membership retirement and teardown; test-only in-process fixture retained'
        sources = @('src/ntcon-exe/session_service.c', 'src/ntcon-exe/session_service.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    frontendBootstrapComposition = [ordered]@{
        target = 'ntcon.exe'
        selected = ($Architecture -eq 'x86')
        disposition = 'independent authenticated visible presentation owner; no native backend, parser or helper'
        sources = @('src/ntcon-exe/main.c', 'src/run16-exe/frontend_bootstrap.h', 'src/ntcon-exe/frontend_session.c', 'src/ntcon-exe/frontend_session.h', 'src/ntcon-exe/console_frontend.c', 'src/ntcon-exe/console_frontend.h', 'src/ntcon-exe/console_video.c', 'src/ntcon-exe/console_video.h', 'src/ntcon-exe/console_channel.c', 'src/ntcon-exe/console_channel.h', 'src/common/protocol/console_video.h', 'src/common/protocol/console_io.h' | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    nativeConsoleComposition = [ordered]@{
        target = 'ntvwm.exe'
        disposition = 'independent registered hidden Console worker; shared frontend channel and direct target completion; acceptance remains gated'
        sources = @('main.c','next_command.c','next_command.h','console_state.c','console_state.h','execution.c','execution.h','presentation.c','presentation.h','text_frame.c','text_frame.h' | ForEach-Object {
            $path = 'src/ntvwm-exe/' + $_
            [ordered]@{ path = $path; sha256 = Get-NodeSha256 (Join-Path $root $path) }
        })
    }
    frontendWindowComposition = [ordered]@{
        target = 'frontend-window.lib'
        consumer = 'ntcon.exe'
        disposition = 'private frontend presentation closure; session runtime wiring pending, not product Window acceptance'
        provenance = 'src/ntcon-exe/nxvm-import.json'
        nativeFont = [ordered]@{
            source = 'src/mvdm/softpc.new/roms/v7vga.rom'
            sha256 = Get-NodeSha256 (Join-Path $root 'src/mvdm/softpc.new/roms/v7vga.rom')
            offset = 0x2230
            bytes = 3584
            disposition = 'immutable bitmap-only data; no firmware execution or guest modification'
            generator = 'tools/build/Generate-FrontendFont.ps1'
            generatorSha256 = Get-NodeSha256 (Join-Path $root 'tools/build/Generate-FrontendFont.ps1')
        }
        sources = @($frontendWindowSources | ForEach-Object {
            [ordered]@{ path = $_; sha256 = Get-NodeSha256 (Join-Path $root $_) }
        })
    }
    selectedX86GeneratedInputs = @('PigReg_c.h', 'sas4gen.h', 'gdpvar.h' | ForEach-Object {
        [ordered]@{
            path = 'src/ntvdm-exe/softpc/include/generated/x86/prod/' + $_
            sha256 = Get-NodeSha256 (Join-Path $ccpuProductIncludeRoot $_)
        }
    })
    selectedCcpuFallbacks = @(
        [ordered]@{
            path = 'src/ntvdm-exe/softpc/mvdm_softpc_ccpu_fallback.c'
            selector = 'MVDM_CCPU_VECTOR_DEFAULTS_ONLY'
            symbols = @('EDL_fast_bop', 'c_sas_touch', 'c_VirtualiseInstruction')
            buildDisposition = 'compile-and-force-link-debugbreak-vector-defaults-only'
        }
        [ordered]@{
            path = 'src/ntvdm-exe/softpc/mvdm_softpc_ccpu_fallback.c'
            selector = 'MVDM_CCPU_ACTIVITY_CHECK_ONLY'
            symbols = @('ActivityCheckAfterTimeSlice')
            buildDisposition = 'compile-and-normal-link-empty-activity-callback-only'
        }
    )
    forbiddenInputs = @('src.old', 'bochs-core', 'adapter-bochs')
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8

Write-Host "Generated T310 original SoftPC CCPU candidate graph: $build (CCPU sources: $(@($ccpuNames).Count))"
