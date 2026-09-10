# M0 T261 S3 P9 — PIF/COMMAND layout formal closure

## Question

Did the completed PIF/COMMAND owner re-homes remain buildable in the formal
eight-component graph, and do the focused reached provider fixtures retain
their prior behavior?

## Inputs

- T261/S3 moves through `09db66e5`.
- `tools/build/t260-s8-component-manifest.json` and
  `tools/build/New-T260S8FullNinjaGraph.ps1`.
- Fresh disposable root `build/M0-T261-S3/r009`.

## Procedure

1. Generated the formal MSVC x64 `/MT` graph in fresh `r009`.
2. Ran `ninja -C build/M0-T261-S3/r009` to completion, then repeated it for
   its incremental no-work result.
3. Ran the focused PIF/COMMAND and related owner-rehome fixtures from that
   exact root.

## Observations

- The graph completed all 458 recorded Ninja edges and produced the seven
  static libraries, 79 fixture executables, and `ntdos64-native.exe`.
- The repeat invocation reported `ninja: no work to do.`
- The following focused fixtures returned zero:
  - `t231-s6-command-check-binary-direct-fixture.exe`;
  - `t234-s2-command-dynamic-environment-fixture.exe`;
  - `t230-s10-dem-native-session-direct-fixture.exe`;
  - `t237-s2-xms-source-mirror-fixture.exe`;
  - `t237-s5-bop-nosupport-fixture.exe`.
- Two unrelated whole-graph fixtures did not provide passing runtime evidence:
  - `t245-s11-config-complete-fixture.exe` returned 2 because its imported
    `MS_bop_E` body reaches `demDasdInit`; its fixture supplies no admitted
    DASD initialization context. This is the pre-existing limitation recorded
    by `m0-t259-s4-p4-final-host-machine-reconciliation-001.md`.
  - `t251-s3-redir-ingress-fixture.exe` returned 9: its malformed `57:23`
    event has no readable descriptor, so the present source-derived provider
    returns `ERROR_INVALID_ADDRESS` before its later opaque-token check. This
    is a Redirector-provider/fixture contract issue, not a PIF/COMMAND move.

## Interpretation

The formal build and S3-owned PIF/COMMAND behavior are closed. The two
non-passing fixtures are retained, explicit follow-ups: neither is evidence
that the PIF parser, COMMAND composition, RTL facade, DEM ingress, XMS ingress
or configuration-complete file move changed behavior. They must be addressed
only by their owning DASD/Redirector package; they are not repaired as layout
side effects.

## Follow-up

T261/S4 starts the ledger's DEM host-support recovery. It must preserve the
existing original call shapes while separating adapter-softpc mapping/mechanics
from source-derived host filesystem capability. Redirector remains a joint
S5 owner split because a one-file move would create `opennt-host -> opennt-bop`.
