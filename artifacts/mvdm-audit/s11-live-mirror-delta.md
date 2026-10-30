# S11 live mirror reconciliation checkpoint

The frozen S10 host C/H ledger classified every one of its 202 recorded
mirror-difference rows as partial. Rechecking the current worktree against
that frozen checkpoint now reports one newly differing file and six stale
estimates; all seven paths remain classified partial in the existing host
inventory. The checker also reports seven missing monitor paths, each already
excluded in the frozen inventory. The inventory contains all 1,794 current
C/H paths, but this path check does not refresh its earlier build/dependency
observations or prove functionality.

The [live delta](s11-live-mirror-delta.csv) binds the seven paths to their
current hashes and verifies the primary original hashes. Four paths now match
the original bytes after the separately committed P1 restoration; retain
their prior partial source-role decision until behavior and selected profiles
are verified. Three remain different: `c_oprnd.h` is a new difference and
`c_xfer.c` and `sas.h` have changed since the frozen estimate. All three
remain partial. Their matching original-code-line estimates are 99.72%,
100.00%, and 99.87% respectively; 100% here means the original code lines
still match under the estimator, not that the file is byte-identical or that
all behavior is restored. The frozen table has not been rewritten to pretend
these worktree candidates were already delivered.

The first checks passed for the 202 frozen rows but the current coverage
checker returned nonzero for the expected delta above. The existing
`EstimateMirrorDiffRetention.py` generator was then run against current
worktree bytes and verified original hashes, without changing the frozen
source ledger. Its regenerated table contains 205 differences overall,
204 with textual code estimates. Both checks subsequently passed: 199 host
C/H differences remain partial, all 1,794 current C/H paths are in the
inventory, and there are no unlisted host differences or stale estimates.
The seven previously recorded excluded monitor paths remain absent. This is
a worktree-bound source reconciliation, not build-selection refresh or
behavioral completion. Regenerate after any further source change, including
when the S11 candidate set is finalized.

Current worktree C/H reconciliation was rechecked after the test-only edits:
1,794 existing C/H paths are listed, no unlisted changed paths or stale
retention estimates were reported, and all 199 identified changed host C/H
rows are partial. Seven absent monitor paths are individually recorded as
excluded. The two check scripts and input hashes are captured in
`coverage-check-hashes.json`. Their success is a source-inventory check only:
recorded `full` rows are source-role decisions, not proof of runtime behavior;
S11–S19 still own the matching capability and lifecycle acceptance. Retention
is a textual matching estimate and not an execution coverage percentage.

After the supplemental export-definition mirror was restored, the current
retention generator passed with 204 difference rows (203 textual estimates),
one fewer than the previous 205. The host C/H check remains 199 changed rows,
all partial; all 1,794 current C/H paths remain listed and no stale host
estimates or unlisted host differences were reported. The restored definition
is outside that C/H-only count. These checks classify source differences, not
runtime restoration or exported ABI compatibility.

The current C/H classification and frozen coverage checks were rerun after the test-only menu negative: 199 changed host rows are partial; all 1,794 current paths are inventoried, with no unlisted differences or stale estimates, and the seven absent paths have excluded decisions. [Outputs and input hashes](s11-foundation-evidence/reconciled-boundary/mirror-inventory/) record this source check; they do not refresh build selection or prove capabilities.

After the Calculator test-only changes, `python tools/audit/EstimateMirrorDiffRetention.py` completed successfully against current worktree bytes. Its regenerated CSV reports 204 differing rows overall, of which 199 are host C/H; an independent join with `combined-initial-status.csv` found all 199 marked partial and all 199 have textual estimates. The 204-row CSV SHA-256 is A7DB47B6524BF34704B8B05BD7AE62F13FDEDFB866B0BCD905207CC4E5600F99. This is a diff-row and ledger consistency check, not a fresh full path-census, runtime test, or behavioral percentage.

The retention generator was rerun after the current host-adapter and
Alt+Enter changes. It again passed with 204 mirror differences (203 textual
estimates) and the same CSV SHA-256. Its assertion requires every differing
mirror row to be classified partial. This revalidates the current textual
comparison and source hashes only; it does not close pending capability or
guest-target decisions, refresh selected-build ownership, or prove runtime
behavior. The retained excluded/missing paths and current-drift list remain
in the generated summary.

