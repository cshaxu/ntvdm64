# Bochs 2.6 Minimum Mechanics Closure

## Question

What is the smallest Bochs-owned executable slice that demonstrates machine
mechanics without a BIOS product, PC device policy, BOP, DOS, DEM, COMMAND,
WOW, or an OpenNT owner?

## Inputs

- authoritative source: `O:\repos.external\bochs-2.6-compat\bochs-2.6`;
- imported identity manifest:
  `artifacts/research-inputs/bochs-2.6-source-manifest.json`;
- `cpu/init.cc` (`BX_CPU_C::initialize` and `BX_CPU_C::reset`),
  `cpu/cpu.cc` (`BX_CPU_C::cpu_loop`), `memory/`, `pc_system.cc`, and the
  historical top-level `Makefile`;
- retained diagnostic logs below `artifacts/build/bochs-2.6-minimal-r1`,
  `-r8`, `-r14`, and `-r15`.

## Procedure And Observations

1. The source identity import copied 672 files and compared every target file
   against the authoritative local tree by path, byte count, and SHA-256.
2. An isolated copy was configured with legacy `configure` switches and driven
   through the historical top-level `bochs.exe` target. The graph compiled CPU,
   CPUID database, memory, GUI/configuration, and device libraries before
   encountering MSVC-2022 compatibility diagnostics. It therefore measures the
   old complete product graph, not a defined minimum.
3. The final such attempt was stopped deliberately. Its retained log proves
   that a full-target build is not a valid way to discover the intended object
   closure. No additional compatibility patches, devices, or OpenNT behavior
   are admitted from these attempts.
4. `BX_CPU_C::reset` establishes the architectural reset state in
   `cpu/init.cc`; for 386-and-later source configurations it assigns RIP
   `0xFFF0` and the reset CS cache base `0xFFFF0000`. `BX_CPU_C::cpu_loop` is
   an unbounded execution loop. A finite instruction budget and an explicit
   return/stop disposition are therefore not supplied by the historical public
   top-level executable and must be designed as a minimum-slice contract.

## Selected Mechanical Target M0

M0 is exactly:

1. create one CPU and the memory mapping required to observe reset;
2. perform a hardware reset and capture the resulting register state;
3. execute a finite, caller-supplied instruction budget from a bounded,
   caller-supplied memory region; and
4. return a typed `budget-exhausted`, `halted`, or `faulted` stop record with
   register snapshot and checked memory effects.

M0 intentionally does not boot a BIOS, enumerate a PC device, decode a BOP,
invoke OpenNT, or interpret a DOS interrupt. A budget is the controlled stop;
no magic instruction or service number is introduced.

## Closure Method

The next pass traces from the M0 operations, not from `main.cc` or the
historical `bochs.exe` target. For each referenced symbol, record whether it
is:

| Class | Admission rule |
| --- | --- |
| Required | Directly required by reset, bounded fetch/decode/execute, checked memory, or typed stop. |
| Default but not required | Present in a historical constructor/configuration path but not reached by M0; exclude it. |
| Rejected | GUI, config UI, plugins, BIOS product, PC device policy, networking, USB, sound, SMP/APIC/PCI, debugger, and all OpenNT semantics. |

The closure record must list source file, symbol, reason, incoming dependency,
and removal test. No item is called "core" merely because the old Makefile
lists it.

## Initial Classification (Not Yet a Complete Closure)

