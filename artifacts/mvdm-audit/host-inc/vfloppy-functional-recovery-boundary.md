# Original file-backed floppy alternate provider: finite recovery boundary

`softpc.new/host/src/nt_vflop.c` remains pending. Current/OpenNT SHA-256 `B2766665ECD787CDC9DBABAB3C44F25FCD6185D90B221CDD201A159DD77A339D`. The body is guarded by `VFLOPPY`; the observed CCPU40 worker graph does not select this source or define that switch. Its original file-backed diskette role is not demonstrated by the existing no-media BIOS reset or the separately selected NT real-floppy provider. The `GFI` table is the shared device dispatch owner; no second table should compete with it.

## Original capability and static failure candidates

The original `gfi_vdiskette_init` forces the target to B: and installs only command/on/off/reset callbacks in the per-drive table. `fl_int_reset` closes the previous file, concatenates configured directory and diskette name, attempts read/write then read-only open, and calls `fl_read_SID`; that loader opens the fixed `c:\softpc\dos.SID` format description. The command handler obtains FDC DMA state and reads or writes per sector through ordinary file descriptors. `gfi_vdiskette_term` releases and closes the open descriptor.

The following are **conditional original source risks, not observed failures in the active product**:

- In the read branch, failed seek or short read can print a diagnostic while the branch later writes successful-looking FDC status; establish exact original guest-visible status and continuation before any change.
- Initialization overwrites four of the seven current GFI dispatch members; which initial values supply the other three and whether activation/replacement can leave stale callbacks must be tested against actual profile initialization. Do not assume either corruption or safety from this observation.
- SID loading parses disk geometry and sector indices from a fixed path with limited visible validation; the capacity, malformed sidecar and partial initialization behavior must be bounded using only generated test-owned media. Its source uses fixed legacy paths, which are not automatically an acceptable product path contract.

## Finite original-first evaluation

1. Reconstruct the complete original `VFLOPPY` compilation, `gfi_vdiskette_init` caller, drive configuration, ownership and seven-slot initial table from original source and build profile; determine whether any existing supported product image backend already owns the same operation. Do not enable the flag in the default product or assume a selected real-device provider is a file-image provider.
2. If file-backed floppy remains required, use the actual original body in an isolated fixture with **test-owned, explicitly rooted** temporary image and SID inputs. Preserve the FDC command/result, DMA count and EOI/IRQ owner while adapting only necessary file/format paths. Verify valid 40-track/2-head sector reads/writes, read-only fallback, media change and idempotent attach/detach; then inject missing/malformed SID, bad sector, short read/write, seek failure, open and close failure, out-of-range DMA and termination. Check actual FDC status/carry, bounded memory and exactly-once handle release. Do not use old hard-coded `c:\softpc` files or any user's image as fixture inputs.
3. Follow with independently authored, disposable-media guest INT13 tests on a coherent candidate if and only if that feature is admitted; retain no-media/reset and existing real-device regressions. The original immutable guest must not be modified or rebuilt. If a failure contract must diverge from the original, record precise evidence and exception before changing it.
4. Keep the separate SLAVEPC remote-physical-device and gfi_test diagnostic decisions in `gfi-functional-handoff.md`; neither is a substitute for file-backed media. Likewise no currently observed alternative justifies declaring this pending source's user-visible capability complete.

No body has been compiled, executed, enabled or changed by this review. The branch-level concerns are source-derived hypotheses pending a safe fixture, not evidence that today's selected NTVDM is broken.
