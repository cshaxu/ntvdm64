# OpenNT Historical Host Stack Build Graph

Status: active research direction as of 2026-08-06. The fixed historical
inputs now live under `src/opennt/`; their standalone, default-off CMake
targets are the authoritative object-closure gate. Earlier
`toolchain-probe/` references below are chronological evidence only, retained
to explain individual compatibility discoveries. This document does not
authorize default-runner linkage, changes to `ntvdm64`, guest-media
distribution, host installation, injection, registry changes, or system-file
deployment.

## Current Authoritative State

- `ntdos64-opennt-host-stack-objects` builds all 16 original DEM units plus
  the selected original BIOS, `sim32`, `nt_bop`, ICA, and QEVNT units using
  the isolated i686 clang-cl configuration. The final record is
  `artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/host-stack-final-object-closure.log`.
- This is **object closure only**. Its unresolved-symbol inventory identifies
  historical CCPU, BIOS/device, OEM/media, optional-profile, and host/CRT
  boundaries; it does not create an executable or a shim implementation.
- The original CCPU target now has compile-only closure using an
  owner-authorized, separately manifested patch-derived x86 SAS overlay. Its
  record is `artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/ccpu-overlay-gate.log`. No
  default target consumes it, and it has not been linked or executed.

### Initialized CCPU Link Experiment (2026-08-06)

`ntdos64-opennt-ccpu-register-bridge-fixture` is a default-disabled,
non-runtime experiment that calls only `c_cpu_init` and the one-to-one
register bridge. It deliberately does not call `c_cpu_simulate`, dispatch a
BOP/SVC, or make a DOS startup decision.

Its CCPU source gate remains the complete 130-unit historical closure. The
link archive uses the separately recorded 129-unit software-FPU profile: it
excludes only `ntstubs.c`, because `fpu.c` and `ntstubs.c` both define the
otherwise no-op `initialise_npx` and `npx_reset` exports. This is an explicit
profile selection, not ` /force:multiple` or a source patch. The archive is
then combined with default-disabled original monitor/BIOS/system/support,
keyboard/mouse, disk, serial/parallel, and video provider objects.

The experiment established two repeatable historical-build facts:

- DEM's `Vr*` VDMREDIR function pointers are C tentative globals, while
  `nt_bop.c` owns initialized defaults. The historical host-stack target uses
  clang's `-fcommon` compatibility mode so those declarations retain their
  original common-symbol meaning; no duplicate implementation is selected.
- With original provider objects linked, the remaining unresolved symbols are
  exclusively host seams: `EDL_fast_bop`, timer/event and error/trace hooks,
  CCPU exception/software-interrupt hooks, SAS allocation, mouse install,
  BOP bridge, and ICA lock/IRQ notification callbacks. No unresolved DEM SVC
  is satisfied by substitute code.

Thus the fixture is intentionally still non-linkable and non-runnable. Its
current negative result is the baseline for a narrow modern-host shim; it is
not evidence that a hand-written BIOS, DEM handler, IVT/BDA scaffold, or DOS
runtime should be added.

## Decision

The executable reference is the historical stack as a stack, not a growing
collection of replacement SVC handlers:

```text
CCPU / SoftPC monitor
  -> BIOS BOP table
  -> NT BOP dispatcher
  -> DEM
  -> NTDOS runtime and DOS programs
  -> narrow modern-host compatibility shim
  -> research ntvdm.exe
```

The hand-written CCPU BOP/DEM adapter remains frozen as a bounded startup
trace tool. It proves entry-state and transition facts only. It must not gain
additional individual SVC implementations in place of the historical
dispatcher.

This ordering preserves the existing global DOS state and its implicit
sequencing: DEM owns services over DTA/PDB/SFT and redirector-related state;
the historical BOP dispatcher owns service-byte fetch and IP advancement; the
BIOS table owns selector-to-service assignment. The result lets a failed modern
run be classified as either a historical protocol dependency or a missing host
compatibility facility.

## Source-Proven Dispatch Path

| Layer | Historical source | Proven role |
| --- | --- | --- |
| CPU execution | `base/mvdm/softpc.new/base/ccpu386` | Executes guest instructions and recognizes historical BOP encodings. |
| Monitor event loop | `base/mvdm/v86/monitor/i386/monitor.c` | `EventVdmBop` dispatches a monitor-delivered BOP number through `BIOS[]`; this specific monitor depends on the removed VDM host model. |
| BIOS BOP table | `base/mvdm/softpc.new/base/bios/bios.c` | In an `NTVDM` build, selectors `50h..5Fh` map to `MS_bop_0..MS_bop_F`; ordinary BIOS selectors remain owned by their established services. |
| NT BOP layer | `base/mvdm/softpc.new/host/src/nt_bop.c` | `MS_bop_0` reads the byte at `CS:IP` through `Sim32GetVDMPointer`, calls `DemDispatch(DemCmd)`, then advances `IP` by one. |
| DEM dispatcher | `base/mvdm/dos/dem/demdisp.c` | `apfnSVC[]` maps the service byte to the original DEM routines and maintains their shared process/DOS state. |
| DOS runtime | `base/mvdm/dos/v86/doskrnl` | `NTIO.SYS` requests the DEM load transition; `NTDOS.SYS` subsequently uses the established BOP/SVC surface. |

The source graph directly refutes an implementation that replaces every
`50h/<service>` by independent callbacks: the service table is not merely a
switch statement; it is connected to historical global state and filesystem,
process, and error paths.

## Original Component Build Graph

```text
ccpu386 + generated CPU headers
          |
          v
softpc base libraries: system, bios, support, keymouse, disks, XMS/DPMI as selected
          |
          +--> host/src: sim32, nt_bop, timing, memory, input, console, error, VDD adapters
          |
          +--> v86/monitor: monitor, sas, int, thread, fastpm
          |
          +--> dos/dem: dem, demdisp, demfile, demdir, demhndl, demsrch, demerror, ...
                    |
                    +--> dos/command + NTIO.SYS + NTDOS.SYS + DOS programs
```

The primary manifests are:

- `softpc.new/base/bios/sources`: `bios.c`, `reset.c`, RTC/CMOS, memory and
  control-BOP functions;
- `softpc.new/base/system/sources`: DMA, PIC, ROM, timer, event scheduling;
- `softpc.new/base/keymouse/sources`: keyboard, PPI and mouse state;
- `softpc.new/base/support/sources`: basic lifecycle, I/O, time-of-day;
- `softpc.new/host/src/sources`: historical NT host bindings including
  `sim32.c`, `nt_bop.c`, `nt_event.c`, `nt_reset.c`, `nt_sec.c`, and console,
  display, input, media and VDD bindings;
- `v86/monitor/i386/sources`: original monitor loop, SAS, interrupt/thread
  support, and `fastpm.asm`;
- `dos/dem/sources`: the 16-file DEM service/state library;
- `v86/scaffold/sources`: the early `ntvdm` composition which links monitor,
  XMS, DEM, debugger and command libraries.

## Modern-Host Classification

### Directly useful historical code

These are candidates for direct source compilation in an isolated x86 staging
build, subject first to header/toolchain repair only:

- CCPU instruction implementation and SAS mechanics;
- BIOS BOP table and most base device/state code;
- `nt_bop.c` service-byte boundary and the original `DemDispatch` table;
- DEM's DOS namespace, file, process, DTA/PDB/SFT and error-state logic;
- DOS runtime, command interpreter, drivers and programs produced by the
  historical build.

“Directly useful” does not mean current Windows can supply every dependency;
it means the source is retained at its original function and data boundary
rather than translated into per-service replacement code.

### Requires a narrow compatibility shim

The original monitor and host sources contain legacy `nt.h`, `ntrtl.h`,
`nturtl.h`, `vdm.h`, `NtVdmControl`, VDM TEB/context, console/server, device,
and native object-manager assumptions that current ordinary Win32 processes do
not provide. The shim is the only place allowed to adapt these assumptions.

Its first contract groups are:

1. **Guest address/context:** preserve `Sim32GetVDMPointer` and register access
   semantics over private CCPU/SAS guest memory; do not expose host pointers as
   guest addresses.
2. **BOP/run control:** replace monitor-only VDM event delivery with an
   in-process bounded run loop that enters the existing `BIOS[]` table at the
   historical dispatch boundary. It must preserve the original BOP service-byte
   and IP behavior.
3. **Process/console/streams:** map the chosen CLI's inherited handles,
   standard input/output/error, Ctrl-C and child-launch behavior to the
   historical host call boundary. No console-server ABI, injection, or global
   configuration is permitted.
4. **Filesystem and media:** provide containment-rooted Win32 file/directory
   operations plus explicit logical media profiles. Native symbolic-link and
   VDM-drive queries are not assumed available.
5. **Time, input, scheduling:** provide explicit clock, keyboard/mouse and
   bounded event/IRQ policies. The existing CCPU IRQ ambiguity remains open;
   no PIC/IRQ support is claimed until an end-to-end trace exists.
6. **Errors and diagnostics:** translate historical hard-error/debug paths to
   local structured diagnostics, never system dialogs or protected binary
   capture.

The shim must prefer documented Win32 APIs where an equivalent exists. It must
not require a missing VDM subsystem or rely on unverified/private host ABI.

## Staged Build Order

1. Create a source-only manifest for the selected `NTVDM` configuration and
   all generated headers/assembler inputs; record exact snapshot revision and
   no-content hashes where appropriate.
2. Compile, without editing upstream, the original CCPU, BIOS BOP table,
   `nt_bop.c`, and DEM sources in isolated staging. Record each compile error
   as one of: portable C/toolchain repair, missing historical header, or host
   service boundary.