| Source / symbol | Classification | Evidence and limit |
| --- | --- | --- |
| `cpu/init.cc`: `BX_CPU_C::reset` | Required | M0 explicitly performs hardware reset; this method establishes the observed reset state. Its transitive calls remain to be traced. |
| `cpu/cpu.cc`: `BX_CPU_C::cpu_loop` | Required for analysis, not admitted for build | It is the available execution owner, but its unbounded `while (1)` does not satisfy M0. The exact bounded-step cut is unresolved. |
| physical instruction fetch/memory owner | Required, unresolved object | M0 must execute only from a checked caller-supplied range. The responsible memory methods and all their callbacks must be traced before admission. |
| `main.cc`, `config.cc`, `plugin.cc` | Default product path, excluded | They belong to historical program/config/plugin startup; M0 has no command-line configuration or plugin loading. |
| `gui/`, text configuration, display selection | Rejected | M0 emits a typed stop record and has no display or configuration UI. |
| `iodev/`, disk, network, USB, sound, PCI, APIC, SMP | Rejected unless a traced M0 call proves otherwise | M0 neither boots a BIOS nor enumerates hardware. Their presence in the full Makefile is not evidence. |
| OpenNT, BOP, DEM, DOS, WOW source | Rejected | M0 is pre-bridge machine mechanics only. |
| CPU model database / CPUID factories | Default but unresolved | `initialize` conditionally uses them at CPU level 4 and above. The M0 CPU-level decision must be recorded before either inclusion or exclusion. |

The classification is deliberately incomplete: a source file may move from
unresolved to required only with a symbol-level incoming edge from the M0
contract. It may not move because a full build reaches it.

## M0 Symbol-to-Object Ledger, Pass 1

| M0 operation | Source owner | Direct next edge | Current disposition |
| --- | --- | --- | --- |
| Reset state | `cpu/init.cc`: `BX_CPU_C::reset` | `flushICaches`, CPU state fields, conditional FPU/APIC/CPUID branches | `init.cc` is required; optional branch objects remain excluded until the selected CPU configuration proves otherwise. |
| Bounded execution control | no historical public M0 entry | `BX_CPU_C::cpu_loop` is unbounded | Superseded by the native-container decision; no project-owned CPU-loop shim will be built. |
| First fetch | `cpu/cpu.cc`: `prefetch` | linear translation, `getHostMemAddr` | Candidate-required; trace its real-mode path before admitting paging or device paths. |
| Decode and dispatch | `cpu/cpu.cc`: `getICacheEntry`; `cpu/icache.cc`; `cpu/fetchdecode.cc` | `serveICacheMiss`, `fetchDecode32`, `BxOpcodesTable` | Candidate-required. `init_FetchDecodeTables` initializes the historical opcode table from `ia_opcodes.h`; it can reference far more handlers than an M0 NOP fixture executes. Do not treat the historical `cpu/Makefile` object list as the M0 closure. |
| Physical fetch backing | `memory/misc_mem.cc`: `BX_MEM_C::getHostMemAddr` | A20, ROM/RAM mapping, optional direct-access handler | Candidate-required. The chosen M0 mapping must prove the exact memory objects and show why no BIOS/device object is reached. |
| Instruction implementation | `cpu/proc_ctrl.cc`: `BX_CPU_C::NOP` | processor state update only | Candidate for the first finite-budget fixture; admission awaits the bounded dispatch cut and link analysis. |

This pass yields an important negative result: an M0 program containing only
NOP bytes does **not** by itself justify compiling only `proc_ctrl.cc`.
`init_FetchDecodeTables` and the decoder table can keep references to many
handlers. The next analysis therefore measures the symbol/link closure of a
backend-local bounded dispatch cut, rather than selecting objects by test
opcode or by the historical full Makefile.

It also defines what “minimal” means here. It does not mean inventing a
project-owned NOP interpreter. If the selected Bochs decoder requires a broad
conditioned CPU instruction set as one link closure, that closure is admitted
as **one Bochs CPU-mechanics component** with its evidence; it is not feature
creep. The compression boundary is around the CPU component: no historical
program/configuration shell, GUI, BIOS product, or unused PC-device component
may enter merely because the CPU component needs a large internal object set.

## M0 Symbol-to-Object Ledger, Pass 2: Object Evidence

`dumpbin /symbols` was run against the isolated MSVC object files already
produced before the full-target stop. This is inspection only; it does not
restart a build.

