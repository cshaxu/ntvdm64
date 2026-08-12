# BOP Dispatcher Source Recovery

The fixed NT4 OpenNT sources identify the original owners reached by
`host/src/nt_bop.c`:

- `DemDispatch`: `base/mvdm/dos/dem/demdisp.c`;
- `CmdDispatch`: `base/mvdm/dos/command/cmddisp.c`;
- `DpmiDispatch`: `base/mvdm/dpmi32/dpmi32.c`;
- `DBGDispatch`: `base/mvdm/dbg/dbg.c`.

The staged owner files are provenance-recorded exact copies. The full COMMAND
library is now a default-disabled object target and is admitted only to the
link-closure fixture; no handwritten dispatcher substitute is admitted.

## Original NT4 Library Boundaries

`dos/command/sources` declares one COMMAND library containing `cmd.c`,
`cmddata.c`, `cmddisp.c`, `cmdexec.c`, `cmdexit.c`, `cmdmisc.c`, `cmdpif.c`,
`cmdredir.c`, `cmdconf.c`, `cmdkeyb.c`, and `cmdenv.c`, with `WIN_32` and
`NTVDM` defined. `cmddisp.c` is only the table and dispatch entry; compiling
it alone would conceal the command SVC state/handler dependencies.

`dpmi32/sources` declares one DPMI32 library containing `debug.c`,
`dpmi32.c`, `dpmiint.c`, `dpmiselr.c`, `buffer.c`, `data.c`, `int21map.c`,
`modesw.c`, `register.c`, `savestat.c`, `stack.c`, `vxd.c`, `xmem.c`, plus
the x86 sources `i386/dpmi386.c`, `i386/dpmimem.c`, and `i386/dpmimisc.c`.
It uses `LINKED_INTO_MONITOR` and `precomp.h`. This is an interpreter/monitor
integration boundary, not a generic DPMI shim candidate.

The fixed OpenNT DPMI32 sources retain `VDM_TIB.PmStackInfo`, while the
adjacent fixed private `vdm.h` calls the prefix-compatible field `DpmiInfo`
and extends it with a later member. NT4.5 retains `PmStackInfo`. The isolated
compile target therefore uses a target-local `PmStackInfo=DpmiInfo` spelling
alias only to measure the source group's closure. It is explicitly not proof
that either TIB layout can be paired with the other at runtime; that versioned
kernel/monitor ABI remains a separate recovery question.

The complete NT4 x86 DPMI32 source group now compiles in a default-disabled
object gate using the preserved native NT declaration order (`nt.h`, `ntrtl.h`,
`nturtl.h`) and the historical `_X86_` profile. Its sole source overlay rewrites
one obsolete cast-lvalue pointer advance in `i386/dpmimem.c` into an equivalent
typed assignment accepted by current clang-cl. The overlay provides no VDM TEB,
CPU, memory, DPMI, or host implementation.

## DPMI32 Closure Evidence

The DPMI32 group is admitted only to the link-only closure fixture. Its
historical C tentative-global pattern requires target-local `-fcommon`; no
`Ldt` implementation was added. Relative to the COMMAND closure, it resolves
`DpmiDispatch` and `Sim32pGetVDMPointer`, and raises unresolved symbols from
166 to 185. The newly exposed owners are the original monitor register bridge
(`get/set` EAX through ESP and SP), `IntelMSW`, `VdmTib`, `VdmDebugLevel`,
`VdmFeatureBits`, `ThreadSetDebugContext`, and `GetFastBopEntryAddress`.
This records an NTVDM monitor/thread-context contract; it does not authorize a
replacement DPMI runtime or a fabricated TEB.

`dbg/sources` declares a single-source DEBUG library, `dbg.c`, with `WIN_32`
and `DEBUG`. It remains a distinct historical debugger product boundary. The
source now compiles in a default-disabled object gate using the native NT
preamble and a target-local SEH spelling bridge. The bridge supplies only the
old `try/finally/except` spellings after the native preamble; it introduces no
exception, debugger, or host behavior.

## Recovery Order

1. Compile the already staged `demdisp.c` in an isolated object gate and
   record its original handler-table closure.
2. Import and compile the complete COMMAND library source set, default
   disabled, before admitting its `CmdDispatch` owner to a closure fixture.
3. Import and compile the complete x86 DPMI32 library source set under its
   original profile before admitting `DpmiDispatch`.
4. Recover DBG separately, after the normal DOS runtime path is observable.

