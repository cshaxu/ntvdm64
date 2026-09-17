# M0 T420 S15 XMS Package Recovery

## Scope and source-first result

S15 closes the selected `src/mvdm/xms.486` package.  The manifest is eight
original OpenNT units: `xms.c`, `xms.h`, `xmsa20.c`, `xmsblock.c`,
`xmsdisp.c`, `xmsmemr.c`, `xmsmisc.c`, and `xmsumb.c`.  The comparison source
is `base/mvdm/xms.486` in the admitted OpenNT reference; the original
`i386/xmsmem86.c` backend was also examined as the allocation/move boundary
reference.

Six manifest members now match the pinned source after normalized comparison:
`xms.c`, `xmsa20.c`, `xmsdisp.c`, `xmsmemr.c`, `xmsmisc.c`, and `xmsumb.c`.
S15 restores `xms.c` directly, deleting the non-effective
`MVDM_XMS_SESSION_BACKEND` source condition, and restores the original
`xmsa20.c` spelling.  The former `MVDM-HOST-DIV-132` registry entry is deleted
with that source diff.  No mirror source file was added.

Two remaining source deltas are finite, source-proven boundaries rather than
duplicate XMS policy:

| Source | Retained delta | Why it remains |
| --- | --- | --- |
| `xms.h` | `xmsMoveMemory` states `Destination, Source, Count`. | The original declarations in `xmsmemr.c` and `i386/xmsmem86.c`, plus `PSAMEMORYMOVEROUTINE` in `suballoc.h`, use that callback order.  The historical header's reversed prototype is an ABI misdeclaration, not a runtime policy to preserve. |
| `xmsblock.c` | Original descriptor decode and copy calls use bounded guest-memory leases. | The original `GetVDMAddr(SS:BP)` and direct linear aliases presume durable host pointers. CPU40 guest backing cannot expose that pointer lifetime. The original 12-byte descriptor order, overflow/failure branch, forward-move direction and AX result are retained. |

`i386/xmsmem86.c` cannot be directly composed in the standalone CPU40 worker:
it converts Intel linear values to native pointers and delegates allocation to
the NT virtual-memory process API.  The selected worker adapter retains the
same-shaped `xmsCommitBlock`, `xmsDecommitBlock`, and `xmsMoveMemory` exports;
it supplies only bounded mapping, zero-on-commit/reuse and worker teardown.
Likewise, the A20 source calls retain their original SAS control shape and
bind directly to CPU40 SAS A20 state.  Neither adapter owns XMS allocation,
descriptor, or failure policy.

## Footprint and similar-logic sweep

The recovery removes one mirror divergence registration and returns two
manifest sources to pinned spelling.  It adds no adapter/autonomous source.
Three stale focused-build generators were corrected to consume the canonical
`src/mvdm` and `src/ntvdm-exe/session` layout; they are test composition only,
not product implementations.

The sweep covered every selected allocation, move, mapping and teardown
consumer, the original i386 backend, `suballoc` callback declaration, and the
worker XMS/A20 bindings.  No further duplicate local XMS implementation was
found.  The remaining two mirror deltas above are the complete XMS boundary
ledger and must be revalidated by the owning callback/guest-memory packages,
not copied into an overlay.

## Verification

- `original-xms-common.lib` rebuilt from the eight-unit manifest in
  `build/M0-T420/S15/xms-static/x86`.
- The mapped-memory fixture passed: bounded commit/decommit zeroing, 5000-byte
  overlapping move, forward copy, invalid-range handling, and session
  preservation.
- The XMS startup fixture passed both the original successful allocation path
  and `STATUS_NOT_IMPLEMENTED` fallback through `SAInitialize`.
- Fresh formal x86 `product-programs` rebuilt all five deliverables:
  `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`, and `VDMREDIR.dll`.
  A fresh Ninja dry run reported no work.
- The rebuilt package was deployed to `O:\winnt`; direct `run16.exe MEM.EXE`
  and `run16.exe COMMAND.COM /C MEM.EXE` both exited `0`.

The prior owner-accepted interactive COMMAND/MEM/EDIT regression remains the
established product baseline.  S15's direct XMS regressions and focused
mapping/startup fixtures were rerun against the new artifacts; no new manual
interactive-input observation was represented as an automated result.