| Object | Observed undefined edges | Classification consequence |
| --- | --- | --- |
| `cpu/init.o` | CPU helpers (`flushICaches`, `setEFlags`, `handleCpuContextChange`, decode initialization), logging, and parameter/state-registration classes | `reset` is required, but `init.cc` also contains configuration/state paths. Old object granularity is too coarse to equate all of its undefined edges with M0 requirements. |
| `cpu/cpu.o` | `bx_pc_system`, `bx_mem`, `handleAsyncEvent`, instruction-cache miss, physical fetch, linear translation, exception handling | PC time, memory, fetch/translation, and fault paths are real M0 candidate dependencies. |
| `cpu/icache.o` | `fetchDecode32`, `prefetch`, exception and CPU state | Fetch/decode is a direct M0 path, independent of the historical executable shell. |
| `memory/memory.o` | `getHostMemAddr`, allocation, write-stamp handling, `bx_pc_system` and CPU state | Memory must be admitted as a mechanics component, but its lifecycle must be stripped to the M0 mapped range. |
| `memory/misc_mem.o` | memory handlers plus parameter/state-registration classes and logging | As with `init.o`, a whole-object pull would incorrectly add configuration machinery. |

The old compiler command does not use function-level sectioning. Therefore a
minimum build cannot use “one legacy object file equals one M0 component” as
its model. The candidate build shim must first prove function-level retention
and dead-code removal (or source-unit splitting limited to the Bochs backend)
so that required reset/memory routines do not pull unrelated registration
methods. This is a build-mechanics concern, not permission to rewrite CPU or
OpenNT behavior.

## M0 Symbol-to-Object Ledger, Pass 3: Reset Section Proof

`cpu/init.cc` was separately compiled into
`artifacts/analysis/m0/init-gy.obj` with MSVC `/Gy`; this was a single-object
closure inspection, not a product or full Bochs build. `dumpbin` identifies
`BX_CPU_C::reset` as its own COMDAT section. The relocations of that section
show these concrete reset edges:

- `bx_cpu` state storage;
- `logfunctions::info` for the original reset diagnostic;
- `parse_selector` for CS and data-segment reset state;
- `flushICaches` after reset state assignment; and
- `BX_CPU_C::handleCpuContextChange` at reset completion.

The same object contains parameter/state-registration symbols, but they are in
other function sections. Thus the M0 build strategy is now evidence-backed:
compile the selected Bochs source units with function-level sections and link
with dead-code removal, then inspect the surviving symbol graph. It does not
yet prove that the above five edges are sufficient for execution; it proves
that unrelated configuration paths need not be admitted merely because they
share `init.cc`.

## M0 Symbol-to-Object Ledger, Pass 4: Execution And Reset-Window Proof

Three further Bochs source units were each compiled once with MSVC `/Gy` into
`artifacts/analysis/m0/{cpu-gy,icache-gy,misc_mem-gy}.obj`, and their COFF
relocations were retained beside them.  This is object-closure inspection; it
does not configure, link, or run the historical `bochs.exe` target.

| M0 symbol | Direct observed edges | Classification consequence |
| --- | --- | --- |
| `cpu/cpu.cc`: `BX_CPU_C::cpu_loop` | `setjmp`, `handleAsyncEvent`, `getICacheEntry`, `bx_pc_system::countdownEvent`, CPU state, and the instruction callback | The historical loop is not an M0 entry: it is unbounded and synchronizes historical PC time after every completed instruction.  A bounded backend-local sibling must preserve the fetch/decode/execute ordering and fault escape, while making both instruction accounting and time/event policy explicit. |
| `cpu/cpu.cc`: `BX_CPU_C::getICacheEntry` and `prefetch` | `serveICacheMiss`, `translate_linear`, CPU `getHostMemAddr`, `bx_mem`, exceptions, and logging/panic paths | Fetch/decode and the checked physical-memory veto path are required mechanics.  Paging, protected-mode translation, and fault handling remain code-reachable CPU behavior; they are not PC-device admissions. |
| `cpu/icache.cc`: `BX_CPU_C::serveICacheMiss` | `fetchDecode32`, `prefetch`, `boundaryFetch`, page-write-stamp state, CPU state, and diagnostics | The decoder/cache component is required as one evidence-backed CPU-mechanics closure.  A NOP fixture cannot reduce it to the NOP handler because trace construction references the generic decoder. |
| `memory/misc_mem.cc`: `BX_MEM_C::getHostMemAddr` | A20 calculation, memory-handler lookup, bounded RAM/ROM vectors, optional VGA/PCI/SMRAM branches | The needed M0 map must use the normal checked-memory entry and prove that its selected address range does not reach handlers, VGA, PCI, or SMRAM.  Those branches are not admitted as devices merely because the function contains them. |

