# mvdm-softpc-firmware

Canonical original MVDM firmware-input mirror. Its retained
upstream-relative package set is `softpc.new/bios`, `softpc.new/roms` and
`softpc.new/data`.

This is a separate original mirror component, not an executable DOS/WOW16
guest image and not a parallel SoftPC machine. ROM/BIOS code inputs remain
immutable. CMOS/profile records retain their original runtime persistence
semantics but must be read and written only below the app-selected session
firmware root. Executable `softpc.new/base/bios` and `base/keymouse` source is
rooted in `mvdm-host`, where original SoftPC composition can select it. Its
source, makefile and manifest inputs are byte-exact mirrors; retained
historical `obj/` products have no selected source-root counterpart and remain
evidence.

T288 S2's former `base/keymouse` placement was superseded by T310/S4 after
the original SoftPC startup audit showed that it is executable controller
source, not firmware media. Its Git move preserves byte identity and original
relative paths under `mvdm-host`.

T301 S1 P21 also preserves missing byte-exact `opennt-src-2` firmware-adjacent
products at their original-relative paths.  They add no selected firmware or
machine behavior; see the per-file provenance record in
[`m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv`](../../docs/etc/operations/m0-t301-s1-opennt-src-2-mvdm-extra-import-manifest.tsv).

## Divergence register

None. The re-rooted keymouse files are exact upstream mirrors with no
registered divergence.
