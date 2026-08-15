# M11 Core Boundary Migration Work Order

Status: planning and research only. This work order does not authorize source
changes, guest binaries, external runtime dependencies, DLL packaging, or a
runnable external wrapper. It uses neutral names for any external runtime.

## Objective

Evolve the current shared core so that:

* NXVM remains a configurable, bootable whole-machine product;
* NXVDM can later construct a direct prepared guest environment without
  inheriting PC/AT boot policy; and
* a separate external wrapper can eventually bind a user-supplied runtime
  without putting runtime-specific ABI or assets into the releasable project.

The immediate deliverable is a clean source boundary under the current
single-executable build. Future DLL artifacts are a packaging consequence of
proven source boundaries, not a first-phase implementation requirement.

## Target Topology

```text
src/core/       -> future core.dll
src/vm/         -> nxvm.exe       -> core
src/mantle/     -> future mantle.dll -> core
src/vdm/        -> nxvdm.exe      -> mantle -> core
external wrapper -> ntvdm.exe     -> mantle -> core
```

`src/mantle` is not created by this work order. It is admitted only after two
real consumers prove a shared, releasable, policy-free VDM host mechanism.

## Non-Negotiable Boundaries

* `core/machine` owns product-neutral mutable guest mechanics. It has no DOS,
  PC/AT product, external-runtime, filesystem-policy, or host-UI semantics.
* `core/platform` owns narrow host capability contracts and shared providers.
  It does not mutate guest state and does not define DOS paths or drive policy.
* `core/product` retains generic tooling only. NXVM Console and NXVDM UX remain
  product-owned. Existing debug-target contracts are reused before any new
  debug abstraction is proposed.
* `src/vm` keeps PC/AT topology, BIOS/image generation, reset-state selection,
  media choice, mount/eject policy, and UI.
* `src/vdm` keeps the independently implemented DOS runtime and its product
  policy.
* The external wrapper keeps runtime-specific bootstrap, selectors, guest data
  structures, BYOB validation, DOS namespace, and host file/device policy.
* No task below may add protected guest files, a default external runtime, or
  a source dependency on external historical code.

## Admission Labels

| Label | Meaning |
| --- | --- |
| Now | Approved for planning and implementation packet creation after normal repository intake. |
| Gate | Research/design only until named evidence satisfies its admission condition. |
| Deferred | Do not plan implementation work yet. |

## Work Sequence

### M5 T243 S1: Freeze Baseline And Produce Gap Map

**Status:** Complete - planning task.

**Purpose:** turn the architectural tree into a file-level migration map before
moving any source.

**Work:**

* inventory the current public `core/machine`, `core/platform`, and
  `core/product` contracts;
* identify exact VM owners for CMOS/RTC, FDC/HDC, firmware generation, media
  backend, host providers, runner/control, and UI;
* classify each candidate as unchanged, `VM -> core`, core extension, new
  capability, gate, or deferred;
* record the retained NXVM boot/device/Console regression commands that every
  subsequent implementation task must run.

**Must not:** move source, change a public API, create mantle, or infer a
second consumer.

**Acceptance evidence:** reviewed ownership table with file paths, dependency
edges, and a before/after test matrix; no source diff.

### M5 T243 S2: Checked Guest-Memory Contract Audit And Extension

**Status:** Active - first core-machine implementation candidate.

**Purpose:** give independent composition and approved execution facilities a
safe way to copy guest state without raw guest pointers or product-private CPU
layouts.

**Work:**

* audit existing real/linear memory helpers, their validation behavior, and
  ownership/thread assumptions;
* define only checked physical read/write/copy and a physical-range query with
  `READ`/`WRITE` access, never a fetch permission;
* make zero length invalid and any address-plus-size overflow or out-of-route
  range inaccessible without wrapping;
* route query, read, and write through one shared frozen device-provider/RAM
  resolver, not a capacity-only RAM check or a parallel mapping path;
* return only neutral `ORDINARY_RAM` or `PROVIDER` route classification, never
  a provider identity or mapping internal;
* make failure results deterministic and preserve normal CPU fault behavior;
* add focused boundary/fault tests and retain the whole NXVM matrix.

**Must not:** expose internal RAM buffers, let platform mutate guest memory,
add DOS structures, or add an external-runtime callback.

**Acceptance evidence:** public contract review, focused out-of-range and
mapping tests, current GCC gate, full CTest matrix, and required developer
artifact under repository policy.

