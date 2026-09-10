# M0 T372 S4 P3 — Original termination-cleanup closure

The final S4 check was deliberately made against the retained original owner,
not against a second adapter cleanup implementation.

`mvdm-host/vdmredir/vrmisc.c` owns `VrTerminateDosProcess`.  It reads the
terminating DOS PDB from `AX`, then retains the original ordering:

1. `VrTerminateMailslots(DosPdb)`;
2. `VrTerminateNamedPipes(DosPdb)`.

The latter original owner, `vrnmpipe.c`, retains the process termination and
per-thread cancellation contracts.  It stops/joins the named-pipe worker,
cancels pending I/O, closes the event resources, releases each bounded
completion record, and clears the worker event.  The required modern safety
body is registered as `MVDM-HOST-DIV-169`: original source had declared an
empty termination body, which cannot safely release a modern session worker.
The divergence preserves the original owner and cleanup order; it adds no
provider routing or BOP selection.

The existing focused fixture was rebuilt externally as x86:

```text
powershell.exe -NoProfile -ExecutionPolicy Bypass \
  -File tools/build/New-T290S3VrMiscNinja.ps1 \
  -Architecture x86 -RepositoryRoot O:\\repos.hobby\\ntvdm64
ninja -C build/M0-T290/S3/vrmisc-x86 -j 8
build/M0-T290/S3/vrmisc-x86/bin/t290-s3-vrmisc-fixture.exe
```

It compiled the original `vrmisc.c`, linked, and exited successfully.  The
fixture proves that the original top-level PDB value reaches both cleanup
owners in order.  The existing S4 P1 completion fixture separately proves
bounded completion copy, stale-session rejection and observable worker-stop
state; the adapter never owns the original Redirector queue policy.

Together with S4 P1/P2 this is an S4 source-first closure.  It does not claim
that a dynamically loaded `VDMREDIR.dll` or a live guest `57:xx` named-pipe
execution has occurred; those remain S6 whole-package integration work.
