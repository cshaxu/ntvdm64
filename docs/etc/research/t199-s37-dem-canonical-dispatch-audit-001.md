# M0 T199 S37: DEM canonical dispatch audit

## Finding

`src/opennt/base/mvdm/dos/dem/demdisp.c` is the only selector/service-number
authority for DEM. Its `apfnSVC[]` has 73 entries: the valid range is
`50:00..48`; `49` is the sentinel. The service value is the array index, not
a trace-derived identity.

The audit found an erroneous unaccepted clock attachment: `demQueryDate`,
`demQueryTime`, `demSetDate`, and `demSetTime` are `50:14`, `50:15`,
`50:19`, and `50:1C`. It was corrected before acceptance. FastRead
compatibility belongs at `50:42`; `50:3C` is `demTerminatePDB`.

## Canonical table

| Range | Original `apfnSVC[]` owners |
| --- | --- |
| `00-0C` | file/dir change pointer, chmod, close, create, mkdir, delete, rmdir; FCB delete; file time; pathname/FCB first and next |
| `0D-1C` | boot/free-space/drives/media; DOS load/open/current directory; date/time; read/rename; state/DTA setters |
| `1D-2B` | V86 address, write, original no-ops, FCB rename, IOCTL, create-new, disk reset, DPB, commit, absolute read/write |
| `2C-3F` | FCB create/open/close/I/O/date; file info; hard-error/retry; DOS-app symbols/lifecycle; console/debug; PDB/VDM/WOW lifecycle; lock |
| `40-48` | original no-op; computer name; original FastRead/FastWrite no-ops; check path; system symbols; DPB list; pipe EOF state |

## Required consequence

When a selected original, CLI-adapted, or source-derived leaf declines a mode
or precondition, DEM must return that identity's explicit source-derived
failure or terminal result. It must never leak a generic CPU pass-through that
could masquerade as an unimplemented BOP.

## Evidence and next check

The `t199-s37-dem-package-failure-r2` build exposed a duplicate component
case during correction. r3-r7 then exposed an existing-provider decline leak
in the whole-family sweep. These artifacts are failed regression evidence,
not closure. Continue package implementation; do not run a native trace until
the complete `00..48` family regression passes.