### M5 T244: Registered Undefined-Instruction Transition Facility

**Status:** Planned - design after M5 T243 S2; implementation candidate
immediately after its contract and executor tests are approved.

**Purpose:** provide the generic execution interception required by the
external wrapper without introducing runtime-specific instruction encodings or
service semantics into core.

**Required generic shape:** consumer-registered 1--15 byte undefined-
instruction patterns with mode restrictions and frozen conflict rules.
Registration is allowed only while `INITIALIZED` and becomes immutable with the
execution-provider freeze. Identical patterns and any prefix overlap are
rejected, so no priority or longest-match rule exists. Core matches only the
physical bytes it captured for the faulting instruction fetch, at the
architected invalid-instruction boundary before default fault delivery, never a
consumer-supplied linear address. Core alone consumes the registered pattern
length. A handler receives copied state
and may return only `unhandled`, `handled-resume`, `stop`, or `fault`.
`handled-resume` carries only an explicitly validated CPU-state patch. The
initial writable set is `EAX`, `EBX`, `ECX`, `EDX`, `ESI`, `EDI`, `EBP`, plus
arithmetic-status `CF`, `PF`, `AF`, `ZF`, `SF`, and `OF`; it excludes `EIP`,
`ESP`, all other FLAGS, segments, descriptor caches, control/debug registers,
FPU state, and guest memory. Core commits the complete permitted patch and
fixed IP advance atomically, or commits neither. Guest-buffer writes require a
separate future bounded transactional contract. The handler cannot set
arbitrary IP or change real/protected/V86 mode.

**Rationale:** direct source evidence establishes an external wrapper as a
concrete consumer of a controlled invalid-instruction transition. This is a
new executor capability, not an extraction of VM product code; it may proceed
with that consumer because core must own any safe interception of its own CPU
execution. Its genericity is protected by the contract: it must avoid fixed
opcode sequences, selector namespaces, and guest-service semantics.

**Must not:** implement an arbitrary instruction override engine, a code patch
hook, a raw guest-memory callback, a named historical selector protocol, a
handler-owned CPU mode transition, or transition-owned guest-memory writes.
Any mode transition remains in the core CPU's formally supported instruction,
exception, and IRET semantics.

**Acceptance evidence:** a source-level contract review; registered-pattern
conflict, IP-consumption, state-write-mask, and atomic commit/rollback tests;
checked CPU/guest-memory access tests; and no change to NXVM's default
real-machine path when no handler is registered.

### M5 T245: Generic ROM And Read-Only Mapping Contract

**Status:** Planned - second core-machine implementation candidate.

**Purpose:** add generic immutable byte images to the one existing frozen
checked device-memory route, without a parallel ROM mapping or fetch path.

**Work:**

* define an immutable byte-image provider registered only during `INITIALIZED`;
* reject every provider/image/RAM routing conflict at registration and freeze it
  with existing device-memory routing;
* ensure ordinary guest read and instruction fetch use that same route, image
  writes fault rather than falling through to RAM, and no host-side raw pointer
  enters core;
* preserve immutable image bytes across reset while normal CPU/device reset
  behavior remains unchanged;
* adapt the existing VM profile firmware path as a consumer without moving the
  BIOS generator or BIOS service contents out of `src/vm`.

**Must not:** put BIOS bytes, BDA/IVT contents, interrupt services, reset
vector policy, ROM catalogues, or external media validation in core.

**Acceptance evidence:** routed read/fetch/write-fault/overlap/reset tests,
unchanged NXVM ROM boot regressions, and focused profile/composition ownership
review.

### M5 T246: Validated Initial-State And Entry-Plan

**Status:** Planned - depends on M5 T243 S2 and M5 T245.

**Purpose:** make one generic post-reset start contract usable by both a
conventional firmware boot and a direct prepared guest environment.

**Required model:**

1. Topology/image references register only while `INITIALIZED` and freeze with
   the existing machine routing.
2. `core_machine_reset` restores only a clean generic machine baseline and
   reaches its `STOPPED` boundary.
3. Product composition validates an immutable initial-state/entry-plan against
   that frozen topology.
4. Core atomically applies the validated plan at the `STOPPED` boundary.
5. The product begins execution through the selected entry.

**Work:**

* specify a real-mode architectural register state and entry point only; core
  derives real-mode hidden state and admits no protected-mode state, descriptor
  cache, or CRx field;
