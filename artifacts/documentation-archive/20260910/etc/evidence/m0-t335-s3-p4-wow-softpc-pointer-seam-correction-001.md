# M0 T335 S3 P4 — WOW guest-pointer seam correction

## Correction

The earlier scoped-frame bridge reached `session_guest_memory_*` directly.
That was mechanically safe but violated the architecture rule that every
historical guest-pointer exposure pass through the `softpc` family.  P4
removes the bypass.

`adapter-mvdm-host-out/softpc` now exposes only two bounded synchronous
operations:

- `mvdm_softpc_guest_memory_acquire(address, span, access, ...)`;
- `mvdm_softpc_guest_memory_release(lease, commit)`.

They bind the already-current session and delegate to its sole guest-memory
lease instance.  `adapter-mvdm-host-out/wow` continues to own only the
WOW-specific packed-VP conversion and the fixed `VDMFRAME` view; it cannot
select a mapper, bind a session or acquire a raw guest pointer by itself.

## Scope

The correction deliberately does not change original `wow32` source and does
not enable its general `GET*VDMPTR` macro family.  It makes P1's narrow frame
bridge conform to the same historical boundary that will be used by any
future source-backed WOW pointer-plane binding.

## Verification

Fresh focused Ninja graphs compiled and ran successfully on both profiles:

- x86: eight compile/link actions, fixture exit code 0;
- x64: eight compile/link actions, fixture exit code 0.

The fixture verifies both the current-task TLS slot and a frame read/commit/
discard transaction.  Its CCPU `c_sas_*` stubs exist only to satisfy the
already-selected SoftPC boundary translation unit; no test stub is production
input.

## Remaining boundary

P4 does not solve arbitrary WOW aliases.  The complete `GETVDMPTR`, string,
variable-span, flush/free and long-lived/asynchronous caller family remains
the active S3 source-wide lifetime problem.  No original `CallBack16` body or
generic Win16 route is enabled by this correction.