### Reset-window consequence

`BX_CPU_C::reset` starts a 386-level CPU at CS base `0xffff0000`, RIP
`0xfff0`, hence the first physical fetch is `0x000ffff0`.  In the standard
`getHostMemAddr` path, the high `0xf0000..0xfffff` range is ROM-backed, not a
general caller RAM region.  M0 therefore needs a precise **private reset
window**: an owned, bounded byte image mapped only for the M0 reset fetch.
It is not a BIOS image, BIOS loader, boot flow, or PC-device policy.  Its
mapping and bounds checks are still unresolved build-shim work and must be
proved before a target is created.

### Bounded-dispatch decision

The observed configuration already has
`BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS=0`; with that setting the historical
loop increments `icount` after each instruction callback.  This gives one
credible cut location for the M0 budget: immediately after the historical
instruction completion/accounting sequence.  It does **not** yet authorize a
patch.  The remaining design question is whether the sibling entry retains a
minimal `pc_system` time call and async-event handling, or rejects/holds those
inputs at M0 initialization.  The answer must be recorded as an executable
contract before source changes or a build target are introduced.

### Current classification update

| Component / concern | Disposition | Reason |
| --- | --- | --- |
| Reset, CPU state, selector parsing, cache flush | Required | Direct reset edges and observed reset state. |
| Generic CPU fetch/decode/execute and CPU fault escape | Required as a component, still object-closed | Direct execution path; not reducible by fixture opcode. |
| Checked memory, a bounded reset window, and write-stamp maintenance | Required as a component, still object-closed | Direct fetch path and reset vector placement. |
| Historical PC clock/countdown | Default but unresolved | `cpu_loop` references it; M0 has no external time/device source, so the bounded entry must explicitly retain a no-device clock shim or exclude this call by a documented, behavior-preserving control cut. |
| Async interrupts, debugger/GDB stop paths | Default but not required for M0 | M0 has no injected interrupt or debugger contract.  Their no-input behavior must be proved, not assumed. |
| BIOS product/loader and all PC devices | Rejected | A private reset window supplies only bounded bytes; it does not start a firmware product. |
| GUI, config, plugins, storage, networking, USB, sound, PCI, APIC, SMP, OpenNT services | Rejected | No direct M0 operation reaches an admitted owner. |

## M0 Initialization Ledger, Pass 5: Not Yet Closed

Source inspection of the candidate lifecycle owners establishes a hard stop
before defining a build target:

| Candidate lifecycle method | Direct observation | Decision |
| --- | --- | --- |
| `BX_CPU_C::initialize` in `cpu/init.cc` | At CPU level 3 it initializes the generic fetch/decode table; the CPUID factory is compiled out.  Optional MSR/VMX/WX code is conditional. | Candidate-required for decoder setup, with CPU level 3 recorded as an evidence-based M0 constraint. |
| `BX_MEM_C::init_memory` in `memory/misc_mem.cc` | It allocates RAM plus Bochs ROM storage, initializes handler tables, reads the `I440FX` parameter through `SIM`, and calls `register_state`. | Not admitted as-is.  Its allocation/reset-window mechanics are candidates; its parameter/state-registration tail is default product behavior.  A source-level M0 memory initializer or an equally narrow proven construction path is still required. |
| `bx_pc_system_c::initialize` in `pc_system.cc` | It establishes a null timer and timing counters; unrelated `pc_system` methods delegate I/O to `bx_devices`. | Default but unresolved.  The no-device time state may be required solely to satisfy the bounded execution loop; all I/O delegation remains rejected. |

This means the M0 object closure is **not stable yet**.  In particular, no
custom target, linker response file, runtime shim, or Bochs source patch may
be added at this point.  The next permitted analysis is narrow: identify the
minimal original constructors/state fields needed for CPU, memory, logging,
and the no-device clock, then decide whether a backend-local initialization
unit can instantiate them without `SIM` parameter registration.  That unit
must expose no OpenNT operation and must not substitute a new CPU
interpreter.

