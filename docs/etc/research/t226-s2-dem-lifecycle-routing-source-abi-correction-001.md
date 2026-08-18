# T226 S2 DEM lifecycle routing source/ABI correction 001

## Question

Does the current CLI route preserve the distinct OpenNT contracts for
`50:24 demNotYetImplemented` and `50:36 demEntryDosApp`?

## Source basis

- `src/opennt/base/mvdm/dos/dem/demdisp.c` maps index `24h` to
  `demNotYetImplemented` and index `36h` to `demEntryDosApp`.
- `demNotYetImplemented` calls `setCF(0)` and returns. It is an original
  no-op, not an unmodified-register return.
- `src/opennt/base/mvdm/dos/dem/demmisc.c:demEntryDosApp` calls
  `VDDCreateUserHook(PDB)` only after the first DOS-app entry, then returns;
  its debugger-only block is conditional.

## Defect and correction

`bx_ntvdm_dem_session_lifecycle_provider_v1` had compared the decimal literal
`36u`. That denotes service `24h`, so the lifecycle provider intercepted
`demNotYetImplemented` before the original-noop provider could clear CF. The
actual `demEntryDosApp` service `36h` instead passed through the miscellaneous
CLI no-op path.

The corrected route uses explicit hexadecimal identities:

- `50:36` remains in the DEM miscellaneous/lifecycle component and returns
  through the no-VDD, non-debug CLI branch; and
- `50:24` declines lifecycle handling and reaches the existing original-noop
  provider, which resumes and explicitly clears CF.

The original function cannot compose directly because it requires historical
VDD/CCPU host composition. The retained no-VDD result is therefore the
smallest source-derived compatibility seam, not a new VDD implementation.

## Verification design

`bx_ntvdm_dem_session_lifecycle_provider_v1_test.c` now checks both dispatch
boundaries, and the full DEM package fixture checks `50:24`'s CF-clearing
result. The lifecycle fixture is registered as
`t226-s2-dem-lifecycle-routing-fixture` in the formal Ninja manifest.

These changes await one clean formal MSVC x64 `/MT` build and fixture run. No
native trace, guest image change, Bochs change, VDD implementation or Overlay
behavior is admitted.