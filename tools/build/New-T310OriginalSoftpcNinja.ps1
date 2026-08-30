[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('x86')] [string]$Architecture,
    [string]$RepositoryRoot = '',
    [string]$BuildRoot = '',
    [string]$NodeExecutable = ''
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
    $match = [regex]::Match($raw, '(?s)SOURCES\s*=\s*(.*?)(?=\r?\n\s*(?:!INCLUDE|UMTYPE|i386_SOURCES|MIPS_SOURCES|ALPHA_SOURCES))')
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
$vs = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
if (!(Test-Path -LiteralPath $vs -PathType Leaf) -or !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    throw 'MSVC Build Tools and Ninja are required.'
}

$ccpuRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/ccpu386'
$ccpuOverlayRoot = Join-Path $root 'src/mvdm-host-overlay/softpc.new/base/ccpu386'
$biosRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/bios'
$keymouseRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/keymouse'
$systemRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/system'
$disksRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/disks'
$supportRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/support'
$videoRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/video'
$cvidcRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/cvidc'
$gdpGenerator = Join-Path $root 'tools/build/Generate-T310GdpSlots.mjs'
$gdpOverlayRoot = Join-Path $root 'src/mvdm-host-overlay/softpc.new/base/cvidc'
$umbOverlayRoot = Join-Path $root 'src/mvdm-host-overlay/softpc.new/host/src'
$commandOverlayRoot = Join-Path $root 'src/mvdm-host-overlay/dos/command'
$commsRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/comms'
$dosRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/dos'
$demRoot = Join-Path $root 'src/mvdm-host/dos/dem'
$commandRoot = Join-Path $root 'src/mvdm-host/dos/command'
$xmsRoot = Join-Path $root 'src/mvdm-host/xms.486'
$dpmiRoot = Join-Path $root 'src/mvdm-host/dpmi32'
$xmsOverlayRoot = Join-Path $root 'src/mvdm-host-overlay/xms.486'
$suballocRoot = Join-Path $root 'src/mvdm-host/suballoc'
$oemuniRoot = Join-Path $root 'src/mvdm-host/oemuni'
$sessionRoot = Join-Path $root 'src/session'
$baseDebugRoot = Join-Path $root 'src/mvdm-host/softpc.new/base/debug'
$hostRoot = Join-Path $root 'src/mvdm-host/softpc.new/host/src'
$hostEntryRoot = Join-Path $root 'src/mvdm-host/softpc.new/obj.vdm'
$adapterSoftpcRoot = Join-Path $root 'src/adapter-mvdm-host-out/softpc'
$adapterWin32Root = Join-Path $root 'src/adapter-mvdm-host-out/win32/source'
$appRoot = Join-Path $root 'src/app'
$adapterBaseSrvRoot = Join-Path $root 'src/adapter-mvdm-host-out/basesrv/source'
$adapterMonitorRoot = Join-Path $root 'src/adapter-mvdm-host-out/monitor/source'
$patchRoot = Join-Path $root 'src/mvdm-softpc-patch/x86/prod'
$patchBodyRoot = Join-Path $root 'src/mvdm-softpc-patch/patches/common'
$patchEvidenceRoot = Join-Path $root 'src/mvdm-softpc-patch/patches'
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
$xmsManifest = Join-Path $xmsRoot 'sources'
$dpmiManifest = Join-Path $dpmiRoot 'sources'
$suballocManifest = Join-Path $suballocRoot 'sources'
$oemuniManifest = Join-Path $oemuniRoot 'sources'
$hostManifest = Join-Path $hostRoot 'sources'
$hostEntrySource = Join-Path $hostEntryRoot 'ntvdm.c'
$ccpuNames = Get-OriginalSources $ccpuManifest
# The historical manifest carries the real FPU body and a host-profile stub
# carrier in the same lexical list.  The selected CPU_40/FPU profile is the
# real `fpu.c` body; selecting `ntstubs.c` beside it creates duplicate FPU and
# SAS fallback definitions during a complete-library forced link.  Retain the
# original stub as mirror evidence, but do not select the alternate body for
# this profile.  The separately selected NTVDMx64 CCPU-vector-default patch
# owns only its registered default symbols.
$ccpuNames = @($ccpuNames | Where-Object { $_ -ne 'ntstubs.c' })
# The original CCPU manifest omits the identical `vglob.c` carrier even though
# the selected original video sources call its public VGLOB accessors. Select
# the same-named original CCPU-root form rather than synthesize those globals.
$ccpuNames = @($ccpuNames + 'vglob.c' + 'localfm.c') | Select-Object -Unique
$ccpuOverlayNames = @('localfm.c')
$ccpuSasFacadeSource = Join-Path $ccpuOverlayRoot 'sas_overwrite_memory.c'
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
$xmsNames = @(Get-OriginalSources $xmsManifest)
# `i386_SOURCES` is an NT4 kernel-VDM host variant: it writes the host LDT,
# queries VDM kernel feature bits and mutates the fixed NTVDM V86-state page.
# It is retained in the complete mirror but is not a CCPU40 source member.
# The product has no kernel VDM/V86 route; selecting the manifest's portable
# `SOURCES` block preserves the original CCPU-compatible DPMI owner package.
# The portable original fallback bodies supply the dispatch members that the
# CCPU40 profile reaches without taking the NT4 i386 kernel-VDM branch.
$dpmiNames = @((Get-OriginalSources $dpmiManifest) + 'dpmimemr.c' + 'dpmimscr.c' | Select-Object -Unique)
# XMS uses the complete original SubAlloc package.  Keeping it out of the
# machine graph made its allocator algorithms look like a collection of
# missing adapter symbols, rather than selecting their original owner.
$suballocNames = @(Get-OriginalSources $suballocManifest)
$oemuniNames = @(Get-OriginalSources $oemuniManifest)
$xmsOverlayNames = @('xms_a20_state.c')
$sessionNames = @('mapping_manager.c', 'guest_memory_lease.c', 'session.c')
# `trace_file` belongs to the selected SoftPC base debug implementation.  The
# separate MVDM `dbg` product is a debugger/CSR owner package and must not be
# smuggled into this machine candidate merely to satisfy this one trace edge.
$baseDebugNames = @('trace.c')
# Select the complete original SoftPC host source package before deciding which
# unresolved edges are genuine modern-boundary adapters.  Individual fixtures
# and hand-written stand-ins are not a source-selection mechanism.
$hostNames = @(Get-OriginalSources $hostManifest)
# `nt_cprgs.c` is a selected original host carrier.  The historical
# `nt_aorc.c` bridge does not cover the 32-bit CCPU register surface reached
# by DPMI; the complete original bridge is generated in CVIDC `accessfn.c`.
$hostNames = @($hostNames + 'nt_cprgs.c') | Select-Object -Unique
if (!(Test-Path -LiteralPath $hostEntrySource)) { throw "Original SoftPC host entry missing: $hostEntrySource" }
$adapterWin32Names = @('dialog_context.c', 'ntioapi_facade.c', 'thread_start_compat.c',
                          'nt_thread_alert_compat.c', 'nt_wait_compat.c',
                          'opennt_support_rtl.c', 'console_compat.c')
