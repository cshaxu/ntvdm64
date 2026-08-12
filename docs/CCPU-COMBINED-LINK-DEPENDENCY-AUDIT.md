# CCPU Combined Link Dependency Audit

Status: static object-symbol audit, 2026-08-06. No linker or executable was
run for this record.

## Input Set

The audit reads the actual object paths listed by the current CMake
`DependInfo.cmake` files, rather than recursively globbing the build directory.
That avoids stale objects from earlier source configurations.

```text
130 CCPU objects
 23 historical host-stack objects
153 total objects
```

`llvm-nm` finds 1,769 distinct definitions and 888 distinct undefined
references. After removing names defined by another member of the same 153
object set, 273 external unresolved names remain. The reproducible object-path,
defined, undefined, and external-undefined lists are retained under
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/historical-combined-*.txt`.

This result proves that the real CCPU objects naturally resolve their own
instruction and register implementation names. It does **not** prove a
successful COFF link, executable initialization, or guest execution.

## Required Chain Evidence

The combined set contains the historical layers needed to inspect this chain:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM
```

The corresponding object symbols `_c_cpu_simulate`, `_BIOS`, `_MS_bop_0`, and
`_DemDispatch` are each defined by this set and absent from its external
unresolved list. `bop(n)` is a historical macro rather than a link symbol.
This is symbol-level chain connectivity only: the call path still needs its
original initialization, device providers, and admitted host facilities before
it can execute.

The remaining external set must be satisfied only by original providers or by
an individually admitted host capability below the relevant original caller.
It is not a list of DOS SVCs to replace.

## CCPU Extension Exclusions

Three combined unresolved names deserve special treatment:

| Symbol | Evidence | Admission decision |
| --- | --- | --- |
| `c_sas_touch` | Introduced by the NTVdmx64 SAS patch; its `fmstubs.c` implementation executes `int 3` and returns `NULL`. | No runtime implementation admitted. A future profile must find an original provider or stop as unsupported before use. |
| `c_VirtualiseInstruction` | Also introduced by that patch; the NTVdmx64 stub executes `int 3` then returns zero. | No runtime implementation admitted. It cannot be treated as a generic instruction interception facility. |
| `EDL_fast_bop` | NTVdmx64's stub executes `int 3`; OpenNT CCPU's `C4 C4` branch uses it only for its extended fast-BOP condition. | Explicitly excluded from the CCPU reference profile. Encountering it is a bounded unsupported stop, never a BOP/SVC replacement hook. |

The approved x86 SAS header overlay admits the declarations and vector layout
for object analysis; it does not admit any of these implementations.

### CVIDC Recovery Pass

The historical CCPU link profile selects both `ccpu386.lib` and `cvidc.lib`.
The original 145-file `base/cvidc` source tree was consequently admitted under
its original relative path and compiled as a separate default-disabled,
39-unit object gate. A symbol inspection found no definition of
`c_sas_touch`, `c_VirtualiseInstruction`, or `c_effective_addr`.

The initialization-only CCPU fixture was then linked again with the resulting
original CVIDC archive. Its unresolved list remained exactly the same ten
names listed above; in particular, all three CCPU completion names remained
unresolved. This proves CVIDC source closure but rules it out as the missing
CCPU provider. The full provenance, NTVdmx64 stub distinction, and reproducible
link result are recorded in `CCPU-COMPLETION-RECOVERY-AUDIT.md`.

## Remaining Dependency Families

The 273-symbol set includes these families:

- **CRT/Win32/NT native imports:** ordinary runtime support plus historical
  time, file, registry, native object and volume queries. These require the
  isolated toolchain's import libraries or an admitted contained host facade.
- **Historical BIOS/device providers:** BOP table entries, disk, keyboard,
  mouse, video, serial, printer, RTC, EMS, reset, and interrupt routines.
  Their normal providers must be compiled from the original source graph;
  individual handlers must not be replaced.
- **Historical OEM/filesystem/media facade:** `CreateFileOem`, find/path,
  drive and volume functions, and `nt_floppy_*`/`nt_fdisk_*`. These map to
  SH-04, SH-05, SH-06, SH-11, and SH-12 in
  `MODERN-HOST-SHIM-DEPENDENCY-LEDGER.md`; no host-root or physical-drive
  access is admitted.
- **Optional profile families:** command pipe, debugger, DPMI, XMS,
  redirector, WOW, printer, and extended device support. They remain outside
  the first bounded CLI profile unless a source-proven caller reaches them and
  their own admission records exist.
- **CCPU/ICA host notification:** `host_ica_*`, timestamp, simulation, and
  hardware-interrupt notification calls. The original ICA policy remains in
  the historical source; any modern provider is constrained by SH-09.

## Provider-Layer Extension

The same CMake `DependInfo.cmake` method was rerun after adding the independent
historical BIOS and system provider object gates. `llvm-nm --format=posix`
reads all COFF symbols from the following fixed build objects:

