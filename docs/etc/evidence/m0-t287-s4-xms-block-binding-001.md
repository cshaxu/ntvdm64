# M0 T287 S4 — XMS block and copy binding evidence

## Result

S4 is closed. The original `mvdm-support/suballoc/suballoc.c` allocator and
the original `mvdm-host/xms.486/xmsblock.c` allocation, free, reallocation,
query and move control flow are now locally composed on x86 and x64.

## Binding disposition

- `suballoc.c` is used unchanged. It remains the owner of allocation identity,
  capacity and commit/decommit sequencing.
- `xmsblock.c` is retained as a mirror except `xmsMoveBlock`, whose direct
  `GetVDMAddr` aliases are replaced by the matching private
  `mvdm-host-overlay/xms.486/xms_block_move.c` (`MVDM-HOST-DIV-011`). The
  overlay reads the same `SS:BP-12` layout under one bounded lease.
- The historical x86 `xmsCommitBlock`, `xmsDecommitBlock` and `xmsMoveMemory`
  exports are provided by `adapter-mvdm-host-out/softpc/mvdm_xms_memory.c`
  (`ADAPTER-SOFTPC-009`). Their names, parameter widths and status/void shape
  are retained; numeric XMS addresses are checked or copied through typed
  Bochs mechanics and session leases, never cast to `PVOID`.
- The existing session guest-memory lease is the only guest-memory lifetime
  authority. A linear XMS address is deliberately not an opaque map token:
  original XMS/suballoc arithmetic requires it to remain a numeric address.
  No additional mapping-manager instance was added.

## Verification

`tools/build/New-T287S4XmsBlockNinja.ps1` produces a formal `/MT` Ninja graph
for both architectures. It tracks the S4 source identity and explicit S3
Bochs static-library prerequisites.

- `build/M0-T287/S4/x86/bin/t287-s4-xms-block-fixture.exe`: exit `0`.
- `build/M0-T287/S4/x64/bin/t287-s4-xms-block-fixture.exe`: exit `0`.

The fixture verifies source allocator success, linear stack move from the
original `SS:BP-12` layout, malformed stack rejection, reallocation/free,
allocation-capacity failure, and out-of-range commit/copy rejection.

## Explicit remaining boundary

S5 owns UMB policy and IVT/INT15 source families. The S4 fixture deliberately
does not call `XMSInit`, `xmsInitUMB`, an XMS dispatcher or a `52:xx` route;
none has been enabled by this binding.