# ExitVDM is an original Base client call reached by selected SoftPC teardown
# sources.  It already has one same-shaped session-owned implementation; keep
# it in the formal closure instead of accepting an unresolved external edge.
$adapterBaseSrvNames = @('base_vdm_client.c', 'base_vdm_local.c')
$adapterMonitorNames = @('vdm_control.c')
$adapterSoftpcNames = @('mvdm_softpc_firmware.c', 'mvdm_xms_memory.c', 'mvdm_a20.c', 'mvdm_softpc_physical_mapping.c', 'mvdm_host_identity.c',
                        'mvdm_guest_location.c', 'mvdm_command_redirection.c', 'mvdm_command_guest_state.c',
                        'mvdm_vdd_sft_shadow.c', 'mvdm_softpc_execution.c', 'mvdm_softpc_termination.c')
$appNames = @('machine_shell.c')
$effectiveAddressSource = Join-Path $adapterSoftpcRoot 'mvdm_softpc_effective_address.c'
$effectiveAddressObject = 'obj/adapter-softpc/mvdm_softpc_effective_address.obj'
$patchNames = @('PigReg_c.h', 'sas4gen.h', 'gdpvar.h')
$patchBodyNames = @('fmstubs.c')
$patchEvidenceNames = @('minnt/callconv.patch')
foreach ($name in $ccpuNames) {
    if (!(Test-Path -LiteralPath (Join-Path $ccpuRoot $name))) { throw "Original CCPU source missing: $name" }
}
foreach ($name in $ccpuOverlayNames) {
    if (!(Test-Path -LiteralPath (Join-Path $ccpuOverlayRoot $name))) { throw "Required CCPU private overlay missing: $name" }
}
if (!(Test-Path -LiteralPath $ccpuSasFacadeSource)) { throw "Required CCPU SAS facade missing: $ccpuSasFacadeSource" }
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
foreach ($name in $xmsOverlayNames) {
    if (!(Test-Path -LiteralPath (Join-Path $xmsOverlayRoot $name))) { throw "MVDM XMS private overlay source missing: $name" }
}
foreach ($name in $sessionNames) {
    if (!(Test-Path -LiteralPath (Join-Path $sessionRoot $name))) { throw "Required session source missing: $name" }
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
foreach ($name in $adapterSoftpcNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterSoftpcRoot $name))) { throw "Required SoftPC adapter source missing: $name" }
}
foreach ($name in $appNames) {
    if (!(Test-Path -LiteralPath (Join-Path $appRoot $name))) { throw "Required app composition source missing: $name" }
}
foreach ($name in $adapterMonitorNames) {
    if (!(Test-Path -LiteralPath (Join-Path $adapterMonitorRoot $name))) { throw "Required monitor adapter source missing: $name" }
}
foreach ($name in $patchNames) {
    if (!(Test-Path -LiteralPath (Join-Path $patchRoot $name))) { throw "Registered SoftPC patch carrier missing: $name" }
}
foreach ($name in $patchBodyNames) {
    if (!(Test-Path -LiteralPath (Join-Path $patchBodyRoot $name))) { throw "Registered SoftPC patch body missing: $name" }
}
foreach ($name in $patchEvidenceNames) {
    if (!(Test-Path -LiteralPath (Join-Path $patchEvidenceRoot $name))) { throw "Registered SoftPC patch evidence missing: $name" }
}