```text
130 CCPU objects
 23 historical host-stack objects
 13 historical BIOS-provider objects
  9 historical system-provider objects
175 total objects
```

The resulting set has 2,053 distinct definitions, 1,009 distinct undefined
references, and 332 unresolved names after definitions within the set are
removed. The four reproducible lists are retained as
`historical-with-bios-system-provider-{object-paths,defined,undefined,external-undefined}.txt`
under `artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/`.

The original system provider eliminates these 18 names that remained after the
BIOS-provider pass:

```text
_cmos_clear_shutdown_byte  _cmos_init       _cmos_post
_cmos_read_byte            _cmos_write_byte _dma_init
_dma_lim_setup              _dma_post        _dummy_int
_illegal_dvr_bop            _illegal_op_int  _rom_init
_search_for_roms            _softpc_version  _SWTMR_init_funcptrs
_timer_init                 _timer_post      _unexpected_int
```

The required chain symbols remain locally defined and no longer external:
`_c_cpu_simulate`, `_BIOS`, `_MS_bop_0`, and `_DemDispatch`. This is still an
object-symbol observation, not a link or runtime result. New dependencies
introduced by an original provider layer are expected: the question is whether
the next complete original library supplies them, not whether an individual
DOS service can be substituted.

## Next Link Work

The first two original provider groups have now been admitted as separate
object gates: the 13-unit BIOS provider layer and the 9-unit system provider
layer. The next action is to identify the next complete original provider
library from the 332-name external set. Each decrease in unresolved names must
name the original provider or the corresponding admitted host seam; a forced
executable link is still out of scope.

### Keymouse Provider Pass

The complete five-unit original `base/keymouse` library has since passed its
own clang-cl/i686 object gate. Adding it to the same audit produces 180
objects, 2,367 distinct definitions, 1,060 distinct undefined references, and
332 external unresolved names. The reproducible lists use the
`historical-with-keymouse-provider-*` prefix.

The library resolves 22 original keyboard/mouse/PPI names, including
`_keyboard_init`, `_keyboard_post`, `_keyboard_io`, `_keyboard_int`,
`_mouse_init`, `_mouse_install1`, `_mouse_install2`, both mouse interrupt
paths, `_mouse_video_io`, and `_ppi_init`. It introduces the expected lower
host/display dependencies such as `_host_os_mouse_pointer`, `_host_x_range`,
`_host_y_range`, `_HostReleaseKbd`, `_WaitKbdHdw`, `_PCDisplay`, and the
historical mouse/timer suspension hooks. Those names remain unresolved: no
replacement callback or device behavior has been added.

The next complete provider candidate is `base/disks` together with the
original host disk/floppy backend layer. It must be admitted as a source
library pair, not decomposed into an `INT 13h` substitute.

### Support Provider Pass

The five original support-library members not already represented by the
historical `ios.c` overlay were compiled as the separate
`ntdos64-opennt-support-provider-objects` gate. The provider-layer audit adds
their exact `DependInfo.cmake` object paths to the prior video-provider set;
it does not glob stale objects, link an archive, or create an executable.

The case-sensitive `historical-with-video-support-provider-casesafe-*` reports
contain 214 objects, 3,265 distinct definitions, 1,533 distinct undefined
references, and 380 external unresolved names. Compared with the
video-provider input, the
original support units resolve exactly:

```text
_SPC_Product_Name  _terminate  _time_of_day  _time_of_day_init
```

They add only their own historical lifecycle/configuration dependencies:
`_config`, `host_applInit`, `host_applClose`, `host_start_cpu`,
`host_terminate`, `host_fdisk_term`, `host_deinitialise_EM`,
`InitialiseDosEmulation`, `setup_global_data_ptr`, `setup_vga_globals`,
`Sleep`, and `exit`. These are not DOS SVCs and must be resolved
either by the complete original provider owning them or, where the modern host
cannot provide the historical facility, by a separately admitted lifecycle or
clock seam. In particular, the presence of historical `main.c` in an object
gate does not select it as the research process entry point.

The reproducible evidence is
`artifacts/build/legacy/referenced/build-opennt-standalone-clang-x86/historical-with-video-support-provider-casesafe-summary.json`
and its adjacent object-path, definition, undefined, external, resolved, and
introduced lists. The next provider selection should examine the original owner
of `_config` before considering any lifecycle facade.

### Case-Sensitive Symbol Rule And SH-12 Facade Pass

COFF symbols are case-sensitive. The provider audit now preserves that fact
with ordinal `HashSet` membership and `Sort-Object -CaseSensitive -Unique`.
An earlier PowerShell-default case-insensitive pass conflated the historical
`_Timer_init` and `_timer_init` symbols; its counts and apparent timer delta
are superseded by the `*-casesafe-*` evidence and must not be used for provider
admission.

