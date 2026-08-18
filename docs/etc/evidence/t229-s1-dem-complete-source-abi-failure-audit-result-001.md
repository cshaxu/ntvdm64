# M0 T229 S1 P1 — DEM Complete Source/ABI/Failure Audit Result

## Question

Can the first queued DEM owner-package task begin from a complete service
population rather than trace-reached endpoints?

## Inputs

- Original `src/opennt/base/mvdm/dos/dem/demdisp.c`, whose `apfnSVC[0..72]`
  table is the callable `50:00..48` authority and whose `49h` sentinel is not
  callable.
- Current `src/bx-vdm` BOP ingress, provider registry, DEM package session,
  whole provider and special-plane routes.
- Earlier T199, T200, T221, T225, T226 and T227 source/route/conformance
  records, rechecked rather than upgraded by assertion.
- The regenerated [T229 machine-readable ledger](t229-s1-dem-source-abi-failure-ledger-001.json).

## Procedure

1. Regenerated the 73-row ledger using the existing cardinality-preserving
   exporter against the current repository.
2. Verified the declared count, row count, uniqueness and exact canonical
   service set `50:00` through `50:48`.
3. Re-read original `DemDispatch`: its table confirms the seven ordinary
   `demNotYetImplemented` rows preserve CF-clear continuation, `50:42` is
   separately recorded because a retained compatibility branch diverges from
   that original no-op, and `50:49+` is the CF-set rejection boundary.
4. Reconciled current routes at owner-group level against T225's complete
   bound-route audit.  No source change under `src/bx-vdm` occurred after
   T228 P30 that would turn a typed route into proof of complete historical
   behavior.

## Observations

| Original owner group | Services | Current conclusion |
| --- | ---: | --- |
| File, handle, FCB and search | 29 | Original CCPU/SAS bodies are not composable in the x64 process. Existing checked-copy/token seams are source-derived inputs only; S2 must establish the complete Direct/Readonly provider behavior. |
| GSET, drive, CWD and volume | 15 | Existing host capability and snapshot routes need direct-host/source-contract reconciliation; Readonly must retain the same routing shape and source-derived mutation failures. |
| Misc, lifecycle, console and WOW | 15 | No generic success fallback is valid. Each must retain the original continuation, explicit unavailable/deferred result, or a complete source-derived provider. |
| Error and lock | 3 | Existing locator and retry paths cannot collapse into generic access-denied behavior. |
| DASD and IOCTL | 3 | Raw device/VDD/SoftPC dependencies remain DEM-owned but require explicit CLI disposition; no namespace shortcut is permitted. |
| Original no-op | 7 | These are complete only when the original CF-clear/no-I/O continuation is preserved. |
| FastRead exception | 1 | `50:42` must be deliberately retained or removed as a package decision; it cannot be silently credited as original no-op. |

The ledger has 73 unique identities and its group cardinalities sum to 73.
It records the source owner, dispatcher, current bound route, shared-profile
disposition, current shim/fallback and recovery-rung disposition for every
identity.

## Interpretation and next step

This closes the **audit** portion of T229 S1 only.  It does not claim that the
existing source-derived routes fully implement historical DEM behavior, nor
that a local fixture proves native guest integration.  The recovered package
must next converge as one S2 delivery: all 73 routes are retained, migrated,
replaced or deleted by their original owner contract; Direct and Readonly run
whole-family regressions; Overlay receives only an ABI-compatible extension
boundary.

No native trace was used to choose a service, and no BOP, guest, Bochs or
mutation-profile behavior changed in this P.
