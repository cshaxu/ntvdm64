# M0 T421 S3 — WOW32 target-component boundary

## Question

Can the late-loaded WOW32 target have an explicit component owner without
moving original MVDM source, creating another NTVDM worker state, or treating a
load/link result as WOW16 acceptance?

## Classification and move map

The original WOW32 carrier remains exactly where OpenNT places it:
`src/mvdm/wow32/sources` selects 77 C translation units, with `wow32.def` and
`wow32.rc` beside them.  Original selected USER translation units remain in
their canonical `src/opennt-host/windows/core/ntuser/...` paths.

`src/wow32-dll/` now owns the 21 target-only C bindings selected by the formal
DLL link and 30 target declarations.  This is a path-only relocation: no
original MVDM or OpenNT-host translation unit moved.  The two target-only USER
facades (`ntuser_rtl_compat.c` and `wow_public_user_facade.c`) were included
in the relocation sweep; leaving them under `ntvdm-exe/win32` would have made
the DLL boundary misleading.

The worker intentionally retains only `wow_user_runtime.c` and
`wow_user_session_binding.c`, plus their declarations.  Those are the unique
owner of worker TLS and the current worker/session association.  `nt_vdd.h`
remains a shared worker include because non-WOW MVDM consumers use it; S4 owns
that ABI-location review.  The generic historical CRT compatibility unit is
also not relabelled as a WOW32 binding merely because WOW32 compiles its own
stateless copy.

## Narrow parent ABI

The sole target binding that had reached into worker state was
`wow_user_task_lifecycle.c`.  It formerly called `session_thread_current()`
and inspected the worker state directly.  It now calls the narrow parent ABI
`wow_user_worker_active()`, implemented by the existing unique worker binding.
The retained observable question is only whether the current worker is active;
the DLL has no authority to inspect or bind the session object.

This is the smallest source-shaped facade: no second TLS, `session.lib`, CCPU
executor, or copied session structure is linked into WOW32.

## Fresh construction and checks

1. Generated and linked `build/M0-T421/S3/formal-x86-r4` as a full x86 product
   graph (four programs and `VDMREDIR.dll`).  It incorporates the relocated
   paths and linked `ntvdm.exe`, `run16.exe`, `basesrv.exe`, `dtmgr.exe`, and
   `VDMREDIR.dll` successfully.
2. Generated and linked a fresh formal WOW32 target in
   `build/M0-T421/S3/wow32-component-r4`.  `dumpbin /headers` reports x86
   machine `14C`.  Its import table names `ntvdm.exe` and exactly the narrow
   worker imports `wow_user_runtime_current`, `_enter`, `_leave`,
   `_set_context`, and `wow_user_worker_active`; it contains no
   `session_thread_*` import.
3. `verify-wow-output-composition.ps1` passed its structural gate: local
   registration bridge present, 20 output slots present.  Its own result is
   `composition-observation-not-runtime-acceptance` and is deliberately kept
   that way.
4. The full graph's existing WOW worker fixtures passed:
   `WOW_PAGE_DOMAIN_LIFECYCLE_OK` and
   `WOW_ORIGINAL_USER_CLIENT_LAYOUT_OK wnd=176 desktop=100`.
5. The deployed, unchanged runtime package passed all 17 established
   transcript-gated COMMAND/MEM/EDIT/native/guest-exit cases under log prefix
   `m0-t421-s3-boundary-r1`.  The matrix ended with no product process left.
   This validates non-WOW regression only; S3 did not deploy a new WOW32 DLL
   and makes no WOW16, WRITE, WINMINE, or SOL claim.

## Result

S3 closes the directory and source-owner split.  The follow-up S4 review must
remove any remaining obsolete aliases or duplicate target inputs and audit the
shared `nt_vdd.h`/generic compatibility locations; it must not reinterpret this
composition proof as a functional WOW32 closure.
