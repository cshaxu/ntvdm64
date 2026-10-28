# Original virtual floppy versus selected NT real-media route

This review addresses pending `softpc.new/host/src/nt_vflop.c` and its distinction from the current guest-visible floppy owners. No guest image or physical medium was opened or modified and no runtime test was performed in this pass.

## Original owners and selection

`nt_vflop.c` places its original virtual-floppy implementation under `VFLOPPY`; its header describes an NT OS/2-era single B: file-backed server. Source hardcodes legacy image/sidecar names and defines its own read/write, command and open/close state. The selected NT `host/src/sources` does not name this file and the observed worker profile does not define `VFLOPPY`. This evidence does not establish that its old B: protocol is the same as modern image mounting, nor that a filename should be imported or patched into the product.

The current original CPU40 route instead runs `gfi_floppy_active` → `host_gfi_rdiskette_active` in `nt_rflop.c`: activation calls `nt_gfi_rdiskette_init`; deactivation terminates the real provider and installs the empty GFI provider. `host_gfi_rdiskette_valid/change` contain separate `#ifndef NTVDM` behavior; in selected NTVDM the generic pathname/CMOS validation branch is not run. The selected `nt_floppy_read/write` obtain the current PDB, acquire a drive handle and delegate to shared disk I/O. The guest DEM absolute I/O route separately selects those functions for appropriate removable media. None of that proves legacy VFLOPPY is active or equivalent to a real drive.

Existing `../disk-native-role-review.md` records actual original GFI object tests with recording seams and a no-media guest reset; `../nine-way/part-4/gfi-real-floppy-provider-review.md` explicitly says they do not perform real-media read/write/format. GFI, FLA, physical drive lifetime and failed PDB/handle paths remain partial. No physical or virtual device failure is asserted here.

## S13 executable investigation

1. Extend the existing selected-object GFI fixture with the actual selected `nt_rflop` object and controlled host seams; inject activation/initialization failure, empty→provider→empty transitions and a controlled host handle. The existing fixture currently uses recording host callbacks, not `nt_rflop`; do not label its present results as media I/O. Verify actual `gfi.obj`/`nt_rflop` ownership, PDB/handle identity and once-only release.
2. Where an authorized isolated floppy device or verified disposable medium exists, execute actual guest read, write, media-change, unavailable/removed-device and reset/teardown on selected provider; report absent hardware as untested. Never substitute host file path presence for an actual floppy request.
3. Independently reconstruct the original VFLOPPY server's `gfi_function_table` registration, DOS caller, exact file/sidecar format, readonly/failure and provider retirement. Determine whether that legacy profile is required and whether its complete bounded host interface can be composed without hardcoded user paths or a second owner. Do not infer need from Win95 experiments or automatically re-enable it.

Keep `nt_vflop.c` **pending** pending scope/dependency/behavior evidence; selected real-media operations remain their own partial S13 owner. An image-backed capability and a real removable-volume provider are not interchangeable without per-operation proof.
