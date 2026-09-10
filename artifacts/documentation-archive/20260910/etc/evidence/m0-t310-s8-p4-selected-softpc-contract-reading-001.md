# M0 T310 S8 P4 — selected SoftPC source-contract reading, pass 1

## Purpose

This record starts the required source reading behind the complete static ABI
and compiler inventories. It does not treat a warning as proof of a defect,
and it does not authorize a one-line cast. Each conclusion below comes from
the original declaration, initializer and caller relationship.

## CCPU access-table contract

`softpc.new/host/src/nt_cprgs.c` declares the historical register-access and
`host_simulate_func` slots and, in the selected `CCPU` branch, initializes them
with `c_get*`, `c_set*` and `c_cpu_simulate`. `softpc.new/host/inc/host_cpu.h`
publishes macros that call the same slots. The source identity of the selected
implementations is the CCPU declaration cohort (`base/ccpu386/cpu4gen.h` and
`c_main.c`), not a modern host callback provider.

This is a complete CPU-engine selection contract, not a guest-memory address
or host-handle crossing. The mapping manager is therefore prohibited here.
The later repair must preserve original entry names and selected CCPU ordering;
it needs a source-proven typed table or a minimal mirror declaration correction,
not a per-register adapter wrapper.

## CCPU executor and SAS contract

`base/ccpu386/c_main.c` constructs local opcode implementation pointers,
assigns original instruction bodies such as `ADD`, `LAR`, `CALLF` and `SHLD`,
then invokes them after the original decode/fetch macros have prepared operands.
Those calls are local CCPU execution dispatch. They must be audited as one
executor contract and must not be redirected through the session mapping
manager or an adapter callback.

`base/ccpu386/ccpusas4.c` owns the original `MEM_HANDLERS` table registration
and dispatches byte/word/video handlers. Its `gmi_define_mem` path initializes
handler slots from the original controller owner. `c_GetPhyAdd` and the
reachable physical accessor paths are different: they cross from a guest
physical address to selected machine backing, so only that narrow crossing may
use the session guest-memory mapping manager. The table itself carries private
SoftPC function pointers and must retain native pointer width.

## C video generated contract

The complete `base/cvidc` source package contains the original `SEVID000.c`
through `SEVID030.c`, `SINIT011.c` through `SINIT013.c`, `ev_glue.c`,
`evidfunc.c` and the original table headers. The selected package does not
contain an upstream EDL/generator program from which this repository can
recreate the `SEVID*` output. The imported generated C is therefore the
authoritative source identity.

The dominant C4456/C4457/C4102/C4127 diagnostic families arise from generated
local labels, nested return blocks, shadowed temporary names and constant
generator branches. They occur on both x86 and x64 and are not, by themselves,
pointer-width findings. Their eventual repair must be a package-level,
source-derived generated-output transform or a registered overlay boundary; it
may not be thousands of manual edits or a compiler suppression.

The separate C4311/C4312/C4057/C4113 cases are material: `vglob.c`, generated
rules and the `VideoVector`/EVID carrier exchange private native pointers. The
selected `CVIDC_RULE_WORD` currently makes `IUH` native-width for this private
generated-rule target, while `IHPE` is already pointer-sized. That is not a
guest token contract. The next pass must prove every `IUH` slot as one of
private pointer, fixed guest scalar, or guest/physical address before retaining
or replacing that divergence.

## Controller paths: first source classifications

`base/keymouse/keybd_io.c:kb_setup_vectors` obtains a guest table through
`effective_addr` and then forms `Start_of_M_area + segment:offset` pointers for
keyboard/timer state. `base/system/timer.c:timer_generate_int` likewise reads
and updates IVT/BDA fields through `Start_of_M_area`. Those are genuine
guest/physical-memory crossings. Their final x86/x64 route must use the
already-admitted checked guest-memory mapping-manager lease behind the original
call shape; keeping the raw host pointer arithmetic is not valid on either
host width.

`base/video/ga_glue.c` is more explicit about the old assumption: its CCPU
branch converts `UTINY *` values to `ULONG` before comparing them with video
RAM bounds and before passing offsets to `C_vid_writes`. The original comments
already describe an Intel address carried in a host pointer. This is a true
guest/physical-address representation problem, not a generic private-pointer
case. The repair must derive the checked physical/linear offset via the
mapping-manager lease and preserve the original RAM-versus-VGA and direction
branches. It cannot truncate the pointer or tokenise every internal callback.

