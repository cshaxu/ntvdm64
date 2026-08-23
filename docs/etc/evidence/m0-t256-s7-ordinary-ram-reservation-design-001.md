# M0 T256 S7 — Ordinary-RAM Reservation Design

## Selected contract

Capacity alone is insufficient.  Add a separate, copied session interval:

```text
reserved_memory_base
reserved_memory_bytes
```

Both are zero by default, meaning no reservation exists.  A nonzero interval
must be 64 KiB aligned, start at or above 1 MiB, fit entirely within the
already configured guest-RAM capacity, and be preflighted as ordinary RAM at
stage initialization.  This is a machine-composition declaration, not a
DPMI allocation request.

## Why explicit instead of automatic high memory

Automatically treating `[1 MiB, guest_memory_bytes)` as free would be a
guess: a later BIOS/guest image/device composition may use any part of it.
An explicit interval has a single owner at session admission, prevents guest
image collision, and makes zero-reservation current profiles unchanged.

## Next ABI shape

The prospective mantle action is selector-blind and lifecycle-scoped:

- input: bounded byte count and power-of-two alignment;
- result: 32-bit guest physical/linear address and an opaque non-pointer
  allocation ID;
- release: opaque ID only;
- allocation source: only the declared interval, after ordinary-RAM
  preflight; and
- reset: releases every record with the minimal machine.

It will not know DPMI, xmem, BOPs, selectors, descriptors, page tables, host
memory or host pointers.  The future bx-vdm DPMI mirror alone may request the
source-required 64 KiB alignment and translate an opaque ID into its own
source-derived block record.

## Explicit non-goals

No automatic high-memory claim, no paging, no LDT change, no raw backing
pointer, no host virtual allocation, and no existing low-memory shim rewrite.
The configuration field and the reservation action are separate S tasks so a
future provider cannot bypass admission geometry.
