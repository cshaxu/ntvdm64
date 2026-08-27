# M0 T288 S2 monitor root and static-composition evidence

## Question

Can the selected monitor/keyboard source advance one recovery rung without
silently enabling a historical monitor, a second CPU executor, or a new
machine boundary?

## Inputs

- The S1 union/source-identity ledger and interface audit.
- Both canonical OpenNT MVDM roots named in the source policy.
- Exact `sim32.h`, `sim32.c`, `v86`, `ieuvddex`, and
  `softpc.new/base/keymouse` selections.

## Procedure

1. Re-rooted the seven selected `softpc.new/base/keymouse` files with Git
   renames from `mvdm-host` to `mvdm-softpc-firmware`; recomputed their source
   ledger entries against both canonical roots.
2. Generated formal MSVC `/MT` Ninja declaration-only graphs for x86 and x64.
   Each graph compiles the focused fixture through the exact original
   `sim32.h` and archives only the fixture object.
3. Performed an x86 direct syntax survey of the exact original `sim32.c`.
   The source has its original FAR/PASCAL declaration incompatibilities,
   unavailable serial-device/NT I/O dependencies, and malformed tail; no
   mirror edit or replacement was admitted.
4. Recorded every selected non-direct unit in the S2 composition ledger.

## Observations

- All 78 S1 selected paths remain hash-identical to both canonical roots.
- The seven keymouse paths now have their firmware owner root and retain
  byte-exact contents.
- Both formal declaration-island graphs pass.  They select neither `sim32.c`,
  any `v86` monitor body, a prebuilt archive, nor `src.old`.
- The exact `sim32.c` cannot currently be a direct modern translation unit.
  This is a source-proven composition boundary, not authorization to rewrite
  its serial/WOW protocol.

## Interpretation

S2 closes the only direct composition rung available without inventing monitor
semantics.  The exact declarations are available on both supported host
architectures; monitor state, VDM_TIB, event, SoftPC/SAS and debugger/WOW
dependencies proceed only through the named S3/S4 and later-owner work.

## Follow-up

S3 must recover the smallest source-shaped copied VDM_TIB/event boundary.  It
must not enable a `sim32.c` serial transport, Bochs CPU route, BOP selector or
new mapping manager.
