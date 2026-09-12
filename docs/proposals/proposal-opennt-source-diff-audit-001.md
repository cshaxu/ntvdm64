# OpenNT MVDM and non-MVDM source-diff audit and restoration

## Objective

Audit all current OpenNT-derived host behavior and autonomous replacements,
including working COMMAND and EDIT paths, then implement source-proven
restoration of original OpenNT behavior and remove superseded autonomous
implementations. Audit and repair are phases of this same candidate, not
separate optional tasks. Preserve original observable contracts through the
smallest necessary modern bindings. This candidate is first in the queue;
its registration is not implementation
admission and has no allocated numeric task.

## Scope and baselines

- Freeze the repository revision and selected upstream provenance/hash for
  each comparison. Distinguish accepted product history from retained WIP.
- Cover `mvdm-host`, its overlays and SoftPC patches; separately cover
  non-MVDM `opennt-host`, its adapters, and replacements in app/session and
  all adapter families even when no upstream mirror file is linked.
- Include startup, BaseClient/BaseSrv, environment, DOS file and memory
  services, DPMI/WOW transitions, keyboard, mouse, Console/text video,
  BOP/callback returns, diagnostics and mirror-internal special cases.
- Use the selected OpenNT/OpenNT-4.5 provenance, not an undocumented
  opennt-src-2 substitution. External sources remain read-only audit inputs.

## Method and deliverables

For each functional unit, record original path/function, current provider,
actual build/link selection, direct caller, complete outgoing boundary,
retained or altered ordering/layout/failure rules and confidence. Classify:
original reuse, necessary thin binding, duplicate autonomous implementation,
unjustified semantic modification, diagnostic-only, or inactive code.

Report functional-unit counts, distinct files, and physical additions and
deletions separately for MVDM and non-MVDM. Include overlay code; report
non-comparable replacements separately rather than presenting whole-file
size as an exact upstream diff. Separate generated/declaration-only inputs,
line-ending-only changes and executable behavior. State comparison coverage
and unresolved provenance; do not equate every divergence marker with a hack.

Deliver a prioritized restore/replace/retain/delete ledger. Every proposed
replacement identifies the original owner, smallest missing modern binding,
superseded code, and COMMAND/EDIT/WRITE regression requirements. Kernel-origin
logic is not automatically excluded: assess finite semantic reuse under the
source policy and place accepted non-MVDM owners in `opennt-host`.

## Implementation and verification

After the source-owner and outgoing-boundary audit and owner approval of its
findings, implement each justified
restoration in its original mirror, bind only unavailable interfaces, and
delete the superseded replacement and obsolete build selection. Do not leave
parallel providers or relabel autonomous code as original source. Once this
candidate is admitted, in-scope source-proven repairs do not require a second
task admission merely because the audit phase has finished, but the owner's
explicit audit-result approval is required before repair begins.

Verify fresh Win32/x86 CCPU40 builds, focused positive/negative boundary
tests, COMMAND/EDIT keyboard and mouse regressions, and the recorded WRITE
frontier. Report before/after MVDM and non-MVDM diff/overlay counts, actual
restorations, retained bindings and exact unresolved boundaries. Preserve
provenance and original error behavior; a successful build alone is not
runtime acceptance. Commit/push reviewed deliveries and finish with a clean
Git worktree. Builds and logs follow the existing output-location rules.

## Non-goals and exit evidence

No new broker architecture, speculative guest/CPU patch, or invented
compatibility provider. Existing successful runs are regression baselines,
not source-fidelity exemptions. Completion requires reproducible coverage,
implemented source-proven restorations and regression evidence, not merely
an audit report or the initial seven findings. Any unrestorable boundary
must carry explicit evidence and disposition rather than silent omission.
