# T225 S12 Adapter-Owned BOP Observation Result

## Question

Can the current CLI report every reached BOP-shaped C4 C4 invocation without
implementing a BOP service, modifying a guest image, or adding BOP semantics
to Bochs or the mantle?

## Implementation

The existing fixed 128-record bx-vdm sequence observer is now explicitly
selected by the CLI-only --observe-bop-sequence flag.

The observer remains default-off. When enabled, the CLI activates it before
the engine worker begins, copies its bounded record after the normal worker
return, prints the record count/overflow state and each copied record, then
disables and clears it. The engine contract, machine-stage contract, bx-core
and bx-mantle do not change.

Each record carries only:

- fault CS:EIP;
- selector and, for service-shaped selector families, service byte;
- service-shape flag;
- generic bridge disposition.

It carries no general registers, guest memory, provider identity, host handle,
DOS interpretation, or unbounded event stream.

The generic bridge now invokes the observer for a declined C4 C4 event before
returning the unchanged native decline. The synthetic declined outcome has
only the existing PASS_THROUGH disposition. Accepted routes retain their
existing observer call and their existing provider/CPU behavior.

## Source-recovery disposition

This is an adapter observation mechanism, not recovered host-service behavior.

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Not applicable: this modern post-run diagnostic is not a historical OpenNT provider. |
| Smallest existing seam | Selected: reuse the existing bx-vdm bounded sequence record and generic bridge, exposing it at the CLI after normal engine return. |
| External-code intrusion | Rejected: no Bochs/bx-core/bx-mantle code changed. |
| New behavior | Limited to default-off CLI selection/rendering and recording a declined existing bridge fact. No service semantics were authored. |

## Focused regression

The formal Ninja fixture
t225-s12-bop-decline-observation-fixture constructs one C4 C4 50 11 event
while both composition handles deliberately decline it.

It verifies:

- generic bridge returns decline;
- caller-supplied outcome disposition remains unchanged;
- the enabled observer copies exactly one record;
- record fields are CS=0070, EIP=00001234, selector=50, service=11,
  has-service=1, disposition=PASS_THROUGH;
- disabling the observer prevents later copy.

This proves the listener covers unconnected invocation without converting it
into a provider route or a CPU result.

## Native verification

A fresh formal Ninja/MSVC x64 /MT graph was generated at
build/M0-T225-S12/ninja-r1. It built 303 edges, including the focused fixture
and ntdos64-native.exe. The fixture exited zero and Ninja subsequently has no
work to do.

Using the source-built S10 primary stage:

| Run | Result |
| --- | --- |
| Direct, 1000 ticks, flag absent | Normal budget terminal/exit 4; no bop-sequence output. |
| Direct, 1000 ticks, flag present | bop-sequence count=0 overflow=0; normal budget terminal/exit 4. |
| Readonly, 1000 ticks, flag present | same zero-count report and normal budget terminal/exit 4. |
| Direct, 10000 ticks, flag present | same zero-count report and normal budget terminal/exit 4. |
| Duplicate flag | usage rejection; exit 2. |
| Overlay plus flag | usage rejection; exit 2. |

The zero native count is a positive bounded observation: at these budgets the
primary execution did not reach any C4 C4 BOP. It does not prove that the
catalogue has no other BOPs, that 50:11 failed, or that a provider is absent.

## Boundary

S12 establishes observation and classification only. A trace hit must still
be mapped to its original owner package before implementation. The next
package may use this listener to locate the first continuity boundary at a
larger admitted internal budget, but must not turn a newly observed selector
into an immediate leaf patch.