New-Item -ItemType Directory -Force $build, (Join-Path $build 'generated/gdp'), (Join-Path $build 'obj/ccpu'), (Join-Path $build 'obj/bios'), (Join-Path $build 'obj/keymouse'), (Join-Path $build 'obj/system'), (Join-Path $build 'obj/disks'), (Join-Path $build 'obj/support'), (Join-Path $build 'obj/video'), (Join-Path $build 'obj/cvidc'), (Join-Path $build 'obj/comms'), (Join-Path $build 'obj/dos'), (Join-Path $build 'obj/dem'), (Join-Path $build 'obj/command'), (Join-Path $build 'obj/xms'), (Join-Path $build 'obj/dpmi'), (Join-Path $build 'obj/suballoc'), (Join-Path $build 'obj/session'), (Join-Path $build 'obj/debug'), (Join-Path $build 'obj/host'), (Join-Path $build 'obj/adapter-softpc'), (Join-Path $build 'obj/adapter-win32'), (Join-Path $build 'obj/patch') | Out-Null
if (!(Test-Path -LiteralPath $gdpGenerator -PathType Leaf)) { throw "GDP slot generator missing: $gdpGenerator" }
if (!(Test-Path -LiteralPath $gdpOverlayRoot -PathType Container)) { throw "GDP overlay root missing: $gdpOverlayRoot" }
if (!(Test-Path -LiteralPath $umbOverlayRoot -PathType Container)) { throw "UMB overlay root missing: $umbOverlayRoot" }
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
$gdpGeneratedRoot = Join-Path $build 'generated/gdp'
& $NodeExecutable $gdpGenerator $root $gdpGeneratedRoot | Out-Null
$environment = Join-Path $build 'msvc-mt.cmd'
@('@echo off', 'set "MVDM_T310_CALLER_CWD=%CD%"', 'if defined VSCMD_VER goto ready',
  ('call "' + $vs + '" -arch=' + $Architecture + ' -host_arch=x64 >nul'),
  'if errorlevel 1 exit /b %errorlevel%', ':ready', 'cd /d "%MVDM_T310_CALLER_CWD%"', '%*') |
    Set-Content -LiteralPath $environment -Encoding ascii

