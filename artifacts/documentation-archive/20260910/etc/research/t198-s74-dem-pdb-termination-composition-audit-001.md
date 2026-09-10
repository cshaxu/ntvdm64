# M0 T198 S74: DEM PDB-Termination Composition Audit

## Question

Can the first S73 boundary, `50:3C`, use the already source-derived finite
DEM session-lifecycle provider through the current composition without
restoring the historical intrusive host runtime?

## Original owner and excluded effects

OpenNT `demdisp.c` dispatches `SVC_PDBTERMINATE` to `demTerminatePDB` in
`demsrch.c`. The original order copies PSP/PDB from BX, conditionally notifies
VDD, invokes `HostTerminatePDB`, and releases PSP-owned search state. The VDD,
floppy/FDISK and ambient-host cleanup paths cannot enter the contained profile.

## Existing finite replacement closure

The existing `bx_ntvdm_dem_session_lifecycle_provider_v1` admits only exact
real-mode `C4 C4 50 3C`, ingress service `60`, source-derived-after-blocker
registry precedence, and DEM namespace-plane classification. It calls only
`bx_ntvdm_boot_namespace_provider_v1_terminate_pdb`, which releases matching
adapter-owned finite search-session slots. It returns the original no-result
`fault_rip + 4` resume without a GPR or flags delta.

The common structures already select the same route: provider registry gives
DEM `60` source-derived-after-blocker precedence, and DEM plane classifies it
as namespace/deferred. The focused provider test proves matching-slot release,
foreign-slot preservation, absent-list success, and wrong-vector rejection.

## Current gap and admitted change

The provider was retained outside the S50 composition manifest and not called
by `bx_ntvdm_boot_namespace_composition_v1`; it survives only through the
detached historical adapter runtime. The admitted correction is limited to:

1. compile and link that existing provider as a current composition member;
2. after common ingress/registry/DEM-plane classification, call it with the
   composition's existing namespace provider; and
3. add a composition-level exact `50:3C` resume test.

No raw recognizer, new selector contract, guest/DOS logic, VDD, device,
ambient-host search, Bochs change, or ABI revision is admitted.
