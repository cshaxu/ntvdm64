# T225 S19 DEM PDB lifecycle ordering correction 001

## Question

Does `50:3C` own Direct opaque-token cleanup, or must that token remain valid
until guest DOS performs its ordinary close path?

## Source facts

- `src/opennt/base/mvdm/dos/dem/demsrch.c:667-691`, `demTerminatePDB`, takes
  `BX` as the terminating PSP, invokes the profile-dependent VDD and host
  device notifications, and releases only that PSP's FindFirst/FindNext list.
  Its return is void.
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm:977-988` emits
  `SVC_PDBTERMINATE`, then calls `DOS_ABORT` while the child PDB is still
  current.
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/abort.asm:59-147` walks the JFT
  and SFT/FCB state after that notification.  `Close_NT_Handle` emits
  `SVC_DEMCLOSE` with the SFT's stored 32-bit NT-handle field.
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/handle.asm:137-142` confirms the
  ordinary `$close` path likewise emits `SVC_DEMCLOSE` only after DOS has
  selected the SFT.

Thus the source order is fixed:

```text
50:3C demTerminatePDB  -> per-PSP search/device notification
DOS_ABORT / normal close -> guest JFT/SFT selection -> 50:02 demClose
```

## Current composition and correction

The Direct provider returns a bounded opaque token in the historical `AX:BP`
shape.  It is not a JFN, SFN, or SFT; guest DOS retains those structures and
later returns the token to `50:02`.  `bx_ntvdm_dem_file_session_v1` records a
copied PDB only for diagnostics and a future separately proven compatibility
seam.

The previous `50:3C -> release_owner(BX)` binding was therefore wrong: it
closed the adapter's opaque replacement before DOS reached its source-owned
`SVC_DEMCLOSE`, turning the later normal close into a forged stale-token
failure.  This record supersedes the contradictory Direct-token clause in
T225 S2 P1/P3; the older source maps remain indexed historical evidence.

`bx_ntvdm_dem_session_lifecycle_provider_v1` now releases only the boot
namespace's per-PDB search state.  It intentionally takes no action on the
whole-provider file session.  Direct token release remains `50:02`; session
teardown remains a host-composition destruction action, not an emulation of
DOS PDB termination.

## Recovery ladder and boundaries

1. Reuse original OpenNT ordering and void ABI: admitted.
2. Smallest adapter rehost: per-PSP search cleanup only, because original DEM
   translation units cannot compose directly with the current x64 CLI's
   CCPU/SAS/VDD/product-host dependencies.
3. Bochs intrusion: rejected; this is neither a machine nor CPU behavior.
4. New JFT/SFT model: rejected; it would duplicate guest DOS ownership.

VDD, floppy/fdisk and FCB-specific lifetime remain their original-owner
profile dispositions.  Readonly declared startup tokens remain session-wide;
Overlay remains unselected and receives no lifecycle fallback.

## Regression

The formal Ninja fixture
`t225-s19-dem-package-lifecycle-fixture` creates a Direct opaque token owned
by `BEEFh`, dispatches the real package route `50:3C` with the same `BX`,
proves the void `RIP+4` result and that the token remains live, then proves
ordinary `50:02` releases it.  Its separate named Direct-only scenario avoids
confusing this source-order proof with the unrelated legacy all-family
fixture baseline.  The [result](../evidence/t225-s19-dem-pdb-lifecycle-ordering-result-001.md)
records its fresh MSVC x64 `/MT` closure, explicit Readonly refusal, and the
remaining all-family limitation.