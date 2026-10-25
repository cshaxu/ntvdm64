# M0 T459 S2 — common-dialog consumer acceptance

**Status:** closed. No production source or guest media changed. S2 validates
the selected original modeless common-dialog lifetime path through the
immutable guest, together with the retained native leaf/callback boundaries.

## Executed package and consumer

The real run used `build/t457-s3/package-r1`, whose thirteen release images
match the current `assets/release/release-manifest.json` hashes. The existing
test staged that build below its short, marker-owned physical test root; no
user installation or guest binary was changed. It compiled an independently
authored `CDLPROBE.EXE`, placed it beside the immutable `COMMDLG.DLL`, and ran
it as a sibling task in a surviving immutable `WINMINE.EXE` worker.

`verify-wow-mine-gameplay.ps1 -RetiringSibling -CommonDialogRounds 16` passed
at `build/t459-s2/common-dialog-guest-r2`:

```text
PASS same-worker sibling normal exit and resource cleanup; surviving WINMINE flag/reset/open/close
PASS flag/counter, reset, open cell, WM_CLOSE and run16 completion
```

The guest report has zero errors across 16 explicit create/destroy rounds and
one final live modeless dialog. Its final record is `created=17`, `closed=16`,
`hooks=17`, `leaving=1`; each explicit destroy reports
`DestroyWindow=1` and `IsWindow=0`. The lifetime trace contains the final
`TaskRetirement ... stage=complete` exactly once, after the original resource
callback returns. This establishes that the retained original
`WCD32FindReplaceText`/`WCD32FindReplaceDialogProc` path reaches actual guest
creation, hook delivery, explicit destroy, terminal resource cleanup and task
retirement without a substitute dialog registry.

## Modal/error boundary

S1's exact thin-leaf boundary witness was re-used rather than pretending the
modeless guest probe covers every host UI outcome. It passes seven leaves and
six scenarios per leaf: normal return, cancellation, nested reentry, rejected
scope, native exception and finalization failure. The native standard-template
reference independently passed Find/Replace in both modes. The callback
fixture passes modal dispatch plus the modeless resource-only `WM_DESTROY`
ordering. Thus the result/error/exception authority remains native COMMDLG and
the original `wcommdlg.c` conversion body; `wow32-dll` retains only scope
entry/exit and no copied state.

The source audit and real consumer together found no product defect and no
need for a release: all exercised product images were already hash-identical
to the current publication. The only S2 edit repairs the observer script's
PowerShell 7 file-list invocation before runtime staging; it has no provider
or guest effect.

## Remaining boundary

The selected OLE HTASK-alias consumer remains S3. S2 does not claim actual
`OLECLI`/`OLESVR` guest interoperability, native printer UI, or an arbitrary
modal-dialog visual interaction beyond the selected conversion/callback and
modeless lifetime evidence.
