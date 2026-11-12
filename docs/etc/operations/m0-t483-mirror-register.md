# Mirror difference register migration

Current execution follows the owner-approved
[remaining-diff re-audit](../../../artifacts/mvdm-audit/t483-remaining-reaudit/README.md).
The category execution below is historical after S5 P2. The old D-register
remains predecessor evidence; it is not a second active audit queue.


## Owner direction and scope

This round audits mirror differences only. Later rounds address compat and
then the eleven product components. Only owner-approved, proved formatting-only restorations may be applied
during this review; substantive product edits are not authorized here except
for the following owner-approved whole-file disposition. Current owns the active packet.

Owner clarification: when the audit proves an entire file should be deleted,
execute that deletion in this review rather than leaving a recommendation.
First verify the registered whole-file conclusion and actual build, include,
resource and test references; reconcile affected references and provenance
records with the deletion. A recommendation to remove an include directive,
a function or a diagnostic block is not authorization to delete its source
file. Pending evidence is not a proved deletion. Preserve unrelated changes.
Report deleted-file counts separately from restored formatting and retained
mirror classifications. At this clarification, review of the existing ordered
ledger and 1412 register entries found no confirmed whole-file deletion;
existing delete decisions concern local declarations, includes or code blocks.

## Order and authority

The [file order](../../../artifacts/mvdm-audit/t483-s1/file-order.csv) follows
actual Windows `dir /s /b /a-d /o:n` output: mvdm-host, then non-mvdm.
Guest entries are listed separately for preservation, not modification.
Each file is reviewed fully before advancing; dependencies may be read as
evidence without becoming a separate out-of-order audit. Non-excluded code
changes are registered in ascending current line order, with original ranges.
Pure exclusions/line scaffolding, format differences and non-source records
remain separately classified and do not inflate semantic review completion.

The [successor register](../../../artifacts/mvdm-audit/t483-s1/diff-register.csv)
is the working authority for the completed per-file classification and future
remediation order. Historical README exception lists remain auxiliary provenance,
not a parallel execution queue. Preserve their rationale, source identity,
ABI/lifetime/error constraints and evidence links. Old-ID associations remain
explicitly incomplete in the crosswalk; do not claim full historical-range
coverage or delete the old evidence. Delivery verification remains separate.

## Review fields and evidence

Record source/current hashes, both ranges, old IDs, initial necessity analysis,
counterargument/review, disposition, evidence and unresolved limits. The same
agent performs both review roles; this is not an independent second person.
A file is complete only when all its differences have explicit dispositions.
The [initial batch](../../../artifacts/mvdm-audit/t483-s1/first-dir-batch.json)
is provisional evidence, not a ten-file completion claim.

## Frozen starting evidence

At 0fef12d013b72a5a2a768fb7ab7f1b6b4a9affdb the host C/H census contains
1060 files: 471 byte-identical and 589 differing, with 9636 lexical blocks.
All 1060 original pins match. These counts exclude other extensions and guest;
lexical blocks are not semantic changes. The directory inventory includes
1383 host entries and 2704 guest entries, including metadata and artifacts.
No build, runtime pass, publication or product correction is claimed here.

## Restoration reporting

Each completed-file report also states applied restoration sites and distinct
files, separately from retained lexical diff blocks and recommended corrections.
A whole-file newline restoration counts as one site; a contiguous comment/layout
restoration counts as one site. C/H and other mirror files are distinguished.
The [restoration evidence](../../../artifacts/mvdm-audit/t483-s1/format-restorations.json)
is delivery evidence, not a retained mirror exception list.

## Executable remediation register

This register is the input to later item-by-item repairs, not a read-through log.
Every item must distinguish these outcomes:

- Retain: state necessity and evidence; no repair required for this item.
- Restore original / delete / minimize / correct: specify exact code locations,
  concrete changes, coupled declarations/callers and required verification.
- Pending evidence: state the missing evidence and how to obtain it. Do not
  mix an unproved risk into the confirmed repair list.
