# Modern Host Shim Dependency Ledger

Status: design/admission ledger, 2026-08-06.

This ledger implements the admission rule in
`HISTORICAL-RUNTIME-RESTORATION-DECISION.md`. A shim implementation is not
admitted until its row has a concrete source file, verification record, and
bounded test. The ledger does not authorize a replacement DOS runtime.

## Rule

The historical chain remains:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

A shim can supply only an unavailable host dependency below a historical
module. It must not decide an SVC number, consume a DOS service byte, manage
DEM's PDB/SFT/DTA/FCB state, or introduce a PC/AT device policy.

An observed BOP is not a shim acceptance criterion. A row is admitted only
when its stated historical dependency is unavailable on the modern host and
the bounded test proves that the original caller and callee still own the
historical data flow.

## Historical Process Shell Exclusion

`host/src/nt_reset.c` is a historical NTVDM process shell, not a neutral
machine provider and not an SH-row candidate.  `host_applInit()` rejects a
direct launch unless it sees `-f`, then installs the old console/UI, event
thread, ICA lock, PIF, stream-I/O and WOW policy.  `host_reset()` continues
that product policy by initializing the console/mouse surface and releasing
the heartbeat/ICA ordering; `host_terminate()` exits through the old VDM
process mechanism.

The research CLI's direct invocation is therefore an outer runner assembly
decision.  It may establish a new per-session argument/resource/console
environment and invoke the historical machine chain at its documented entry
points, but it must not call `host_applInit()` merely to bypass its `-f`
guard, manufacture a legacy command line, or use `nt_reset.c` as a source of
DOS, BOP, DEM, PIC, BIOS, video-ROM, or media semantics.  Any eventual runner
must separately record its initialization order and prove that it reaches
original `CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM` dispatch without
a handwritten BOP/SVC substitute.  This explicit outer-product exception does
not broaden the modern host shim surface.

## Historical NTIO Bootstrap Facts

`host/src/nt_msscs.c` contains the fixed NT4 DOS-binary bootstrap, and its
order is an input to any future BYOB assembly rather than a generic BIOS boot
contract:

1. It preserves the virtual-interrupt word at
   `FIXED_NTVDMSTATE_LINEAR`, calls `io_init()` and the original `reset()`.
2. It initializes the original VDD/LIM and `scs_init()` chain, whose order is
   `CMDInit`, `DemInit`, `XMSInit`, then `DBGInit`.
3. It reads an unmodified `ntio.sys` byte image at
   `NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET`, restores the saved interrupt word,
   and sets CS:IP to that same entry point.

This source is a strong NT4-specific reference, but it is not admissible
unchanged for the non-invasive direct CLI.  Its file lookup hard-codes
`GetSystemDirectory() + "\\ntio.sys"`; its first-command helper
`AddSystemFiles()` probes and may create placeholder `C:\\IO.SYS` and
`C:\\MSDOS.SYS` on the host root.  Neither host-system discovery nor host-root
writes may be enabled by default, hidden behind a capability, or treated as a
DOS/BIOS requirement.

The future BYOB runner must instead use an explicit user-selected resource
manifest for the NT4 file group, provide a read-only NTIO byte stream, and
record the resulting load address/length/hash without retaining guest bytes
in project outputs.  It must preserve the original init and entry ordering
above, but it must make the placeholder-system-file behavior unavailable (or
return the original failure at an explicitly contained guest-drive boundary).
This is a loader/resource-policy adapter outside CCPU, BIOS, BOP, DEM, and
the modern-host SH rows.  It cannot synthesize NTIO, substitute NTDOS, or
change DOS service behavior.

## Initial Admission Rows

