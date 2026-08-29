# M0 T310 S8 P4 — original video profile source-selection closure

## Question

The x64 all-tree ABI worklist contained 31 rows from
`softpc.new/base/video/ga_glue.c`, including C4311/C4312 pointer-width
conversions. This record determines whether they are a required repair of the
selected SoftPC machine or a mistakenly selected historical variant.

## Original-source evidence

The exact OpenNT `softpc.new/base/video/sources` manifest lists
`ga_glue.c` only on a line beginning with `#`. The same disabled line is
present in the selected OpenNT baseline and `opennt-src-2`.

`ga_glue.c` is the earlier C-video glue: its `C_VID`/CCPU branch deliberately
passes an Intel physical address through a `UTINY *` parameter, then truncates
it into a `ULONG` before calling the old `WRT_POINTERS` table. In contrast,
the selected CPU_40/EVID source path uses `EVID_WRT_POINTERS`, whose generated
move entries carry an `IHPE` source value. The two paths are not one
width-conversion seam that may be combined.

## Disposition

- Keep `ga_glue.c` byte-identical in `mvdm-host` as original source evidence.
- Do not compile it into the selected CPU_40/EVID SoftPC machine profile.
- Do not add an adapter, mapping-manager path, pointer cast, or overlay for
  this unselected alternate glue.
- The selected `cvidc`/EVID path remains subject to its separate full
  x86/x64 ABI audit.

## Build correction

`Get-OriginalSources` formerly extracted `.c` names from commented lines.
It now removes complete `#` comment lines from the original `SOURCES` block
before choosing translation units. This restores the original source-list
selection; it does not alter a controller algorithm or suppress a warning.

## Verification

A Node 22 parse of the exact `video/sources` block confirms that the selected
source list excludes `ga_glue.c`. A forced x64/x86 compilation of the old
file was used only to identify the stale 31-row warning cluster; it is not
runtime evidence for the selected profile.