- Restored: implemented format-only restoration belongs in restoration evidence,
  with changed files and identity checks; do not schedule it again as a repair.

Audit completion and repair completion are independent. File audit completion
means all differences have dispositions; it never means its recommended repairs
were implemented. Existing mixed labels such as retain-with-boundaries must be
reconciled into a justified retained part plus a separately stated unresolved
question or confirmed correction, without inventing proof. Existing findings
must gain concrete caller and validation requirements before repair execution.
All 3042 currently recorded entries carry these execution fields; all fixed-census
files have classifications. Pending findings and delivery verification remain open.

Later implementation follows these entries in order and records changed revision,
verification evidence and implementation status per item. No functional repair
is currently implemented by T483 S1; only explicitly recorded format restorations
are applied. Unresolved evidence does not silently become either a repair or a
claim that the behavior is safe.

## Native host boundary decisions

Owner clarification: omission of original code is not itself a defect. A modern
Win32 API can be the correct and minimal boundary of the MVDM dependency closure.
For each such difference, compare responsibility, reached parameter/result/error
contracts, state and lifetime ownership, and the cost of retaining original code.
Do not default to restoring original implementation or require identical internal
implementation where the host owns the operation. A justified native boundary is
a retain decision. Missing contract evidence remains pending, not a confirmed
repair. If both architectures are defensible and the decision depends on product
policy, present concrete tradeoffs to the owner before implementing either.
Apply this criterion to earlier entries as well as subsequent files; source-first
review does not mean restoring private NT host implementation indiscriminately.

## Current execution order and historical evidence

Owner clarification after per-file classification: future remediation follows the
new register. The subsequent owner-approved category sequence below takes
precedence across categories; fixed file/line order remains within each category. `audit_id` values T483-D0001 through
T483-D3042 identify its current entries. Preserve these identifiers during repair;
do not renumber completed/removed entries. Original IDs are provenance aliases,
not execution ordering or additional repair work. Snapshot line coordinates must
be interpreted together with file, original range, classification and evidence.

All 1060 host C/H files now have a census disposition. Historical-ID migration
must not be described as unreviewed files or newly discovered defects. The old
README records remain intact as auxiliary evidence while range associations are
reconciled; no missing alias alone invalidates an independently source-backed
new finding. Conversely, an alias match does not prove semantic coverage. Do not
silently discard old rationale or claim unfinished mappings are verified.

The categorized result is in the [classification summary](../../../artifacts/mvdm-audit/t483-s1/classification-summary.md).
The [integrity check](../../../artifacts/mvdm-audit/t483-s1/register-integrity.json)
records unique identifiers, ordering and required-field coverage. This is a
register check, not build/runtime acceptance or functional repair completion.

## Owner-approved S2–S6 remediation sequence

After S1 closure, the owner requests one S for each non-retain category in the
reported category order. This supersedes the original proposal's S2–S5 family
sequence and the suggested compat-first S2. It schedules work inside T483;
it does not reopen S1 or activate several S packets. Current records admission.

| S | Category | Register entries | Scope and exit condition |
| --- | --- | ---: | --- |
| S2 | Restore original | 76 | Apply registered coupled restorations of original code/structure; preserve proven adaptations and verify the affected ABI/call contracts. |
| S3 | Delete redundant local code | 120 | Remove registered redundant bodies, wrappers and diagnostics with callers/declarations/build references; do not confuse entries with whole files. |
| S4 | Minimize/consolidate | 117 | Consolidate duplicate ownership, selectors and declarations; preserve behavior without moving equal complexity into another shim. |
| S5 | Correct | 207 | Correct registered semantic/contract and diagnostic-line deviations with the specified positive/negative validation. These are not 207 reproduced runtime bugs. |
| S6 | Pending evidence | 755 | Obtain each named missing proof, then record retain/restore/delete/minimize/correct disposition. Implement justified resolutions within the admitted packet; unresolved evidence or subjective boundary choices must remain explicit and need owner disposition before closure. |

### Exact allocation and execution

