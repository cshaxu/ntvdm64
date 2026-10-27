# MVDM cross-audit: first parallel review

This is an in-progress S10 preparation record, not S9 closure or T acceptance.
The owner authorized four parallel roles; three agents and the primary session
used the disjoint frozen path shards described by `manifest.json`.

`frozen-file-reconciliation.csv` freezes the 4,684 unique MVDM union paths.
`combined-initial-status.csv` is the current merged index; detailed per-file
reasoning and limitations are in each shard directory. Current counts are
224 partial (205 existing source-difference rows and 19 partially integrated
build manifests), 626 excluded (documented profile, historical recipe,
source roles or unused retained objects), 23 full selected leaf files and
3,811 pending. Exclusion of an object or provider does not prove its guest
functionality works; see the WOW32 object audits for [shard 0](shard-0/historical-wow-objects.md),
[shard 1](shard-1/historical-wow-objects.md), [shard 2](shard-2/historical-wow-objects.md)
and [shard 3](shard-3/historical-wow-objects.md).
The [pending-role census](pending-role-census.md) separates initial inventory
hints from final dispositions; it does not remove any pending path from scope.

The [build-carrier review](build-carrier-review.md) distinguishes configuration
reads from Ninja execution dependencies. Its 100 additional exclusions concern
historical recipe files only, never the capabilities of their source members.
The [19 partial build carriers](partial-build-carriers.csv) have an explicit
non-numeric estimate basis; executable code-retention measurements for the
existing 205 source rows are unchanged.

The [architecture-header checkpoint](architecture-header-review.md) accounts
for 125 additional nonselected MIPS/PPC declaration variants without excluding
their common algorithms or the selected C-VID/x86 declarations.

The live hash sweep records divergence-registry changes in
`src/mvdm/README.md`, `dpmi32/modesw.c` and `nt_bop.c`; see
`initial-summary.json`. These are observed baseline drifts, not
accepted runtime repairs from this ALU audit. This review changed no product
source or guest media. `frozen-external-audit-rows.csv` preserves 42,963 extra WOW audit
paths outside the MVDM union for relevance and dependency review.

Review breadth differs by shard. Shard 0 has metadata for 62 diff files, nine
source-level detailed reviews, initial source-manifest scans of 43 unselected
host files and a focused generic-thunk branch analysis. Shard 1 has 48
differential owner initial reviews, 34 unselected-role reviews and selected
owner analysis for 73 of 73 pending product sources. Shard 2 has recovered
the previously interrupted CSV, renewed initial traces of all 48 diff files
and checked 24 CCPU and 10 WOW32 product-source linkages. Shard 3 has 47 differential,
23 unselected-role and 73 selected-source initial reviews, plus a FastWOW
comparison. These investigations do not replace semantic or runtime proof.

Read `findings.md` for actionable cross-group discrepancies. Source-line
matching percentages in the frozen input are textual heuristics, not the
requested functional restored-code percentage. Calculate and justify the
latter only when original owner, selected edition and actual behavior are
known. Regenerate the combined index after any shard changes and verify
uniqueness, shard ownership and frozen identity before relying on counts.

`external-priority-boundaries.csv` retains the original audit's 93 explicit
non-MVDM boundary rows, including its reasons and reviewed slices. The
remaining 42,870 external rows were still marked provider-membership pending
by that audit. Neither category is automatically admitted as product code
or discarded; each requires relevance and current-owner analysis. The
accompanying summary records the exact source-catalog hash and the narrow
local-path comparison; a missing same-shaped path is not proof that no
modern host or differently located adapter supplies a capability.

The first separately traced external provider row is the WINMM callback
layout: see `external-winmm-callback-boundary.md`. It remains pending
behavioral and compatibility verification.

Shard 0 additionally records historical link-response and include evidence for
28 pending COMMAND guest files in `shard-0/command-guest-build-evidence.csv`;
this does not identify or validate the deployed guest binary.

A separate pending relevance census for the external rasprint tree is in
`external-rasprint-boundary.md`; it does not bulk-exclude driver resources.

All 205 current partial paths have a reproducible source-text retention
metric in `partial-code-retention.csv`; its limitations are in
`partial-code-retention.md`. It is not a functional completeness percentage.

The [provisional recovery worklist](recovery-worklist.md) groups evidenced
findings by approved stage while keeping unresolved inventory in S10 scope.

The [additional historical-build cohort](shard-0/historical-objects-reconciliation.md)
excludes 75 verified retained output files as current product inputs only.

The [kernel VDM boundary trace](external-kernel-vdm-boundary.md) separately
identifies the directory-reset service and its selected DEM fallback.

Further history-only output reviews are [shard 1](shard-1/historical-object-batch.md), [shard 2](shard-2/historical-i386-artifacts-review.md), and [shard 3](shard-3/historical-i386-binaries-review.md); each excludes the retained file, not its capability.

The dormant original [multimedia host body](shard-2/wow-multimedia-inactive-body-review.md) is excluded as a source-file body, not as a guest multimedia capability.

The focused unselected-source records are [shard 0](shard-0/nonruntime-source-disposition.md), [shard 1](shard-1/unselected-finite-worklist.md), [shard 2](shard-2/unselected-host-role-disposition.md), and [shard 3](shard-3/unselected-host-tooling-disposition.md). Independent component nonselection alone is not a capability exclusion.

The [23 full selected leaf contracts](full-selected-leaf.csv) combine
unchanged original source, actual published code/decoder targets and
[real guest positive and negative evidence](ccpu-sign-flags-contracts.md).
Their status does not close the broader CPU decoder or execution owners.

The [twelve arithmetic/logic source contracts](ccpu-alu-contracts.md) add
native-reference data and 3,260,544 actual guest instruction checks across
register and memory forms. Their full status leaves decoder/fault owners open.