| ID | Historical caller and dependency | Permitted shim responsibility | Must remain historical | Verification evidence |
| --- | --- | --- | --- | --- |
| SH-01 | V86 `monitor.c`: `NtVdmControl(VdmStartExecution)` plus `VDMEVENTINFO` production | Conditional private V86 execution slice that reports BOP/fault/stop and exact consumed-instruction size. Not used by the direct CCPU profile. | V86 monitor event dispatch, `BIOS[]` selection, and the BOP handler invocation. | Trace proving exactly one `BIOS[bop]` call for a reported V86-monitor BOP; no BOP/SVC switch in the shim. |
| SH-02 | `int.c`, `nt_eoi.c`: `VdmQueueInterrupt` and `VdmDelayInterrupt` | Private pending/deferred IRQ queue and scheduling boundary. | PIC/ICA handler logic, IRET-hook state, and all guest-visible interrupt semantics. | Repeated bounded IRQ/order test attached to the historical handler path. |
| SH-03 | `monitor.c`: native current-thread/TEB/context and obsolete selector/FPU context setup | Private session context and supported x86/CCPU FPU initialization policy. | Guest register state, mode transitions, and monitor event dispatch order. | Context save/restore trace across a bounded BOP/fault stop. |
| SH-04 | `demerror.c`, `demgset.c`: native volume/object/device queries, including `GetPhysicalDriveType`'s `NtOpenFile`/`NtQueryVolumeInformationFile` chain | Contained-drive filesystem/media query facade with a documented Win32 result/error mapping. It enumerates only configured guest drives and never opens host roots or physical devices implicitly. | DEM error classes, drive state, and media/DPB decisions. | Query success and failure cases reach the original DEM error/drive paths, with no host-path escape. |
| SH-05 | `demsrch.c`: native directory-handle open/query/reset/close and status/path conversion | Contained directory enumeration cursor under the existing `FileFindOpen`/`FileFindNext`/`FileFindReset` boundary. | DEM DTA, FCB, PSP find-list, wildcard, and find-id state. | Original FindFirst/FindNext updates the historical DTA/FCB state across a bounded directory fixture. |
| SH-06 | `demfile.c` and related DEM code: host file handles, OEM paths and sharing/locking | Explicit contained Win32 file/stream facade and DOS-visible error translation. | DEM SFT/JFT/PDB ownership and VDD handle bookkeeping. | Open/read/write/close fixture through original DEM functions, including at least one error result. |
| SH-07 | `demmisc.c`: `DbgPrompt` and `ProcessDebugPort` inquiry | Optional private debug-console/diagnostic provider, or explicit unavailable result. | All DEM dispatch/process data flow. | Default CLI profile shows no prompt side effect; enabled diagnostic profile records a redacted event. |
| SH-08 | `nt_bop.c`: idle/wake/terminate helpers | Session run-state, cancellation, and cooperative host wait semantics. | `MS_bop_0` service-byte fetch, `DemDispatch` call, and IP increment. | Bounded idle/terminate trace with unchanged BOP dispatch ordering. |

| SH-09 | CCPU-profile `ica.c`/`qevnt.c`: host synchronization, clock and interrupt notification (`host_ica_lock`, `host_ica_unlock`, `host_set_hw_int`, `host_clear_hw_int`, `host_EOI_hook`, `host_TimeStamp`, `host_get_q_calib_val`) | Only an in-process synchronization/notification boundary after the identified original CCPU provider (`nt_cpu.c`) and its legacy build dependencies have been classified. | ICA priority, masking, acknowledgement, guest-visible IRQ state, and quick-event queue ownership. | Source/link evidence that original ICA and QEVNT own all IRQ/time ordering decisions; ordered hardware-IRQ and timer-event fixtures through those providers. |
| SH-10 | Original DEM modules consume the monitor-style `softpc.h` register ABI (`getAX`, `getDS`, `setAX`, `setCF`, and peers), while the selected CCPU archive exports `c_get*`/`c_set*`. | A one-to-one CCPU ABI bridge only after every forwarded symbol and width is recorded. It may call the matching CCPU accessor and contain no independent register storage. | CCPU register state, flags, mode handling, and every DEM service body. | Symbol map proves each generic accessor resolves to exactly one `c_*` implementation; bounded register round-trip fixture through an original DEM caller. |
| SH-11 | `base/mvdm/oemuni/file.c`: historical OEM file/path facade uses obsolete TEB/PEB static-string fields and RTL helpers. | Contained modern Win32 OEM/path facade at the original `CreateFileOem`/find/attribute/volume API boundary, with explicit error mapping. | DEM path parsing, FCB/SFT/DTA state, DOS error policy, and all service dispatch. | Each shim entry names its matching original facade entry and demonstrates success/failure through an original DEM caller without path escape. |
| SH-12 | `demdasd.c` imports original `nt_floppy_*`/`nt_fdisk_*` providers; `demioctl.c` has an existing direct `CreateFileA`/`DeviceIoControl` device-parameter fallback. | A contained-media policy may bind only the original provider/call boundaries to user-selected media and must reject physical-drive or path-escape access. | DEM DASD/IOCTL state, BPB/DPB updates, media-change rules, INT 13h semantics, and DOS error conversion. | Original DEM read/write/format/device-parameter paths are exercised against an explicitly selected fixture and an escape attempt reaches the original failure path. |
| SH-13 | `demgset.c`: `GetLocalTime` and `SetLocalTime` in DEM date/time SVC bodies. | A per-session virtual clock provider at the original Win32 call boundary. It may return and accept a guest clock value but must never change the host system clock. | DEM register/return decision logic for query and set date/time. | Original DEM query/set services observe a session-local clock; an audit proves no host time-setting API is reached. |
| SH-14 | Original `base/comms` controller library: `com.c`, `rs232_io.c`, `printer.c`, and `printer_.c` import `host_com_*`, `host_lpt_*`, and printer callbacks. | Optional explicitly configured serial/parallel stream provider. An absent port must be reported through the original callback contract; a configured port may bind only a user-selected host stream. | Original UART/LPT controller state, BIOS BOP handlers, port I/O, status timing, and IRQ choices. | Original `rs232_io`/`printer_io` path observes configured and absent-port outcomes, while an audit proves no implicit host COM/LPT/printer discovery/open. |
| SH-15 | Initialized CCPU/provider link closure: private SAS allocation, extended-BOP forwarding, non-V86 hook fall-through, bounded diagnostics. | Per-session CCPU host foundation, following `SH-15-CCPU-HOST-FOUNDATION-CONTRACT.md`. | CCPU opcode/IP decoding, original `BIOS[]`, `MS_bop_0`, DEM dispatch/service logic, PC/AT initialization, and IRQ policy. | Original `c_cpu_init` lifecycle plus a harmless original BIOS selector fixture; bridge proves one dispatch with no state patch and imports no VDM/physical-device APIs. |

