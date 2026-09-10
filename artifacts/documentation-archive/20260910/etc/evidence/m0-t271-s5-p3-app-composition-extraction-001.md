# M0 T271 S5 P3 — DEM/COMMAND/XMS app-composition extraction

## Question

Can the remaining modern, one-session DEM/COMMAND/XMS entry and binding code
leave the `opennt-bop` mirror without moving selector/service semantics into
the selector-blind `adapter-bop` boundary?

## Inputs

- T271/S5/P1's 22-file non-mirror ingress inventory.
- Imported OpenNT `nt_bop.c` subsets `MS_bop_0` and `MS_bop_4`, and the XMS
  table/provider mirror.
- Existing app BOP composition callback and `adapter-bop` copied-frame
  transaction interface.
- Formal component manifest and the focused DEM, COMMAND and XMS fixtures.

## Procedure

1. Used `git mv` to move the nine project-authored DEM/COMMAND/XMS ingress
   bodies and their headers into `src/app/bop/`:
   route, DEM host/session/runtime/entry, COMMAND session/entry, and XMS
   session/entry.
2. Repaired all production, fixture and formal-manifest inputs to the new
   app-owned paths.  The remaining SoftPC, Redirector and top-level ingress
   headers are referenced explicitly from the app route; none was implicitly
   reclassified.
3. Regenerated the formal graph at `build/M0-T271-S5/r001`, ran its focused
   Ninja build, then executed the three resulting binaries.

## Observations

- `opennt-bop/ingress/` no longer contains the nine moved sources.  Its
  remaining 13 bodies are the already-classified SoftPC/firmware,
  Redirector, configuration-complete, keyboard handoff and top-level owner
  packages.
- `adapter-bop` remains unchanged and selector-blind: it accepts a fixed
  copied #UD frame and invokes one opaque app-composed callback.  It contains
  no `50:xx`, `52:xx`, `54:xx`, selector or service routing.
- The app code only establishes the modern session/context and passes the
  bounded frame to the imported `MS_bop_0`, `MS_bop_4`, or original XMS table.
  It does not replace the DEM/COMMAND/XMS provider tables or source ordering.
- Formal Ninja compiled and linked all three requested outputs.  The focused
  executables returned zero:
  - `t237-s2-xms-source-mirror-fixture.exe`;
  - `t231-s10-command-native-session-fixture.exe`;
  - `t230-s10-dem-native-session-direct-fixture.exe`.

## Interpretation and confidence

The modern bounded-frame/session wiring is final application composition, not
an OpenNT mirror input and not a generic BOP adapter service.  Keeping it in
`app/bop/` makes the ownership visible while preserving the source-owned
provider invocation beneath it.  The move is behavior-preserving: the same
three focused source paths compile, link and pass after the change.

This reduces the S5/P1 non-mirror ingress count from 22 to 13.  It does not
claim closure for the residual owner packages, nor does it recast them as app
code merely because the app composes their callback.

## Follow-up

Audit the remaining 13 ingress bodies against their named SoftPC/firmware,
Redirector and top-level owners.  Each must be recovered from a source-shaped
package, retained as a registered private mirror exception, or transferred
with an explicit failure disposition before T271/S5 can close.
