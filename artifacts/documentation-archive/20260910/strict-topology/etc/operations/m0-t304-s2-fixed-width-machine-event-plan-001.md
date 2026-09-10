# M0 T304 S2 — fixed-width machine-event ingress

## Objective

Close the mechanical half of the kernel-VDM replacement boundary: one
versioned, fixed-width copied #UD machine-event ABI between the Bochs overlay
and `adapter-mvdm-host-in`.

## Required work

1. Make one public event/outcome layout authoritative in
   `adapter-mvdm-host-in`; require x86/x64 size and offset assertions.
2. Keep the overlay-private record private and use `adapter-bochs` as the
   bounded opaque-copy conversion boundary. The overlay cannot include an
   upper adapter header; therefore the public layout is authoritative only
   outside the mirror, and the conversion carries no provider, selector or
   guest object.
3. Retain source-compatible opaque callback registration via `adapter-bochs`.
4. Test malformed records, copied instruction window/state, unbound decline,
   and resume/stop/pending validation on both targets. The wrapper's bounded
   local-copy lifetime is inspected as a boundary property rather than treated
   as a provider capability.

## Explicit exclusions

No BOP selector decoding, `VDMEVENTINFO` dispatch, EIP advancement policy,
DEM/COMMAND/XMS/DPMI behavior, I/O/IRQ/page-fault event support, device
enablement, or kernel-VDM recreation.

## Exit

One formally-tested selector-blind *public* ABI is the CPU-to-MVDM ingress
contract. The next owner package may consume it to form a source-shaped BOP
event without reopening the CPU/Bochs boundary.