3. Build the first shim as headers and implementation outside the upstream
   checkout, sufficient to link the original dispatcher path. It must not add
   a new SVC switch or alter the DEM table.
4. Restore original base BIOS/device initialization in dependency order, first
   only as far as the bounded CLI smoke trace reaches.
5. Run a bounded private-image smoke sequence:
   `NTIO.SYS -> NTDOS.SYS -> COMMAND.COM /C <owned test command>`.
   Every stop must be classified and logged; success requires a command exit
   status and no host-global side effect.
6. Only after this reference executes should its observed contracts be reduced
   to neutral `mantle` capability requirements. No `ntvdm64` code is changed
   by this work.

## Current Evidence And Immediate Next Task

The frozen v16 trace already proves the historical BOP encoding and that a
source-derived early scaffold IVT is traversed. It stops at the scaffold's
default `BOP_UNIMPINT` after the original-style startup sequence. That is not
a reason to add a `5Fh` hand-written service. The immediate task is instead a
source-only compile/link inventory for the original `bios.c` table,
`nt_bop.c`, and the full DEM library, with unresolved symbols assigned to one
of the shim contract groups above.

### Compile-only inventory v1 (2026-08-06)

An isolated x86 `clang-cl` compile of original `demdisp.c`, `nt_bop.c`, and
`bios.c` was run with the OpenNT MVDM, base, host, and public-SDK include roots
only. The first pass produced no objects and did not modify the upstream
snapshot. Logs and compiler inputs are under
`toolchain-probe/opennt-host-stack-inventory-v1/`.

| Source | First blocker | Classification |
| --- | --- | --- |
| `dos/dem/demdisp.c` | `dem.h` reaches `oemuni.h` before the historical build's Windows base types (`HANDLE`, `BOOL`, `DWORD`, `WINAPI`) are established. | Historical precompiled-header/include-order contract; a staging compatibility preamble, not a DEM service rewrite. |
| `softpc.new/host/src/nt_bop.c` | The OpenNT public NT headers conflict with modern x86 SDK context declarations (`PCONTEXT`, `CONTEXT`, `KSYSTEM_TIME`, `PLDT_ENTRY`). | Legacy NT header universe; must be isolated behind a controlled compatibility-header layer before host APIs are classified. |
| `softpc.new/base/bios/bios.c` | Old public C runtime declarations conflict with modern UCRT `perror` declarations, then generated `CpuInt_c.h` is absent from the selected include graph. | Two separate build-input issues: C runtime/header compatibility and generated CCPU configuration closure. Neither justifies replacing the BIOS table. |

Adding only a staging `windows.h` preamble and the source-required `DEVL=1`
declaration configuration produces the unmodified original `demdisp.c` object
(`SHA-256 1B5D3C71F80625C76E1D8383C991A2D797E607DDB3E0C6A53D767F1351A35F29`).
This is source-closure evidence, not a linked or runnable DEM result.

The first full DEM pass with exactly that preamble/configuration compiled 7 of
the 16 manifest sources: `dem.c`, `demdata.c`, `demdir.c`, `demdisp.c`,
`demlock.c`, `demmsg.c`, and `demlabel.c`. A subsequent staging-only
release-`ASSERT` include-order repair plus removal of the modern
`GetDiskSpaceInformation` macro collision compiled the unmodified
`demfcb.c` too. A `demfile.c` staging overlay then isolates the current SDK's
incompatible `nt_vdd.h` declarations and restores the source's own historical
`VDDRetrieveNtHandle` forward declaration. It removes one obsolete `PVOID **`
cast at that self-call; it does not alter FCB/SFT/JFT or file-service logic.
An additional `demhndl.c` overlay replaces one compiler-invalid `STOREWORD`
lvalue cast with the equivalent direct assignment to its packed `USHORT`
`ExtendedError` field; all PDB/SFT and file-handle behavior remains original.
The current result is therefore 10 of 16 source-closed DEM objects:
`demfcb-prod-devl.obj` SHA-256 is
`5C888371775423F77C2B091ECA502E0DB70BAF55D6ED538C73EC7BB9C275B9A0`.
The `demfile.c` overlay SHA-256 is
`287860D7A8C1736FEB9F73116A80CF3217DB835B53620687F16E8EE64F38B74F`, and
its object SHA-256 is
`164B6C5BBAAB0761AFC84A2A60F2498C115905DDC90A49A32406BDFEBBB3BE69`.
The `demhndl.c` overlay SHA-256 is
`4D05BBF1980888B6AA2A87B3499996166F0BC3D064431975B63171C30BF8A431`, and
its object SHA-256 is
`598992F2E5A94F0F55670670D4B8BC2D58A0107F3DB93557198639E32EA23455`.
The remaining first failures are:

| Sources | First blocker | Classification |
| --- | --- | --- |
| `demerror.c` | Missing legacy NT types such as `PUNICODE_STRING`. | NT header compatibility layer. |
| `demgset.c` | `MEDIA_TYPE` was initially absent. | The remaining source reaches native path/volume queries and needs filesystem/media dependency classification. |
| `demioctl.c`, `demdasd.c` | `MEDIA_TYPE` was initially absent. | Closed by the original OpenNT public `winioctl.h` under the required i686 configuration; their unresolved providers remain a separate contained-media seam. |
| `demmisc.c` | Undeclared legacy calls at its process/host boundary. | Candidate modern-host shim surface. |
| `demsrch.c` | Missing private `winbasep.h`. | Legacy private-header dependency; must be replaced by a narrow, documented compatibility declaration or isolated. |

This is a useful first cut: the original DEM dispatch table has object-level
closure, and the remaining failures are now individually classified. No
modern-host shim behavior has been implemented or inferred from these errors.

### DEM disk-module header closure (2026-08-06)

The two original DEM disk modules were then recompiled under the same isolated
i686 `clang-cl` staging profile. The only compatibility change was to include
OpenNT's own `public/sdk/inc/winioctl.h` from the shared staging preamble after
its historical `windows.h`; this restores the historical `MEDIA_TYPE`,
`DISK_GEOMETRY`, and `IOCTL_DISK_GET_DRIVE_GEOMETRY` declarations. No current
SDK disk header, device provider, media backend, DOS service, or controller
logic was added.

The resulting original-source objects are:

| Source | Object | SHA-256 | Compile errors |
| --- | --- | --- | --- |
| `demdasd.c` | `obj/demdasd-historical-winioctl-i686-diagnostic.obj` | `3B206193180211FA3D957D44044F642A0EEA1147C539337F7520203CCE3A7EEC` | 0 |
| `demioctl.c` | `obj/demioctl-historical-winioctl-i686-diagnostic.obj` | `C8D872053F0B1131F7C9EF0A24F7B0741111AE13F2AAEC804174DDC03F96FAF5` | 0 |

This advances the compile inventory to 12 of 16 DEM modules; it is not a
runtime or media-admission claim. `demdasd.c` imports the original
`nt_floppy_*` and `nt_fdisk_*` providers, the original INT 13h/CCPU accessor
surface, and ordinary diagnostic/error helpers. `demioctl.c` calls the
original DEM DASD helpers and, in its device-parameter fallback, direct Win32
`CreateFileA`/`DeviceIoControl`/`CloseHandle`. A future contained-media layer
must constrain those exact historical provider and call boundaries. It may not
replace `demDasd*`, DOS IOCTL dispatch, media-change policy, or INT 13h
semantics with a new service implementation. In particular, these objects do
not authorize access to host physical drives or arbitrary host paths.

### Remaining `demgset.c` seam classification (2026-08-06)

Reading the original source confirms that its first native-header failure is
not a purely mechanical compile problem. `GetPhysicalDriveType` converts each
`A:\\` through RTL routines, opens the resulting NT object, and queries volume
device information before `demGetDrives` populates the historical
`PhysicalDriveTypes[26]` table. That is SH-04's contained-drive query seam:
the shim may supply results for configured guest drives only, while DEM keeps
the drive-count, DPB, and error decisions.

The same source's `demSetDate` and `demSetTime` call `SetLocalTime`. A
non-invasive runtime cannot permit that call to alter the host clock. This is
recorded as SH-13: a future virtual-clock provider must sit at the original
call boundary and preserve DEM's register/return behavior, without changing
the host's time. `demgset.c` therefore remains outside the object-closure set
until those dependency contracts have a separately verified implementation.

### Historical reference link baseline (2026-08-06)

The source-closed x86 objects are packaged as the non-runnable staging archive
`obj/opennt-historical-reference-v1.lib` (SHA-256
`1EACE247CB95B787E5B55405E413060B4F079DF5AD4C57AA74C4394F22C26E11`).
It contains the original BIOS table, original `sim32.c`, the recorded
`nt_bop.c` overlay, and the ten source-closed DEM modules. Its undefined-symbol
map is `obj/opennt-historical-reference-v1.undefined.txt` (SHA-256
`A51B73EB3ED0C134D34C6A0D6307A19187B60B5C79F651961C2B3002B07D6FA7`).

The map is a link inventory, not a runnable claim. It separates: remaining
original DEM exports still named by `DemDispatch`; original CCPU/SAS register
and guest-memory helpers; original monitor/ICA/XMS/DPMI/CMD/debug providers;
and direct public Win32 imports such as file, time, and loader APIs. The
future shim may address only the explicitly admitted host-provider subset in
the shim ledger. It may not satisfy unresolved DEM service names with a new
replacement dispatcher.

The focused `demerror.c` compile probe refines its classification: it is not
only a missing-type/header issue. Its remote-device/error path uses
`NtQueryVolumeInformationFile`, `NtQueryObject`, `FILE_FS_DEVICE_INFORMATION`,
and native object-name data to decide DOS-facing error behavior. This is an
explicit filesystem/device shim dependency. A later shim must preserve the
original `demerror` caller/callee boundary and supply a documented equivalent
or explicit unavailable result; it must not replace the DEM error SVC logic or
make the result a compile-time constant merely to obtain an object.

