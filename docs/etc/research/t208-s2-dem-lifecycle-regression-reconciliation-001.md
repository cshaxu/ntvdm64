# T208 S2 — DEM lifecycle family regression reconciliation

## Question

Does the current source closure support one honest regression of the declared
`50:38..3D` lifecycle family, without treating a trace or a typed route as
provider recovery?

## Procedure

1. Correct the DEM family fixture so `50:3C` is no longer classified as a
   no-debug no-op.  The fixture now states only the weaker fact it proves:
   every DEM identity receives a typed session route, not a reconstructed
   historical provider.
2. Add the already-existing `dem_session_lifecycle_provider` owner test as a
   selectable MSVC x64 `/MT` fixture.
3. Repair the generic fixture manifest by adding the passive
   `normal_terminal_sequence_observation_v1` source now referenced by the
   generic bridge.
4. Build the package and lifecycle fixtures against the same current source
   closure.  No product provider source, BOP ingress or machine contract was
   modified.

## Observations

| Fixture | Build | Result | Interpretation |
| --- | --- | --- | --- |
| `dem-package`, `artifacts/build/t208-s2-dem-package-r2` | MSVC x64 `/MT` compiles and links after the manifest correction | run exit `91`, from the pre-existing FCB BOP transaction regression | The declared whole-DEM fixture is currently not a passing package regression.  This is an FCB/namespace closure issue, not evidence about a lifecycle service. |
| `dem-lifecycle`, `artifacts/build/t208-s2-dem-lifecycle-r4` | MSVC x64 `/MT` compiles and links | run exit `11`: its provider cannot initialize | The current boot-namespace provider cannot construct the declared unbound startup-configuration state required by this test. |

The second observation has a concrete source cause.  T204 intentionally left
unbound `CONFIG.SYS` and `AUTOEXEC.BAT` with zero bytes until the configuration
provider binds them.  However,
`bx_ntvdm_boot_namespace_provider_v1_initialize` immediately calls
`bx_ntvdm_profile_search_snapshot_v1_initialize`, and its per-file helper
rejects `file->bytes == 0`.  The provider therefore fails before
`bind_startup_configuration` could ever make those images non-empty.

This is not a missing or unusable modern Win32 API.  It is an internal
namespace/configuration lifecycle inconsistency.  It must not be concealed by
making `50:3C` a no-op, by restoring synthetic boot bytes, or by changing a
BOP's result to success.

## Reconciliation decision

- `38..3B` remain source-shaped selected-profile outcomes: ordinary no-debug
  no-ops plus `demIsDebug`'s `AL=0` response.
- `3C` remains a bounded PDB search-state cleanup with its own owner test; it
  is not part of the no-debug group.
- `3D` remains an explicit fatal controlled stop, not ordinary completion.
- The two fixture failures are retained as negative evidence.  They block a
  claim of a passing lifecycle-family regression, but they do not alter the
  T200 all-BOP audit or authorize a trace-led implementation.

The required follow-up is a separately admitted **startup-configuration /
boot-namespace initialization closure**: it must define and test whether a
declared but unbound configuration file is a valid snapshot entry, or change
the provider's construction/bind order through the correct owner.  It is not
a DEM lifecycle patch and not a Win32 capability task.  Only after that owner
package provides a passing fixture may T208 S2 rerun the six-service family
regression and make a final close/defer decision.