Every group remains excluded from default builds and the research runner until
the original chain is linked and its host obligations are explicitly measured.

## COMMAND Closure Evidence

The complete COMMAND target compiled under its historical `WIN_32` profile.
Adding its objects only to the CCPU/BIOS/DEM closure fixture resolved
`CmdDispatch`, `cmdPipeFileDataEOF`, `cmdPipeFileEOF`, and `IsFirstCall`.
It exposed the original COMMAND host obligations rather than a replacement
runtime: environment conversion/mutation, OEM path search, PIF configuration,
console event-thread coordination, current-directory synchronization, and
process exit notification. The fixture rose from 154 to 166 unresolved symbols
because it now reaches those real historical boundaries. The target remains
`EXCLUDE_FROM_ALL` and is not a runner dependency.

## DBG Closure Evidence

Adding the compiled DBG object only to the closure fixture resolves
`DBGDispatch`, `ModuleLoad`, `ModuleFree`, and `ModuleSegmentMove`. It reduces
the unresolved count from 185 to 184 and exposes only three additional monitor
register bridge obligations: `getIntelRegistersPointer`, `setEBP`, and `setSS`.
Thus the original debug dispatcher, including the module lifecycle owners, is
recovered without treating the research runner's debug console as a substitute
for the historical debugger product.

## Event/Input Host Closure Evidence

The original `host/src/nt_event.c` now compiles as a default-disabled object
target. Its overlay changes only the source/header-consistent
`PointerAttachedWindowed` storage type and one obsolete cast-lvalue assignment;
the event algorithm is unchanged. Adding it only to the closure fixture reduces
unresolved symbols from 159 to 156. It recovers console event-thread, mouse,
keyboard-release, and DOS termination coordination, while exposing the true
modern-host seams: console input/output/wait/display APIs, keyboard scan-code
translation, VDD user hook block/resume, `ExitVDM`, and TLS direct-error state.
No event, console, keyboard, or VDD substitute is supplied by this recovery.

## Session/Initialization Host Closure Evidence

The original `host/src/nt_msscs.c` compiles unchanged as a default-disabled
session host object. Adding it only to the closure fixture reduces unresolved
symbols from 156 to 149, recovering historical DOS-emulation initialization,
PIF configuration initialization, standard-handle notification, PDB
termination, and VDD create/block/resume/terminate hook owners. It introduces
only `lim_page_frame_init`, identifying the remaining EMS/LIM initialization
owner. No PIF, VDD, session, or DOS startup replacement is introduced.

## Historical Register-Bridge Evidence

`host/src/nt_aorc.c` is the original CCPU register bridge: it forwards public
monitor `get/set*` entries to generated `c_get/c_set*` owners and owns no CPU
state. It compiles as a default-disabled object target. In the closure fixture
only, `CCPU_USE_HISTORICAL_REGISTER_BRIDGE` excludes the overlapping trace
adapter symbols; neither target executes guest code. The historical bridge
reduces unresolved symbols from 184 to 159, resolving 25 register accessors
without introducing a new unresolved symbol. This demonstrates that the
already-selected CCPU archive supplies that generated accessor ABI. It does
not address the remaining generated CCPU blockers such as `c_effective_addr`.

## NT4 EMS Host/Manager Evidence

The original DOS EMM manager, `base/dos/emm_mngr.c`, and its x86 monitor host
backend, `host/src/x86_emm.c`, now compile as separate default-disabled object
targets under their original `LIM`/`MONITOR` profile. The manager has one
compiler-only signedness overlay for its `deallocate_altreg_set` definition;
the x86 backend is an exact source copy. The native declaration-only host
compatibility input records the original OpenNT `OBJ_CASE_INSENSITIVE`,
`SECTION_INHERIT`/`ViewUnmap`, and `MEM_DOS_LIM` declarations needed to compile
the backend. It implements none of those APIs.

Adding both objects only to the CCPU/BIOS/DEM closure fixture initially
reduces the unresolved count from 166 to 153: the original x86 backend resolves
the manager's `host_*` allocation, page-map, copy, exchange, and storage calls.
Its three native section references were implicit cdecl calls because the
historical source did not include `ntmmapi.h`; recording the exact OpenNT
stdcall declarations lets the current x86 `ntdll.lib` resolve them and reduces
the closure to 150 unresolved symbols.