The focused `demgset.c` probe confirms the same distinction for drive state:
after public `ntdddisk.h` provides `MEDIA_TYPE`, its first remaining failures
are historical native path conversion and volume/device queries
(`RtlOemStringToUnicodeString`, `RtlDosPathNameToNtPathName_U`,
`NtOpenFile`, `NtQueryVolumeInformationFile`, `OBJECT_ATTRIBUTES`, and
`FILE_FS_DEVICE_INFORMATION`). Its `demGetDriveFreeSpace`/media paths are
therefore a filesystem/media shim dependency, not a missing PC/AT controller
or a justification for a replacement DEM service. The shim must eventually
provide an equivalent contained-drive/media answer at the original call
boundary, or the configuration must return the historical error path.

`demsrch.c` likewise refines the earlier `winbasep.h` classification. It owns
the historical DOS-facing DTA, FCB, PSP file-find list, wildcard and find-id
state itself; those structures must remain in DEM. Its host-facing lower layer
uses native path conversion/status translation plus directory-handle query,
reset and close operations. The future filesystem shim therefore supplies
contained directory enumeration at the existing `FileFindOpen`/
`FileFindNext`/`FileFindReset` boundaries. It must not replace
`demFindFirst`, `demFindNext`, FCB search, or the DEM PSP/DTA state machine
with a separate DOS find implementation.

The focused `demmisc.c` compile probe reaches `DbgPrompt` and
`NtQueryInformationProcess(ProcessDebugPort)` first. These are historical
debugger-prompt/debug-port detection dependencies, not evidence that the
normal DEM process state must be reimplemented. The initial non-invasive CLI
profile may declare the optional debugger prompt unavailable. If later
enabled, a shim must bind it to the project's private diagnostic/debug-console
surface and preserve the existing `demmisc` call boundary; it must not add a
new DOS SVC or use host injection/debug-port manipulation.

The original `softpc.new/base/bios/bios.c` table also compiles to an x86 object
with the historical `NTVDM`, `MONITOR`, `CPU_40_STYLE`, `CCPU`, `NEW_CPU`,
`SPC386`, and `ANSI` definitions, plus the provenance-recorded generated
`CpuInt_c.h` input. Its SHA-256 is
`D9918B8F307517DAF0C3B7BB193A867BFECE071ECFB8350EAB542EF4BB04023B`.
The initial failure was only a conflict between two OpenNT declarations of the
Unix utility `memset4`; a diagnostic `REAL_VGA` definition suppresses the
irrelevant `host.h` declaration and exposes the full table. This definition is
not accepted as a final machine configuration: the build layer must instead
resolve that declaration mismatch without selecting unrelated video behavior.
The result proves the original `BIOS[]` table can reach object-level closure;
it neither links device services nor enables any BOP at runtime.

### NT BOP dispatcher closure (2026-08-06)

The original `softpc.new/host/src/nt_bop.c` has now reached x86 object-level
closure in the isolated staging build.  The produced object is
`toolchain-probe/opennt-host-stack-inventory-v1/obj/nt_bop-modern-host-overlay.obj`
(SHA-256 `86F5DD64A641EF38D6A02B2CE0C35F037674A34C808979BC6D2F7B4309949350`).

The upstream source remains unmodified.  The staging overlay is a copy with
the following limited source-port changes, recorded here rather than hidden in
compiler warning suppression:

| Overlay change | Reason | Scope |
| --- | --- | --- |
| Historic `MYFARPROC` casts from `GetProcAddress` pass through `PVOID`. | Current clang rejects the implicit conversion between the historic cdecl callback declaration and modern Win32 `FARPROC`.  The historic callback declaration, variable types, and calls are retained. | Dynamic WOW/VDMREDIR/ISV module-entry acquisition only. |
| Two ISV-loader assignments use `GetProcAddress` directly. | Their destination variables are already `FARPROC`; the historic intermediate cast is incompatible with the current declaration. | ISV loader only. |
| Build defines `DbgBreakPoint=DebugBreak`. | `MS_bop_B` expects a historical debug helper.  `DebugBreak` is the public Win32 equivalent for this debug-only BOP. | Debug BOP only; it does not alter `MS_bop_0`. |

The overlay source SHA-256 is
`40EE0A2CB5A71E487851954D30FAF160B2950E300D0A68AD8AE973F617E6922A`.
`MS_bop_0` remains textually equivalent to upstream: it reads the service byte
with `Sim32GetVDMPointer`, calls `DemDispatch`, and increments `IP` once.  No
DEM SVC, BIOS BOP selector, VDM API, or host behavior was substituted.

This evidence closes only the dispatcher's **compile** boundary.  It does not
validate any dynamically loaded module, link the dispatcher, implement
`NtVdmControl`, emulate monitor event delivery, or establish a runnable DOS
session.  The remaining task is to link this original dispatcher with the
original DEM and CCPU objects behind a narrow host shim, then observe a bounded
`NTIO.SYS -> NTDOS.SYS -> COMMAND.COM /C` path.

### Monitor boundary diagnosis (2026-08-06)

The original monitor confirms both the desired preservation point and the
portion that cannot be carried to an ordinary modern Win32 process unchanged:

```text
cpu_simulate
  -> real mode: NtVdmControl(VdmStartExecution)
  -> protected mode: FastEnterPm
  -> receives VdmTib.EventInfo from the NT VDM subsystem
  -> adds EventInfo.InstructionSize to EIP
  -> EventVdmBop
  -> BIOS[EventInfo.BopNumber]()
  -> MS_bop_0
  -> DemDispatch(service byte), then IP += 1
```

`EventVdmBop` itself is small and preserves the central original table call:
it range-checks `BopNumber`, invokes `BIOS[BopNumber]`, and restores its monitor
TEB bookkeeping.  `cpu_simulate`, however, obtains real-mode execution exits
and `VDMEVENTINFO` only through `NtVdmControl(VdmStartExecution)`, with
`VdmTib`, V86 flag handling, interrupt queueing, and monitor-thread state as
part of the same removed host contract.  `int.c`, `nt_eoi.c`, and `nt_sec.c`
also call `NtVdmControl` for interrupt queueing, delay, and initialization.

Therefore the first modern-host shim must replace the **event producer**
(execution slice -> explicit BOP/fault/stop event) rather than replace the
BIOS or DEM tables **only for a V86-monitor source port**. The direct CCPU
interpreter is a separate historical path: `c_main.c` decodes its BOP and calls
the `bop(n)` macro, defined as `(*BIOS[n])()`, directly. The initial CCPU
profile must preserve that direct call and must not route it through an event
shim. If the V86 monitor is ported, its BOP event must provide a bounded,
private execution result containing at least BOP number, instruction size, and
CS:EIP/register state; the historical monitor then calls the existing `BIOS[]`
entry exactly once. `MS_bop_0` continues to own service-byte consumption and
its one-byte IP advance. This division prevents either execution profile from
becoming a second DEM/BOP dispatcher.

### VDM-only dependency ledger (2026-08-06)

The initial read-only call-site inventory identifies the following direct
`NtVdmControl` dependencies. They are the starting scope of the modern-host
compatibility shim, not permission to recreate the old VDM system ABI.

| Historical caller | Operation | Required modern replacement category |
| --- | --- | --- |
| `v86/monitor/i386/monitor.c` | `VdmFeatures`, `VdmStartExecution`, optional printer direct I/O close | Private execution-slice/event producer; explicit feature policy; no kernel VDM invocation. |
| `v86/monitor/i386/int.c` | `VdmQueueInterrupt` | Private pending-IRQ queue bound to the selected CCPU run loop. |
| `softpc.new/host/src/nt_eoi.c` | `VdmDelayInterrupt` | Deferred interrupt policy in the same private event queue. |
| `softpc.new/host/src/nt_sec.c` | `VdmInitialize` | One-time private interrupt-controller/event bridge setup. |

Related VDM-context dependencies occur in `monitor.c`, `int.c`, `thread.c`,
and `fastpm.asm`: `VdmTib`, current TEB state, thread context, and V86/PM
transition bookkeeping. They require a separate in-process context model;
they must not be answered by a fabricated `NtVdmControl` export. The host
files `nt_cpu.c`, `nt_error.c`, `nt_fdisk.c`, `nt_rflop.c`, and `stubs.c` also
refer to legacy context/VDM types, but the present inventory has not yet shown
a direct `NtVdmControl` call in them.

By contrast, the object-closed `nt_bop.c` does not call `NtVdmControl`.
For the first bounded DOS smoke path, it can remain original dispatcher code
behind the private event bridge. Network/redirector, WOW, ISV-module, printer
and debugger BOP paths remain explicitly unvalidated and out of the initial
CLI scope.

### Monitor compile diagnostic (2026-08-06)

A compile-only pass of the unmodified `v86/monitor/i386/monitor.c` reaches its
first blockers before any `NtVdmControl` call is type-checked:

1. `vdmtib.h` includes the historical absolute build path
   `\nt\private\inc\vdm.h`, which is unavailable outside the original tree.
2. `host/inc/nt_mon.h` defines `boolean` as `BOOL`, conflicting with the
   `unsigned char` RPC declaration that arrives through current public SDK
   headers.

Both are historical header-composition defects, not evidence for a runtime
shim behavior. The next source-port layer must supply a canonical staged
`vdm.h` path and prevent the incompatible public SDK/RPC include composition.
Only after that closure is it meaningful to compile-classify the monitor's
actual execution, TEB/context, and `NtVdmControl` requirements. No monitor
overlay or host behavior has been added at this stage.

