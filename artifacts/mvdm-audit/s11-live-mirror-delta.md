# S11 live mirror reconciliation checkpoint

The previously tested unpublished debug-context candidate was reconciled with
`EstimateMirrorDiffRetention.py`: 206 differing original-source rows, 205
with textual estimates, and a join against `combined-initial-status.csv`
found all 206 classified `partial`. The current retention CSV SHA-256 is
`B429211C1E6AA43C4D1DC4AD022CCAA953D3CF85A18EE9F4418EA866226C1FF4`.
Among the current candidate's mirror files, `c_main.c` retains an estimated
3,885 of 3,923 original code lines (99.03%) and `ntthread.c` 195 of 198
(98.48%). This is current source-text matching, not proof of execution,
semantic equivalence, test coverage or an S11 completion claim.

After the external-debugger TLS experiment was held for its complete S18
integration, the generator again reported 206 differing rows, 205 measured,
and all 206 marked partial. The current CSV SHA-256 is
`5D8C96108E0979551D94E734707302C22503CD3D8469D81BFB9F1FCF6F8D17AA`;
`ntthread.c` is again at the previously committed source and its textual
estimate is 196/198 (98.99%). The separate historical diff in that file
remains classified partial.

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

The current CPU-monitor candidate was reconciled again: the retention generator reports 205 mirror-difference rows, 204 with textual estimates; the C/H checks inventory all 1,794 current paths, report 200 host C/H differences, all partial, no unlisted differences or stale estimates, and seven absent paths already classified as excluded. The generated CSV SHA-256 is `AE3176BCE3872ACF23965EC121054B122E7A09BF956E5B0DCABB54927D2AA1F0`. These numbers describe this unpublished worktree, not proven runtime restoration. Raw output is in `build/t473-s11/cpu-monitor-{inventory,coverage}`.

After restoring `c_tlb.c`'s original LF representation, the retention generator again reports 205 rows and 204 estimates; the current CSV SHA-256 is `ED6E68AA2392A2A71F16046BD817E7E780E25C59742A916D83E252C62C69E72B`. The previous hash above records the pre-normalization candidate.

Following correction of duplicate exception identifiers, the regenerated current CSV SHA-256 is `96850EA9E952DF46851C84B43E4172B2B648B6AAC5B560D99C79FC8C73C3442F`; row and estimate counts remain 205 and 204. The identifiers are now DIV-473 through DIV-476, distinct from the previously assigned 312–315.

After the duplicate-TLB mapping correction, the same generator reports 205 difference rows, 204 textual estimates, and current CSV SHA-256 `910B062D47279ABD9A4B385577A14097CDB5D3C016A33A282DFB5A27A7D25484`. This updates the earlier worktree hash; it does not represent completion of runtime recovery.

After the protected-mode prefix and LMSW source alignment, the generator again reports 205 rows and 204 textual estimates. The current CSV SHA-256 is `A2F20DA7AA87BEB9A068330ED35E56748DAF8A3B96BA0C118BE2E0FE5A0ACBA5`; all previous hashes above describe earlier worktree states.

The current post-alignment worktree was checked again (`build/t473-s11/cpu-monitor-coverage-r5`): 1,794 C/H paths, none unlisted; 200 differing host C/H files, all partial; no stale estimates; seven absent paths already marked excluded. These are source-inventory and textual-estimate results, not functional acceptance.

After bounded string-I/O storage replaced the two heap allocations, the regenerated worktree CSV SHA-256 is `9B3E8A2140B90CD7E374148BC5B29194BBE45F3EE598D7631946DF9BB54AF595`; it contains 205 mirror-difference rows, of which 204 have textual estimates. The earlier coverage run predates this edit and is not the final current-worktree check.

The subsequent current-worktree coverage check (`build/t473-s11/cpu-monitor-coverage-r6`, exit 0) lists all 1,794 current C/H paths, finds no unlisted differences or stale estimates, and classifies all 200 host C/H mirror differences as partial. Seven absent monitor files retain their recorded excluded disposition. This is a source coverage check, not proof of behavior or stage closure.

