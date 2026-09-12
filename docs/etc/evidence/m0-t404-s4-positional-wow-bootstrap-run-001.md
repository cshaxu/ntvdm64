# M0 T404 S4 positional WOW bootstrap run 001

## Scope

This is bounded runtime evidence for the active S4 positional launch packet.
It records the first run of the S4 worker; it does not claim a runnable WRITE
window, a recovered WOW provider, or task closure.

## Product input

- Formal x86 worker: `build/M0-T404/S4/r002-productized-wow-bootstrap/original-softpc-process.exe`
- Formal link result: success; output is 3,223,552 bytes.
- Staged public product: `O:\ntvdm64\ntvdm32.exe`
- Staged SHA-256: `f37a5647bfc4c7a2f13a061fec215ee6223c77b166b506fc65674b1138685c45`.
- Invocation: `ntvdm32.exe system32\WRITE.EXE` from `O:\ntvdm64`.

The run supplied only default-off observation paths for DEM-open, BOP-return,
CPU-illegal, and exception reports.  No public worker or observer command-line
option was passed.

## Static checks

The focused x86 executables all returned zero:

| Executable | Result |
| --- | --- |
| `app-launch-declaration-wow-entry-test.exe` | 0 |
| `broker-base-vdm-record-test.exe` | 0 |
| `basesrv-base-vdm-broker-test.exe` | 0 |

The app fixture verifies public rejection of `--command`, `-w`, `-a`, and
`-o`, DOS absence of a WOW carrier, and the session-bound Win16 carrier.

## Runtime result

The DEM log proves successful opens of `COMMAND.COM`, `DOSX.EXE`, and
`SYSTEM.INI`; in particular it records successful phase 0 and phase 1 opens
of `O:\NTVDM64\SYSTEM32\DOSX.EXE`.  This advances the current formal worker
beyond the earlier DOSX illegal-instruction frontier.

The process then remained live and CPU-active.  Its BOP report ended at
repeated `50:01` dispatches.  Original `nt_bop.c` identifies `50:01` as
`MS_bop_1`, the WOW route which initializes and dispatches `WOW32.DLL`.
Neither the optional CPU-illegal nor exception report was created.  The
observer process was explicitly terminated after the repeat was established.

## Boundary discovered

The S4 formal graph builds the parent worker only.  It does not produce
`WOW32.DLL`; the deployed provider has SHA-256
`9529e5ddbdf65b73bda23afb17b5c0c939c2547397520cfcb1c097574f2680c7`, while
the archived A200 package has a different provider hash.  More importantly,
the current source tree lacks several provider files named by the active
adapter-WOW registry (`wow_private_user_compat.c`,
`wow_sim32_pointer_compat.c`, and `wow_window_projection.c`); they occur only
in the explicitly non-production preserved-WIP commit.

S4 must not import those files or substitute a WOW provider: that is a new
source-import and provider-composition packet.  The evidence therefore keeps
S4 active only for its admitted product-entry recovery, and transfers formal
same-version WOW32 provider closure to the dependency-first next packet.