The next diagnostic pass supplied the staged `vdmtib.h`, removed the redundant
`nt_mon.h` `boolean` typedef, and enabled the VDM user-mode declarations only
inside staged `vdm.h` with `OPENNT_STAGE_VDM_TYPES`. It intentionally did not
define `_NTDEF_` globally because that alters current Windows SDK header
behavior. This closes the header-path/type gate and exposes these real monitor
port categories:

| Remaining `monitor.c` blocker | Classification |
| --- | --- |
| `NTVDMpLockPrefixTable` pointer value is stored in a historical 32-bit `DWORD` initializer. | 32-bit monitor/fastpm ABI port; preserve an explicit 32-bit guest/monitor address contract. |
| `KGDT_R3_DATA`, `KGDT_R3_TEB`, and old V86 monitor selectors. | Removed kernel/TEB selector model; requires private monitor-context policy, not a public Win32 substitution. |
| `NtGetContextThread`, `NtCurrentThread`, `NtCurrentTeb`. | Historical native-context model; a private current-thread/context adapter is required. |
| `FLOATING_SAVE_AREA.Cr0NpxState`. | Obsolete x86 context layout; translate the intended DOS FPU control policy using supported x86 context operations or a CCPU-owned FPU initialization path. |
| Historic lvalue cast and `int`/`LONG` pointer mismatch. | Narrow C dialect/source-port repairs. |
| `NumTasks`, assertion and printer definitions. | Historical configuration/header closure; separate from the execution ABI. |

This confirms that an unmodified historical `monitor.c` cannot be the first
runtime executable on current Windows. The reference implementation should
first retain its **observable role**--bounded execution, event dispatch, and
one `BIOS[]` call per BOP--in a private event bridge. A later transparent
monitor source-port may compare against that bridge, but the bridge must not
invent substitute DOS/BOP service handlers.

### NT BOP link-boundary ledger (2026-08-06)

`llvm-nm --undefined-only` on the compiled original-dispatcher overlay yields
the following relevant unresolved groups. This is direct link evidence, rather
than an inference from source names.

| Unresolved group | Expected provider | Shim rule |
| --- | --- | --- |
| `_DemDispatch`, `_demDasdInit` | Original DEM library | Must remain original DEM linkage. |
| `_XMSDispatch`, `_DpmiDispatch`, `_CmdDispatch`, `_DBGDispatch` | Original historical XMS/DPMI/CMD/debug libraries selected by the original configuration | Do not replace with per-service callbacks. |
| `_c_get*`, `_c_set*`, `_c_sas_hw_at` | CCPU/SAS object set | Direct CPU/guest-memory linkage. |
| `_Sim32pGetVDMPointer` | Original `sim32` source or a layout-compatible private guest-address adapter | Must preserve segmented-address, access-size, and mode semantics. |
| `_DelayIrqLine`, `_host_ica_lock`, `_host_ica_unlock`, `_ica_iret_hook_called`, `_ica_restart_interrupts`, `_MonitorEndIretHook` | Monitor/ICA event bridge | Private event/interrupt adapter; no system VDM API. |
| `_WaitIfIdle`, `_WakeUpNow`, `_HostIdleNoActivity`, `_TerminateVDM` | Modern host shim | Bounded session lifecycle and scheduling only. |
| `LoadLibraryA`, `GetProcAddress`, `FreeLibrary`, `CloseHandle`, `DebugBreak`, `OutputDebugStringA` | Documented Win32 | May link directly; dynamic WOW, redirector, and ISV use remains outside initial CLI validation. |

The object has no unresolved `NtVdmControl` reference. Consequently, the
first reference link can retain the original BOP/DEM path while isolating
obsolete VDM execution-event production to the monitor bridge. This ledger is
not a proof that all listed providers are source-closed or runnable; it is the
contract for the next link inventory.

### Original Sim32 translation closure (2026-08-06)

The unmodified `softpc.new/host/src/sim32.c` compiles directly in the isolated
x86 staging configuration. Its object is
`obj/sim32-original-diagnostic.obj` (SHA-256
`5414FEFD6EE8B5F22C3869402E307B8AC8C6CE4B1B01E1B998515ECC62951F1D`), and
`llvm-nm --undefined-only` reports no unresolved symbols.

This is important boundary evidence: the original
`Sim32pGetVDMPointer`/`Sim32GetVDMPointer` route used by `MS_bop_0` can be
retained as direct historical code in the current CCPU configuration. No
modern-host shim is needed merely to translate the service-byte address at
`CS:IP`. It does not by itself prove full pointer safety or a runnable monitor,
but it narrows the first shim to execution events, context, interrupts, and
external host capabilities rather than guest-address translation.

### Optional full-screen initialization boundary (2026-08-06)

`softpc.new/host/src/nt_sec.c` shows that its `NtVdmControl(VdmInitialize)`
call is confined to `GetROMsMapped()` under `#ifdef X86GFX`. That same routine
requests host ROM mapping and wires historical ICA/IRET callback pointers into
the old VDM subsystem. A first non-invasive CLI reference build may therefore
select the historical non-`X86GFX` configuration and exclude this optional
full-screen graphics path. It must state that choice in its build manifest;
it does not emulate, replace, or silently claim the unavailable host-ROM/VDM
initialization behavior.

The non-`X86GFX` functions in `nt_sec.c` (`LoseRegenMemory` and
`RegainRegenMemory`) still use historical native virtual-memory APIs and are
separate source-port candidates. They are not on the first DOS command
execution path until a trace demonstrates otherwise.

### NTVDMx64 HAXM comparison and exclusion (2026-08-06)

The experimental HAXM monitor provides a narrow corroborating comparison, not
a candidate runtime implementation. Its `cpu_simulate` loop handles HAXM
execution exits, identifies the historical `C4 C4` BOP encoding, advances the
guest instruction pointer, and calls `EventVdmBop`; its `EventVdmBop` selects
and invokes `BIOS[iobuf[2]]`. This confirms the desired preservation point:
an execution-event producer belongs below the historical BIOS dispatch table.

It cannot satisfy SH-01 directly. The producer is tied to HAXM exit structures
and `DeviceIoControl` calls, and the source recognizes a specific BOP encoding
rather than providing a neutral execution-event contract. Separately,
`ntvdmpatch/src/ldntvdm/ldntvdm/ldntvdm.c:FixNTDLL` obtains
`ntdll!NtVdmControl`, changes page protection, and overwrites its code. That
is host-process patching, prohibited by the research profile. No HAXM source,
loader patch, instruction-specific interception, or `FixNTDLL` behavior is
adopted. The only resulting design conclusion is already recorded in SH-01:
the private event bridge must report a bounded event and invoke the historical
`BIOS[]` entry exactly once, without choosing BOP or SVC semantics itself.

### Direct CCPU path and link inventory (2026-08-06)

The staged historical CCPU source changes the first executable-path priority.
`softpc.new/base/inc/bios.h` defines `bop(n)` as `(*BIOS[n])()`. In
`softpc.new/base/ccpu386/c_main.c`, both the `C4 C4` BOP decoding path and the
`0xD6` BOP instruction path update CCPU's instruction pointer and invoke that
macro directly. Consequently, the direct CCPU profile is:

```text
c_cpu_simulate -> historical bop(n) -> historical BIOS[n]
  -> MS_bop_0 -> DemDispatch -> DEM
```

This is a historical primary path, not an exception to the restoration rule.
It must not be wrapped in a synthetic `VDMEVENTINFO` or routed through the
V86-only SH-01 bridge. The V86 monitor remains source evidence only and is
not an implementation candidate for the current CCPU-only software-emulation
profile.

A staging-only `probe/ccpu-link-probe.c` references `c_cpu_init` and
`c_cpu_simulate`; it is never executed and contains no BOP or DOS behavior.
Forcing that member from `ccpu386-fpu-profile.lib` through `lld-link` produced
the actual external-closure inventory at
`obj/ccpu-link-probe.full-link.txt` (SHA-256
`579CC86B79B89AC5FEA7AFC828AF85750109D1B5D23AEADC0CBD3A9036879741`).
The probe source SHA-256 is
`247694308181FB69608F49F75FD1788A35BECCAEDCFAE12D592A53E0510770DC`; its
x86 object SHA-256 is
`2360323A759157E2D9B0DE13A6BB796CE2B74C1749EA3C07CCCAF42975CA2F76`.

The remaining symbols group into original SoftPC dependencies, not missing
DOS services:

| Link group | Symbols observed | Required next action |
| --- | --- | --- |
| Historical event/timer/ICA | `dispatch_q_event`, `host_timer_event`, `ica_intack`, `ica_hw_interrupt` | Source-close the original event manager, timer, and ICA modules; admit a modern timing/interrupt shim only at their documented host call sites. |
| Historical BIOS table and optional fast path | `BIOS`, `EDL_fast_bop` | Link the original BIOS table. The OpenNT MVDM snapshot has no `EDL_fast_bop` provider; the first profile records-and-stops on that optional `0xFE` extension and must not substitute a selector switch. |
| Original port-I/O routing | `Ios_in_adapter_table`, `Ios_inb_function`, `Ios_inw_function`, `Ios_out_adapter_table`, `Ios_outb_function`, `Ios_outw_function`, `sub_io_verbose` | Source-close the historical I/O dispatch layer and its device providers before assigning any modern adapter seam. |
| SAS/ROM initialization | `host_sas_init`, `host_sas_term`, `rom_init`, `copyROM`, `host_error` | Reconnect the historical SAS/ROM initialization chain; no scaffold IVT/BDA/ROM profile may replace it in normal execution. |
| Optional/diagnostic and mode hooks | `trace_file`, `host_exint_hook`, `host_swint_hook`, `host_simulate`, `LIM_b_write`, `LIM_w_write`, `read_pointers`, `AlreadyInYoda` | Classify each by the selected first CCPU configuration. Debug-only behavior may be explicitly unavailable; protected-mode, LIM, and host-simulation paths require original-provider evidence before admission. |