The frozen S1 register is the input, with stable T483-D identifiers. Match the
`decision` column as follows: S2 = `restore-original`, `restore`, `restore original`;
S3 = `delete`; S4 = `minimize`, `simplify`; S5 = `correct`; S6 =
`pending-evidence`, `pending`, `pending evidence`. This assigns every one of the
1275 non-retain entries once; the 1767 retain entries get no separate repair S.
Keep the S1 snapshot unchanged and record execution against its identifiers.

Within each S, filter the existing file/line-ordered register without reordering
or renumbering. Coupled entries describing one change are one implementation
unit, with every covered ID updated. Read dependencies before editing; never
apply half a coupled change just to satisfy category order. If a safe atomic
change crosses categories, record all affected IDs and the dependency before
implementation; a material scope change requires owner approval. Later S work
must not repeat an already delivered change.

The 365 completed format restorations and eight completed file deletions remain
S1 results, not newly scheduled repairs. Retained boundaries may be read as
constraints but are not reopened without new evidence. Missing original bodies
are not automatically bugs: modern Win32 API ownership may be the correct
boundary, and genuine architectural choices go to the owner.

### Per-S verification and handoff

Use one agent for initial implementation/review and a separate counter-review
pass. Check current source identities against S1 before using old evidence.
Report assigned/completed/remaining entry counts, coupled change count, affected
files, mirror/local footprint and any reclassification with its evidence.
Each production-changing S requires the applicable focused regressions, cached
build, Full gates, coherent 11-image publication to assets/release and installed
system32, manifest hash checks, governance checks, commit and push. Apply existing
owner-authorized identical-output test exemptions only with evidence. No guest
editing/rebuild or winnt.zip changes are authorized.

Compat-wide and eleven-component minimization remain later T483 scope; they are
not cancelled or claimed completed by S2–S6, and receive subsequent S planning.
Do not put these S packets into the project T queue or duplicate them in TODO.

## Execution authorization through S5

Owner authorizes S2–S5 sequential implementation and closure. S6 remains held
until a new instruction, regardless of S5 completion. Every changed source file
must have its conditional structure and original logical diagnostic lines checked
with the change; do not defer line correctness to another pass.

Count distinct modified implementation files per batch, including coupled
non-mirror sources when changed; repeated edits to one file do not advance that
count. Deliver a batch when ten files have been modified, or when that S has no
remaining files to process (the final batch may be smaller). Finish the current
file's assigned changes and any necessary atomic dependency before delivery.
Record the exact batch file list. Each batch follows cached full build,
publication, applicable tests, commit and push under the existing release gates;
failed tests are resolved before calling the batch delivered. Documentation and
ledger-only edits do not manufacture a ten-source-file threshold.

## Dynamic delivery reconciliation

Each P delivery must update the successor CSV decisions and implementation
statuses from the delivered per-S ledgers, then refresh the
[current classification](../../../artifacts/mvdm-audit/t483-s1/classification-summary.md)
and [baseline](../../../artifacts/mvdm-audit/t483-s1/current-baseline.json).
Stable completed IDs remain for provenance but are excluded from outstanding
counts. Frozen S allocations and historical snapshot statistics remain labelled
historical; they do not override the current decisions or imply live diff counts.
A delivered change with outstanding verification remains outstanding.

## Superseding original-first remaining-diff audit

The owner requires a new ordered worklist for all remaining categories,
including retain, excluding completed points and deleted files. The active
R-register links each historical D-ID; old decisions are leads, not accepted
necessity evidence. Current and original ranges must be verified at each turn.

Presume the original implementation valid. A difference requires concrete
proof that restoration fails an actual supported context. Restore original
code directly where justified; minimize necessary differences without moving
complexity into shims. If the proposed result retains unchanged or increases
mirror diff, STOP at that point, report necessity and alternatives, and obtain
owner approval before continuing. Audit sequentially in the R-list order;
read dependencies only as needed without crediting later items as complete.
The new progress record is updated with every review/delivery; historical
counts and stable IDs must never be reported as remaining live defects.
