# M0 T256 S6 — Guest-Memory Geometry Contract Design

## Admission

Admit a copied session geometry field, not a DPMI allocator.

The field is `guest_memory_bytes`, carried from the native CLI through the
fixed-width bx-vdm engine request to the selector-blind machine-stage request.
It accepts 1 MiB through 16 MiB inclusive in 64 KiB units.  Zero, sub-1-MiB,
non-granular and oversized values are rejected.  Existing callers retain a
1 MiB default.

## Boundary

- `bx-mantle` uses the value only as the already-configured native RAM size.
  It learns no DPMI, DOS, BOP, selector, allocation or host capability term.
- `bx-vdm` only copies the already validated session value into the stage
  request.  It does not reserve or map a range yet.
- CLI exposes `--guest-memory-kib` solely as session machine geometry.
- Existing low-memory BOP shim bounds remain unchanged in this S; their
  later classification is not a bulk textual replacement.

## Verification

The focused fixture must prove default 1 MiB, admitted 2 MiB ordinary RAM at
the first extended address, and invalid geometry rejection before a machine
is created.  Engine-contract and CLI validation must prove the same copied
range rules.  No adopted Bochs source may change.
