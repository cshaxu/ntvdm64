# T220 S5: direct DEM namespace owner transaction result

## Question

Can the current direct DEM namespace path for `50:03`, `50:12`, and `50:22`
copy the original `CurrentPDB` identity before a host open, commit it with the
opaque token, and preserve ordinary token close without attaching process
cleanup to `50:3C`?

## Inputs

- OpenNT `base/mvdm/dos/dem/demfile.c`, `demgset.c`, and `inc/doswow.h`.
- T220 S1--S4 ownership and transaction maps.
- Current `bx_ntvdm_dem_dta_service`, package-session, namespace partition,
  local-file backend, and file-session sources.

## Procedure and source disposition

The original `demfile.c` implementation cannot be linked into the x64 CLI
composition because it depends on CCPU/SAS/VDD and historical host state. The
change remains a rung-2, source-derived host seam: it preserves the original
ordering that reads the published `CurrentPDB` before publishing a handle.

`bx_ntvdm_dem_package_session_v1` requests exactly the already-registered,
two-byte `CurrentPDB` word before direct `03/12/22` completion. It supplies
only the copied nonzero PDB segment to `open_ex_owned`. The owned backend
rejects owner zero before a host open; the normal legacy backend remains
available for the separately scoped FCB and other pre-existing ownerless
paths. A live token slot now holds the copied 16-bit PDB, while ordinary
`release(token)` retains its token/generation semantics. `release_owner(pdb)`
is present and PDB-scoped but intentionally has no `50:3C` binding.

No BOP selector was added, no Bochs/mantle/guest/CLI ABI changed, and neither
FCB nor search state is claimed to participate in this ownership model.

## Observations and focused verification

The installed VS 2022 Build Tools compiled and ran two x64 `/MT` fixtures in
`build/M0-T220-S5/001/`:

1. `dem-file-session-test.exe` proves owned-token adoption, exact-owner
   release, foreign-owner preservation, ordinary close, and stale-token
   rejection.
2. `dem-local-file-owner-test.exe` proves `open_ex_owned(..., 0, ...)`
   rejects before use, a nonzero owner can open and be released by that owner,
   and existing direct/readonly/overlay/virtual policy outcomes remain intact.

The changed production translation units (`file_session`, `local_file_backend`,
`namespace_partition`, `whole_provider`, and `package_session`) also compile
under the same MSVC x64 `/MT` command with the normal `src`, `src/cli`, and
`src/bx-vdm` include surface.

## Interpretation, confidence, and composition limitation

The current full-composition compile script was invoked with a fresh S5 build
root, but cannot begin because its declared historical baseline
`artifacts/build/t198-s74-dem-pdb-termination-r1/link.rsp` is absent from this
checkout. This is a missing verification input, not a compile pass or a
runtime result. The current composition manifest nevertheless now includes
`bx_ntvdm_dem_process_owner_v1.c`, preventing a later full link from silently
omitting the package-session dependency.

Confidence is high for the isolated direct-token transaction and its profile
declines, and intentionally limited for the unlinked composition path. No
native guest-execution conclusion follows from these fixtures.

## Follow-up

Restore or re-admit a source-built full-composition verification closure before
claiming T220 S5 complete. A later lifecycle S may decide whether the proven
PDB-scoped release belongs in the guarded `50:3C` contract; it must separately
map child/parent, TSR, FCB, search, VDD, and host-device behavior.