* permit prepared copies only to ordinary RAM ranges proven writable through
  T243's physical query. Validate every preload before committing any write;
  they may not target a ROM/device provider or use A20 or any logical/linear
  translation;
* distinguish reset-state plans from direct prepared-state plans without
  assigning either a product name in core;
* make plan validation atomic: validate every field and every preload copy as
  writable ordinary RAM before changing registers or RAM. On any validation
  failure, leave reset CPU state, RAM, and frozen mapping unchanged; no topology
  registration occurs at this stage;
* adapt NXVM's reset-state plus firmware image as the first consumer.

**Must not:** encode a BIOS image builder, BDA/IVT byte layout, DOS loader,
external runtime bootstrap, or a second reset mode in core.

**Acceptance evidence:** plan rejection/rollback tests, reset-state regression
through NXVM's existing boot path, one synthetic prepared-state test with no
runtime-specific data, and full current regression matrix.

### M5 T247: Narrow Shared Host Capability Audit

**Status:** Planned - depends on M5 T246. Before that dependency is complete,
this item is evidence collection only; it must not add a platform contract or
provider implementation.

**Purpose:** prevent `core/platform` from becoming a hidden host-services
object while extracting genuinely shared providers from VM where proven.

**Admitted design targets:**

* copied normalized input events;
* copied presentation sinks;
* wait/monotonic host-clock support that never sets guest time; and
* an optional byte-stream contract only after NXVM and NXVDM demonstrate the
  same semantics.

**Conditional target:** an opened random-access media/backend contract after
shared controller use is demonstrated.

**Explicitly not admitted:** generic file/directory APIs, drive visibility,
DOS paths/namespaces, mount/eject policy, media selection, host wall clock as
guest time, and product UI.

**Acceptance evidence:** per-capability two-consumer proof, provider ownership
map, thread/lifecycle contract, and no platform-to-guest mutation edge.

### Mantle Admission Gate (Unallocated)

**Status:** Gate.

**Purpose:** decide whether a real shared VDM host mechanism exists above core.

**Admission condition:** NXVDM and the external wrapper both use the same
releasable mechanism with identical ownership, lifecycle, and error semantics.
It must not contain product UX, DOS namespace, runtime-specific ABI, guest
asset assumptions, or BYOB policy.

**Candidate examples only:** a single-session VDM runtime envelope or a
prepared-state composition helper. These are not admitted merely by naming
them.

**Acceptance evidence if admitted:** two concrete call sites, dependency audit,
public contract, and a decision whether the source-level module is mature
enough to package later.

### Controller Extraction Gate (Unallocated)

**Status:** Deferred.

**Scope:** CMOS/RTC, FDC, HDC, and any other VM machine controller.

**Admission condition for each controller:** two real consumers plus a
controller contract with no PC/AT default ports, IRQ/DMA routes, CMOS defaults,
media file paths, image geometry policy, or mount/eject behavior.

**Current disposition:** retain all controller implementations in `src/vm`.
Core's existing PIC, PIT, DMA, memory, execution, and timing remain the
unchanged foundation used by the VM controllers.

## Implementation Ordering

```text
M5 T243 S1
  -> M5 T243 S2
  -> M5 T244
  -> M5 T245
  -> M5 T246
  -> M5 T247 (design, then individual admissions)

The mantle and controller-extraction gates are independent; do not schedule
them merely because the earlier items completed.
```

## First Implementation Packet

The first source-changing packet should be **M5 T243 S2 only**. It is small enough
to audit, supplies checked guest-memory access needed by the next packet, and
does not commit the project to mantle or controller migration. **M5 T244** is
the immediate second packet: it is the earliest core capability directly needed
by the external-wrapper objective.

Before opening that packet, attach:

* the M5 T243 S1 ownership/gap table;
* the existing machine-memory contract inventory;
* an explicit list of checked operations needed by M5 T246;
* focused tests and the full retained NXVM verification command; and
* a negative proof that no DOS, external-runtime, or host-policy term enters
  the core API.

## Change-Control Rules

* One source-changing packet at a time, with normal repository task tracking.
* Each source move uses `git mv`, preserves ownership, and carries focused
  tests before a broader regression run.
* No task creates a DLL as a shortcut around an unclear source boundary.
* No task turns a missing runtime-specific feature into a generic core API
  without the stated admission evidence.
* A later external-wrapper experiment may guide a gate review, but its files,
  data, logs, and dependencies remain outside the releasable repository.