### SH-11 OEM Facade Contract Facts

The original `base/mvdm/oemuni` library consists of `file.c` and `process.c`.
Together they expose 29 OEM entry points, not merely `CreateFileOem`.
`file.c` owns the file/directory/volume-facing 19-entry subset:

```text
CreateFileOem             SetFileAttributesOem      GetFileAttributesOem
DeleteFileOem             MoveFileOem               MoveFileExOem
FindFirstFileOem          FindNextFileOem           GetFullPathNameOem
GetCurrentDirectoryOem    SetCurrentDirectoryOem    CreateDirectoryOem
RemoveDirectoryOem        GetDriveTypeOem           GetDiskFreeSpaceOem
GetVolumeInformationOem   OutputDebugStringOem      GetComputerNameOem
RemoveFontResourceOem
```

`process.c` additionally supplies the system/windows/temp/search, process,
environment, and short-path helpers declared in `base/mvdm/inc/oemuni.h`.
Existing DEM directly calls the first group and `SetEnvironmentVariableOem`.
In particular, a partial
`CreateFileOem` replacement would leave original `FindFirst/Next`, attributes,
volume queries, and current-directory behavior still dependent on unavailable
TEB/PEB static strings and RTL conversion helpers. It is therefore not an
admitted shortcut.

A future SH-11 facade must preserve each original function name, calling
convention, return sentinel, and Win32 `GetLastError` result. It can implement
OEM conversion with documented modern Win32 conversion APIs, but it may only
receive paths after the separately selected contained-drive policy has mapped
them. It must not turn `GetFullPathNameOem` or `SetCurrentDirectoryOem` into a
host-global current-directory policy, and it must not let a DOS path escape a
configured guest root. `OutputDebugStringOem`, computer-name and font-resource
entries are optional-profile members: first CLI admission may return the
historically appropriate unavailable result rather than adding a host product
integration.

The source boundary is intentionally narrower than a complete DOS filesystem.
`demdir.c` and portions of `demfile.c` call the published OEM facade, whereas
`demsrch.c` keeps its own native directory handle and directly uses
`RtlDosPathNameToNtPathName_U`, `NtOpenFile`, and `NtQueryDirectoryFile` via
the original `FileFindOpen`/`FileFindNext`/`FileFindReset` path. Thus SH-11
does not admit directory enumeration. SH-05 must adapt only that original
`FileFind*` seam to a contained cursor while DEM retains DTA, FCB, PSP
find-list, wildcard, and DOS error decisions. Likewise an OEM open facade
does not prove the later DEM file-handle/SFT paths.

