# M0 T421 — DLL component-boundary closure

T421 closed on 2026-09-22 with deliveries `9055e1b88` and `ab363a6f5`.
It made the independently loaded `VDMREDIR.DLL` and `WOW32.DLL` explicit
target components without moving canonical OpenNT mirror bodies.

`src/vdmredir-dll` owns the finite redirector target bindings; the one worker
retains only its guest-memory lease and asynchronous completion providers.
`src/wow32-dll` owns 21 target bindings and 30 declarations, while the worker
retains only the unique WOW runtime/TLS/session association.  The final audit
removed stale generator paths and a dead historical build script, and placed
the WOW32-only `nt_vdd` declaration wrapper with its target.

Fresh x86 product/DLL links and focused WOW worker fixtures passed.  T421 makes
no WOW16 or three-application acceptance claim; the successor T422 owns that
complete runtime recovery.
