# M0 T245 S8 — Config-complete notification source and capability map

## Original Contract

The source-built guest reached `BOP_NOTIFICATION` (`5E`) with `AL=0` after
the completed selector-17 SI=1 path.  The guest source is
`dos/v86/doskrnl/bios/sysinit1.asm:ConfigDone`, immediately before its own
`AllocUMB` call.  The original NT host body is exactly:

```
MS_bop_E:
    if (getAL() == 0) {
        UMBNotify(0);
        demDasdInit();
    }
```

`host/src/nt_umb.c:UMBNotify` is itself an original no-op.  It is not an UMB
reservation request and requires no reimplementation.

## DEM DASD Dependency Split

The imported `demdasd.c:demDasdInit` has three ordered effects:

1. clear `demBDS`; set floppy and fixed-disk counts to zero;
2. `demFloppyInit()` — historical floppy/BIOS/SoftPC path;
3. `demFdiskInit()` — enumerate fixed drive letters and build DASD records.

The current direct-DASD seam intentionally preserves `demFdiskInit` for its
direct services, but it does **not** compose `demFloppyInit`.  More
importantly, its present source-derived fixed-volume probe documents the old
eligibility rule of attempting each host fixed drive and has no CLI
include/exclude-drive enforcement.  Calling `demDasdInit` from 5E today would
therefore bypass the project’s host-drive admission contract.

## Disposition

| Original part | Present viability | Required owner |
| --- | --- | --- |
| `UMBNotify(0)` | Direct original no-op; no capability needed. | top-level 5E mirror |
| reset `demBDS`/counts | Source body available, but inseparable in its original caller from later init. | DEM DASD composition |
| `demFloppyInit` | Needs historical BIOS/floppy/physical-media machinery. | machine/floppy owner |
| `demFdiskInit` | Source body and current host-volume shim exist, but its all-fixed-volume enumeration violates current drive policy until repaired. | host-drive policy + DEM DASD |

## Decision

No 5E route is admitted in S8.  A source-faithful recovery must first repair
the current DEM fixed-volume seam to consume the CLI session’s admitted-drive
mask and must separately specify a no-floppy source disposition.  Only then
can an `MS_bop_E` mirror call the original functions in source order without
using a generic ignore or touching unadmitted host media.

This is a top-level composition prerequisite, not a residual printer task.
