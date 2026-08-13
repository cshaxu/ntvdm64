# T198 S26 P2 DEM Initialization Trio Native 001

## Question

Can the exact source-built NTIO/NTDOS initialization path complete the next
two source-defined DEM continuations without importing their historical host
services?

## Source Mapping

`50:45` is `SVC_DEMSYSTEMSYMBOLOP`, dispatched to `demSystemSymbolOp`.
`demmisc.c` returns immediately when `IsDebuggee()` is false.  The profile
already demonstrated that non-debug disposition at `50:3B`; the admitted
service therefore has no symbol store or debugger backend.

`50:0F` is `SVC_DEMGETDRIVES`, dispatched to `demGetDrives` in `demgset.c`.
Its original count begins at A/B and increases only across contiguous accepted
drive types from C.  The existing source-derived drive provider implements
that exact count against an immutable CLI snapshot; it never queries the host
during a BOP.

## Result

The new DEM-MISC entry returns `RIP+4` only for the real-mode `50:45` form.
The new GSET plane accepts only service `0F` after common ingress, original
provider selection, DEM GSET classification, and an explicitly injected valid
snapshot.  The native fixture supplies one already-filtered `C:` fixed-drive
entry, then intentionally converts the successful `50:0F` resume into its
test-only typed stop.

The MSVC x64 `/MT` composition closure and exact CPU5 run exit zero:

```
t198-s23 status=4 observed-5011=1 observed-503b-resume=1 observed-dta-resume=1 observed-hard-error-resume=0 observed-system-symbol-resume=1 observed-drive-resume=1 observed-stop=1 next=00:00 plane=0/0/1 error-lock=0 gset=1 terminal=1:0000:0ad2
```

The zero hard-error marker in this particular repeat is not a regression
claim: `50:32` is separately closed by S26 P1.  This run is deliberately
stopped at the newly admitted `50:0F` result; it proves that the continuation
can reach and complete the MISC/GSET pair without taking the former unbounded
path.

## Boundary Observation

One diagnostic run allowed execution past `50:0F`.  It subsequently reached
real-mode `LSL` faults and the minimal machine's unmapped VGA trigger path,
where it emitted `vga called in trigger_timer stub` and timed out/panicked.
This is retained as a machine-owner frontier, not evidence to enable VGA or
other devices.  Any extension beyond the controlled `50:0F` stop requires a
separate source/trace audit of the post-drive guest path and the minimal
machine lifecycle.