The only remaining EMS-specific symbol is `HoldEMMBackFillMemory`. Fixed NT4
OpenNT contains its definition in `base/mvdm/v86/monitor/i386/sas.c`: it maps
A20-section backing pages into conventional memory after an EMS page is
unmapped, preventing an access violation. That owner belongs to the actual
x86 V86 SAS path, which is intentionally not imported or built for this
software-CPU research path. This locates EMS at a historical DOS-manager to
host-section-mapping boundary. It neither makes EMS part of the CCPU
interpreter nor supplies a modern section/memory substitute.

## Text Display Host Closure Evidence

The original `host/src/nt_det.c` text-display owner now compiles as its own
default-disabled object target, separate from the generic display/input group.
It owns `initTextSection`, `textBuffer`, `textBufferSize`, `stateLength`, and
the VDM console registration/unregistration state transitions. It is admitted
only to the link-closure fixture. Relative to the EMS closure, it reduces the
unresolved count from 150 to 145 by resolving exactly those five historical
symbols and introduces none. This retains the original console/driver call
flow for later host-shim analysis; it does not add a console policy, graphics
implementation, or runtime dependency.

## PIF Host Closure Evidence

The original `host/src/nt_pif.c` PIF parser/configuration owner now compiles
as a separate default-disabled object target. Its exact private declarations
remain at `host/inc/nt_pif.h`; the fixed NT4 private `pif.h` declaration input
was restored at its original `public/internal/windows/inc/pif.h` path. The
historical `WINNT` compile profile is required because that header exposes the
`WNTPIF31` structure only under that profile.

Adding this original object only to the closure fixture reduces unresolved
symbols from 145 to 141. It resolves `GetPIFData`, `GetPIFConfigFiles`,
`dwWNTPifFlags`, `WNTPifFgPr`, and `WNTPifBgPr`, and newly exposes only the
historical OEM environment-expansion host seam,
`ExpandEnvironmentStringsOem`. No replacement parser, configuration policy,
or runtime dependency is supplied: the result identifies the precise modern
host shim obligation while retaining the historical PIF data flow.

## OEM File/Process Facade Evidence

The complete original OEM facade is staged as the two-source `oemuni` group
(`base/mvdm/oemuni/file.c` and `process.c`) and now compiles as a separate
default-disabled x86 object target. Its source directly inspects the original
NT user-process state (`PUNICODE_STRING`, `PCURDIR`, `UNICODE_STRING`,
`NtCurrentPeb()->ProcessParameters->CurrentDirectory`, and
`NtCurrentTeb()->StaticUnicodeString`). The compile gate therefore selects
the fixed OpenNT SDK umbrella and private VDM API declaration roots before
the local minimal staging headers, defines the historical `_X86_` profile,
and applies only the existing legacy `try/finally/except` spelling bridge.
No synthetic TEB/PEB layout, process-state implementation, or OEM behavior
was added.

Adding the original facade only to the CCPU/BIOS/DEM closure fixture reduces
unresolved symbols from 141 to 119 and introduces none. It resolves the
historical OEM file, directory, path, environment, volume, debug-output, and
PIF expansion owners, including `ExpandEnvironmentStringsOem`,
`CreateFileOem`, `FindFirstFileOem`, `GetFullPathNameOem`,
`SetCurrentDirectoryOem`, and `SetEnvironmentVariableOem`. This confirms
that these calls are one coherent host facade rather than independent PIF or
COMMAND shims. The facade remains excluded from every runtime/default target;
the next work is to measure its actual modern import and process-layout
execution assumptions rather than replace any individual OEM operation.

## Idle/Lifecycle Host Closure Evidence

The original `host/src/nt_unix.c` idle/lifecycle owner now compiles as its own
default-disabled object target. It retains the historical `host_idle_init`,
`WaitIfIdle`, `WakeUpNow`, `PrioWaitIfIdle`, timer block/release, file lookup,
and host reset/termination flow. Its x86 CRT declaration bridge fixes only
two header-era differences: the original 32-bit `time_t` profile and the raw
`_sys_errlist`/`_sys_nerr` identifier spellings after the modern UCRT headers
have been included. It implements none of those CRT, clock, idle, or
lifecycle services.

Adding the original object only to the closure fixture reduces unresolved
symbols from 119 to 109. It resolves `ExternalWaitRequest`,
`HostIdleNoActivity`, `PrioWaitIfIdle`, `WaitIfIdle`, `WakeUpNow`, and the
historical `host_*` timer/lifecycle/file lookup owners. It introduces exactly
three modern CRT compatibility imports: `__sys_errlist` and `__sys_nerr`
(including the import thunk spelling). These are a narrow error-text adapter
question, not DOS, BIOS, or CCPU semantics. The owner remains outside every
runtime/default target.