$includeRootPaths = @(
    'src',
    # The adapter owns the modern `nt.h` type binding. Original reached NT
    # public-header subsets are restored under opennt-host below, so source
    # files still resolve historical short names without an adapter copy.
    'src/adapter-mvdm-host-out/win32/include',
    # Reached original non-MVDM OpenNT declaration slices retain their
    # source identity under opennt-host. Keep this after the adapter's nt.h:
    # nt.h owns modern type binding while this directory owns original
    # public-header subsets such as ntexapi.h.
    'src/opennt-host/public/sdk/inc',
    'src/opennt-abi/source/public/sdk/inc',
    'src/opennt-abi/source/public/internal/base/inc',
    'src/opennt-abi/source/public/internal/windows/inc',
    'src/opennt-abi/source/private/windows/inc',
    'src/opennt-abi/source/public/ddk/inc',
    'src/mvdm-host/inc',
    # NTVDMx64's original patch script deletes the CCPU-local generated GDP
    # carrier, then supplies this selected x86 product carrier. Keep the
    # mirror source intact and express that historical selection in build
    # include order instead.
    'src/mvdm-softpc-patch/x86/prod',
    'src/mvdm-host/xms.486',
    # DPMI's original precompiled header owns dpmidata.h beside its source
    # bodies.  Select the original directory rather than copying the carrier
    # into an adapter or overlay.
    'src/mvdm-host/dpmi32',
    'src/mvdm-host/softpc.new/base/ccpu386',
    'src/mvdm-host/softpc.new/host/inc',
    # Original sas.h includes generated sas4gen.h. The selected mirror retains
    # the CVIDC generated carrier; the historical host/genPg output is absent.
    'src/mvdm-host/softpc.new/base/cvidc',
    'src/mvdm-host-overlay/softpc.new/base/cvidc',
    'src/mvdm-host-overlay/softpc.new/host/src',
    'src/mvdm-host-overlay/dos/command',
    'src/mvdm-host/softpc.new/base/inc',
    'src/adapter-mvdm-host-out/softpc/include',
    'src/adapter-mvdm-host-out/basesrv/include',
    'src/adapter-mvdm-host-out/monitor/include',
    'src/session'
)
$includeRoots = $includeRootPaths | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }
$gdpGeneratedInclude = '/I "' + (NinjaPath $gdpGeneratedRoot) + '"'

# `base/inc/egacpu.h` deliberately includes one of two original generated
# `evidgen.h` variants.  The C-video and base/video translation units require
# the C-video variant; CCPU itself keeps its own shorter generated variant.
# This is original generated-header provenance, not a product-wide include
# preference or a replacement API.
$cvidcFirstRootPaths = [System.Collections.Generic.List[string]]::new()
foreach ($path in $includeRootPaths) {
    if ($path -eq 'src/mvdm-host/softpc.new/base/cvidc') { continue }
    if ($path -eq 'src/mvdm-host/softpc.new/base/ccpu386') {
        $cvidcFirstRootPaths.Add('src/mvdm-host/softpc.new/base/cvidc')
    }
    $cvidcFirstRootPaths.Add($path)
}
$cvidcFirstIncludeRoots = $cvidcFirstRootPaths | ForEach-Object { '/I "' + (NinjaPath (Join-Path $root $_)) + '"' }

# `i386` is never a product-wide host switch. The current CCPU recovery build
# retains its original CPU40 compatibility carrier definitions; any unavoidable historical x86-only unit must carry
# its own registered, target-local compilation exception.
# `softpc.new/obj.vdm/cdefine.inc` selects CCPU's generated C video-memory
# backend through C_VID for both the historical x86 and non-x86 CCPU paths.
# Without that original configuration carrier cvidc's generated glue is
# compiled out, leaving artificial same-package forced-link misses.
$baseCommonFlags = '/nologo /TC /c /MT /W4 /showIncludes /D_NO_CRT_STDIO_INLINE /DWIN32 /DWINNT /DOPENNT_ADAPTER_NT_ALERT_THREAD /DMVDM_SOFTPC_NO_HOST_BOOT_FILE_MUTATION /DNTVDM /DCPU_40_STYLE /DNEW_CPU /DCCPU /DC_VID /DSPC386 /DSIM32 /DANSI /DPROD ' +
    '/FI "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/nt.h')) + '" ' +
    ''
