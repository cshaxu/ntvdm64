# M0 T253 S1 — selector-blind async-delivery mechanics map

## Decision

The existing native 8259 PIC supplies the mechanical half of the original
Redirector completion path. The original guest deliberately selects slave-PIC
IRQ 14, whose normal default vector is `76h`; it does not require arbitrary
CPU-vector injection or a bx-core change.

S2 may add a lifecycle-checked, selector-blind mantle request for a physical
IRQ. It must use the existing PIC `raise_irq()` path. bx-vdm decides only
whether a copied completion is ready; original guest `int5c.asm` owns ANR,
stack and IRET mechanics.

## Original source map

| Source | Fact | Consequence |
| --- | --- | --- |
| `redir/int5c.inc:31..39` | `NETWORK_INTERRUPT equ 76h`; comment identifies synthetic IRQ 14. | Guest expects a maskable hardware interrupt, not a host far call. |
| `redir/redir.asm:457..477` | Installs `IntNetworkHandler` in IVT `76h`. | Guest resident code owns IVT publication and handler address. |
| `redir/int5c.asm:267..413` | Switches stacks, calls `57:26`, interprets returned registers, calls ANR, acknowledges `57:30`, IRETs. | No worker/core/mantle code may receive a BOP number, ANR, guest pointer, selector or callback. |
| `vdmredir.h:81..89` | `NETWORK_ICA=ICA_SLAVE`, `NETWORK_LINE=6`. | Slave IRQ6 / physical IRQ14 is the source-defined trigger. |

## Native machine map

| Native source | Fact | Boundary result |
| --- | --- | --- |
| `bx-mantle/pic.cc:656..680` | `bx_pic_c::raise_irq(unsigned)` raises a native PIC line. | Existing device behavior; no CPU decoding change. |
| `pic.cc:817..821,835..876` | Slave IRQ cascades via master IRQ2; `IAC()` returns slave offset plus IRQ. | Normal `70h + 6` produces `76h`; PIC masks/priorities remain authoritative. |
| `bx-core/cpu/event.cc:101..131,249..302` | CPU consumes `DEV_pic_iac()` only at an interrupt boundary, respecting IF/inhibit state. | Do not call `BX_CPU_C::interrupt()` or debugger-only `dbg_force_interrupt()`. |
| `bx-mantle/bx_ntvdm_minimal_machine.cc:70..82` | Minimal machine owns initialized PIC via `pluginPicDevice`. | A mantle wrapper can be lifecycle-checked without widening bx-core. |
| `bx_ntvdm_mechanical_action_v1.*` | Existing action ABI transports ordinary RAM only. | Use a distinct no-payload IRQ request, not a fake RAM action. |

## Rejected candidates

- `dbg_force_interrupt(vector)`: debugger-only and bypasses normal PIC/IF behavior.
- bx-vdm IVT writes or ANR-frame construction: violate original guest ownership.
- a new core CPU interrupt export: redundant to the extant PIC/CPU path.

## S2 contract

Expose only `request_physical_irq(irq)` while minimal-machine lifecycle is
active; accept `0..15`, reject inactive/out-of-range calls, and test accepted,
masked and inactive cases. The ABI carries no selector, vector purpose, BOP,
DOS, VDM, Redirector, callback, guest pointer or host handle.