## M0 Configuration Admission Record

The following are source-conditional decisions, not inherited `configure`
switches and not a claim that these are universally "core":

| Compile-time choice | M0 disposition | Source evidence |
| --- | --- | --- |
| `BX_CPU_LEVEL=3` | Required | Reset semantics under test are the 386 path; `initialize` omits the CPUID factory at levels below 4, removing its `SIM` parameter dependency. |
| `BX_SUPPORT_X86_64=0`, `BX_SUPPORT_SMP=0` | Required | M0 has one 32-bit CPU.  At level 3, APIC is consequently compiled out by `config.h`. |
| `BX_CONFIGURE_MSRS=0`, `BX_WITH_WX=0` | Required | These are the remaining `SIM`-based CPU initialization/state-UI branches. |
| `BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS=0` | Required | The ordinary loop accounts after every instruction; chained traces cannot provide a caller-observable one-instruction budget. |
| `BX_SUPPORT_FPU=0` | Required for M0 | M0 supplies no FPU instruction contract.  This removes the hard-reset `the_i387.reset()` edge; disabled-FPU decode remains Bochs CPU behavior through the existing `FPU_ESC` path. |
| PCI, VGA, storage, network, USB, sound, debugger/GDB/plugins/GUI | Rejected | No M0 request can reach a corresponding original owner.  `getHostMemAddr` retaining conditional code is not evidence to build a device. |

The eventual build must generate or provide a configuration header that
records exactly these choices.  It may not reuse a broad historical
`configure` product output merely because it happens to have compatible
defines.

## Lifecycle Separation Result

The original Bochs constructors for `BX_CPU_C`, `BX_MEM_C`, and
`bx_pc_system_c` establish storage/log identity and ordinary null state; the
historical `main.cc` owns their global definitions together with the product
program.  This does **not** admit `main.cc` as a product-startup dependency.
After the M0 configuration above, `BX_CPU_C::initialize` reaches generic
fetch/decode-table and SMM-map initialization without `SIM`.

The unadmitted operations are precise:

- `BX_MEM_C::init_memory` reads `SIM` only for the historical I440FX/PCI
  setting and then calls `register_state`;
- `BX_MEM_C::register_state` and `bx_pc_system_c::register_state` build GUI
  parameter trees through `SIM`; and
- `main.cc` contains global storage definitions mixed with full executable
  startup.

Accordingly, the only admissible future backend-local lifecycle unit is one
that owns the required global storage definitions and calls a narrow memory
allocation/reset-window setup that neither reads `SIM` nor registers state.
It must set PCI/SMRAM handlers absent, initialize the null clock, initialize
the CPU decoder, and perform hardware reset.  This is now a **defined design
constraint**, but still needs a symbol-level implementation/link ledger
before a build target can be created.

## M0 Decoder Closure, Pass 7: Port-I/O Boundary

The retained CPU Makefile/object inventory has 74 declared and 74 produced
CPU object files for the inspected level-3 build.  More importantly,
`fetchdecode.o` has 1,012 undefined instruction-handler symbols: the generic
decode table retains handlers for the instruction set, not merely the bytes in
the first M0 fixture.  The CPU decoder is therefore admitted as one component;
selecting a NOP-only object subset would be a project-created interpreter, not
Bochs execution.

One consequence is now explicit.  `cpu/io.cc` handlers use `BX_INP` and
`BX_OUTP`, which expand directly to `bx_devices.inp/outp`.  The historical
`bx_devices_c::init` is **not** an M0 initializer: after creating default
port tables it reads `SIM`, initializes virtual timers, and loads core
plugins.  Leaving `bx_devices` uninitialized is invalid because `inp/outp`
dereference the port-handler arrays.

| Item | Classification | M0 rule |
| --- | --- | --- |
| Generic CPU `io.cc` instruction handlers | Required as part of decoder closure | Must remain Bochs instruction semantics; they cannot silently vanish because the first fixture has no IN/OUT. |
| Historical `bx_devices_c::init` / plugins / individual devices | Rejected | They are product/device initialization and contain `SIM` and plugin dependencies. |
| Empty port-space router | Required but unresolved | A backend-local, no-device port-space owner must make every port unmapped.  It is a boundary mechanism, not a device or OpenNT service. |

