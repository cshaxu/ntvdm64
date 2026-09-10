# M0 T272 S4 guest-memory lease reuse audit

## Decision

`src/session/guest_memory_lease.{c,h}` is a neutral, project-owned copied
lease implementation.  It supplies the common synchronous address/span/access
boundary required before any recovered OpenNT caller can use a host pointer.
It does not select a BOP service, name a Bochs object or interpret a DOS/VDM
data structure.

## Four-rung review

1. **Original source reuse:** no OpenNT translation unit owns the generic
   cross-provider lease.  Historical callers use CCPU/SAS
   `GetVDMAddr`/`Sim32GetVDMPointer` forms, whose concrete implementations
   rely on the removed SoftPC/CCPU product composition.
2. **Same-shaped adapter:** the old
   `src.old/adapter-softpc/guest_pointer_manager.{c,h}` is the closest local
   reference.  Its copied read/write and epoch release ordering are retained,
   but it cannot compose directly: it embeds a process-global registry,
   real-mode policy and host-handle/session-data registries.
3. **External-code intrusion:** none.  No Bochs or imported OpenNT file is
   modified.
4. **Minimal authored seam:** the new neutral lease preserves the only shared
   observable contract: checked copied bytes, explicit access intent,
   synchronous lifetime, epoch invalidation and optional writeback.  It omits
   real-mode addressing and resource ownership, which belong to later
   adapter-softpc/adapter-bochs work.

## Contract and failure behavior

- A caller supplies opaque callback context plus checked read/write callbacks.
- Every acquire validates `address + byte_count`, access bits and available
  lease capacity, then seeds a bounce buffer through the read callback.
- Release writes only for a write-authorized lease and only when `commit` is
  requested.  Read failure, write failure, invalid lease, stale epoch and a
  terminated context fail rather than resuming with an unchecked pointer.
- `end` frees every bounce buffer and invalidates all active leases.  No native
  pointer is serialized, passed to an asynchronous worker or retained after
  release/end.

## Verification

`tests/session/guest_memory_lease_test.c` passes as independent MSVC `/MT`
x64 and Win32/x86 builds in `build/M0-T272-S4/r001/`.  It covers read seed,
write commit, overflow, bad access, stale-end rejection and session teardown.
No input comes from `src.old/`.

## Deferred owner work

`adapter-softpc` will later adapt historical `GetVDMAddr`/
`Sim32GetVDMPointer` call shapes to this contract, and `adapter-bochs` will
later provide a Bochs-backed checked callback.  Neither is introduced here.
