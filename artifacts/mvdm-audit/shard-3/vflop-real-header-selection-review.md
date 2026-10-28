# VFLOPPY and real-floppy header: selected routes

This is a static review of host C/H plus existing tests; no guest image, device or product code was changed. Both files match their `O:/repos.external/OpenNT/base/mvdm/softpc.new` counterparts byte-for-byte: `host/src/nt_vflop.c` SHA-256 `B2766665ECD787CDC9DBABAB3C44F25FCD6185D90B221CDD201A159DD77A339D`; `base/inc/gfirflop.h` `9E033D9D00AFA92AA71B157B043DF550F5BB1D62AED97AA8D77E29C46E3372EC`.

## nt_vflop.c

The original virtual-floppy server is wholly under `VFLOPPY`; `gfi_vdiskette_init` registers its own command, on/off and reset methods in `gfi_function_table`. In the current source tree the init/term names have no callers outside their definitions, and the selected `host/src/sources`, current worker Ninja and generating script do not name `nt_vflop.c` or define `VFLOPPY`. Thus there is no demonstrated selected runtime path to this provider. Retaining it as pending original functionality is warranted until original profile activation, file/sidecar semantics, consumer and scope are resolved; the source body or a compiled empty object is not evidence of partial execution. Current local physical-floppy support is not proof of a substitute for file-backed VFLOPPY.

## gfirflop.h and selected real floppy

This original header declares seven real-floppy functions, but the captured selected-header dependencies do not contain it. `base/inc/gfi.h:583–592` separately declares the actually selected `active`, `valid`, and `change` functions; `base/disks/gfi.c:289–325` calls them and `host/src/nt_rflop.c:385–475` implements them. Activation selects `nt_gfi_rdiskette_init`; deactivation retires it and installs the empty server. Do not claim the four other declarations are selected from that connection. The header itself is a historical unselected declaration, whereas the separate GFI/real-floppy implementation is **partial**, not excluded.

`../nine-way/part-4/gfi-real-floppy-provider-review.md` describes selected map ownership, actual GFI/empty-object test with recording host seams, and a guest no-media INT13 reset. Neither the host seam nor the no-media reset proves real-media read/write/format, removal, failure rollback, or the FDC asynchronous-thread path. `../host-ch-scope/virtual-real-floppy-owner-review.md` specifies the missing independent VFLOPPY reconstruction and physical-medium acceptance; `floppy-lease-failure-role-review.md` describes separately evidenced session/lifecycle defects. Do not assert real-device failure or classify VFLOPPY as equivalent based on these tests.

Recommendation for central ledger: retain `nt_vflop.c` pending for original-profile/scope investigation, classify `gfirflop.h` itself as unselected historical declaration in this captured configuration, and preserve current GFI/real floppy partial work and explicit S13 gaps. No shared ledger change was made.
