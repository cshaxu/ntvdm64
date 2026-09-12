# OpenNT MVDM and non-MVDM source-diff audit

## Objective

Audit all current OpenNT-derived host behavior and autonomous replacements,
including working COMMAND and EDIT paths. Identify where original OpenNT
implementation can replace project code without changing its observable
contract. This candidate is first in the queue; it is not implementation
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

## Non-goals and exit evidence

No source repair, guest modification, new compatibility provider or broker
implementation during this audit. Existing successful runs are regression
baselines, not source-fidelity exemptions. Completion requires a reproducible
coverage inventory and owner-by-owner findings, not only the initial seven
grouped findings. Replacement implementation follows owner review.