The concrete SH-05 boundary, prohibition on `NtVdmControl(VdmQueryDir)`,
record-layout contract, and required admission fixtures are recorded in
`SH-05-CONTAINED-DIRECTORY-SEARCH-CONTRACT.md`.

Admission requires one complete, default-disabled facade target and bounded
fixtures through original DEM callers: file open/read/write/close, directory
find continuation, attributes, and one volume-query failure. A standalone
conversion-only test is useful but insufficient, because it would not prove
that DEM retains ownership of DTA/FCB/SFT state and DOS error selection.

The first bounded original-caller proof is directory-only: on 2026-08-06,
the default-disabled `ntdos64-opennt-contained-oemuni-demdir-fixture` linked
unmodified `demdir.c` to the facade with passive test-local register and
guest-memory observation. It proved the original DEM create/set/delete
directory calls stay at the published `*Oem` boundary and route a rejected
escape path to `demClientError`. The fixture is not a CPU adapter and does not
dispatch BOP/SVC. It leaves the required file/search, DTA, FCB, SFT, and
volume-error fixtures outstanding.

### SH-12 Raw-Image Contract Facts

`demdasd.c` passes byte offsets and byte counts to the `nt_fdisk_*` and
`nt_floppy_*` boundary, then converts the returned completed-byte count back
to sectors. A read/write provider must therefore return the exact count
transferred, return zero on failure, and leave the documented Win32 last-error
available for DEM's existing DOS-error conversion. It must never report a full
request when a host read/write was short.

For a non-removable drive, `demFdiskInit` accepts a provider only when
`nt_fdisk_init` returns both a FAT BPB and a `DISK_GEOMETRY`; it uses those
values to establish the historical BDS/DPB state. A raw-image provider must
derive them from the selected image and explicit profile geometry, not inspect
or enumerate a host volume. The provider rejects an unconfigured drive, an
overflowing/out-of-image range, a malformed BPB, and writes when its selected
image is read-only.

The historical `nt_fdisk.c` and `nt_rflop.c` are not reusable host backends:
they construct `\\DosDevices\\X:` names, issue native disk/volume controls,
and lock physical media. The first SH-12 implementation may open only a
user-selected regular image file through documented Win32 file APIs. It must
not open drive letters, `\\.\\` device paths, physical disks, or paths derived
from a DOS request.

### SH-12 Contained Facade Evidence

`src/opennt/local/archived/legacy-adapters/contained_media_fdisk.c` is the first deliberately
small implementation at the original `nt_fdisk_*` boundary. It is an
OpenNT-island, default-disabled source input, not a DEM replacement and not a
runtime admission decision. A caller explicitly configures one regular image
before `nt_fdisk_init`; initialization derives the BPB and `DISK_GEOMETRY`
from its boot sector and length. Read and write calls accept only the physical
slot established by that initialization and return an exact completed byte
count or zero. Its wrapper-only `ntdos64_media_reset` closes configured image
handles and clears the session mapping; DEM never calls that helper.

The facade rejects a null/empty path, directories and reparse points, the
`\\.\\` device namespace, malformed BPBs, unconfigured drives, overflow or
out-of-image requests, and writes to a read-only selection. It neither
enumerates drives nor constructs a drive-letter/device path. Its current scope
does not provide removable-media behavior, host-volume locking, format, or
filesystem namespace policy; those remain unadmitted historical dependencies.

On 2026-08-06, the default-disabled
`ntdos64-opennt-contained-media-fdisk-test` created and deleted a 4 KiB
FAT-like temporary image. It proved configure -> `nt_fdisk_init` -> exact
512-byte write -> exact 512-byte read, rejected an out-of-image read, and
rejected `\\.\\PhysicalDrive0` before any open. This is only a bounded facade
test: it did not link DEM, start CCPU, or run a DOS guest.

SH-01's detailed event ownership, result limits, prohibited behavior, and
admission tests are in `SH-01-EXECUTION-EVENT-BRIDGE-CONTRACT.md`. The same
document records the direct CCPU `bop(n) -> BIOS[n]` path, for which SH-01 is
not inserted. Neither path authorizes instruction-specific public hooks or a
replacement BOP/DEM dispatcher.

