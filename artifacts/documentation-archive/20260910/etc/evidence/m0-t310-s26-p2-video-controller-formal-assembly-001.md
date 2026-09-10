# M0 T310 S26 P2 — video-controller formal assembly

## Scope

This record closes the selected Win32/x86 CCPU40 source assembly of
`softpc.new/base/video`.  It does not enable a display endpoint or claim
guest-visible graphics.

## Source-shaped composition

- All nineteen members of the original `base/video/sources` manifest compile
  directly into `original-softpc-video.lib`.
- The CCPU source selection adds the original `vglob.c` state carrier and a
  private `localfm.c` overlay.  Original `localfm.c` declares `Gdp`, `Cpu`,
  `Sas`, and `Video`; selected `ccpusas4.c` already owns the same `Sas` global.
  The registered `MVDM-HOST-DIV-137` overlay keeps only the identical `Gdp`,
  `Cpu`, and `Video` declarations, avoiding a duplicate selected `Sas` owner.
- `MVDM-HOST-DIV-079` and `MVDM-HOST-DIV-080` select the original CCPU40
  C-VID writer-table branch.  This prevents the legacy A_VID-only table from
  requiring unselected EGA mover bodies.
- `effective_addr(IU16, IU32)` is the original public spelling used by
  non-CCPU video callers.  The adapter is a same-shaped facade over the
  selected original `c_effective_addr`; it neither calculates a second address
  nor converts a guest number into a host pointer.

## Formal result

Fresh build root:

`build/M0-T310/S26/formal-video-x86-r5`

The selected target command completed with no work after constructing the
fresh graph:

```text
ninja -C ... obj/ccpu/localfm.obj original-softpc-video.lib
mvdm-softpc-effective-address.lib -v
ninja: no work to do.
```

The forced full-composition link reached its link stage.  Its unresolved-symbol
log contains no member of `original-softpc-video.lib`, no `C_Video`/EVID
dispatch symbol, no EGA/CGA/VGA controller symbol, and no `effective_addr`
symbol.  The only display-related unresolved edge is the original host endpoint
form `InvalidateConsoleDIBits` from `nt_vga.obj`, `nt_ega.obj`, and `nt_cga.obj`;
that is explicitly `SPC-HOST-VIDEO-BINDINGS` (S27), not a controller owner.

Other forced-link entries belong to their named later owners (DEM/COMMAND,
monitor, platform, firmware, device and BOP packages).  `/force:unresolved`
is an ownership audit only and is not a runnable-product result.

## Closure and successor

Every selected S26 controller declaration, definition, initializer and reached
caller now has a direct original, registered mirror correction or narrow
private-overlay disposition.  S27 owns public host display/window bindings;
S49 remains responsible for integrated guest-visible video behavior.
