# M0 T304 S2 — fixed-width machine-event ingress

## Objective

Close the mechanical half of the kernel-VDM replacement boundary: one
versioned, fixed-width copied #UD machine-event ABI between the Bochs overlay
and `adapter-mvdm-host-in`.

## Required work

1. Make one public event/outcome layout authoritative in
   `adapter-mvdm-host-in`; require x86/x64 size and offset assertions.
2. Replace the overlay-private duplicate layout with a mechanical conversion
   boundary that does not include a provider, selector or guest object.
3. Retain source-compatible opaque callback registration via `adapter-bochs`.
4. Test malformed records, copied instruction window/state, unbound decline,
   resume/stop/pending validation and no retained caller data on both targets.

## Explicit exclusions

No BOP selector decoding, `VDMEVENTINFO` dispatch, EIP advancement policy,
DEM/COMMAND/XMS/DPMI behavior, I/O/IRQ/page-fault event support, device
enablement, or kernel-VDM recreation.

## Exit

One formally-tested selector-blind ABI is the sole CPU-to-MVDM ingress record.
The next owner package may consume it to form a source-shaped BOP event without
reopening the CPU/Bochs boundary.