$baseFlags = $baseCommonFlags + ($includeRoots -join ' ') + ' ' + $gdpGeneratedInclude
$dpmiMonitorInclude = '/I "' + (NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/monitor/include')) + '"'
# The DPMI source revision was built against the reached monitor VDM_TIB
# carrier (PmStackInfo and interrupt/fault fields).  Give that original-shaped
# monitor contract precedence for this package only; the global OpenNT ABI
# header remains the default for unrelated original packages.
$dpmiFlags = $baseCommonFlags + $dpmiMonitorInclude + ' ' + ($includeRoots -join ' ') + ' ' + $gdpGeneratedInclude
$cvidcFirstFlags = $baseCommonFlags + ($cvidcFirstIncludeRoots -join ' ') + ' ' + $gdpGeneratedInclude
$cvidcRuleFlags = $cvidcFirstFlags + ' /DCVIDC_RULE_WORD'
# `accessfn.c` intentionally emits the complete CCPU getXX/setXX facade only
# outside the historical `PROD` build condition.  Compile that single original
# generated unit under its own original condition; all other C-video units
# retain the product profile.
$cvidcAccessFlags = $cvidcRuleFlags.Replace('/DPROD ', '')
$patchVectorDefaultsFlags = $baseFlags + ' /DMVDM_SOFTPC_PATCH_CCPU_VECTOR_DEFAULTS_ONLY'

$graph = [Collections.Generic.List[string]]::new()
$graph.Add('ninja_required_version = 1.10')
$graph.Add('build_root = ' + (NinjaPath $build))
$graph.Add('cflags = ' + $baseFlags)
$graph.Add('dpmi_cflags = ' + $dpmiFlags)
$graph.Add('cvidc_first_cflags = ' + $cvidcFirstFlags)
$graph.Add('cvidc_rule_cflags = ' + $cvidcRuleFlags)
$graph.Add('cvidc_access_cflags = ' + $cvidcAccessFlags)
$graph.Add('patch_vector_defaults_cflags = ' + $patchVectorDefaultsFlags)
$graph.Add('')
$graph.Add('rule cc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cc_cvidc')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cvidc_first_cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cc_dpmi')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $dpmi_cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cc_cvidc_rule')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cvidc_rule_cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cc_cvidc_access')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $cvidc_access_cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule cc_patch_vector_defaults')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' cl.exe $patch_vector_defaults_cflags /Fo$out $in')
$graph.Add('  deps = msvc')
$graph.Add('  msvc_deps_prefix = Note: including file:')
$graph.Add('rule lib')
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' lib.exe /nologo /out:$out $in')
$graph.Add('rule forced_link_audit')
# This deliberately produces a non-runnable DLL.  /WHOLEARCHIVE makes the
# candidate's complete original membership visible to LINK; /FORCE keeps the
# unresolved physical forms in the adjacent log for source-first ownership.
$graph.Add('  command = cmd.exe /d /s /c call ' + (NinjaPath $environment) + ' link.exe /nologo /dll /noentry /force:unresolved /force:multiple /out:$out /implib:$out.lib /wholearchive:original-ccpu386.lib /wholearchive:original-softpc-bios.lib /wholearchive:original-softpc-keymouse.lib /wholearchive:original-softpc-system.lib /wholearchive:original-softpc-disks.lib /wholearchive:original-softpc-support.lib /wholearchive:original-softpc-video.lib /wholearchive:original-softpc-cvidc.lib /wholearchive:original-softpc-comms.lib /wholearchive:original-softpc-dos.lib /wholearchive:original-mvdm-dem.lib /wholearchive:original-mvdm-command.lib /wholearchive:original-mvdm-xms.lib /wholearchive:original-mvdm-dpmi32.lib /wholearchive:original-mvdm-host-suballoc.lib /wholearchive:original-mvdm-host-oemuni.lib /wholearchive:original-softpc-base-trace.lib /wholearchive:original-softpc-host-roots.lib /wholearchive:softpc-bindings.lib /wholearchive:softpc-win32-bindings.lib /wholearchive:basesrv-bindings.lib /wholearchive:monitor-bindings.lib /wholearchive:session.lib /wholearchive:mvdm-softpc-effective-address.lib /wholearchive:ntvdmx64-softpc-patch-evidence.lib ntvdmx64-softpc-ccpu-vector-defaults.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib legacy_stdio_definitions.lib libcmt.lib libvcruntime.lib libucrt.lib > $out.log 2>&1')

