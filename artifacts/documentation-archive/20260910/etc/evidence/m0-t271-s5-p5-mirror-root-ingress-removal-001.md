# M0 T271 S5 P5 — mirror-root ingress removal

## Question

Can `opennt-bop` meet its mirror-component layout rule without retaining
project-authored ingress, route, session, observation, or test-only source in
its production root?

## Inputs

- S5/P1 original 22-file ingress classification.
- S5/P3 and S5/P4 app-composition extractions.
- Remaining SoftPC, Redirector and top-level ingress bodies, their focused
  formal fixtures, and the component manifest.

## Procedure

1. Classified the residual bodies as either final app composition (all
   selector/frame/session wrappers) or a test-only selector-map helper.
2. Used `git mv` to place the 11 final-composition bodies under `app/bop/`
   and the unlisted selector-map helper under `tests/component-integration`.
3. Updated the app route, fixture includes, formal Ninja manifest and
   `opennt-bop` divergence register paths.
4. Regenerated the formal Ninja graph and built/ran nine focused residual
   fixtures: printer, mouse, EMM unavailable, Redirector, memory-size,
   no-support, tape I/O, machine interrupt and keyboard handoff.
5. Searched all live production roots for the removed ingress path and for
   external consumers of `opennt-bop-overlay`.

## Observations

- `src/opennt-bop/ingress/` is empty and has been removed.
- The former 22 non-mirror ingress bodies have all left the BOP mirror:
  DEM/COMMAND/XMS and `MS_bop_E` composition in S5/P3--P4; SoftPC,
  Redirector and top-level final composition in this part.
- All nine focused residual executables compiled, linked and exited zero.
- No live production source names `opennt-bop/ingress`; residual textual hits
  occur only in the retained historical governance script.
- No non-mirror production component includes `opennt-bop-overlay`.

## Interpretation and confidence

`opennt-bop` now has no project-authored ingress or route implementation.  It
contains original source, registered source subsets/minimal modifications and
the registered private mirror boundaries only.  App owns final composition;
`adapter-bop` remains selector-blind.  The app move does not convert a
deferred SoftPC or Redirector provider into a claimed completed provider: the
existing source/failure dispositions remain unchanged.

Confidence is high for source ownership and the admitted focused paths.  This
does not by itself close S5: the remaining source-shaped facade/compatibility
headers and every registered overlay must still be reviewed against the
mirror-rule exception ledger.

## Follow-up

Perform the final per-input mirror/true-subset/minimal-modification/overlay
ledger audit, including the DPMI and VDMREDIR source-shaped compatibility
units.  Only then may T271/S5 close.