The default-disabled `ntdos64-opennt-contained-media-fdisk-objects` target was
then added to the case-sensitive support baseline. The 215-object
`historical-with-contained-media-fdisk-casesafe-*` evidence reports 3,280
definitions, 1,536 undefined references, and 377 external unresolved names.
It resolves only these five original DEM media-provider imports:

```text
_nt_fdisk_close  _nt_fdisk_init  _nt_fdisk_read
_nt_fdisk_verify _nt_fdisk_write
```

The facade introduces only `GetFileSizeEx` and `SetFilePointerEx` imports for
its regular-file contract. It does not resolve or replace floppy support,
DEM's DASD/IOCTL bodies, INT 13h semantics, or any BOP/SVC dispatch. The
unresolved `nt_floppy_*` family remains visible and is outside this fixed-disk
facade's first profile.

### Historical Config Entry Is Excluded From The First Profile

The support diff identifies `_config` as the original
`base/mvdm/softpc.new/host/src/config.c` owner. Reading that source shows it is
not a generic machine-configuration provider: it owns historical PIF lookup,
`VDMINFO`/server registration, console-title discovery, shortcut handling, and
optional full-screen/WOW setup. Those facilities belong to the removed NTVDM
product-launch environment, not to the required direct CCPU execution chain.

Accordingly, `config.c` is retained as a manifested historical reference and
its unresolved import remains visible, but it is excluded from the first
non-invasive CCPU-only runner profile. No modern shim may replace `config()`
with a new PIF, console-server, or global launch policy merely to make the
symbol disappear. The modern CLI supplies its own explicit BYOB invocation
boundary; a future historical configuration profile requires a separate source
provenance and host-seam admission decision.

### SH-11 Contained OEM Facade Pass

The original two-unit `oemuni` gate stops before object creation because it
requires private TEB/PEB and RTL string/heap ABI. It remains source reference.
The separate `ntdos64-opennt-contained-oemuni-objects` target implements only
the published `*Oem` boundary with a wrapper-configured root, OEM conversion,
and session-local environment state.

`historical-with-contained-media-oemuni-casesafe-*` contains 216 objects,
3,317 definitions, 1,561 undefined references, and 383 external unresolved
names. It resolves 18 original `*Oem` imports, including CreateFile, find,
attribute, directory, volume, disk-space, and session-environment APIs. Its
introduced imports are only Win32 wide-file/conversion/heap APIs plus CRT
support; it introduces no BOP, DEM dispatch, DOS SVC, CPU, or device symbol.

The standalone smoke test proves contained create/write/find/move/delete,
session environment/reset, and lexical/reparse-point rejection. In addition,
the default-disabled `ntdos64-opennt-contained-oemuni-demdir-fixture` links
the original `demdir.c` with test-only passive register/memory observation.
On 2026-08-06 it passed `demCreateDir`, `demSetCurrentDir`, and
`demDeleteDir` under a configured `C:` root, and observed the original
`demClientError` path for `C:\..\escape`. The fixture does not execute CCPU,
dispatch a BOP/SVC, or change DOS-return behavior; its observations are
bounded to this historical caller seam.

This advances SH-11 only for the directory slice. The original file/search
DEM paths that own DTA, FCB, SFT, search continuation, and detailed DOS-error
selection remain unproven, so the facade is still pre-admission for a runtime
profile.

The subsequent source classification makes the split explicit: `demsrch.c`
does not route its native directory enumeration through `*Oem`. It retains a
directory handle and calls `RtlDosPathNameToNtPathName_U`, `NtOpenFile`, and
`NtQueryDirectoryFile` through its original `FileFind*` routines. Any later
contained search adapter belongs at that exact SH-05 seam and must preserve
DEM-owned DTA/FCB/find-list state; it cannot be folded into SH-11.

### Original Comms Provider Pass

The full five-unit `base/comms` library was imported from the fixed OpenNT
revision in its original directory and compiled as the default-disabled
`ntdos64-opennt-comms-provider-objects` target. Its `sources` file identifies
the complete provider set: `com.c`, `printer.c`, `printer_.c`, `prt_scrn.c`,
and `rs232_io.c`. The target enables the original printer branch but excludes
the unrelated `GISP_CPU` hardware-assisted CPU header path; the latter's
incomplete `h_exception_t` declaration is not patched or treated as a CCPU
dependency.

The case-sensitive `historical-with-comms-provider-casesafe-*` inventory has
221 objects, 3,398 definitions, 1,589 undefined references, and 398 external
unresolved names. Relative to the contained-media/OEM baseline it resolves
exactly these six historical BIOS provider symbols:

```text
_com_init  _com_post  _rs232_io
_printer_init  _printer_io  _printer_post
```

It introduces 21 lower host-peripheral imports: `host_com_*`, `host_lpt_*`,
`host_print_*`, and `wow_com_inb/outb`. This is the expected boundary below
the original serial/printer controller logic. No BOP, SVC, DOS state, or port
handler replacement is present. A future host provider must be explicit about
which virtual port maps to which user-selected stream; it must never probe or
open host COM/LPT/printer devices implicitly.
