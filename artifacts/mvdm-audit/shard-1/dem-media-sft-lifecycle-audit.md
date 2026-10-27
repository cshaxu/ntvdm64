# DEM media, SFT and FD-access ownership

Frozen universe SHA-256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. `dos/dem/demgset.c` and `demmisc.c` remain partial mirror diff; original line matching is not functional coverage.

`demSetDTALocation` retains numeric addresses for DTA, PDB, extended error and SFT head; DOSWOWDATA is leased briefly and failure clears SFT head. `demfile.c::demSftHeadPointer` converts retained SFT head through `Sim32GetVDMPointer`. `VDDAllocateDosHandle` separately gets JFT and PDB pointers, updates guest JFT and SFT, and hands raw pointers back; original `wow32/wkfileio.c` uses them later during file creation, rollback, association and named-pipe flag changes. `src/wow32-dll/include/nt_vdd.h` specifically says its macros alter declarations only, not call-site aliases/wrappers. Historical map `build/t448-s11-regression/wow-trace-r2/ntvdm.exe.map` resolves `_VDDAllocateDosHandle`, `_VDDAssociateNtHandle` and `_VDDRetrieveNtHandle` to original-mvdm-dem:demfile.obj; this is a historical package, not proof of current deployed PE or every WOW branch.

The previous DIV-007 entry in `src/mvdm/README.md` asserted active VDD wrappers use session-owned shadow/commit and WOW writes have explicit commit/discard, while the inspected current code uses the original linear pointer path. The provenance of any earlier shadow implementation is not established; `git show 622b71cda:src/mvdm/dos/dem/demfile.c` shows the same pointer provider already existed at T414 canonicalization. Do not claim a removal commit or infer production fault. CPU backing lifetime, reentry while guest pointers are held, rollback, session teardown, and handle width still require proof. If pointer lifetime cannot be established, admit a targeted binding/commit design rather than merely changing the documentation.

The S10 package PE tables were subsequently checked directly: WOW32.DLL
SHA-256 `C3188EA4A9A1B59EAA77BD9CFB61DA6F00C25466BCB7E5795E604DBB89D2DF83`
imports `VDDAllocateDosHandle`, `VDDAssociateNtHandle` and
`VDDRetrieveNtHandle` from `ntvdm.exe`; the S10 worker SHA-256
`01A5B459072CA23F49AA43285580BD9017395C936F9DA8B2F18EE7223A52D1F6`
exports all three names. The cached worker link map attributes the VDD
implementations to original `demfile.obj`. This verifies package linkage by
name, not actual call execution or the guest alias lifetime. DIV-007 is now
corrected to describe the current source instead of an absent shadow.

`demmisc.c::demDiskReset` retains original HostFloppyReset -> HostFdiskReset order, then writes zero through worker-local COMMAND SCS location lease, terminating VDM on failure (DIV-111); failure direction is an adaptation requiring explicit lifecycle tests. `tests/adapter-mvdm-host-out/basesrv/t394_s4_command_scalar_state_fixture.c` exercises scalar read/write, wrap and invalid binding; older `tests/component-integration/fixtures/t230_s9_demmisc_direct_import_fixture.c` is a separate historical runtime fixture and is not proof of current full reset/session flow.

`GetMediaId` DIV-279 fixes original filesystem padding destination. `tests/mvdm/oemuni/oemuni_media_fixture.c` tests extracted actual body with labels, filesystem lengths, canaries and failed native query; historical T420 S37 evidence also records a guest witness. Those do not prove current release identity and all guest memory failure paths. Original `demGSetMediaID` obtains a direct GetVDMAddr buffer; this audit does not assert that original synchronous access is unsafe without a specific lifecycle/failure demonstration.

No runtime tests executed in this batch.