`base/video/ega_read.c` initializes `READ_POINTERS` from C-video glue
functions whose old extern declarations default to `int` while the table
expects `IU32`; `base/keymouse/mouse_io.c` passes `MOUSE_SCALAR *` through an
older `word *` callback shape. These are same-width x86 and x64 declaration
contract errors, not mapping cases. They require a grouped source-proven
prototype/type correction, retaining the original controller call order.

`base/system/timer.c` and the ROM/controller paths also have private native
pointer casts. They must be classified one by one as (a) original private
SoftPC storage, which needs a native-width type/overlay contract, or (b) an
actual guest/physical address, which needs a checked mapping lease. The
existing warning alone does not make that decision.

## Current disposition

The four contracts above remain **under source review**. No runtime-video,
controller, or complete SoftPC claim follows from compilation. The associated
candidate and warning cluster ledgers are the exhaustive worklists; this record
only establishes their first source-derived grouping and forbids invalid
mapping-manager or warning-suppression shortcuts.

## Host/control interface findings, first tranche

`softpc.new/host/src/nt_cprgs.c` and `host/inc/host_cpu.h` describe one
historical CPU-selection access table: the `CCPU` selection assigns its
original `c_get*`, `c_set*` and `c_cpu_simulate` bodies to the published
slots, while the public macros dispatch through those slots.  The declarations
are K&R-style unprototyped function pointers.  The repair question is therefore
the full table's typed declaration/definition agreement on both host widths;
it is not a collection of independently replaceable callbacks, and it is not
a mapping-manager use case.

`softpc.new/base/system/timer.c` has two separate contracts.  Its timer
controller tables (`state`, `actionOnWtComplete`, `getTime`, and the
`timer_*_func` port entry points) are native private controller callbacks and
must receive a grouped native-width prototype audit.  By contrast,
`timer_generate_int` directly dereferences IVT and BDA locations below
`Start_of_M_area`; those operations represent selected machine physical memory
and need the checked session guest-memory lease behind the original accessor
shape.  Treating every timer pointer warning as one category would be wrong.

`softpc.new/base/keymouse/mouse_io.c` has the same split.  Its extensive
`word *` service-table callback contract is a controller ABI that needs one
prototype/signature review.  Its `effective_addr` and `sas_*` calls already
describe guest addresses and should remain guest-memory operations rather than
be widened into native pointer tokens.  The file is not evidence that a host
pointer may cross a 16-bit mouse interface.

`softpc.new/base/video/ga_glue.c` and `ega_read.c` are the first video
subcluster requiring a mixed treatment: the former preserves an old
``Intel-address-in-host-pointer`` convention in the CCPU branch, which is a
checked physical mapping issue, while the latter's C-video extern/table
disagreement is a callback declaration issue.  A repair must retain this
division and be assessed together with the C-video table, not by local casts.

## CCPU generated GDP state-layout finding

`softpc.new/host/src/nt_cpu.c:InitNtCpuInfo` consumes generated `GLOBAL_*`
macros such as `GLOBAL_CurrentUniverse`, `GLOBAL_CleanedRec` and the CPU
register fields.  Their selected CCPU `gdpvar.h` forms calculate fixed offsets
from `GDP_PTR`; the historical output stores both scalar values and private
native pointers in a layout generated for a 32-bit host.  The source's
`(IUH) GDP_PTR` / pointer round trips are therefore not guest addresses and
not host-resource identities.  They are private CCPU state-layout accesses.

On x64, widening `IUH` or the pointer fields independently would shift the
generated offsets and corrupt the same original state record; retaining the
old integer width truncates the private address.  Neither a local cast nor the
session mapping manager is valid.  The remaining CCPU access-table review must
classify the complete generated GDP record and select one source-shaped,
native-width layout treatment for the whole selected CPU package (minimal
generated declaration correction where layout remains scalar, or a private
CCPU overlay/transform where pointer-bearing slots require it).  It must then
reconcile `nt_cpu.c`, `nt_cprgs.c`, the selected CCPU generated headers and
their callers as one contract.

