# M0 T328 — FDC/INT15 wait-continuation closure

T328 closed the original FDC/INT15/CPU40 wait-continuation investigation
without a production behavior change.

The complete source map proves that the observed zero-address exception returns
inside original `floppy.c::wait_int`, immediately after its first recursive
CPU40 `host_simulate()` call. The relevant ROM wait body, non-`MONITOR`
vectors, FDC completion, ICA delivery, INT15 completion, BOP-FE exit and
CPU40 entry binding are all present and selected from original sources.

Consequently this task does not create a ROM loader, FDC-success shortcut,
KIO vector patch, BOP handler, alternate executor or guest change. Its narrow
successor is the original CPU40 per-thread recursive `setjmp`/`longjmp` frame
contract and its guest re-entry lifecycle.

The unchanged selected-source x86 and x64 formal Ninja product links both
passed. The x64 link remains a build closure only, not a runtime claim.

Evidence: [T328 S1 original FDC/INT15 contract](../etc/evidence/m0-t328-s1-original-fdc-int15-wait-continuation-contract-001.md).