This is a deliberately failing link inventory, not a runnable CCPU claim. Its
important result is architectural: CCPU can retain the historical direct BOP
call, but it requires the surrounding original SoftPC layers to be restored
before any narrow modern-host replacements can be responsibly identified.

### Original I/O dispatcher source-closure diagnostic (2026-08-06)

The original `softpc.new/base/support/ios.c` was copied unchanged into the
research staging tree and compiled as a source-closure diagnostic. The first
attempt stopped before I/O code because legacy host headers caused current UCRT
`stdlib` and `stdio` declarations of `perror`/`_wperror` to arrive with
incompatible historical qualifiers. A staging-only preamble that includes the
current `stdio.h` and `stdlib.h` before legacy headers resolves that include
order problem; it does not alter `ios.c` or guest-visible behavior. The
existing staged `ccpu386/cpu4gen.h` is also the required generated-header
input.

The subsequent compile reaches the original `VIRTUALISATION` branch and stops
on the absent declaration/provider contract for `IOVirtualised`, plus the
historical `BOOL`/`IBOOL` configuration mismatch at `io_connect_port`. These
are not permission to replace port dispatch. `ios.c` itself owns the adapter
tables and dispatches through `Ios_in*_function`/`Ios_out*_function`; the
missing next work is to recover its matching historical NTVDM monitor-I/O
configuration and provider. No self-authored port handler, PC/AT policy, or
device callback has been added.

Subsequent configuration review identifies `GISP_CPU` as the original
software-emulation selector for this module: it suppresses the
`VIRTUALISATION` branch, leaving the historical adapter-table route. This is
the relevant direction for CCPU source closure. V86 execution and
`NtVdmControl` are excluded from the active profile.

Under the CCPU-only `PROD`, `DEVL=1`, `GISP_CPU`, `SPC386` profile, the staged
source reaches one current-toolchain incompatibility: `ios.h` declares
`io_connect_port` as `IBOOL`, while the NTVDM source definition spells its
return type `BOOL`. In the historical header order `BOOL` expands to unsigned
`UINT`; current clang therefore correctly rejects the signed `IBOOL` public
contract mismatch. The staging overlay changes that single definition spelling
to `IBOOL`; its function body, adapter tables, port mapping, and all handlers
remain textually unchanged. It compiles to
`obj/ios-gisp-ccpu-overlay.obj` (SHA-256
`6804FA12254AE855677C39E895FE4422E062882A50FF2008480BE722A5B4AC07`).
The unchanged staged source SHA-256 is
`E4F39908FF9534ADDE52E46DEF732A93790AC48F2BB481590622C537C2F3B205`; the
overlay source SHA-256 is
`30873E56FF12CDC149E880DA730E4850375F05E0121E1492225036B70CB590FC`.

Linking that object with the existing CCPU inventory probe removes all six
previous `Ios_*` unresolved symbols (`Ios_in_adapter_table`,
`Ios_inb_function`, `Ios_inw_function`, `Ios_out_adapter_table`,
`Ios_outb_function`, and `Ios_outw_function`). The resulting deliberately
failing link report is `obj/ccpu-ios-link-probe.full-link.txt` (SHA-256
`E1361DC58E7A4CA2D18D01DCE3AEFCBDACF81C5C17F3E82473F52E81388FA04C`).
This proves original I/O-dispatch closure only. It neither initializes an I/O
device nor runs a DOS program; no substitute port, device, BOP, or DEM
behavior was introduced.

### Original ICA source-closure diagnostic (2026-08-06)

The original `softpc.new/base/system/ica.c` now source-closes unchanged under
the CCPU-only `PROD`, `DEVL=1`, `GISP_CPU`, `SPC386`, `CPU_40_STYLE` profile.
`CPU_40_STYLE` is required because CCPU uses the CPU 4.x interface rather than
the old `cpu_int_delay` global. The object is
`obj/ica-original-ccpu-profile.obj` (SHA-256
`7735B8F4A7ACDA0549AEEC85067EB847E662FC40C2D28AB874B03F60045C99D1`), from
unchanged staged source SHA-256
`3874E59A27410E56C2D8C28B576C528D82ACD2F04E64C50664D1ECDA9A846075`.

Linking it with the CCPU and original-I/O inventory removes CCPU's
`ica_intack` and `ica_hw_interrupt` unresolved symbols. The new deliberately
failing link report is `obj/ccpu-ios-ica-link-probe.full-link.txt` (SHA-256
`BF7AAC636316F6A25DEFBEEEB51ECAFC771B454F95587B1B37C9C3A73720AB50`). It
exposes ICA's actual host seams: `host_ica_lock`, `host_ica_unlock`,
`host_set_hw_int`, `host_clear_hw_int`, `host_EOI_hook`, `DelayIrqLine`, and
the historically optional `WOWIdle`. These are not an invitation to replace
ICA or synthesize IRQ behavior. The next work must first source-close the
original providers or explicitly admit a narrow synchronization/notification
shim for each unavailable host call.

### CCPU host-IRQ and quick-event source classification (2026-08-06)

The current CCPU-only profile deliberately excludes the V86 monitor and its
`NtVdmControl` path. This does **not** eliminate all host-side interrupt and
event work: the original CCPU interpreter calls `dispatch_q_event`, and the
original ICA code calls host notification functions. Source inspection locates
the historical CCPU host-notification provider in
`softpc.new/host/src/nt_cpu.c`:

```text
host_set_hw_int()   -> cpu_interrupt(CPU_HW_INT, 0)
host_clear_hw_int() -> no CCPU action in the historical CCPU branch
```

Thus the guest IRQ decision remains in original `ica.c`; the identified
CCPU-facing host operation is only a notification to the interpreter. It is
not a new PC/AT device model and it does not use the V86 monitor.

The unchanged staged `nt_cpu.c` was compiled under the same CCPU profile.
It reaches historical host-build incompatibilities before object closure:
its wider CPU register compatibility section expects legacy VIDC generated
types (`GDP_PTR`, legacy `IU32` sizing), and its debug assertions expect the
historical NT `IcaLock`/TEB layout. Therefore the whole source file cannot be
adopted as the CCPU provider merely because it contains the two small
notification functions. Any future modern adapter must first have a narrow
contract whose only effect is the historical `cpu_interrupt(CPU_HW_INT, 0)`
notification; it must not copy the VIDC compatibility block or create a new
IRQ policy.

The original NT-specific quick-event dispatcher is
`softpc.new/base/system/qevnt.c`. It is a CCPU-path component: its own source
states that it synchronizes its global queue using `host_ica_lock`, and CCPU
calls its `dispatch_q_event` entry directly. Under the selected compiler
profile it reaches the missing legacy NT list macros (`IsListEmpty`,
`InsertHeadList`, and `RemoveEntryList`) before object closure. The diagnostic
also exposes its historical host dependencies: `host_TimeStamp`,
`host_q_ev_set_count`, `host_calc_q_ev_inst_for_time`, and the ICA lock.
This is a host ABI modernization issue, not a reason to replace the original
queue with a new DOS timer or BOP handler.

Neither diagnostic was executed. Both sources remain read-only copies in the
research staging tree; no V86 monitor code, `NtVdmControl` call, synthetic IRQ,
or replacement DOS service was introduced.

The staging preamble now also supplies the three legacy list-operation macros
over the already supplied Windows `LIST_ENTRY` layout. It changes neither the
`qevnt.c` source nor its queue algorithm. With that compiler-only adaptation,
unchanged staged `qevnt.c` source (SHA-256
`3E0D97D17BCC4C107B5173BF38868C79AB68D3042D29472A1A7259364DA2947C`)
object-closes as `obj/qevnt-original-ccpu-profile.obj` (SHA-256
`CFE8B0BE5827ECDE6B100E81CB1B4A5085A3B8389DD0573C163D86AB3C8DB3FD`).

The next deliberately failing combined link inventory includes CCPU, original
I/O, original ICA, and that original quick-event object. Its report is
`obj/ccpu-ios-ica-qevnt-link-probe.full-link.txt` (SHA-256
`006C2BDFF866DA20206D63B7DA6BB3A3DB1771A5D5CF6A106A2AE91D7ED7E190`).
It contains no `undefined symbol: _dispatch_q_event`, proving only that the
CCPU interpreter's original quick-event entry is now linked. It newly exposes
`qevnt.c`'s direct historical dependencies on `host_TimeStamp`, the ICA lock,
and the internal `Cpu` vector. `Cpu` is not a host capability: it belongs to
the historical CCPU support-object configuration and must be resolved from a
compatible original build selection. Attempting to add the nearby historical
`ntstubs.obj` directly is invalid because it duplicates the FPU archive's
`initialise_npx` and `npx_reset` symbols. This records a real historical build
selection issue rather than authorizing a substitute vector or event queue.

### CCPU extended-fast-BOP exclusion (2026-08-06)

`c_main.c` has a second, nonstandard `C4 C4` decoding branch. Normal values
advance the CCPU instruction pointer and call historical `bop(ops[0].sng)`,
which is the required `BIOS[]` route. Only when the decoded immediate's low
byte is `0xFE` does it call `EDL_fast_bop(immed)` (apart from the explicit
`0xFE` unsimulate case and optional `SFELLOW` cases).

There is no `EDL_fast_bop` implementation in the OpenNT MVDM source tree.
It is therefore an unavailable CCPU extension, not an unimplemented Microsoft
DOS SVC and not a reason to define a new BOP ABI. The first CCPU reference
profile excludes it: a bounded trace may record such an instruction if one is
encountered, but it must stop as unsupported. No V86 functionality is involved
in this decision, and no substitute handler is admitted until a real target
binary demonstrates the path and an original-provider provenance is found.