## SoftPC memory/EMS/UMB contract, first source reading

`softpc.new/host/src/nt_mem.c` keeps an original host reservation (`intelMem`)
and converts between that private backing pointer and an Intel linear address.
The `VdmAllocateVirtualMemory`, commit, decommit and free algorithms operate
on the Intel numeric address and preserve page/header ownership. Its
`AddVirtualMemory` route is different: it accepts an externally supplied host
address and binds it to an Intel range through `VdmSetPhysRecStructs`. The
existing named physical-mapping boundary is therefore relevant only at this
Add/Remove external-page edge. It must retain the original allocator and
header logic, recover alignment through the mapping manager, and never make
the private `intelMem` backing pointer into a durable session token.

`nt_emm.c` and `nt_umb.c` contain a second, historical NT section/view model:
they pass `effective_addr(segment, 0)` and UMB physical values to
`NtMapViewOfSection`, `NtUnmapViewOfSection` and `NtAllocateVirtualMemory`.
Those calls expressed the old product's ability to map host views directly at
VDM physical addresses. In the selected non-invasive profile, their guest
address arithmetic remains original EMS/UMB semantics, but the old host-view
mechanism must be a same-shaped selected-machine physical-page binding behind
the mapping-manager lease. Raw casts such as `(DWORD)Address` and
`(PVOID)dwBase` cannot remain on either host width.

The complete memory contract has three distinct value classes:

1. Intel linear/segment/physical values remain fixed-width original guest
   quantities.
2. `intelMem`, EMM backing sections and host allocation/view pointers remain
   private native-width values.
3. Only externally supplied page bindings cross through the session
   guest-memory mapping manager and the named SoftPC physical mapping contract.

This cluster is not ready for individual warning edits. The next source pass
must read the `VdmMapDosMemory`/`VdmUnmapDosMemory`, `VdmSetPhysRecStructs`,
`sas_overwrite_memory`, EMM page-map and UMB VDD caller/return contracts as one
group, then select one original-shape adapter or overlay boundary for the old
NT section APIs.

## SoftPC timer/interrupt/boot contract, first source reading

`softpc.new/host/src/nt_timer.c` retains a coherent original heartbeat:
`host_timer_init` creates the worker, `TimerInit` creates resume/suspend
events, `Win32_host_timer` sequences the delay, ICA lock, `time_tick`, RTC
tick, `cpu_interrupt(CPU_TIMER_TICK, 0)` and release.  Public thread, event,
wait and performance-counter APIs can retain this ordering on modern Windows.
The historical `NtAlertThread` termination edge needs a session-owned
cancellation event with the same worker-stop result; it must not be silently
dropped or change into process termination.

`host_init_bda_timer` in that same source writes BIOS Data Area fields through
`Start_of_M_area`. Those are selected-machine guest physical bytes, unlike the
native `ThreadInfo`, event and ICA objects, so only the BDA write path needs a
checked guest-memory lease.  The heartbeat callback and timer controller
function-pointer forms remain native controller ABI work, not mapping-manager
tokens.

`nt_eoi.c` owns ICA synchronization and delayed IRQ scheduling. Its
`pNtVDMState` macro is another physical BDA-style address under
`Start_of_M_area`, whereas `IcaLock`, `DelayHandle` and controller callbacks
are private native state. Its MONITOR-specific `CurrentMonitorTeb`/thread
look-up path is an unselected historical branch and cannot be revived as a
replacement for the selected non-MONITOR interrupt route.

`nt_inthk.c` is a compact but material warning family: it declares typed
hardware/software/exception callback slots but registers callback values
through historical data-to-function pointer casts. The repair must recover the
original registration declarations and make those callback assignments typed;
they are not fixed-width guest data and must neither use mapping IDs nor a
generic callback shim. `nt_reset.c` then remains the original owner of the
startup ordering and must consume the completed timer/ICA interfaces rather
than creating a parallel reset path.

## Host video binding contract, first source reading

`nt_ega.c`, `nt_vga.c` and `nt_cga.c` transform bytes obtained through the
original `get_regen_ptr` interface into a host DIB owned by
`sc.ConsoleBufInfo`. The source thus has two deliberately different pointer
classes: regenerated-video bytes are selected-machine memory and require the
same bounded read/lease discipline as the video controller; `lpBitMap`, mutex
and console/DIB structures are private host display objects and must remain
native-width. No stable mapping token may be exposed to either side merely
because the host presentation code uses pointers.