The original `devices.cc` evidence defines the required no-device behavior:
its built-in default read handler returns `0xffffffff` and its built-in
default write handler ignores the write, specifically so the emulator can
operate without an `unmapped` device.  M0 preserves that Bochs behavior rather
than inventing a project-specific I/O fault.  The finite budget, HLT, and real
CPU exceptions remain its only stop reasons.

This is still a closure gate.  The first half of the historical
`bx_devices_c::init` allocates and installs those default handler tables, but
the same method later reads `SIM`, starts virtual timers, and loads plugins.
The future backend-local lifecycle unit therefore needs a narrowly copied
`init_m0_port_space` member: only the original default-handler/table setup,
with no `SIM`, timers, plugins, registered ports, or DOS-visible device.  Its
symbol/object ledger must be checked before it is added.  It may not call the
historical full `init` merely for convenience.

`iodev/devices.cc` was also compiled once with `/Gy` as
`artifacts/analysis/m0/devices-gy.obj`.  Its symbols confirm that
`inp`, `outp`, both default handlers, and both default-handler registration
methods occupy independently inspectable function sections.  The historical
product `init` is a separate section.  This makes the intended split
mechanically testable: an M0-specific port-space initializer may retain only
the default table setup and link-time evidence must show no references to
`SIM`, virtual timers, plugin loading, or a concrete device initializer.

## M0 Memory Closure, Pass 8: Reset Window And ICache State

The original `BX_MEM_C::init_memory` supplies three M0-relevant resources:

1. RAM allocation in 1 MiB blocks;
2. Bochs's fixed ROM backing (`BIOSROMSZ` plus `EXROMSIZE`), where the reset
   vector `0x000ffff0` is fetched; and
3. the memory-handler table used by checked physical access.

It also performs two prohibited product operations: reads the I440FX/PCI
parameter from `SIM` and invokes `register_state`.  The M0 memory initializer
must retain only (1)--(3), force PCI/SMRAM disabled, and copy the bounded
caller image only into the exact reset-ROM window and declared RAM range.
It must not use `load_ROM`, because that path is a file-backed firmware
loader, not a caller-owned byte image.

The ICache write-stamp table is a separate required global from
`cpu/icache.cc`.  Its original constructor allocates its fixed stamp array and
calls `resetWriteStamps`; `flushICaches` resets the same state during CPU
reset.  This component is CPU cache maintenance, not a device.  It must be
included in the lifecycle/link ledger, with its normal constructor retained.

The remaining memory work is therefore specific and bounded: trace the
minimal `init_memory_m0` member's symbol closure (allocation, handler-table
initialization, reset-ROM byte copy, and cleanup) and prove that no ROM file
loader, `SIM`, state-registration, PCI, VGA, or SMRAM initializer survives.

## M0 Dispatch Boundary, Pass 9: HLT And Fault Return

The historical `BX_CPU_C::HLT` marks `activity_state` halted, sets an async
event, and continues through the historical loop.  Its next
`handleAsyncEvent` enters a wait-for-event path; it is not a controlled return
to a caller.  Likewise, `cpu_loop` receives `longjmp` from the CPU exception
path and continues execution after updating its historical instruction count.

The M0 owned bounded entry must therefore use the original per-instruction
ordering but have two deliberate, local exits:

| Boundary | M0 disposition | Required ordering |
| --- | --- | --- |
| CPU exception returns through `setjmp` | `faulted` | Capture original Bochs CPU state after the exception transfer; do not re-enter the historical unbounded loop. |
| Executed instruction leaves `activity_state == BX_ACTIVITY_STATE_HLT` | `halted` | Retire/account for HLT once, then return before historical `BX_SYNC_TIME_IF_SINGLE_PROCESSOR` and `handleAsyncEvent` wait processing. |
| Completed ordinary instruction reaches supplied budget | `budget-exhausted` | Retire/account once, then return before a further fetch or historical timing loop. |

