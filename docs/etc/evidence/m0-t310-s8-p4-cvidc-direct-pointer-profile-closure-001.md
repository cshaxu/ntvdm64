# M0 T310 S8 P4 C-video direct-pointer profile closure

## Decision

The selected SoftPC configuration defines `CPU_40_STYLE` and `C_VID`. In that
profile the original C-video `EVID_WRT_POINTERS` tables, rather than the older
`WRT_POINTERS` string handlers, own mode-0/mode-1/mode-2 string access. The
latter transport native callbacks through `ULONG`, which is not valid on x64.

`egawrtm0.c` and `egwrtm12.c` therefore retain the original direct BIOS
byte/word callbacks and table globals but compile their uninstalled
direct-pointer string helpers only outside `CPU_40_STYLE`. This is an
original-source true subset for the selected profile, registered as
`MVDM-HOST-DIV-079` and `MVDM-HOST-DIV-080`.

`fill_both_bytes` operates exclusively on a private native video-buffer
pointer. Its historical `ULONG` alignment carrier is changed to `IHPE`, the
existing SoftPC pointer-width carrier. It is not a guest address and does not
use the session mapping manager.

## Verification

After regenerating the disposable source-shaped declaration carriers from the
changed original mirror inputs, these formal Ninja object builds completed
outside the sandbox:

```text
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/video/egawrtm0.obj obj/video/egwrtm12.obj -v
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/video/egawrtm0.obj obj/video/egwrtm12.obj -v
```

Neither build emits `C4311` or `C4312` for these selected objects. Existing
`C4244`, `C4100`, `C4101`, `C4005`, and historical declaration/source-quality
warnings remain visible and are not suppressed by this closure.

## Scope limit

This is an x86/x64 ABI closure for the selected C-video route only. It does
not claim runtime VGA conformance or dispose of remaining warning-ledger
clusters.
