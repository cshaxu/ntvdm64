# M0 T295 S1 — MVDM zero/first-degree frontier correction

## Objective

Correct the degree boundary after T294 demonstrated that T293's original
parser omitted valid OpenNT C/K&R definitions.  Rebuild the complete zero
degree from the selected `mvdm-host` original source corpus, then derive a new
first-degree ledger solely from calls whose callee is absent from that rebuilt
zero-degree definition set.

## Rules

- A function physically in a selected `mvdm-host` source file is zero degree,
  including a static, K&R or conditionally compiled definition.  Its spelling
  cannot be accepted as a first-degree external definition.
- A first-degree candidate must be absent from the rebuilt zero-degree set
  under the original linkage rules.  Same spelling never overrides a static
  file-local definition or silently merges variants.
- Parser support covers normal C and K&R function definitions and rejects
  prototypes and conditional call expressions.
- Universal Ctags is the recorded definition enumerator for the selected
  original C/C++ files; Node performs identity, linkage and direct-call
  extraction from the same provenance-selected bytes.  Ctags macro-form tags
  are normalized to their source-declared function name and its reported end
  line bounds the body.
- T294's result ledgers remain historical evidence of the prior method only;
  they are not an admissible first-degree input after this correction.

## Scope and non-goals

This package changes only audit tooling, degree ledgers, Status/Queue/history
and evidence.  It does not inspect a rebuilt first-degree body, implement an
adapter, import a package, alter Bochs, change BOP routing or consume
`src.old`.

## Exit criteria

1. Every selected MVDM source file has source/hash coverage and every valid
   normal/K&R definition has one zero-degree identity.
2. No rebuilt first-degree candidate resolves to a zero-degree definition
   under its linkage context; a machine assertion enforces this invariant.
3. T293/T294 closure records are corrected rather than silently overwritten;
   stale T294 output is marked superseded.
4. Documentation governance and `git diff --check` pass.
