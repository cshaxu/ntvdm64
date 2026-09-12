# M0 T405 audit and restoration S plan

## Authority and sequence

The owner admitted the complete audit-and-repair task on 2026-09-12.
CURRENT alone activates a packet. S1 has reached its bounded inventory
conclusion and S2 its MVDM disposition conclusion; CURRENT now activates only S3. The remaining rows are
a sequential plan. Record each bounded S conclusion before advancing within
this authorized T. One agent first executes and then rechecks the evidence
as reviewer; no parallel agent is implied. The updated owner objective
requires approval of audit findings before any repair. S4/S5 remain within
this T, but do not activate before that approval.
Refine dependency order from original-owner evidence, not the next trace hit.

The owner explicitly requires a stop after the complete S2/S3 audit. Present
the combined mirror-diff, overlay and replacement ledger for individual
discussion, separating confirmed duplicated functionality from necessary
bindings and unproven changes. Do not automatically start repairs or treat
standing commit/push permission as approval of a semantic restoration.

| S | Bounded objective | Exit deliverable |
| --- | --- | --- |
| S1 | Freeze baseline, provenance, counting rules and audit coverage. Separate unaccepted snapshot, selected source, build selection and actual runtime identity. | Reproducible inventory, coverage denominator, source mapping and unresolved-input list. |
| S2 | Audit MVDM/SoftPC internal diffs, overlays and adopted patches, covering COMMAND/EDIT input/display, file/memory, BOP/return and DPMI/WOW. | Per-owner semantic ledger, diff counts and restoration candidates with original source, missing interfaces and regression requirements. No product repair yet. |
| S3 | Audit non-MVDM owners and replacements wherever located: BaseClient/BaseSrv, kernel VDM, RTL and Console boundaries. | Reuse/retain/replace/delete decisions and dependency-ordered implementation list joined to S2 without double counting. No product repair yet. |
| S4 | Restore source-proven non-MVDM slices in opennt-host; adapters bind only unavailable interfaces. Remove superseded providers. | Original bodies enter the actual link; package builds, positive/negative boundary tests and affected runtime regressions pass. |
| S5 | Restore source-proven MVDM/SoftPC behavior and reduce overlay/special-case footprint using S4 dependencies as needed. | Original package shapes restored; justified CCPU40 bindings retained; package builds and affected COMMAND/EDIT/WRITE regressions documented. |
| S6 | Re-audit before/after counts and verify the integrated product; reconcile provenance, graph and documentation. | Fresh x86 CCPU40 build, COMMAND/EDIT keyboard/mouse and execution regressions, explicit WRITE frontier, residual debt, reviewed commits/push and clean Git worktree. |

## Common controls

Follow original source, smallest same-shaped binding, registered intrusion,
then exceptional autonomous behavior as last resort. Kernel provenance alone
does not exclude finite semantic reuse and does not admit a kernel product
shell. Do not move autonomous code into a mirror and call it original.

Count functional units, files and physical additions/deletions separately for
MVDM and non-MVDM, including overlays and substitute bodies. Distinguish
non-comparable replacements, inactive inputs, declarations and diagnostics.
Working COMMAND/EDIT are regression baselines, not audit exemptions.

Preserve guest media and the positional public command. Builds belong under
build/M0-T405/Sn/run-id; runtime logs remain under O:\ntvdm64\logs. S4/S5 may
use sequential P deliveries for coherent original-owner packages, never
trace-selected symbol-by-symbol recovery. WRITE depth is measured evidence,
not proof of complete WOW acceptance. The T cannot close on a report alone:
implement and verify justified restorations; explicitly dispose of genuinely
unavailable boundaries rather than silently omit them.
