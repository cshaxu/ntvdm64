# M0 T271 S5 P8 — SoftPC mirror-boundary remediation

## Question

Can the P7 non-original SoftPC fragments be removed from the `opennt-bop`
mirror root without changing their reached behavior, and can the remaining
interrupt-06 machine action state be assigned to its actual mechanical owner?

## Inputs

- P7 family compatibility/overlay ledger.
- Pinned OpenNT source references named in the prior BOP divergence register:
  `nt_mouse.c`, `printer_.c`, `keybd_io.c`, `nt_bop.c`, `nt_umb.c`,
  `illegalp.c` and `unexp_nt.c`.
- The formal component manifest and the five affected focused fixtures.

## Procedure

1. Used `git mv` to move the source-fragment bodies behind
   `opennt-bop-overlay/softpc/`.
2. Added only original-source-unit-named mirror translation units:
   `nt_mouse.c`, `printer_.c`, `keybd_io.c`, and `nt_bop.c`.  Each is a
   declared private include boundary; `nt_bop.c` contains both independently
   reached source fragments and explicitly ends the first facade's macro
   scope before including the second.
3. Moved the self-authored typed interrupt-06 RAM/port-operation state
   machine from `opennt-bop` to `adapter-softpc/interrupt06_provider.{c,h}`.
   It is selector-blind mechanical state, not an OpenNT source mirror or BOP
   provider.  Updated its app consumer, focused test and formal manifest.
4. Replaced remaining task-labelled or free-form mirror divergence comments
   with stable registered `BOP-DIV-*` markers; added `108`--`111` to the BOP
   register and `SOFTPC-DIV-007` to the mechanical adapter register.
5. Corrected the focused keyboard fixture's formal link closure to include
   its required public `user32.lib` dependency, regenerated the formal Ninja
   graph, and rebuilt/ran the affected matrix.

## Observations

- No source below `opennt-bop` or `opennt-bop-overlay` retains a task-labelled
  or unregistered `DIVERGENCE` comment.
- No non-mirror production component includes `opennt-bop-overlay`.
- Formal Ninja rebuilt the changed `adapter-softpc`, `opennt-bop` and app
  objects.  The following executable matrix linked and exited zero:
  `interrupt06-provider-fixture`, `t245-s5-headless-mouse-vector-fixture`,
  `t245-s7-printer-openclose-fixture`, `t243-s2-spckbd-handoff-fixture`, and
  `t245-s11-config-complete-fixture`.
- The interrupt-06 boundary script and the production-tree gate both pass.

## Interpretation and confidence

The P7 placement/registration defects are remediated.  Original-source-name
mirror files now carry only small registered linkage boundaries; the larger
source-derived fragments remain private to those boundaries.  The typed
interrupt-06 action machine is now owned by the selector-blind mechanical
adapter, while app remains responsible for source-owned entry composition.

Confidence is high for layout, linkage and the five reached paths.  This does
not claim recovery of any unadmitted SoftPC device product shell, DPMI,
Redirector or WOW provider.

## Follow-up

Repeat the full S5 per-input ledger with this remediation applied, then run
the DEM/COMMAND/XMS source-path matrix and all governance gates before S5
closure.