After preserving existing guest bytes in the string-I/O input buffer, the retention generator reports 205 mirror-difference rows and 204 textual estimates, with CSV SHA-256 `9B26E48B325423515F41042ED3AC83FB7E7C93F367667769A0297893B448DBCF`. The previous coverage result predates this last edit; the current-worktree check must be rerun.

The post-fix check completed successfully (`build/t473-s11/cpu-monitor-coverage-r7`, exit 0): all 1,794 current C/H paths are inventoried, no unlisted differences or stale estimates exist, and all 200 differing host C/H rows remain partial. Seven missing monitor paths retain their excluded dispositions.

The subsequent unpublished LMSW address and cross-page corrections retain 205 mirror-difference rows and 204 textual estimates; the regenerated CSV SHA-256 is `EC3A932B26738B9BA68D6DA3D3D4C58290899A3E27703071394C33BB99E6EE00` (`build/t473-s11/lmsw-inventory`, exit 0). The preceding coverage check describes the published baseline and must be repeated for this candidate.

The candidate's own coverage check subsequently passed (`build/t473-s11/lmsw-coverage`, exit 0): all 1,794 C/H paths remain inventoried; 200 host C/H differences are partial, none are unlisted or stale, and the seven absent monitor paths retain excluded dispositions.

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

The published DIV-473 host boundary subsequently removed its duplicate PDE/PTE permission calculation. Selected CCPU source `c_tlb.c::xtrn2phy` owns this decision; two mixed-permission fixture cases compare that API with monitor execution. The existing mirror exception remains DIV-473. Current retention CSV SHA-256: `5C378BF80453AC5FF41F9D373AC817A339C0472283883939A79E9CD1306AE3A8`.

DIV-473's string-I/O candidate now passes the validated guest buffer directly to IOS when the mapped bytes are contiguous; its controlled callback witness sees the guest write before returning, unlike the earlier staged-only path. Noncontiguous mappings still use staging and remain a reentry-semantics boundary. Current retention CSV SHA-256: `55CB0C16732D9048974AA98998501DC839F4D86BDEB73CA3DD44DAD10355E444`.

DIV-473 now retains original 32-bit segment-base addition for monitor instruction fetch and string I/O. This removes two adapter-only rejection branches; full segment, linear-end and failure behavior remains partial. Retention CSV SHA-256: `0D7FEE3BA2CBA09FFFCA1A8F1EF6549B6FA89F752C15FC4DF22D129B076DB3E9`.

DIV-473 string I/O now resolves its selector through the existing SIM32-compatible protected-address provider and removes adapter-only segment access/limit restrictions. The original monitor's user event translates the current descriptor and the controlled CCPU cases cover descriptor mutation, read-only ES, small limits and an offset-FFFFh word. The earlier provisional case-28 assertion was updated, not removed. Null/invalid selector error disposition and 4-GiB end crossings remain part of the outstanding exception boundary. Current retention CSV SHA-256: `CFF3C39AF74A4D1464721926896CCFDD48720D723830F29D54ACC7B7B6EAA858`.

The subsequent DIV-473 candidate also handles nonlocal CCPU exit during descriptor lookup: an induced page fault no longer leaves the original contributory-fault flag set and spuriously turns the next I/O into #DF. The flag is restored if lookup returns normally. Cases 73–75 cover fault, subsequent I/O and failed-lookup restoration. Latest retention CSV SHA-256: `8DD46A1CA2D063907EE99DAC70109CADB6BC8D50AFA014DFDE069E37D5349625`. Exact NT4 user-exception delivery remains open.

## S11 monitor minimization refresh

The regenerated source inventory SHA-256 is `3C6AB0E182142C0F4C5BBA001F1F8FD1F4E57BACC12A65F7DB37E39C841A55D9`. The retention check reports 201 differing host C/H rows, all classified partial, with no missing or stale rows. The newly differing `ios.c` enables existing original IOS routines for the selected CCPU. Detailed before/after line accounting and open semantic boundaries are recorded in [monitor minimization](s11-monitor-minimization.md).