## VDD BOP Bridge Evidence

The original `host/src/nt_vdd.c` now compiles as a separate default-disabled
object target using only the legacy SEH spelling bridge. It owns the historical
MS BOP `0x55` table vectoring (`ms_bop`), along with retained VDD adapter,
I/O-hook, IRQ-line, and DMA helper code. Adding the object to the closure
fixture reduces unresolved symbols from 109 to 108 by resolving exactly
`ms_bop`; it introduces none.

The fixture's `/OPT:REF` closure intentionally retains only the live BOP
vectoring path. The unreferenced VDD adapter/IRQ/DMA exports are not evidence
that their provider contracts are runnable or selected. No VDD policy, BIOS
handler, DOS service, or substitute BOP dispatcher was added; this merely
restores the historical owner of the one BOP table the current chain reaches.

## CCPU Host Lifecycle Blocker

The original `host/src/nt_cpu.c` remains a separate default-disabled compile
gate. Its CCPU probe now explicitly excludes `CPU_40_STYLE` and uses the same
native NT declaration order plus `_X86_` declarations as the independently
compiled EOI owner. This removed the unrelated RISC `GDP_PTR`/`ConstraintR*`
and Win32-first-header diagnostics. The last blocker was one historical
ownership collision: `nt_cpu.c` declares `Start_of_M_area` and
`Length_of_M_area` as host-memory globals, while the selected CCPU SAS archive
already owns `Start_of_M_area` and declares `Length_of_M_area` as
`sys_addr`/`IU32`.

The wrapper-only CCPU path in `nt_cpu.c` does not use either global, while a
preprocessor rename is invalid because it also rewrites the included SAS
declarations. The registered same-path `overlay/.../host/src/nt_cpu.c`
therefore excludes exactly those two declarations under `CCPU`, preserving the
entire non-CCPU source branch. It compiles successfully as the default-disabled
`ntdos64-opennt-ccpu-host-cpu-objects` gate. This resolves source ownership
for the narrow wrappers only; it does not add a type cast, duplicate storage,
SAS implementation, host lifecycle substitute, runtime admission, or guest
execution claim. These are not normal host APIs and are not recoverable by
adding a modern console, process, memory, or timing shim.

The recovered NT4 `obj.vdm/CDEFINE.INC` changes the interpretation of this
failure. Its x86 branch selects `MONITOR`, `C_VID`, and `X86GFX`; it does not
select `CCPU` or `CPU_40_STYLE`. The latter are selected only in the non-x86
branch, whose MIPS/PPC generated headers provide the `GDP_PTR`/
`ConstraintR*` host contract. Therefore no historical NT4 x86 CCPU host ABI
is missing from these snapshots: Microsoft used the hardware V86 monitor on
x86, while the shipped software CCPU host integration belongs to RISC.

The fixed NT4 CCPU source closure does contain its own generated
`ccpu386/cpu4gen.h` and `ccpu386/gdpvar.h`; both are hash-identical to the
retained NT5.1 copies. They expose the CCPU-local `Gdp` layout, not the RISC
host-side `GDP_PTR` contract. The CCPU compile fixture is consequently a
cross-profile source/ABI probe only, never evidence of a historical x86
runtime. The current hard boundary is architectural: a modern x64
software-emulation runner needs a newly designed monitor-facing CCPU adapter
or a different software CPU, with its own verified guest-memory, interrupt,
and mode-transition contract. The historical runtime chain must not treat a
handwritten lifecycle adapter as evidence that the NT4 x86 monitor was
recovered.

The reconstructed `overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c`
is a bounded SoftPC vector completion. It proves only real/V86/protected
segment-base translation using the original selector-table and descriptor
accessors; it does not provide descriptor validation, paging, fault, or
SAS-access semantics and is not linked into a normal runtime path.

The NT4 and NT5.1 `ccpusas4.c`, `c_main.c`, `c_reg.c`, and `c_addr.c` inputs
are exact hash matches, as are the CCPU-local generated headers above. They
are useful format/reference evidence only: their `Gdp` layout is not the RISC
`GDP_PTR` host contract, and the NT5.1 `nt_cpu.c` host source differs. The
reproducible `ccpu-host-cpu-generated-abi-blocker-v1.log` records the forced
cross-profile missing `GDP_PTR` and `ConstraintR*` diagnostics. No
cross-version or cross-architecture generated header has been copied or used
to make this target compile.
