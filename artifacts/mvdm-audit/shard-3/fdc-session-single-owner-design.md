# FDC access to COMMAND guest state: ownership boundary

## What is authoritative today

`cmdmisc.c:cmdInit` registers the SCS and FD-access guest locations via `mvdm_command_guest_state_set_scs_scalars`, taking the original guest DS:CX location. The **guest word** is the counter; `nt_rflop.c` and `nt_fdisk.c` increment/decrement it, and `demmisc.c:demDiskReset` writes zero. The host `current_state` is a location/epoch registration, not the counter's authoritative value. It is currently thread-local (`mvdm_command_guest_state.c:24`); `current_state_valid` requires that registration's owner and session and guest-memory epochs to match the current thread's session. Consequently the same session binding on FDC's separate thread is insufficient. The MSVC actual-object test documented in `fdc-session-fixture-plan.md` demonstrates this narrow fact.

The worker session owns guest-memory lease context and its lifetime, but **today it does not own a shared authoritative COMMAND registration**. Saying it already does would be inaccurate. `session_dispose_reason` refuses disposal with bound threads; however, `session_guest_memory_end` may happen separately and `guest_memory_lease_end` frees active bounce buffers. The lease slot table and read/modify/write pair in `mvdm_command_guest_state_add_fd_access` have no evident internal serialization. Epoch checks alone are not a concurrency lock, a join, or proof guest writes cannot race.

## Minimal candidate for S12

Keep the guest FD word as sole value authority. Store one numeric SCS/FD registration and its session/memory generation in the corresponding session-owned COMMAND adapter state; make existing accessors consult that registration rather than retaining a second independent TLS registration. Bind the original FDC thread to its originating session using the matching void-cdecl-parameter thread adapter. Each operation validates the current session and generation and uses the existing bounded guest-memory mechanism; do not retain guest pointers or store an independent counter.

The transfer must include one established serialization domain covering registration replacement/invalidation, **the entire** FD read-modify-write, lease slot use and memory end. It must also respect guest/CPU access to this word; a lock only taken by host workers cannot establish that by itself. Reuse an existing CPU/guest-memory synchronization boundary if one can be shown to cover all of those participants; otherwise S12 must design and test one explicitly rather than assume the current leases are thread-safe. Avoid holding that boundary across `CreateFile`, media waits, UI callbacks or a thread join.

On reset/close, prevent new FDC work, signal its existing cancellation condition, wait for the old worker to stop before reusing `fdc_parms`, ending guest memory, invalidating the registration, or disposing the session; the worker must finish any guest-state operation and unbind before termination. Session binding counts protect disposal but do not by themselves protect a separately invoked memory-end. Handle/counter failure handling must retire native handle, local owner/lock/count exactly once while exposing failure to the owning lifecycle; no false success and no arbitrary timeout are implied. The exact cancellation/wake primitive and lock domain must be chosen from the actual FDC and session execution paths; this is a proposed direction, not a proven drop-in change.

## Required evidence before adoption

1. Actual selected objects and real test-owned session: main and FDC threads see the single registration; wrong session, changed epoch and post-memory-end accesses fail without writing old or newly reused memory.
2. Concurrent FDC/main counter updates cannot lose or duplicate changes, including COMMAND/DOS reset and guest-origin writes; prove the actual serialization boundary, not merely test an adapter-only mutex.
3. Deterministically gate FDC during read, commit, reset, media wait, and exit. Show old thread stops before parameter reuse and memory end; bind rejection/start failure and nested cleanup do not leak a binding or native handle.
4. Fake-device open/close and scalar-failure cases preserve the device/FDC status contract, with separate coverage of synchronous DEM AX/CF. A successful fixture does not prove physical media, DOS/WOW/Win3.x or arbitrary guest timing.

No product, guest, or release change has been made. The present bound-thread witness proves the deficiency; it does not prove this proposed architecture or a real guest failure.