$ccpuObjects = foreach ($name in $ccpuNames) {
    $object = 'obj/ccpu/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    # CCPU itself owns `Sas`; the original localfm state carrier also declares
    # it.  The narrow overlay retains only this profile's original Gdp/Cpu/
    # Video state declarations and avoids a second SAS owner.
    $source = if ($name -in $ccpuOverlayNames) { Join-Path $ccpuOverlayRoot $name } else { Join-Path $ccpuRoot $name }
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath $source))
    $object
}
$ccpuSasFacadeObject = 'obj/ccpu/sas_overwrite_memory.obj'
$graph.Add('build ' + $ccpuSasFacadeObject + ': cc ' + (NinjaPath $ccpuSasFacadeSource))
$ccpuObjects += $ccpuSasFacadeObject
$biosObjects = foreach ($name in $biosNames) {
    $object = 'obj/bios/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $biosRoot $name)))
    $object
}
$keymouseObjects = foreach ($name in $keymouseNames) {
    $object = 'obj/keymouse/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $keymouseRoot $name)))
    $object
}
$systemObjects = foreach ($name in $systemNames) {
    $object = 'obj/system/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $systemRoot $name)))
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
    $object
}
$cvidcObjects = foreach ($name in $cvidcNames) {
    $object = 'obj/cvidc/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $rule = if ($name -eq 'accessfn.c') { 'cc_cvidc_access' } else { 'cc_cvidc_rule' }
    $graph.Add('build ' + $object + ': ' + $rule + ' ' + (NinjaPath (Join-Path $cvidcRoot $name)))
    $object
}
$gdpOverlaySource = Join-Path $gdpOverlayRoot 'mvdm_gdp_state.c'
$graph.Add('build obj/cvidc/mvdm_gdp_state.obj: cc ' + (NinjaPath $gdpOverlaySource))
$cvidcObjects += 'obj/cvidc/mvdm_gdp_state.obj'
$commsObjects = foreach ($name in $commsNames) {
    $object = 'obj/comms/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $commsRoot $name)))
    $object
}
$dosObjects = foreach ($name in $dosNames) {
    $object = 'obj/dos/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $dosRoot $name)))
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
    $object
}
$xmsObjects = foreach ($name in $xmsNames) {
    $object = 'obj/xms/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $xmsRoot $name)))
    # Exact original XMS manifest `C_DEFINES=-DWIN_32`.
    $graph.Add('  cflags = ' + $baseFlags + ' /DWIN_32')
    $object
}
$xmsOverlayObjects = foreach ($name in $xmsOverlayNames) {
    $object = 'obj/xms/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $xmsOverlayRoot $name)))
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
    $object = 'obj/session/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $sessionRoot $name)))
    $object
}
$baseDebugObjects = foreach ($name in $baseDebugNames) {
    $object = 'obj/base-debug/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $baseDebugRoot $name)))
    $object
}
$hostObjects = foreach ($name in $hostNames) {
    $object = 'obj/host/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $hostRoot $name)))
    if ($name -in @('nt_timer.c', 'nt_thred.c', 'nt_com.c', 'nt_event.c', 'nt_error.c')) {
        $threadCompat = NinjaPath (Join-Path $root 'src/adapter-mvdm-host-out/win32/include/thread_start_compat.h')
        # `nt.h` from the modern SDK can predefine the historical include
        # guard before the original source reaches <ntexapi.h>.  Force the
        # selected opennt-host subset after nt.h so the original timer's
        # declarations stay visible on both architectures.
        $ntexapiSubset = NinjaPath (Join-Path $root 'src/opennt-host/public/sdk/inc/ntexapi.h')
        $graph.Add('  cflags = ' + $baseFlags + ' /FI "' + $ntexapiSubset + '" /FI "' + $threadCompat + '"')
    }
    $object
}
$umbOverlaySource = Join-Path $umbOverlayRoot 'mvdm_umb_address.c'
$graph.Add('build obj/host/mvdm_umb_address.obj: cc ' + (NinjaPath $umbOverlaySource))
$hostObjects += 'obj/host/mvdm_umb_address.obj'
$eoiOverlaySource = Join-Path $umbOverlayRoot 'mvdm_ica_eoi_bridge.c'
$graph.Add('build obj/host/mvdm_ica_eoi_bridge.obj: cc ' + (NinjaPath $eoiOverlaySource))
$hostObjects += 'obj/host/mvdm_ica_eoi_bridge.obj'
$hostEntryObject = 'obj/host/ntvdm_entry.obj'
$graph.Add('build ' + $hostEntryObject + ': cc ' + (NinjaPath $hostEntrySource))
# Preserve the exact original entry body while reserving the executable entry
# point for app.  This is a build binding only, not a source edit or a second
# startup implementation.
$graph.Add('  cflags = ' + $baseFlags + ' /Dmain=mvdm_softpc_original_entry')
$hostObjects += $hostEntryObject
$commandWriteLengthOverlaySource = Join-Path $commandOverlayRoot 'mvdm_command_write_length.c'
$graph.Add('build obj/host/mvdm_command_write_length.obj: cc ' + (NinjaPath $commandWriteLengthOverlaySource))
$hostObjects += 'obj/host/mvdm_command_write_length.obj'
$commandLengthOverlaySource = Join-Path $commandOverlayRoot 'mvdm_command_length.c'
$graph.Add('build obj/host/mvdm_command_length.obj: cc ' + (NinjaPath $commandLengthOverlaySource))
$hostObjects += 'obj/host/mvdm_command_length.obj'
$adapterWin32Objects = foreach ($name in $adapterWin32Names) {
    $object = 'obj/adapter-win32/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterWin32Root $name)))
    $object
}
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
$adapterSoftpcObjects = foreach ($name in $adapterSoftpcNames) {
    $object = 'obj/adapter-softpc/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $adapterSoftpcRoot $name)))
    $object
}
$appObjects = foreach ($name in $appNames) {
    $object = 'obj/app/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $appRoot $name)))
    $object
}
$graph.Add('build ' + $effectiveAddressObject + ': cc ' + (NinjaPath $effectiveAddressSource))
$patchBodyObjects = @(foreach ($name in $patchBodyNames) {
    $object = 'obj/patch/' + [IO.Path]::GetFileNameWithoutExtension($name) + '.obj'
    $graph.Add('build ' + $object + ': cc ' + (NinjaPath (Join-Path $patchBodyRoot $name)))
    $object
})
$patchVectorDefaultsObject = 'obj/patch/fmstubs_ccpu_vector_defaults.obj'
$graph.Add('build ' + $patchVectorDefaultsObject + ': cc_patch_vector_defaults ' + (NinjaPath (Join-Path $patchBodyRoot 'fmstubs.c')))
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
$graph.Add('build original-mvdm-xms.lib: lib ' + (($xmsObjects + $xmsOverlayObjects) -join ' '))
$graph.Add('build original-mvdm-dpmi32.lib: lib ' + ($dpmiObjects -join ' '))
$graph.Add('build original-mvdm-host-suballoc.lib: lib ' + ($suballocObjects -join ' '))
$graph.Add('build original-mvdm-host-oemuni.lib: lib ' + ($oemuniObjects -join ' '))
$graph.Add('build original-softpc-base-trace.lib: lib ' + ($baseDebugObjects -join ' '))
$graph.Add('build original-softpc-host-roots.lib: lib ' + ($hostObjects -join ' '))
$graph.Add('build softpc-bindings.lib: lib ' + ($adapterSoftpcObjects -join ' '))
$graph.Add('build app-machine-shell.lib: lib ' + ($appObjects -join ' '))
$graph.Add('build session.lib: lib ' + ($sessionObjects -join ' '))
$graph.Add('build mvdm-softpc-effective-address.lib: lib ' + $effectiveAddressObject)
$graph.Add('build softpc-win32-bindings.lib: lib ' + ($adapterWin32Objects -join ' '))
$graph.Add('build basesrv-bindings.lib: lib ' + ($adapterBaseSrvObjects -join ' '))
$graph.Add('build monitor-bindings.lib: lib ' + ($adapterMonitorObjects -join ' '))
$graph.Add('build ntvdmx64-softpc-patch-evidence.lib: lib ' + ($patchBodyObjects -join ' '))
$graph.Add('build ntvdmx64-softpc-ccpu-vector-defaults.lib: lib ' + $patchVectorDefaultsObject)
$graph.Add('build original-softpc-candidate: phony original-ccpu386.lib original-softpc-bios.lib original-softpc-keymouse.lib original-softpc-system.lib original-softpc-disks.lib original-softpc-support.lib original-softpc-video.lib original-softpc-cvidc.lib original-softpc-comms.lib original-softpc-dos.lib original-mvdm-dem.lib original-mvdm-command.lib original-mvdm-xms.lib original-mvdm-dpmi32.lib original-mvdm-host-suballoc.lib original-mvdm-host-oemuni.lib original-softpc-base-trace.lib original-softpc-host-roots.lib softpc-bindings.lib app-machine-shell.lib softpc-win32-bindings.lib basesrv-bindings.lib monitor-bindings.lib session.lib mvdm-softpc-effective-address.lib ntvdmx64-softpc-patch-evidence.lib ntvdmx64-softpc-ccpu-vector-defaults.lib')
$graph.Add('build original-softpc-forced-closure.dll: forced_link_audit original-ccpu386.lib original-softpc-bios.lib original-softpc-keymouse.lib original-softpc-system.lib original-softpc-disks.lib original-softpc-support.lib original-softpc-video.lib original-softpc-cvidc.lib original-softpc-comms.lib original-softpc-dos.lib original-mvdm-dem.lib original-mvdm-command.lib original-mvdm-xms.lib original-mvdm-dpmi32.lib original-mvdm-host-suballoc.lib original-mvdm-host-oemuni.lib original-softpc-base-trace.lib original-softpc-host-roots.lib softpc-bindings.lib app-machine-shell.lib softpc-win32-bindings.lib basesrv-bindings.lib monitor-bindings.lib session.lib mvdm-softpc-effective-address.lib ntvdmx64-softpc-patch-evidence.lib ntvdmx64-softpc-ccpu-vector-defaults.lib')
$graph.Add('default original-softpc-candidate')
[IO.File]::WriteAllText((Join-Path $build 'build.ninja'), (($graph -join [Environment]::NewLine) + [Environment]::NewLine), [Text.UTF8Encoding]::new($false))

