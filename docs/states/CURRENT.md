# Project Status

## Current Work

**Active: M0 T483 S2 — restore original mirror code and structure.**

The owner authorized S1 closure and has now approved the internal T483 sequence:
S2 restore original (76), S3 delete (120), S4 minimize/consolidate (117),
S5 correct (207), S6 resolve missing evidence (755). See the
[category plan](../etc/operations/m0-t483-mirror-register.md#owner-approved-s2s6-remediation-sequence).
Owner now authorizes sequential execution and closure through S5. S2 is active;
S3–S5 follow only after preceding closure. S6 must not be admitted without a new
owner instruction.
The previously suggested compat-first S2 is superseded.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S2, Ordinary Mode; one agent, initial and counter-review passes. |
| Admission And Approval | Owner authorizes S2–S5 implementation in category order, with ten-modified-file batch deliveries; S6 explicitly held. |
| Objective | Resolve all 76 restore-original entries in the frozen S1 register, preserving original diagnostic line mapping and necessary product contracts. |
| Non-goals | No S6 admission, guest edits/rebuild, unrelated compat audit, or speculative original-owner expansion. |
| Reference Baseline | d43933ad2; S1 package 6ace4e9a-48ea-48ac-abd9-94e966b67c78; stable T483-D IDs and pinned originals. |
| Files And ABI Surface | The 76 S2 entries and their coupled declarations/callers/tests; no unrelated source changes. |
| Applicable Rules | Source policy, mirror subset/line rules, original contracts, native boundary judgments and category plan. |
| Verification | Original/current range and logical-line checks per changed file; applicable focused tests, cached full build, Full regression, publication hashes, governance and Git checks per batch. |
| Expected Markers | Every assigned ID resolved with implementation and counter-review evidence, or explicit owner-approved changed disposition; no unreported omissions. |
| Asset Needs | Existing pinned sources, build cache and maintained fixtures; preserve winnt.zip. |
| Reporting Requirements | Assigned/completed/remaining entries and files, batch distinct modified files, line evidence, source footprint, tests and publication/commit identities. |
| Stop Conditions | Do not force uncertain restoration over proven native ownership; report architectural tradeoffs or material expansion; no silent evidence-based reclassification. |
| Exit Criteria | All 76 entries disposed, necessary changes verified and delivered; scoped worktree clean; then admit S3. |
| Original Owner Request | Process S2 through S5 by their category checklist order; verify line mappings; deliver after ten modified files or exhaustion of each S; hold S6. |
| Similar-Issue Sweep | Complete all assigned ranges within each file once; inspect coupled callers and declarations without reopening unrelated retain/pending entries. |

## S2 Progress

Assigned: 76 entries in 21 mirror files. P1 delivered 20 entries at
`042d67f89`. P2 resolves 46 further entries: 45 restorations and one documented
S1 classification correction (D2233 retains four unconnected message-name
handlers). 66/76 entries resolved; ten remain across eight mirror files.

P2 changed ten production source/header files, plus two focused verification
files. Cached x86/x64 builds, 50 selected compiler-line anchors, RTL/list controls,
six original ThunkNLS controls and all 16 selected Full gates passed. Eleven
images published and hash-verified at both destinations. See
[P2 delivery](../../artifacts/mvdm-audit/t483-s2/batch2-delivery.json) and the
[execution ledger](../../artifacts/mvdm-audit/t483-s2/execution.json).
Next file: non-mvdm/windows/core/ntuser/client/dlgend.c. S6 remains held.

## Latest S Closure

[M0 T483 S1 closure](../etc/evidence/m0-t483-s1-closure.md) records the bounded
mirror-difference audit and P delivery. The new
[ordered register](../etc/operations/m0-t483-mirror-register.md) is the basis for
future repairs, in fixed file order and then line order. Historical IDs are
auxiliary provenance, not a second work queue.

- Fixed host C/H census: 1060 = 491 byte-identical + 259 exclusion-only +
  302 other differences + 8 deleted files. All 569 differing files classified.
- 3042 registered decisions: retain 1767, restore 76, delete 120, minimize 117,
  correct 207, pending evidence 755. Classification is not repair completion.
- 9596 lexical difference blocks: 9588 live plus 8 whole-file deletions;
  admission baseline 9636. These are not semantic-defect counts.
- 365 formatting restoration sites in 249 files (122 C/H, 127 other files).
  Eight proved unnecessary files deleted; selected build graphs reference none.
- No original guest edits or rebuild; assets/winnt.zip unchanged.

## Current Technical Baseline

Package `21d104eb-b607-4a3f-99e6-00828b215b07` (S2 P2): cached x86/x64 build and all 16
selected Full gates passed. The external DOS4GW/NESticle fixture was not supplied
and is explicitly skipped, not passed. All 11 images match the manifest in
assets/release and O:/winnt/system32. See the
[delivery evidence](../../artifacts/mvdm-audit/t483-s2/batch2-delivery.json).
No binary-equivalence claim to P1 is made.

## Remaining Scope

Functional recommendations and evidence gaps remain in the new register.
Modern Windows API ownership can be the correct original closure boundary;
original omissions alone do not justify restoring code. Subjective boundary
choices require owner review. Compat and product-component minimization are
later work under the
[admitted T scope](../proposals/proposal-original-source-implementation-minimization-review-001.md).

The DOS identity/wrong-waiter defect and Win95 exit-code boundary remain in
[TODO](TODO.md). Win95 restart-to-DOS belongs to the
[XP baseline-upgrade package](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md).

Parallel-session QUEUE.md edits and the win203 setup proposal are preserved and
excluded from S1 delivery.

## Recent M0 Closures

- [T483 S1](../etc/evidence/m0-t483-s1-closure.md): mirror audit, bounded cleanup and verified delivery.
- [T482](../history/m0-t482-closure.md): investigation transferred, not repaired.
- [T481](../history/m0-t481-closure.md): guest multitask delivery with explicit boundaries.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md). S1 closure preserves the
parallel-session queue and proposal changes.
