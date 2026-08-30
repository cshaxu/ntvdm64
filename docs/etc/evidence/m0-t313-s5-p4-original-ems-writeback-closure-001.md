# M0 T313 S5 P4 — Original EMS writeback closure

## Original configuration conflict

The selected CCPU40 `ccpusas4.c` contains the original `SAS_MM_LIM` physical
byte and word write paths, and includes the original `emm.h` declarations for
`LIM_b_write` and `LIM_w_write`.  The selected host profile defines `LIM` in
`host_def.h`, so those calls are compiled.

The matching original functions were not absent: they were in the selected
`base/dos/emm_mngr.c` file, together with `LIM_str_write`, `patch_pages`, and
the original page-invalidating writes.  A historical `#ifndef NTVDM` around
that block excluded them from the same NTVDM profile.

## Recovery

`MVDM-HOST-DIV-148` expands only that block's outer conditional for the
selected CCPU40 recovery graph.  The original writeback bodies, call ordering,
EMS state, SAS invalidation, and `host_patch_one_page` algorithm are unchanged.
No adapter, ordinary-RAM fallback, or substitute EMS implementation is added.

## Verification

Fresh external MSVC Win32/x86 CCPU40 graph:

```text
build/M0-T313/S5/ems-writeback-r3
original-softpc-candidate + original-softpc-forced-closure.dll
result: 407/407, exit 0
```

`dumpbin /symbols obj/dos/emm_mngr.obj` contains all three original exports:

```text
_LIM_b_write
_LIM_str_write
_LIM_w_write
```

The forced-link ownership log no longer contains the former CCPU unresolved
`LIM_b_write` and `LIM_w_write` edges.  No live EMS caller is claimed by this
P; it closes the selected source/configuration and link contract only.