`nt_graph.c` owns the original `VIDEOFUNCS` composition and host presentation
initialization. Its predominant diagnostics are historical signed/unsigned
screen-coordinate and unused-parameter forms; their repair must preserve the
original clipping arithmetic and host UI failure behavior, not rewrite the
video algorithm. `nt_aorc.c` is a compatibility wrapper over the `Sas` table;
its `sas_overwrite_memory` and related calls are a typed controller-table
contract that must be reconciled with the CCPU/SAS declaration source before
any wrapper is edited.

The complete video repair boundary therefore spans `base/video`, generated
`base/cvidc`, host `nt_{ega,vga,cga,graph,aorc}.c`, and the selected SAS
provider. It must decide native callback prototypes, checked regenerate-memory
access and host DIB lifetime together. It may not use a callback cast to make
the host vectors compile, and it may not substitute a new display algorithm.

## CCPU segment-setter table cross-check

The first complete table slice confirms an actual three-carrier disagreement,
not an MSVC-only warning:

1. `host/src/nt_cprgs.c:135-138` declares the selected table slots
   `set{CS,DS,ES,SS}_func` as unprototyped `INT (*)()` and assigns
   `c_set{CS,DS,ES,SS}` in the selected CCPU initializer at lines 228-231.
2. `base/inc/cpu.h:203-216` presents those original CCPU setters as
   `INT` functions accepting one `word` value.
3. Generated `base/ccpu386/cpu4gen.h:778-818` presents the same four symbols
   as `IUH` functions accepting one `IU16` value. The formal x64 build records
   C4057 at all four table assignments.

The later CCPU access-table repair must read and reconcile the complete
selected table, including each header macro and all CCPU/alternate-engine
initializers. A per-line cast would merely erase the evidence and is
prohibited. The values are private CCPU callbacks: they must never be
represented by mapping-manager IDs. The available recovery forms are a
source-proven minimal typed declaration correction, or a generated/private
CCPU overlay carrier if the selected table requires a common native-width
layout transformation. No repair has been selected yet.

`m0-t310-s8-p4-ccpu-access-table-ledger.tsv` now records all 62 assignments
in `load_sw_cpu_access_functions`, joining the slot declaration,
implementation assignment, generated declaration and public declaration. It
normalizes only source-proven alias pairs such as `word`/`IU16` and
`half_word`/`IU8`; it does not declare a contract compatible merely because
the historical compiler accepted it. The first result is:

- 23 zero-argument contracts observed;
- 26 value-parameter contracts with an unprototyped table slot despite
  equivalent return types; and
- 13 return-contract disagreements: all nine flag reads (`get{D,I,T,P,A,S,Z,O,C}F`)
  and the four segment setters (`set{CS,DS,ES,SS}`).

Thus the existing C4057 segment-setter messages are only a subset of the same
source contract. The table can be repaired only as one source-shaped CPU
selection interface after its alternate initializers and macro consumers are
read; no subset is accepted as an isolated warning fix.

## CCPU access-table contract closure

The selected formal graph defines `CPU_30_STYLE` and `CCPU`.  Its active
`load_sw_cpu_access_functions` body has 62 assignments.  The same original
file also carries A3, A2 and hardware initializers with overlapping global
symbol spellings; they are not a licence to infer the selected CCPU ABI from a
bare symbol name.  The ledger generator therefore now evaluates the source
preprocessor selection before it joins a slot, generated declaration and
public declaration.

After that selection-aware audit, the 13 true return disagreements were
repaired as one group: `c_reg.c` is the unchanged implementation authority
for `ISM32` flag reads and segment loads, while `cpu4gen.h` and the selected
`nt_cprgs.c` slots now carry that same contract.  The remaining 26 selected
old-style setter slots were then given their already-generated `IU8`, `IU16`
or `IBOOL` parameter types as one group.  This leaves all 62 active
assignments with no return, parameter or unprototyped-slot mismatch in
`m0-t310-s8-p4-ccpu-access-table-ledger.tsv`.

