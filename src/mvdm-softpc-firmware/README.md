# mvdm-softpc-firmware

Canonical original MVDM firmware-input mirror. Its retained
upstream-relative package set is `softpc.new/base/bios`,
`softpc.new/base/keymouse`, `softpc.new/bios`, `softpc.new/roms` and
`softpc.new/data`.

This is a separate original mirror component, not an executable DOS/WOW16
guest image and not a parallel SoftPC machine. ROM/data inputs remain
immutable. `base/keymouse` is retained here because it is original SoftPC
firmware-adjacent code; a later admitted package may compose only a
source-proven fragment through the same-shaped adapters.

T288 S2 re-rooted the seven byte-exact `base/keymouse` files here using Git
renames. They retain their original paths below this component and are not an
app-selected host runtime edge. Any later source composition remains governed
by the monitor/keyboard owner package; only an admitted `adapter-bochs`
manifest may select a firmware image input.

## Divergence register

None. The re-rooted keymouse files are exact upstream mirrors with no
registered divergence.
