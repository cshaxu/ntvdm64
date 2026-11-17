# T483 S6: reduce remaining mirror difference blocks

Owner admitted S6 after [S5 closure](../../../docs/etc/evidence/m0-t483-s5-closure.md).
Source baseline:8c1c54aeac9b97d020e86f5db50ca7d744aebd91.

[file-worklist.csv](file-worklist.csv) is the file order:descending remaining
blocks,then lexical repository path. Each file must be completed as a unit;
within it follow source order. Recount after each delivered file and refresh
remaining order when newly proven masks change a count. Do not restart completed
files or renumber the old R evidence to hide an unresolved difference.

## Counting contract

Inventory covers1052 existing host C/H mirrors plus9 already-audited build,
include and DEF text files:1061 total. Deleted files,guest,binaries and other
resources are outside this source queue.509 files are byte-identical to origin.
Normalized/masked comparison has806 zero-difference files and255 nonzero files.
These are not the old fixed365-file S5 totals.

Normalize CRLF,retain terminal empty line,ignore#line only for difference
counting,and use zero-context SequenceMatcher with autojunk=False. Pair original
and current condition arms by original logical anchors and group/sequence
structure. Only a whole current body containing#error plus optional#line is
eligible. Replace that body's original/current contents with the same marker
for measurement only. Keep actual condition frameworks,selected alternatives,
comments and other code in the comparison. Never remove a whole mixed hunk
merely because it contains#error. No source is changed by inventorying.

The initial sum2333 is conservative:75 cases across31 files could not be masked
reliably,or are not error-only bodies. They remain counted with their source
ranges in [source-inventory.json.gz](source-inventory.json.gz). It is not a claim
that all2333 are necessary semantic adaptations. Resolve such mapping cases
before editing their file;update counts/order instead of treating them as zero.
The first8 files have no mask ambiguity and precede every ambiguous file even
at its conservative count. The first file is kernel/ntstubs.c:131 blocks.

[inventory.py](inventory.py) reproduces the read-only census from the pinned
origin manifest and maintained conditional parser. Run from repository root
with --output build/<fresh-evidence>/inventory.json. Its parser extension records
branch-end rows;the script is a task evidence utility,not a product component.
#line correctness and semantic necessity still require per-file review.

## Decisions and delivery

The [existing pending decisions](../t483-remaining-reaudit/pending-decisions.csv)
remain the canonical owner backlog;[R register](../t483-remaining-reaudit/review-register.csv)
retains historical identities. S5 handed off1650 pending points;these are not
2333 new points and are not approved by closure. Link surviving/current ranges,
remove only fully restored portions,and update residual size/evidence/choices
in both registers after every file. No duplicated second decision state machine.

Restore complete original units where valid. Preserve harmless original code.
Both block count and changed-line count should decrease;net increases need prior
owner approval. No equal-complexity transfer into adapters or shims. Proven
retained boundaries remain decisions;unsupported speculation is unfinished work.

Report file/point entry and exit,raw and error-masked blocks/lines,delta and
cumulative progress. Finish the whole file and coupled changes before cached
full build,publish,test,commit,push. Pure format-only changes may accumulate.
Do not modify/rebuild guest,change#error policy or enter compat/component audit.

## First whole-file review

[ntstubs review](ntstubs-whole-unit-review.json) records282 complete function
probes and the restored unused CALLPROC declaration. [Diagnostic evidence](ntstubs-diagnostic-evidence.zip)
contains generators,actual commands and result logs,not product binaries.
Non-error blocks131->130 and changed lines9238->9237;remaining omissions
are evidence-backed owner decisions,not approved. Worklist is current;the
compressed admission inventory stays immutable. Full P2 delivery evidence is recorded below.

[Second-file progress](wmdisp32-progress.json) records the completed wmdisp32.c whole-file
source review. R1519 is restored locally and removed from pending decisions;
final P2 verification is complete.

[P2 delivery checkpoint](batch-p2-checkpoint.json):cached build and dual publication
complete;message191,focused37/task-order,Full20 and governance passed. The P2
commit accompanies the receipt;remote push is verified separately.

[Third-file review](ntcftxt-whole-file-review.json):non-error65->62 blocks,
1065->1057 lines. Seven harmless original declarations restored;23 excluded
bodies unchanged. P3 final verification/publication passed;see receipt below.
[Prior controls](ntcftxt-prior-controls.zip) preserve historical per-function
compiler/linker and real capture logs,not a pass for the new candidate.

[Fourth-file review](sevid000-whole-file-review.json):64 blocks and132 lines
remain unchanged,each linked to its specific restoration counterexample.
Known CHAIN4 word and original UNCHAINED string limitations remain explicit;
source review does not imply those tests passed. No product edit or new run.

[Fifth-file review](gfx-updt-whole-file-review.json):58 blocks/138 lines
unchanged;span-only results distinguished from pixel failures. Selected-code
logical lines checked;no new build or runtime claim.

[Sixth-file review](usercli-whole-file-review.json):49->45 blocks,125->114
lines;original harmless declaration/macros and contiguous formatting restored.
P3 final verification/publication passed;residual groups stay pending.

[P3 checkpoint](batch-p3-checkpoint.json):cached full and11x2 publication
completed;message/focused/task-order,Full20,boundary143/9 and governance passed.
Git delivery accompanies the receipt;remote push is checked after commit.

[Seventh-file review](process-whole-file-review.json):47 blocks remain;168->167
lines,one added blank removed. [Controls](process-controls.zip) contain diagnostic
source/scripts/logs,no binaries. Six original functions fail CP932 contracts;
original expansion fails native encoding and cleanup controls. Residuals remain
pending. Formatting-only edit delivered with P4.

[Eighth-file review](user-whole-file-review.json):45->45 blocks,112->110lines;
remove redundant final common-marker guard. Complete-file review finished;
remaining ABI/layout/multipass decisions stay pending. P4 verification passed.

[P4 checkpoint](batch-p4-checkpoint.json) records the frozen source and owned
delivery:cached full,publish11x2,message/focused/task-order,Full20,boundary
and governance passed. Git delivery accompanies receipt.