Formal MSVC `/MT /W4` recompilation of the unchanged original selection body
plus these declaration corrections succeeds on x86 and x64 without C4057.
No cast, wrapper, warning suppression, mapping-manager token or new callback
provider was introduced.  The inactive A2/A3/hardware profiles retain their
original declarations and remain separately unselected; this is selected
CCPU-contract closure, not a claim that every historical CPU engine is now
runnable.

## CCPU FPU compilation-contract closure

The selected original `base/ccpu386/fpu.c` is one complete CCPU FPU provider,
not a collection of BOP or fixture-specific paths. Its modern compilation
review found five independent original contracts:

1. NT4 `_controlfp` is a current-thread host floating-point-control operation.
   The source-facing `Host*` macros retain their names, argument values and
   sequencing, while `adapter-mvdm-host-out/win32/fpu_control_compat.c` uses
   public `_controlfp_s` with the same result-word direction. This is a
   same-shaped Win32 binding, not a new FPU implementation.
2. `FPREM` and `FPREM1` calculate the low quotient bits only on complete
   reduction. Their original second status test permitted an analyser to
   observe an uninitialised use. Initialising that private byte makes the
   original completed path explicit without changing its quotient, condition
   bit order or incomplete-reduction behavior.
3. Original FPU tags are declared as private `IUH` fields. Keeping temporary
   OR/XOR state at the same width removes an x86-coincidental narrowing; the
   values are controller flags, never guest addresses or mapping IDs.
4. Binary64-to-x87 conversion explicitly reconstructs the one-bit sign and
   15-bit exponent field. Exponent differences use a 32-bit signed local
   value; this is an architectural-width correction, not a pointer-width
   conversion.
5. The non-product stack-register trace helper now carries the original
   caller's buffer bound, uses bounded CRT operations and formats the original
   raw sign/exponent value rather than passing a C bitfield structure through
   a variadic `%x` call. Its sole original trace caller owns an 80-byte buffer.

The formal selected CCPU graph was regenerated and compiled with `/MT /W4`:

```text
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/ccpu/fpu.obj
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/ccpu/fpu.obj
```

Both commands completed with no diagnostics. This proves x86/x64 source and
compile closure of the selected original FPU translation unit and its
same-shaped control-word adapter. It does not yet prove guest FPU instruction
runtime behavior, full CCPU execution continuity, or every alternate
historical FPU/CPU profile.

## CCPU SAS/BIOS interface correction, first closure slice

The complete selected `base/ccpu386/ccpusas4.c` object was read together with
its original `sas.h`, `host/src/sim32.c`, `host/inc/host.h` and the original
EMS `base/dos/emm_mngr.c` provider.  This establishes four source contracts:

1. `c_SasRegisterVirtualSelectors` writes both original LDT descriptors and
   then records `selectors_set`; its declared `IBOOL` result must therefore be
   `TRUE` after those writes, rather than an indeterminate C return value.
2. `host_sas_term` is an existing original SoftPC provider in `sim32.c`; the
   CCPU caller had only lost its declaration.  `host_simulate` and
   `LIM_{b,w}_write` likewise have existing original declarations/providers in
   `host.h` and `emm_mngr.c`, so the correct recovery is original header
   composition, not a new adapter body.
3. `Length_of_M_area` is supplied from a selected physical-memory extent and
   returned through fixed-width `TYPE_sas_memory_size`.  It is not a host
   pointer.  Its CCPU declaration is consequently `PHY_ADDR` on both x86 and
   x64; `Start_of_M_area` remains the separate native host backing pointer.
4. On a protected-mode page-translation failure before selectors exist, the
   original read branches had undefined C returns.  The current source now
   returns the same width-specific all-ones unavailable value that `phyR`
   already returns for inaccessible physical memory.  The original selector
   route and all writes remain unchanged.

Formal `/MT /W4` recompilation of
`obj/ccpu/ccpusas4.obj` on both x86 and x64 confirms removal of the lost
declarations, return-contract diagnostics and x64 capacity narrowing.  The
remaining C4996/C4100/C4389 findings are recorded SAS package work: secure
environment lookup, source-defined no-op unsupported operations, and the
`simulate_level` signedness contract.  None was suppressed or converted into
a mapping-manager token.