A fresh whole-inventory path census compares all 1,815 host C/H inventory paths with the 1,794 current `src/mvdm` C/H files: zero current files are unlisted, and 21 inventory paths are absent, all already classified excluded or 整体排除. Seven are monitor paths; the other 14 are scaffold candidates. The earlier seven-path check covered only its frozen-original comparison subset and did not count the scaffold inventory. [Full census](host-ch-scope/current-path-census.json) records each absent row and the inventory hash. This confirms path accounting, not correctness of the exclusions or restored behavior.

Normalizing Chinese and English status labels in the same 1,815-row host C/H source-role inventory yields 53 full, 1,116 partial, 451 excluded and 195 pending rows ([count and input hash](host-ch-scope/current-status-counts.json)). These are frozen audit dispositions, including the 21 currently absent paths, not a functional coverage percentage. In particular the 195 pending rows prevent claiming every file has received a final full/partial/excluded disposition; S11–S19 must resolve them against actual owner and original capability, rather than mechanically relabeling them.

Correction to the preceding normalized-status paragraph: all 195 pending rows carry scope `guest-target-observed`; none are in the inventory's `host-candidate`, `role-pending`, or `tool-or-scaffold-candidate` scopes. See [pending scope breakdown](host-ch-scope/pending-scope-breakdown.json). Thus the paragraph's claim that these 195 prevent final classification of all host C/H paths was too broad. They remain T-wide capability/guest-boundary handoffs as applicable; the scope label alone proves neither guest exclusivity nor runtime behavior. The counts remain counts of the mixed-scope inventory, not 195 pending host-candidate files.

The mixed inventory is now cross-tabulated by source-role scope and status in [status by scope](host-ch-scope/current-status-by-scope.json). Its 1,270 host-candidate rows comprise 876 partial, 341 excluded and 53 full; the 278 `role-pending` rows are separately 183 partial and 95 excluded. `role-pending` describes unresolved role even though a provisional status exists, so it must not be reported as resolved simply because it lacks a `pending` status label. The 72 tool/scaffold and 195 guest-target rows remain distinct. Scope and status are audit decisions, not runtime capability or proof of safe exclusion.

The 278 `role-pending` rows group by original source path into 38 `dos/` and 240 `wow16/` entries ([row-level listing](host-ch-scope/role-pending-tree.json)); none is under the SoftPC C-VID/CCPU or DPMI source directories. This is only a routing aid: these path names do not by themselves prove guest-only status, exclude host consumers, or remove any original capability obligation. The separately captured guest-header review and its differing snapshot counts must be reconciled before changing any row.

The original COMMAND message-builder source and its country header are now
traced as guest-build tooling, with a bounded host-excluded recommendation;
the executable guest behavior remains assigned to S12. [Call chain, exact
hashes and selected-graph limitations](host-ch-scope/command-buildmsg-boundary.md)
are recorded without silently rewriting the frozen mixed-scope inventory.

The 30 `dos/v86/h` provisional entries now have a per-file hash and selected
worker/WOW32 dependency [census](host-ch-scope/dos-v86-header-dependency-census.md).
The observed guest source/build-input role does not settle unbuilt configurations
or remove the S12/S13 runtime capability obligations; frozen labels remain
unchanged pending broader source-role reconciliation.

The 240 provisional `wow16/` C/H entries now have a per-file selected-graph
and original-hash [census](host-ch-scope/wow16-role-pending-census.md). It
also records why neither a WOW16 path nor a matching original hash determines
host/test applicability: the DDE stress source includes a WIN32 build, and
the fax source exposes a shared guest/host provider boundary. No provisional
labels or later DDE/fax capability obligations were removed.

After ADAPTER-WOW-108 and the updated focused fixtures, the worktree-bound
retention generator passed again: 204 current mirror differences, 203 textual
estimates, CSV SHA-256
`A7DB47B6524BF34704B8B05BD7AE62F13FDEDFB866B0BCD905207CC4E5600F99`.
Its partial-diff assertion is source classification only; it is not proof of
runtime restoration. The new adapter and test are outside the mirrored tree.

The later current-worktree generator rerun at `f0880afb7` passes with 204
differing files, including 199 C/H files. An independent join against the
source ledger finds all 199 C/H rows `partial` and all 199 have a textual
estimate. Four other differences have source-text estimates and one build
input has no applicable code-text percentage. The current generated CSV
SHA-256 is `D2DB2B75B732244295375FC383A1F94641AF396FDEE0D9F6090532F796EE92EF`;
the earlier A7DB value is a historical snapshot, not the current artifact's
hash. The generator checks its frozen original identities; it does not
refresh profile selection or establish runtime behavior.