This is a backend execution-control shim only: it neither changes opcode
handlers nor defines an OpenNT callback.  It also makes the no-device clock
decision simple for M0: no historical PC timer tick is performed in the
bounded entry.  The `bx_pc_system` null-state object remains only where the
retained CPU code requires its fields; no timer/device event is scheduled.

## M0 Object Census, Pass 10: PC-System References

The existing inspected CPU objects were scanned with `dumpbin /symbols`; the
CPU object units with a direct `bx_pc_system` undefined reference are:
`cpu.o`, `event.o`, `exception.o`, and `paging.o`.  Thus the PC-system state
object is required by the generic CPU component even after M0's new bounded
entry removes the normal timing call.  This does not admit PC devices:

- `bx_pc_system_c` constructor and `initialize` establish null-clock state
  without `SIM` or a scheduled device;
- `pc_system.cc` port I/O methods are separate functions and must be removed
  by `/Gy` plus dead-code linking unless another admitted call proves them
  necessary; and
- `register_state`, `exit`, and all `bx_devices` calls remain rejected.

The future link-map gate must show `bx_pc_system` storage and only the
reachable null-clock/event helpers, never the `inp/outp` delegation methods.

## Rewrite-Boundary Finding: Exception Triple-Fault Path

This finding activates the T95 rewrite-stop rule; no M0 source or build target
may be added until it is decided.

An object-symbol scan of the retained level-3 CPU objects shows direct product
references from `event.o` and `exception.o` to `bx_devices`; `exception.o`
also directly references `SIM`.  Source inspection locates the cause in
`cpu/exception.cc`: its triple-fault branch reads
`BXPN_RESET_ON_TRIPLE_FAULT` through `SIM`, reads a CMOS value through the
device macro, and calls `bx_pc_system.Reset(BX_RESET_HARDWARE)`.

Generic CPU instruction and paging paths rely on `exception`, so ordinary
dead-code linking cannot remove this branch from the exception function.
There are only two currently visible ways to proceed:

1. admit a narrowly configured Bochs `SIM`/CMOS/product-support path solely
   to preserve the historical triple-fault behavior; or
2. alter the M0 exception/triple-fault path to return the typed M0 `faulted`
   disposition.

The first risks importing the forbidden configuration/device framework.  The
second changes core Bochs exception semantics.  Either route crosses the
project’s declared rewrite boundary, so work is intentionally paused for user
direction rather than selecting one implicitly.

### Read-only scope refinement

Further source inspection narrows, but does not resolve, option 1.  The
`DEV_cmos_get_reg` macro dispatches through `bx_devices.pluginCmosDevice`; the
ordinary `bx_devices_c` constructor already points that field at `stubCmos`,
so a real CMOS device need not be initialized for the exceptional diagnostic
read.  The only observed `SIM` request on this path is the
`RESET_ON_TRIPLE_FAULT` Boolean.  Satisfying it appears to require the Bochs
parameter-tree Boolean type plus a minimal simulator-interface implementation
that returns `false`; it does not yet prove that a full text/GUI configuration
front end is necessary.

This remains a user decision: admitting this parameter-model fragment may
preserve the original exception behavior without real device activation, while
changing exception handling would not.  No implementation was added during
this refinement.

An attempted single-object `/Gy` inspection of `gui/paramtree.cc` stopped at
the already-recorded Bochs 2.6/MSVC literal-token diagnostic
(`"..."FMT_LL`).  No compatibility patch was made: fixing it before deciding
whether the parameter model is admissible would resume the prohibited
build-repair loop.  The failed inspection is retained only as toolchain
evidence, not as a reason to broaden the M0 closure.

## Follow-up

1. Close the lifecycle ledger for CPU, checked memory, logging, and the
   no-device clock; specifically separate original construction state from
   `SIM` parameter/state registration.
2. Decide the backend-local bounded dispatch and private reset-window contract,
   including the disposition of asynchronous events and PC time.
3. Only after those two ledgers are stable, introduce an owned minimum build
   target and a build shim limited to its admitted objects.
4. Define the adapter-facing fixed-width M0 request/response records and their
   negative tests without naming an OpenNT service.