### BIOS table and original NT BOP dispatcher link diagnostic (2026-08-06)

The existing `bios-ntvdm-ccpu-realvga-diagnostic.obj` is a historical BIOS
table object that exports `BIOS`; it is usable only for a link diagnostic. Its
`REAL_VGA` compile flag suppresses an obsolete `memset4` declaration conflict
in `host.h`, but has not been admitted to the first runtime configuration.
It must not be mistaken for a V86 component or for an approved display policy.

Linking that diagnostic object with the original CCPU/I/O/ICA/quick-event
objects and the staged original `nt_bop.c` dispatcher produces
`obj/ccpu-bios-ntbop-link-probe.full-link.txt` (SHA-256
`2013D8AE4860A699CB5C083CDAC0DF36F5122D4BF052E671F56DCF4E35052265`).
The deliberately failing link contains no `undefined symbol: _BIOS`,
`undefined symbol: _MS_bop_0`, or `undefined symbol: _dispatch_q_event`.
It does contain `undefined symbol: _DemDispatch`, which is the intended next
historical boundary after `MS_bop_0` fetches its service byte. This is direct
link evidence for the required CCPU path:

```text
c_cpu_simulate -> BIOS[] -> MS_bop_0 -> DemDispatch
```

The remaining unresolved BIOS names (`reset`, `keyboard_int`, `disk_io`,
`time_of_day`, `video_io`, mouse functions, and related helpers) are original
handler modules. They must be restored or explicitly excluded by the initial
hardware profile; they are not permission to place PC/AT policy in a modern
shim. The dispatcher also exposes original host and service dependencies such
as `Sim32pGetVDMPointer`, idle/terminate helpers, and `DemDispatch`; these
remain subject to their existing historical ownership rules.

### Original DEM dispatcher link diagnostic (2026-08-06)

The staged historical `demdisp.obj` exports `DemDispatch` and was added to the
same deliberately failing diagnostic chain. The resulting report is
`obj/ccpu-bios-ntbop-demdisp-link-probe.full-link.txt` (SHA-256
`8B405ACC74EB2BBBF6800993160E97722861C1762900116E2E2E0AE294BF6D0C`).
It contains no `undefined symbol: _DemDispatch`, while preserving the earlier
absence of unresolved `BIOS`, `MS_bop_0`, and `dispatch_q_event`.

This advances the proven original call graph to:

```text
c_cpu_simulate -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM service functions
```

The report deliberately exposes unresolved service implementations such as
`demOpen` and `demFindFirst`. That is the correct next boundary: the dispatcher
still owns service selection and its state flow, while each corresponding
historical DEM module must be linked or receive a separately admitted host
dependency shim. No service switch, DOS SVC emulation, filesystem behavior, or
DOS runtime code has been added by this diagnostic.

### Ten-object DEM link inventory (2026-08-06)

The ten source-closed DEM objects were next joined to the CCPU/BIOS/BOP
diagnostic chain: `dem`, `demdata`, `demdir`, `demdisp`, `demfcb`, `demfile`,
`demhndl`, `demlabel`, `demlock`, and `demmsg`. The contemporary staging
configuration emits VDD named-pipe helper definitions from more than one
historical compilation unit. This is a historical provider-selection issue;
the original build chooses a single VDD provider. It is not valid to claim a
runtime link from a first attempt that reports duplicate definitions.

For dependency classification only, the linker was run with `/force:multiple`.
Its deliberately non-runnable report is
`obj/ccpu-bios-ntbop-dem10-force-multiple-link-probe.full-link.txt` (SHA-256
`26B357FF20C73374CD0D52A2513D0259D79F6A4DCAE27879597DD1726C79515A`).
Compared with the previous dispatcher-only report, it removes 29 historical
DEM exports, including `demOpen`, `demRead`, `demWrite`, `demCreate`,
`demClose`, directory operations, FCB operations, file times, rename, and
locking. This proves only original-object linkage coverage, not service
execution.

The new unresolved set identifies the next original layers rather than a DOS
replacement opportunity:

* CCPU register accessors (`getAX`, `getDS`, `setAX`, `setDX`, and peers).
* Historical OEM host-file facade (`CreateFileOem`, `FindFirstFileOem`,
  `GetVolumeInformationOem`, path and attribute helpers).
* Historical command pipe handoff (`cmdPipeFileEOF` and
  `cmdPipeFileDataEOF`).
* The remaining original DEM families: DASD, search, drive/media state,
  error handling, IOCTL, date/time, load/exit, and optional WOW/debug paths.

The `/force:multiple` result is evidence of dependency shape only. It cannot
be used as a runtime library, and it does not authorize a new file API, a DOS
file-service implementation, or any BOP/SVC handler.

### DEM register and OEM-facade boundary classification (2026-08-06)

The original DEM files include `base/mvdm/inc/softpc.h`. In the historical
non-monitor branch that header declares generic register imports such as
`getAX`, `getDS`, `setAX`, and `setCF`. The selected CCPU archive instead
exports the corresponding `c_get*` and `c_set*` accessors. Defining `CCPU` at
the DEM compile command does not alter `softpc.h`'s monitor-facing declaration
branch, so it cannot by itself resolve these imports. This is a host/CPU ABI
joining issue, not missing DEM service behavior. Any future bridge must be
one-to-one, have no register shadow state, and preserve CCPU as the sole owner
of flags, registers, and mode semantics.

The unresolved OEM path/file names have an original provider:
`base/mvdm/oemuni/file.c`. A staged unchanged compile reaches its first blockers
at the removed NT `PCURDIR`, `NtCurrentPeb`, PEB lock, TEB
`StaticUnicodeString`, and RTL OEM-string assumptions. Thus it cannot be
linked unchanged on the modern host. A future contained Win32 OEM facade is a
narrow host shim at those historical API names; it must not replace DEM's
DOS-path, DTA/FCB, SFT/JFT, or DOS-error state.

### CCPU register ABI bridge link diagnostic (2026-08-06)

The staging-only source
`shim/ccpu_register_abi_bridge.c` implements SH-10 for the exact accessor set
observed from the ten linked DEM objects. Every function is a direct call to
the matching CCPU archive export and the source has no CPU-state storage,
guest-memory access, BOP/SVC code, or DOS decision. The bridge covers only
`getAL`, `getBL`, `getDL`, `getAX`, `getBP`, `getBX`, `getCX`, `getDI`,
`getDS`, `getDX`, `getES`, `getSI`, `getZF`, plus the corresponding observed
`set*` functions and `setCF`.

The bridge object (SHA-256
`3D9798EB756C07030D9F206A3F466EA367A5F7B5BFE35DE9F99BDFCB00B0EF07`) has
only the matching CCPU `c_get*`/`c_set*` unresolved imports. Adding it to the
ten-object `/force:multiple` dependency inventory produces
`obj/ccpu-bios-ntbop-dem10-ccpuabi-link-probe.full-link.txt` (SHA-256
`E0B1E1CBE152F561FB39D0BC16E082CD3FB056CD8A6747907E475ECFED57B729`).
Compared with the prior report, it removes exactly 22 generic DEM register or
flag symbols and adds no new unresolved symbol.

This is link-level evidence only. It does not establish a runnable session or
the required register round-trip test, because the historical CCPU
initialization/provider chain is not yet restored. The bridge remains a
staging shim candidate; it is neither linked into `ntdos64-run` nor a DOS
runtime implementation.

An attempted 2026-08-06 narrow initialization fixture that selected only
`c_main.c`, `c_reg.c`, `fpu.c`, and `ntthread.c` was deliberately rejected and
removed rather than completed with local stubs. `c_cpu_init` immediately needs
the original descriptor/debug/event/SAS execution dependencies in addition to
its reset and NPX setup. Supplying substitute `load_pseudo_descr`, `MOV_DR`,
event, or SAS functions would make an apparent initialization pass without
restoring the historical CCPU provider chain. Therefore an initialized SH-10
round-trip remains gated on a linkable complete historical CCPU closure; the
existing one-to-one bridge may not be promoted by a partial-CCPU fixture.

### Standalone host-stack unresolved-symbol audit (2026-08-06)

The last pre-standalone diagnostic was re-read after the source roots were
separated into standalone CMake projects. Its input is
`artifacts/build/legacy/referenced/build-opennt-gate-clang-x86-v3/opennt-host-stack-undefined.txt`: 494
object-level unresolved occurrences, or 334 unique decorated symbols. It is
an inventory for selecting original modules and bounded host seams; it is not
a proposal to implement 334 replacements.

The classifications overlap where a BIOS table entry itself calls an ICA or
media provider:

* 60 unique names are the two historical register-ABI spellings, `get*` /
  `set*` and `c_get*` / `c_set*`. The generic side is DEM's historical
  `softpc.h` contract; the `c_` side is the selected CCPU contract. This is
  only the one-to-one SH-10 bridge seam, and it cannot own register state.
* 52 unique names are referenced by the original BIOS dispatch table or its
  original device/BOP routines, including `MS_bop_0` through `MS_bop_F`,
  disk, keyboard, mouse, serial, printer, RTC, EMS, reset, and illegal-BOP
  handlers. These are first an original-source closure problem. Replacing
  individual entries would violate the required `BIOS[] -> MS_bop_0` route.
* 15 unique names belong to the original CCPU/ICA/quick-event side, including
  the `host_ica_*` notification boundary, port definitions, and quick-event
  count accessors. They require historical-provider closure or the separately
  constrained SH-09 host notification seam; none is a DOS service seam.