## Configuration Exclusions

The first CLI reference configuration is non-`X86GFX`. Thus host ROM mapping
and `nt_sec.c`'s `NtVdmControl(VdmInitialize)` path are excluded by the
historical build configuration, not emulated by a shim. WOW, redirector,
ISV-module, printer, and debugger BOP paths are also out of the first smoke
profile until separately admitted.

The current executable profile is additionally CCPU-only. The V86 monitor,
`NtVdmControl(VdmStartExecution)`, `VDMEVENTINFO` transport, and SH-01 are
historical-reference material only and are not implementation work. The direct
CCPU `bop(n) -> BIOS[n]` route remains the required execution path.

`nt_cpu.c` and `qevnt.c` are historical CCPU-path sources, not V86 monitor
sources. The former supplies a minimal `host_set_hw_int -> cpu_interrupt`
notification but currently fails whole-file closure on obsolete VIDC/NT thread
layout assumptions. The latter owns the historical quick-event queue but
currently fails on obsolete NT list and timing/lock host dependencies. Their
modernization remains constrained by SH-09: it may reproduce only the named
host call contracts while preserving the original ICA and quick-event data
flow. It may not define new interrupt, timer, BOP, or DOS-service behavior.

The independent `nt_eoi.c` and `nt_timer.c` gates refine that classification.
`nt_eoi.c` requires a coherent fixed historical native-header universe; mixing
OpenNT `ntexapi.h` with modern `winternl.h` is a rejected declaration conflict,
not a permission to emulate delayed IRQ behavior. `nt_timer.c` requires the
absent private `conapi.h` and a wider historical console/thread/UI stack. It
is not a source candidate for a generic clock. Any future modern clock or
interrupt-delay surface must be per-session, separately contracted, and must
not claim the old console-server behavior.

The ten-object DEM link inventory further distinguishes two non-SVC seams.
`softpc.h` supplies monitor-style generic register imports to DEM, while CCPU
exports `c_get*` and `c_set*`; SH-10 may only bridge those exact accessors.
The historical OEM facade source exists but depends on removed TEB/PEB layout
and RTL APIs; SH-11 may modernize that facade only, without taking ownership of
DOS file or search state.

SH-10 has staging link evidence for the 22 generic accessor names demanded by
the ten-object DEM inventory: the bridge imports only corresponding CCPU
`c_get*`/`c_set*` exports and removes exactly those generic unresolved symbols.
Its required initialized-CCPU round-trip fixture remains pending; this evidence
does not admit it to a runtime target.

The remaining `demgset.c` source also records two distinct host seams before
any compatibility implementation is considered. `GetPhysicalDriveType` uses
OEM-to-Unicode conversion, NT object-path conversion, `NtOpenFile`, and volume
device-information queries to classify every drive letter. That behavior must
be constrained by SH-04 to the configured guest-drive set, not translated into
unrestricted host drive discovery. Separately, `demSetDate` and `demSetTime`
call `SetLocalTime`; SH-13 requires a per-session clock at that original call
boundary. No research runtime may invoke the host system time setter.

## Current Direct-Historical Inventory

The compile inventory contains all 16 DEM modules at object-level closure,
plus direct original `sim32.c`, `nt_bop.c` (with recorded ABI overlay), BIOS
table objects, ICA, and QEVNT. The `host-stack-final-object-closure.log` record
is the successful standalone clang-cl/x86 object build. The declaration bridge
is limited to retained OpenNT NT I/O/object/status/VDM layouts and has no
implementation. Object closure is not runtime proof: it neither supplies host
file/media/directory operations nor restores the CCPU generated ABI, and it
does not produce a link action or executable.

On 2026-08-06, the default-disabled
`ntdos64-opennt-historical-provider-archive` was also successfully built by
the standalone x86 clang-cl gate. It aggregates the independently selected
original host-stack, CCPU platform, BIOS, system, support, keymouse, disk,
comms, and video object groups into a 2,235,302-byte COFF archive. This is a
reproducible compilation and archive-selection result only: it deliberately
does not include CCPU itself, the unresolved generated-vector semantics, a
modern-host shim, guest media, or an executable entry point. It therefore
proves that these historical provider families can coexist as build inputs,
not that their initialization order or PC/AT behavior is ready to run.
