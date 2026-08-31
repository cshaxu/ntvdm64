# M0 T322 S2 P1 — Original XMS static build closure

## Change

`tools/build/New-T287OriginalXmsStaticNinja.ps1` still referenced the retired
`src/mvdm-support/inc` include root.  The original `suballoc.h` mirror now
resides at `src/mvdm-host/inc/suballoc.h`; the generator now names that actual
mirror root.  No XMS source or adapter behavior changed.

## Procedure and result

Generated `build/M0-T322/S2/x86-CPU40/build.ninja` with the original XMS
static generator, then ran Ninja for `original-xms-common.lib`.

All eight selected units compiled and archived:

- original `xms`, `xmsa20`, `xmsblock`, `xmsdisp`, `xmsmisc`, `xmsumb`;
- same-shaped `mvdm_xms_memory` callback binding; and
- `original-xms-common.lib`.

The build retains visible original-source diagnostics (`DbgPrint` declaration,
possible uninitialized locals, and inactive branch locals).  They are not
suppressed or altered by this packet; no diagnostic is evidence of a mapping
ABI failure.

## Scope

This is compilation/link closure of the selected original XMS common package.
It is not a dispatcher runtime test and does not claim UMB/INT15 execution.
T322/S2 remains active for focused positive/negative callback and dispatcher
verification.