* 31 unique names are contained filesystem, drive-query, or selected-media
  boundaries (`*Oem`, `nt_floppy_*`, `nt_fdisk_*`, and related helpers). They
  map to SH-04, SH-05, SH-06, SH-11, and SH-12, each of which preserves the
  original DEM caller, data structures, and error path.
* 47 unique names are normal Win32/CRT imports or compiler support, such as
  `CreateFileA`, `ReadFile`, allocator imports, and security-cookie helpers.
  These are compile-island/linker choices, not runtime shim admissions.
* 85 unique names are cross-module DEM definitions. Before a shim is even
  considered, the missing original DEM families must be compiled and linked:
  in particular DASD, search, drive/media state, error, IOCTL, date/time, and
  load/exit paths. The residual list also contains explicitly excluded first
  profile families such as command-pipe, debugger, DPMI, XMS, redirector, and
  WOW.

Consequently the next safe closure order is: recover the exact CCPU generated
ABI; compile/link the original BIOS and remaining selected DEM provider
families; then admit only individually evidenced host seams from the modern
shim ledger. The audit rejects a tempting but invalid shortcut: satisfying an
unresolved `MS_bop_*`, `dem*`, or device-table name with a newly written
handler merely because it makes the linker progress.

### Full DEM object admission attempt (2026-08-06)

The standalone, default-off historical object target was extended to list all
four previously omitted original DEM sources: `demerror.c`, `demgset.c`,
`demmisc.c`, and `demsrch.c`. No source was replaced, no modern shim was
linked, and no executable target consumes this object target.

The first serialized clang-cl build reaches original `demerror.c` and stops
there. The recorded log is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/host-stack-extended.log`. Its first
source-level failures are missing historical NT object-manager and
volume-query declarations: `PUNICODE_STRING`, `NTSTATUS`, `ANSI_STRING`,
`FILE_FS_DEVICE_INFORMATION`, `IO_STATUS_BLOCK`,
`POBJECT_NAME_INFORMATION`, `NtQueryVolumeInformationFile`, and
`NtQueryObject`.

This is direct evidence for the SH-04 seam: the original `demerror.c` owns
the DOS hard-error and device-name decision path, while its native NT
volume/object query dependency is unavailable in the current compiler SDK
surface. It does **not** authorize a replacement hard-error SVC, a drive
policy, or a fake `demGetPhysicalDriveType`.

Each remaining source was then compiled through its generated single-object
rule. `demgset.c` reaches the same unavailable native NT declaration family
(`NTSTATUS`, `OBJECT_ATTRIBUTES`, `IO_STATUS_BLOCK`, `OEM_STRING`,
`UNICODE_STRING`, `FILE_FS_DEVICE_INFORMATION`), independently confirming
that its drive/media query path is SH-04 rather than a new DOS-service
implementation site. Its log is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/host-stack-new-dem-individual.log`.

`demmisc.c` parses through the historical SDK surface until its use of
`ProcessDebugPort`, identifying the exact unavailable process-debug query
assigned to SH-07. `demsrch.c` stops before parsing its bodies because the
historical private header `winbasep.h` is absent, identifying its first
directory/query compatibility input for SH-05. Those two results are recorded
in `artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/host-stack-new-dem-individual-2.log`.
Neither result authorizes a replacement debug SVC or a replacement FindFirst /
FindNext state machine: DEM remains the owner of those service bodies and
their PDB/SFT/DTA state.

The staging preamble was then extended only to expose the modern SDK's
`winternl.h` declarations. OpenNT's `windows.h` leaves `_WIN32_WINNT` below
the header's `0x0500` gate, so the preamble raises that declaration-only macro
and supplies the otherwise absent `__kernel_entry` annotation as an empty
annotation. A generated `demerror.c.i` confirms that `NTSTATUS`,
`PUNICODE_STRING`, and `IO_STATUS_BLOCK` are then visible. This provides no
new implementation, linkage, or guest-visible behavior.

The follow-up object diagnostics are
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/host-stack-ntdecl-v3.log`. They reduce
`demerror.c` and `demgset.c` to the genuinely unavailable old NT declaration
surface: `FILE_FS_DEVICE_INFORMATION`, `FILE_REMOTE_DEVICE`,
`FILE_REMOVABLE_MEDIA`, `OBJECT_NAME_INFORMATION`,
`SYMBOLIC_LINK_QUERY`, and their information-class values. `demmisc.c` now
parses `ProcessDebugPort` but exposes a separate historical-header ABI issue:
the retained `NtCurrentProcess()` spelling expands as an `int` while the
modern declaration expects `HANDLE`. These are host-header/ABI compatibility
facts, not a basis for modifying any DEM SVC body. The `demgset.c` lvalue-cast
diagnostic is likewise an old compiler-language extension issue and remains
outside a host shim.

### Complete historical host-stack object closure (2026-08-06)

The standalone clang-cl/x86 target now builds all 16 original DEM modules,
the original BIOS table, `sim32.c`, original ICA and QEVNT, the recorded
`nt_bop.c` compatibility overlay, and the staging-only register ABI bridge.
The successful command is recorded in
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/host-stack-final-object-closure.log`; the
target ends with `Built target ntdos64-opennt-host-stack-objects`.

Two source-preserving compiler overlays are deliberately narrow:

* `overlay/base/mvdm/inc/mvdm.h` is a byte-identical upstream copy except for
  the `i386` `STOREWORD`/`STOREDWORD` definitions under `__clang__`. It
  replaces only the obsolete MS lvalue-cast syntax with an equivalent 16/32-bit
  storage write.
`local/include/opennt_dem_ntdecl_compat.h` is a declaration-only historical
PCH bridge. Every non-modern layout/value it supplies is traceable to the
fixed OpenNT public SDK (`ntioapi.h`, `ntobapi.h`, `ntstatus.h`, `ntpsapi.h`,
`vdm.h`, or `crt/excpt.h`). The empty local `winbasep.h` confirms that
`demsrch.c` requires no implementation from the absent private header once
those PCH declarations are present.

This result proves only source/object closure. It does not link the objects,
does not provide a filesystem/media/directory shim implementation, does not
execute CCPU or `BIOS[]`, and does not alter the rule that
`CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM` is the required eventual
runtime path.

### Current external-link boundary (2026-08-06)

`llvm-nm` was run only over the 23 current object paths listed by
`DependInfo.cmake`, not every stale `.obj` in the build tree. The raw inventory
is `host-stack-final-undefined.txt`; after subtracting globally defined symbols
from the same object set, the true external inventory is
`host-stack-final-external-undefined.txt`. The result is 400 unique undefined
names, 133 internally resolved names, and 267 external names.

Those 267 names are a classification input, not a list of functions to
reimplement:

* 29 are CCPU ABI accessors or quick-event accessors (`c_get*`, `c_set*`,
  `c_sas_hw_at`, and `c_cpu_q_ev_*`). They are blocked by the separate missing
  generated CCPU ABI and must be supplied by the original CCPU path.
* 36 are original BIOS/device-table entries, including BOP table functions,
  disk, keyboard, mouse, serial, printer, RTC, EMS, reset, and termination
  handlers. They require historical source/provider closure, never one-off
  replacements in a shim.
* 37 are OEM path/file or explicitly selected media boundaries, including
  `*Oem`, `nt_floppy_*`, and `nt_fdisk_*`. These remain candidates for the
  constrained SH-04/05/06/11/12 facades after link admission, while DEM keeps
  its DOS data structures and error decisions.
* 14 are profile-gated CMD, debug, DPMI, XMS, module, or WOW entries. Their
  presence does not make them first-profile requirements.
* 63 are ordinary compiler, CRT, Win32, or NTDLL imports. These are toolchain
  and import-library choices, not guest or DOS semantics.

The remaining names are original monitor/ICA notification and support
providers. This static result deliberately does not choose their replacement;
it prevents an invalid shortcut in which a link error is treated as permission
to add a new BOP, SVC, or device implementation.

### Independent CCPU Host-Callback Gates (2026-08-06)

The former combined callback gate is split into three default-disabled object
targets so that unrelated historical build assumptions cannot conceal one
another: `nt_cpu.c`, `nt_eoi.c`, and `nt_timer.c`. None is linked by the
historical-provider archive or a runner.

`nt_cpu.c` remains blocked at its generated VIDC/monitor ABI
(`GDP_PTR` and `Constraint*` declarations). Supplying local stand-ins would
recreate the excluded monitor context, so it is not an admissible CCPU-host
source in the software-only profile.

`nt_eoi.c` first reaches OpenNT's native header ordering: its direct
`ntexapi.h` include needs the fixed old NT native type universe. Preincluding
the existing declaration-only `winternl.h` bridge was tested and rejected: it
causes incompatible duplicate native enums and structures. The file therefore
needs a separate historical-native-header compilation arrangement, not a
modern-header overlay and not an EOI/IRQ shim.

`nt_timer.c` reaches `#include "conapi.h"`. That private console-server header
does not exist in the fixed OpenNT input tree. The same translation unit also
includes thread, console/UI, full-screen, PIF, graphics, event, and reset
interfaces. It cannot be treated as a portable monotonic-clock provider or
solved by inventing a local `conapi.h`; a future per-session clock must be
admitted through its own narrow contract and fixture.

The source review also corrects the initialized-fixture interpretation:
`c_cpu_init` calls the CCPU thread/reset setup only. It does **not** call
`sas_init`, so a successful CCPU reset cannot prove SAS allocation. Original
`sas_init` is the first caller of `host_sas_init`; it then installs the CCPU
SAS vector, establishes RAM, and calls original `rom_init`/`copyROM`. A SAS
lifecycle fixture must therefore be deferred until the original ROM-provider
path is present, rather than introducing a synthetic IVT/BDA or a partial
memory-startup shortcut.

