# M0 T460 S4 — selected Clipboard-format acceptance

## Scope and authority

S4 accepts only the selected common `HGLOBAL` formats already owned by the
original WOW32 Clipboard callers: `CF_TEXT` and `CF_DIB`.  It does not add a
Clipboard provider, manager, cache, listener, DDE endpoint, guest change, or
guest rebuild.  The existing original owners remain:

| Concern | Original owner | S4 conclusion |
| --- | --- | --- |
| guest-to-host copy and accepted ownership | `wuclip.c::WU32SetClipboardData` | `CF_TEXT` and `CF_DIB` use the same original `HGLOBAL` branch. |
| host-to-guest copy and stale cache retirement | `wuclip.c::WU32GetClipboardData`, `WU32ICBGetHandle`, `W32EmptyClipboard` | Both formats retain the existing original cache/cleanup owner. |
| delayed render, replacement, viewer and owner destruction | `wmtbl32.c` plus `W32Win16WndProcEx` | `WM_RENDERFORMAT`, `WM_RENDERALLFORMATS`, `WM_DESTROYCLIPBOARD`, and `WM_DRAWCLIPBOARD` retain the original no-thunk callback route. |
| cooperative handoff | `wkman.c::WK32Yield` through registered `pfnYieldTask` | A Win16 owner may yield once before a native peer requests a delayed DIB; this is the original cooperative scheduler path, not a host sleep, poll, or new scheduler. |

`WU32ICBRenderFormat` is an old header declaration with no callable definition
in the current MVDM source set.  It is not a missing S4 implementation seam:
the traced live callback is the existing `WM_RENDERFORMAT` window-procedure
route.  No implementation was invented for that declaration.

## Executable evidence

All evidence below used the already published S3 R5 package, an immutable
test-only Win16 NE probe and native peer, and the shared public Clipboard.  No
production image was changed or published by S4.  Each normal-transfer result
asserted guest-to-native data, native-to-guest data, rejected `SetClipboardData`
ownership, delayed rendering, native replacement, viewer notification and
owner `WM_RENDERALLFORMATS`; task-loss runs assert that a retired owner is not
called back.

| Format | Lifetime path | Evidence root | Result |
| --- | --- | --- | --- |
| `CF_TEXT` | full normal/rejection/replacement/deferred/owner-destroy | `build/t460-s3/s4-text-shared-r2` | 18 checkpoints passed. |
| `CF_DIB` | full normal/rejection/replacement/deferred/owner-destroy | `build/t460-s3/s4-dib-shared-r2` | 18 checkpoints passed. |
| `CF_TEXT` | callback return | `build/t460-s3/s4-text-callback-shared-r2` | 13 checkpoints passed. |
| `CF_DIB` | callback return | `build/t460-s3/s4-dib-callback-shared-r2` | 13 checkpoints passed. |
| `CF_TEXT` | owner task exit | `build/t460-s3/s4-text-task-shared-r2` | 7 checkpoints passed. |
| `CF_DIB` | owner task exit | `build/t460-s3/s4-dib-task-shared-r2` | 7 checkpoints passed. |

The `CF_DIB` probe validates the declared 44-byte `BITMAPINFOHEADER` plus
one padded pixel prefix, rather than treating Win16's rounded movable-allocation
size as a format requirement.  The full DIB trace records successful original
callback dispatch for `0305`, `0306`, `0307`, and `0308`.

## Test-runner correction

The runner now normalizes the different top-level-array shape produced by
Windows PowerShell 5.1 and PowerShell 7 before validating a package manifest.
The earlier shared-DIB failure was a pre-launch runner failure caused by that
serialization shape, not a product Clipboard failure.  The corrected runner
was executed under both Windows PowerShell 5.1 and PowerShell 7; the latter's
shared-DIB full run is retained at
`build/t460-s3/s4-dib-shared-pwsh7-r1`.

## Reusable handoff facts

The later DDE package may reuse only these established facts: common Clipboard
`HGLOBAL` formats retain the original `wuclip.c` owners; public USER messages
enter the existing Win16 window-procedure callback bridge; delayed rendering
can require the original cooperative task yield; and task retirement must not
deliver a later callback into its departed owner.  These facts do not establish
DDE conversation, reply, cancellation, or DDE-data ownership semantics.
