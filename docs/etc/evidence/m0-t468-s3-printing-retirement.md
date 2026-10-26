# M0 T468 S3 — printing and spool retirement disposition

## Scope and sources

This record traces the existing original retirement owners before any change.
The sources are `wow32/wspool.c`, `wow32/wcall16.c`, `wow32/wkman.c`, and
the original Win16 kernel task exit in `kernel31/i21task.asm`.  No guest,
provider, profile, production source, or release binary changed.

## Raw spool carrier

`WG32OpenJob` dynamically uses the original `WINSPOOL.DRV` entry table,
starts a native document, and stores its `HANDLE` through `GetPrn16`.
`GetPrn16` calls `LocalAlloc16(..., gUser16hInstance)`: this is an original
USER-instance local carrier, not a new NTSRV/worker job table.  `WG32CloseJob`
is the only ordinary owner that calls `EndDocPrinter`, `ClosePrinter`, and,
after a successful close, `FreePrn`.

The original `W32FreeTask` list contains DC, timer, Comm, form-feed, WinSock,
resource, hook, cursor/icon, accelerator and parameter cleanup, but no
`FreePrn` or printer close.  The original guest task exit does invoke
`GlobalFreeAll` for task global objects; that does not change this local
carrier's explicitly supplied `gUser16hInstance` owner.  `WG32DeleteJob`
also calls its original `DeletePrinter` function-table entry without retiring
the carrier.  Therefore abrupt raw-job task loss is an original capability
limitation, not a missing hook-up that can be safely repaired by adding a
parallel host job registry.

The current selected host has no legacy `[devices]` profile mapping, so raw
`OpenJob` reaches its explicit original zero return before native job creation.
There is no real raw job from which to claim task-loss cleanup.  S3 does not
create a mapping, provider, fake handle, or synthetic cleanup to manufacture
that coverage.

## PrintDlg returned-object owner

The original `WCD32PrintDlg` converts successful default printer data back to
16-bit `hDevMode` and `hDevNames`, frees its temporary 32-bit copies, and
returns the two 16-bit global handles to the application.  They are therefore
application-owned, not callback-, worker-, or spooler-owned.

The real guest witness `build/t468-s3/printdlg-default-r4` calls
`PD_RETURNDEFAULT`, observes nonzero returned handles, then calls original
KERNEL `GlobalFree` ordinal 17 on each.  It records:

```text
result=1; extendedError=0; devMode=6110; devNames=6102;
freeDevMode=0; freeDevNames=0
```

Zero is the original `GlobalFree` success return.  This proves the selected
normal returned-object handoff and retirement path.  The preceding r2 probe
used ordinal 15 by mistake and is test-construction failure only; r4 is the
accepted result.

## Decision

No production diff is admitted.  Original normal owners remain connected and
the observable PrintDlg handoff retires correctly.  The raw abrupt-task edge
remains an explicit original limitation pending an external original-source
correction, not a license for a new host lifecycle manager.