An x86 clang-cl link-granularity experiment also tried both `/Gy` and LLVM
`-ffunction-sections` only on the 129-unit software-FPU fixture profile.
`llvm-readobj` still reports no `.text$<function>` COMDAT sections in
`c_main.c.obj`; rebuilding the initialization fixture therefore continues to
retain the full interpreter object's unresolved callback set. The experiment
is rejected and the extra compiler option is not retained. This is evidence
that an initialized CCPU fixture must close the original provider chain; it
cannot be made valid by dead-stripping or by supplying broad success stubs.

### CCPU Reset-Profile Link Inventory (2026-08-06)

The reset fixture was separated from the full PC/AT provider archive. That
archive eagerly pulled mouse, video, and virtual-device objects and therefore
could not answer what plain CCPU itself needs. The fixture now links only the
complete historical CCPU archive and the default-disabled SH-15 foundation.
The foundation supplies private zeroed `host_sas_*` storage and a zeroed
`BIOS[256]` data table solely to satisfy a non-executing reference; it supplies
no BOP handler, IVT/BDA, device response, or DOS state. Each unadmitted host
callback records its own name and aborts rather than returning success.

With the linker error limit temporarily lifted for one diagnostic build, the
remaining CCPU-archive imports are exactly: `dispatch_q_event`, `ica_intack`,
`ica_hw_interrupt`, the six `Ios_*` adapter symbols, `rom_init`, `copyROM`,
`read_pointers`, `LIM_b_write`, `LIM_w_write`, `host_simulate`,
`c_sas_touch`, `c_VirtualiseInstruction`, and CRT `sprintf`. The temporary
link option was removed after capture.

This separates ownership rather than closing it artificially: QEVNT, ICA, and
I/O adapters must come from their original source providers; ROM/SAS symbols
need the original generated-vector and ROM-provider inputs; `sprintf` is a
historical CRT/toolchain selection issue. The fixed CCPU source tree contains
declarations but no definition for `c_sas_touch` or
`c_VirtualiseInstruction`, so those are missing generated-input facts, not
candidate shim functions. The reset executable remains intentionally
unlinked until those original-owner dependencies are resolved.

The separate NTVDMx64 input was checked for the two missing CCPU generated
symbols. Its HAXM `i386/sas.c` implements a different memory-mapping route and
does not define `c_sas_touch` or `c_VirtualiseInstruction`; it also depends on
HAXM VM/VCPU ioctls and x64 allocation behavior. It is therefore source
evidence for an incompatible execution model, not a provider for the OpenNT
software-CCPU profile.

### Minimal Original CCPU Provider Extraction (2026-08-06)

The original `ios.c`, `ica.c`, `qevnt.c`, and `rom.c` were moved only at the
CMake object/archive membership level into the default-disabled
`ntdos64-opennt-ccpu-platform-provider-archive`. Their source text and their
historical ownership are unchanged. The full historical-provider archive now
consumes the same object group, so this extraction does not remove any source
from the eventual full chain.

Linking the reset fixture with this archive source-closes the original I/O,
ICA, QEVNT, and ROM exports and reduces the remaining unresolved set to ten:
`sprintf`, `read_pointers`, `LIM_b_write`, `LIM_w_write`, `host_simulate`,
`c_sas_touch`, `c_VirtualiseInstruction`, `config_inquire`,
`host_read_resource`, and `c_effective_addr` (the linker's output groups the
two ROM configuration calls as the final dependency family). The provider
objects compile successfully; the fixture is still intentionally unlinked.

The ownership split is now actionable: `sprintf` is an x86 CRT import choice;
`read_pointers` belongs to the original video-memory provider; `LIM_*` belongs
to original EMS; `host_simulate` is an excluded monitor/host simulation path;
the `c_sas_*` names are missing generated inputs; and the ROM configuration,
resource-read, and effective-address names require original configuration and
guest-address providers. No item is assigned to DEM or to a synthetic BIOS.

### Remaining CCPU Imports: Source-Level Classification (2026-08-06)

The ten imports were then traced in the fixed OpenNT input rather than treated
as one generic ``host shim`` bucket. This gives the next closure work an
explicit order and rules out a number of tempting but invalid stubs.

| Import family | Fixed-source evidence | Required owner | Closure rule |
| --- | --- | --- | --- |
| `sprintf` | CCPU diagnostic formatting, including `ccpu386/c_main.c`. | x86 historical CRT selection. | Resolve only through the selected CRT/library compatibility layer; it is not a guest or DOS API. |
| `read_pointers` | `ccpu386/ccpusas4.c` passes its byte/word callbacks to physical reads; storage is defined by `base/video/ega_read.c`. | Original video-memory provider. | Do not define a zero table. The provider must establish valid read callbacks before any SAS/guest execution. |
| `LIM_b_write`, `LIM_w_write` | `ccpu386/ccpusas4.c` dispatches writes to EMS-addressed ranges; declarations are in `base/inc/emm.h`. | Original EMS/LIM provider. | Do not replace with normal RAM writes. Either link the original EMS configuration/provider chain or prove EMS is disabled before a bounded fixture can execute. |
| `host_simulate` | `ccpu386/ccpusas4.c:biosDoInst` saves CCPU state, selects an instruction in `bios1.rom`, and calls it. `host/src/nt_cpu.c` calls `cpu_simulate`; the CCPU setup in `host/src/nt_cprgs.c` assigns that path to `c_cpu_simulate`. | A constrained CCPU-only recursive execution bridge. | This is not a V86 requirement. A later bridge may enter `c_cpu_simulate` under an explicit recursion/termination contract, but must not introduce monitor, VDD, HAXM, or host-CPU execution. |
| `c_sas_touch`, `c_VirtualiseInstruction` | Referenced by the original `ccpu386/sascdef.c` SAS vector and declared in generated `sas4gen.h`; no definition occurs in the fixed source tree or in the separately sourced NTVDMx64 tree. | Missing generated CCPU source input. | Record as a provenance/input gap. Do not hand-write semantic substitutes. |
| `config_inquire` | `host/src/config.c` is the historical implementation; `base/system/rom.c` queries graphics/configuration choices during ROM setup. | Original profile/configuration provider. | It carries PC/AT profile policy and cannot be reduced to a generic shim. Reintroduce only with a contained, explicit profile input. |
| `host_read_resource` | `host/src/nt_rez.c` reads named ROM/CMOS resources after `host_find_file`; `base/system/rom.c` uses it for ROM bytes. | Original resource provider, adapted to a contained BYOB resource manifest. | Do not inherit ambient path searching. A future provider must enumerate approved read-only resource inputs and preserve the original call/result boundary. |
| `c_effective_addr` | Declared by generated `ccpu386/cpu4gen.h`; it is absent from the original CCPU `sources` list and from the fixed tree definitions. A later source-derived fixture establishes the real/V86 and selector-base contract in isolation. | Monitor selector/descriptor provider plus the bounded local contract completion. | The completion is admissible only through the exact fixture contract; it must not enter the full link until the original-style selector/descriptor providers are separately evidenced. |

`host_simulate` is deliberately distinguished from the user-rejected V86 path:
the fixed CCPU source itself uses it to re-enter the *software* interpreter for
the ROM instruction selected by `biosDoInst`. It is still deferred because a
valid recursive-entry contract needs its own bounded fixture. In particular,
the bridge must preserve the original saved CCPU state and must not turn a ROM
instruction into a direct BIOS, BOP, or DEM call.

The remaining imports therefore establish a closure order: first obtain the
two semantically unresolved generated CCPU inputs (`c_sas_touch` and
`c_VirtualiseInstruction`) and restore the selector/descriptor context needed
to admit the separately proven `c_effective_addr` contract; then construct
original profile/resource, video, and EMS providers with explicit contained
inputs; then prove a bounded CCPU recursive-ROM fixture. Only after that can
`sas_init` legitimately reach original `rom_init`/`copyROM`. The current
fixture remains a non-executing link diagnostic and cannot be presented as
BIOS or DOS startup evidence.

### Historical Runtime Closure Gates (2026-08-06)

The existing object-level chain already establishes the *dispatch order*.
Runtime admission must close it in this order instead of making the CCPU reset
fixture progressively more permissive:

1. **CCPU generated-input and address-context gate:** obtain and manifest
   original providers for `c_sas_touch` and `c_VirtualiseInstruction`, then
   restore the monitor-style selector/descriptor context before admitting the
   separately fixture-proven `c_effective_addr` completion. The two opaque
   vector slots have no admissible local semantic implementation.
2. **Contained machine-resource gate:** adapt the original
   `config_inquire`/`host_read_resource` boundary to an explicit profile and
   read-only approved resource manifest. Ambient working-directory search,
   registry configuration, drive probing, and writes are excluded.
3. **Original device-provider gate:** bring up the original video read-table
   and EMS/LIM provider chains under that profile. They own `read_pointers` and
   `LIM_*`; no RAM alias or success-returning substitute is allowed.
4. **Software recursive-ROM gate:** prove one bounded `biosDoInst`-style
   recursion through `host_simulate -> c_cpu_simulate`, including an original
   CCPU-controlled return. This is specifically not V86, `NtVdmControl`,
   HAXM, a direct BIOS call, or a direct `MS_bop_0` call.
5. **Historical dispatch gate:** only after gates 1--4 may an execution
   fixture enter the already object-proven path
   `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM`. The fixture must then
   retain the original BOP service-byte fetch and DEM dispatcher; it may not
   substitute a service handler to demonstrate progress.

Each gate needs a separate default-disabled target, input manifest evidence,
and bounded test. Failure at any gate is a research result, not authorization
to widen the modern host shim or to attach the trace instrumentation to the
runtime path.