[ordered]@{
    schema = 'm0.t310.original-softpc-machine-candidate.v3'
    architecture = $Architecture
    cpuProfile = 'CCPU40'
    toolchain = 'MSVC /MT via VsDevCmd'
    i386Define = $false
    originalCcpuManifest = 'src/mvdm-host/softpc.new/base/ccpu386/sources'
    originalHostManifest = 'src/mvdm-host/softpc.new/host/src/sources'
    ccpuSourceCount = @($ccpuNames).Count
    ccpuSources = @($ccpuNames)
    ccpuPrivateOverlaySources = @($ccpuOverlayNames)
    ccpuSasFacadeSource = 'src/mvdm-host-overlay/softpc.new/base/ccpu386/sas_overwrite_memory.c'
    biosSources = @($biosNames)
    keymouseSources = @($keymouseNames)
    systemSources = @($systemNames)
    disksSources = @($disksNames)
    supportSources = @($supportNames)
    videoSources = @($videoNames)
    cvidcSources = @($cvidcNames)
    commsSources = @($commsNames)
    dosSources = @($dosNames)
    demSources = @($demNames)
    commandSources = @($commandNames)
    xmsSources = @($xmsNames)
    dpmiSources = @($dpmiNames)
    xmsPrivateOverlaySources = @($xmsOverlayNames)
    suballocSources = @($suballocNames)
    oemuniSources = @($oemuniNames)
    baseDebugSources = @($baseDebugNames)
    baseDebugBuildDisposition = 'selected-original-softpc-trace-only; mvdm-debugger product excluded'
    hostRoots = @($hostNames)
    hostEntrySource = 'src/mvdm-host/softpc.new/obj.vdm/ntvdm.c'
    adapterSoftpcSources = @($adapterSoftpcNames)
    appSources = @($appNames)
    sessionSources = @($sessionNames)
    adapterWin32Sources = @($adapterWin32Names)
    patchInputs = @($patchNames | ForEach-Object {
        [ordered]@{
            path = 'src/mvdm-softpc-patch/x86/prod/' + $_
            sha256 = Get-NodeSha256 (Join-Path $patchRoot $_)
        }
    })
    patchBodies = @($patchBodyNames | ForEach-Object {
        [ordered]@{
            path = 'src/mvdm-softpc-patch/patches/common/' + $_
            sha256 = Get-NodeSha256 (Join-Path $patchBodyRoot $_)
            buildDisposition = 'compile-and-archive-debugbreak-evidence-only'
        }
    })
    patchSelectedRuntimeBodies = @([ordered]@{
        path = 'src/mvdm-softpc-patch/patches/common/fmstubs.c'
        selector = 'MVDM_SOFTPC_PATCH_CCPU_VECTOR_DEFAULTS_ONLY'
        symbols = @('EDL_fast_bop', 'c_sas_touch', 'c_VirtualiseInstruction')
        buildDisposition = 'compile-and-force-link-original-debugbreak-vector-defaults-only'
    })
    patchEvidence = @($patchEvidenceNames | ForEach-Object {
        [ordered]@{
            path = 'src/mvdm-softpc-patch/patches/' + $_
            sha256 = Get-NodeSha256 (Join-Path $patchEvidenceRoot $_)
        }
    })
    forbiddenInputs = @('src.old', 'bochs-core', 'adapter-bochs')
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $build 'source-manifest.json') -Encoding utf8

Write-Host "Generated T310 original SoftPC CCPU candidate graph: $build (CCPU sources: $(@($ccpuNames).Count))"
