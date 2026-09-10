# M0 T333 S3 P1 — Original Redirector synchronous source build

## Question

Can the first original Redirector source cohort compile and link as one x86/x64
library using only the already-admitted thread, handle-identity and guest-span
forms?

## Selected sources

`vrinit.c`, `vrmisc.c`, `vrnmpipe.c`, `vrputil.c`, and `vrdll.c` from the
original `vdmredir` mirror. The T310 formal generator now selects those files
as `original-mvdm-redir-sync.lib`; no async, RAP, NetBIOS, DLC or VDD provider
body was added.

## Procedure and result

Generated independent `build/M0-T333/S3/redir-sync-{x86,x64}` Ninja graphs
and, after loading MSVC's matching architecture environment, built
`original-mvdm-redir-sync.lib` in each.

Both succeeded. The first x86 attempt exposed the only hard source error:
the original cdecl `VrpAsyncNmPipeThread` was passed directly to a WINAPI
`CreateThread` slot. `vrnmpipe.c` now includes the existing
`thread_start_compat` facade, preserving its source call expression, body and
failure sequence. The formal include order also selects the existing
Redirector `vdmredir.h` binding before the original declaration carrier, so
the original `HANDLE_FROM_WORDS` form resolves through the session identity
facade rather than narrowing to a native `HANDLE`. Finally, `vrinit.c` uses
`FIELD_OFFSET(VDM_LOAD_INFO, VrInitialized)` for the original numeric guest
record offset, rather than forming and narrowing a null-base native pointer.

## Interpretation

This is a paired source-library compile/link result, not a runtime provider or
final-product link claim. Remaining MSVC warnings are retained visible; none
was suppressed for this task. The selected static library can contain calls to
other original package members without resolving them until a later final link.

## Blocking lifecycle dependency

`VrInitialize` first calls `VDDInstallUserHook`, then
`VrNetbios5cInitialize` and `VrDlcInitialize`. S2 classified those as VDD and
network/DLC owner boundaries. Consequently the S3 synchronous library does
not establish a runnable Redirector lifecycle: enabling it would require
either a source-shaped VDD/network completion or an explicit source failure.
It cannot honestly be bypassed by a local Redirector shim. T333 must either
reorder those owner cohorts or stop S3 at this source-build boundary.
