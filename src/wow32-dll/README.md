# WOW32 DLL target component

This directory owns the target-local binding edge of `WOW32.DLL`.  It is not
an OpenNT mirror root: the selected 77 original WOW32 C bodies, `wow32.def`,
`wow32.rc`, and their original manifest remain under `src/mvdm/wow32/`.
Original USER support bodies remain in their original `src/opennt-host/`
paths.

`source/` contains the 21 finite bindings selected by the formal WOW32 link.
They live with the late-loaded DLL because they service original WOW32 and
original USER calls in that DLL address space.  `include/` contains their
target-local declarations.

`ntvdm.exe` retains `wow_user_runtime.c`, `wow_user_session_binding.c`, and
their headers in `src/ntvdm-exe/wow/`: those two files own the sole worker
TLS/session binding.  WOW32 imports only the narrow parent ABI
`current/enter/leave/set_context/worker_active`; it does not link or import a
raw session API, `session.lib`, a CCPU executor, or a second worker state
instance.

`include/nt_vdd.h` is a WOW32-only declaration wrapper for three cdecl worker
imports under the original `/Gz` target convention.  The byte-exact original
ABI carrier remains `src/opennt-abi/source/public/sdk/inc/nt_vdd.h`; ordinary
MVDM consumers continue to include that original carrier directly.
