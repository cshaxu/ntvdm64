# Original DPMI and XMS contract restoration

## Owner request and status

On 2026-09-12 the owner requested an independent candidate T for D35-D37 at
the head of [Queue](../states/QUEUE.md), before broker restructuring. This
proposal records the two-stage restoration package and does not allocate a
numeric T or change the active [Status](../states/CURRENT.md) packet.
The [original-owner audit](../etc/evidence/opennt-replacement-audit-20260912.md)
is the input, not proof of a runtime failure or an accepted repair. Reconcile
these rows with the current audit/restoration task at admission to avoid
duplicate implementation ownership.

## Objective and boundaries

Restore original DPMI/XMS allocation, memory movement and mode-transition
contracts with minimal mirror diff and no autonomous duplicate algorithms.
Original MVDM owners remain in `mvdm-host`; any separately admitted non-MVDM
semantic carrier belongs under `opennt-host` at its original-relative path.
Follow the [source policy](../etc/operations/policy/source-policy.md) and
[architecture rules](../rules/ARCHITECTURE.md). Bind only finite unavailable
operations after proving the original owner and complete outgoing boundary.

The worker remains Win32/x86 CCPU40, launched through
`ntvdm32.exe <binary>`. Broker IPC, BaseSrv policy, process splitting, new CPU
instruction semantics, ring-0 execution, trap installation and CPU30 revival
are excluded. Neither complete WRITE recovery nor broker implementation is a
prerequisite for this bounded package. Mandatory in-scope contracts still
require verification; naming a missing boundary is not automatic closure.

## Proposed sequential stages

These are proposed S scopes, not active briefs. At admission establish the
working plan and one active brief using the execution rules.

### S1 - Shared XMS/DPMI memory contract (D36-D37)

Review and recover `xms.486/xmsblock.c`, the original `xmsmemr.c` and
`i386/xmsmem86.c` callback contracts, and `dpmi32/dpmimemr.c` provider
selection together. Revalidate exact source paths and selected graph before
implementation. Original SA allocation is reused already; the problem is
changed selection and memory ownership, not an invented SA allocator.

Restore or source-prove allocation/free/reallocation/query routing. Determine
which owner reserves the extended address range and which subdivides it.
Do not translate genuine allocation failure into STATUS_NOT_IMPLEMENTED to
force fallback. Allocation and capacity queries must describe the same owner.

Restore the distinct original block-copy and overlap-safe move contracts,
with bounded guest access below the original algorithms. Review and retire
the duplicated frame parser and chunk-copy policy in `mvdm_xms_memory.c`
where superseded. Resolve partial-write failure and cancellation semantics;
do not assume cancellation undoes already copied chunks.

Audit commit/decommit callbacks as part of this same lifecycle. Zeroing,
committing, decommitting and original backend no-op behavior are distinct;
select the source-backed contract required by the actual CCPU40 composition.
Do not blindly activate either native-x86 virtual-memory callbacks or a RISC
stub, and do not invent compiled-cache work for an original empty CCPU
overwrite-notification provider. Resolve only the reached U07 memory-lifetime
boundary, not an unrelated generic session redesign.

Acceptance: allocation/exhaustion/free/reallocation and free-space query
consistency; disjoint ownership and no double allocation; copy counts and
overlap cases according to each original contract; range/overflow checks;
commit/decommit/recommit content and availability; injected partial failures;
and affected original XMS/DPMI workloads through the formal x86 CCPU40 graph.

### S2 - Original mode-transition frames (D35)

Recover original `dpmi32/i386/dpmi386.c` frame interpretation and owner shape
instead of maintaining duplicate algorithms in `dpmi32/modesw.c`. Bind
passive saved CONTEXT fields to active CCPU state deliberately: segment
setters load hidden caches, so PE/CPL/segment ordering cannot be copied as
ordinary assignment order. Account explicitly for each original VDM RI/RM
state update, guest stack ownership, register width and return continuation.

Separate this frame restoration from U04 platform initialization. Current
TSS-slot construction, IDT inference/restoration, shared GDT/LDT backing and
TEB/stack projections are not established as original frame logic. For every
one reached by the proposed transition, prove an original owner and finite
binding, or retain a named blocker requiring owner disposition. Do not silently
bless or delete those mechanisms as part of restoring frame offsets. An
unrelated platform reconstruction or instruction-core change requires scope
review, not a trace-driven patch inside this S.

Acceptance: source-paired real/protected transition frames, captured values
before mode change, CS/SS/DS and other affected caches, PE/CPL and descriptor
state, stack/return round trips, malformed or unavailable state behavior,
and correct BOP instruction advancement. Record the same-baseline DOSX/WOW
bootstrap frontier and COMMAND/EDIT regression. A successful frame fixture
does not prove full WOW/WRITE execution or complete U04 correctness.

## Delivery and exit evidence

Freeze actual source, media and executable identity at admission. Retain the
original owner/four-rung recovery evidence, all selected bindings, provider
graph, positive/negative tests and per-stage regression results. Report
removed autonomous code, reverted mirror diff, imported original code and
necessary retained bindings separately. No existing DOSX failure is attributed
to these rows without runtime evidence.

All build/fixture products remain under `build/`. Publish only a verified
formal x86 executable at `build/output/ntvdm32.exe`, deliberately copied to
`O:\ntvdm64\ntvdm32.exe` for acceptance; runtime logs belong under
`O:\ntvdm64\logs\`. Each stage follows review, governance checks, commit and
push under [Execution](../rules/EXECUTION.md). Complete this bounded worker
restoration before broker work; do not make all remaining WOW recovery an
implicit prerequisite or use broker changes to hide unresolved worker state.
