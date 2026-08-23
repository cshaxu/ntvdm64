# M0 T246 S1 — SoftPC BOP 02/06 owner map

## Question

Does the first post-COMMAND `BOP 06` stop require a Bochs intrusion or a new
machine implementation, or is an already admitted original-owner route merely
absent from the active generic ingress?

## Original owner map

OpenNT SoftPC `base/bios/bios.c` maps:

| BOP | Original handler | Required observable work |
| --- | --- | --- |
| `02` | `base/system/unexp_nt.c:unexpected_int` | Read PIC ISR/IMR ports, acknowledge/mask unexpected interrupt as required, and publish interrupt cause at BDA `0040:006B`. |
| `06` | `base/system/illegalp.c:illegal_op_int` then `unexpected_int` | Read interrupted `CS:IP` from `SS:ESP`, increment stored IP (and carry CS at `FFFFh`), then perform the same PIC/BDA sequence. |

The historical diagnostic/UI call in `illegal_op_int` is NT4 product-shell
behavior. The first non-debug profile retains the concrete stack/PIC/BDA
mechanics and does not fabricate that product shell.

## Current composition map

`src/opennt/local/compat/host/opennt_int06_provider_v1.*` is the named
source-derived state machine for exactly those two original operation streams.
`src/bx-vdm/bx_ntvdm_startup_machine_interrupt_v1.c` already adapts it using
only existing typed mechanics:

- preflight checked writes for the stack/BDA effects;
- checked RAM read/write actions for the saved return address and BDA;
- default port-space actions for `20h`, `21h`, `A0h` and `A1h`;
- a typed `RIP+3` resume or controlled stop.

`tests/opennt/opennt_int06_provider_v1_test.c` verifies the software,
hardware/slave-cascade and unexpected suffix sequences; the formal manifest
also contains `t225-s23-int06-transaction-fixture` for the existing typed
transaction path.

The active provider registry,
`src/bx-vdm/bop/dem_v2_composition_bridge.c`, enumerates source-mirrored
SoftPC `12/15/66/C8/17`, config, keyboard, DEM, COMMAND, XMS and `59`; it does
not include the compiled `startup_machine_interrupt_v1_dispatch`. Therefore a
real `C4 C4 06` falls through the generic bridge and repeats. The disabled
historical `bx_ntvdm_machine_composition_v2` route is not an eligible recovery
path: it depends on a retired build gate and direct global SoftPC handler
callbacks.

## Decision

Existing typed mechanics are sufficient. S2 may add one explicit bx-vdm
generic-UD bridge for the whole original `02/06` pair and translate its
already-typed result to the existing outcome ABI. No bx-core/bx-mantle source
change, device enablement, DOS semantic or Bochs exception is required.
