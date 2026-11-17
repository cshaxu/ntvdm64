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

[Ninth-file review](nt-event-whole-file-review.json):43->43 conservative non-error
blocks,124->121 lines. Restore original harmless prototype/local/comment with
original line pins. Two locally added X86GFX exclusions stay conservatively
counted;no fabricated masking reduction. Residual lifecycle correction findings
remain pending. P5 cached/full/publication verified;see final receipt.

[Tenth-file review](dpmiint-whole-file-review.json):42->34 conservative blocks,
231->221 lines. Seven surplus blanks,original endif comment and unused pCode
declaration restored;execution expressions unchanged. P222 DOS4GW repair stays
intact;historical alias-fallback options reconciled in both registers. Local
format/declaration batch verified in P5;see final receipt.

[Eleventh-file review](wowhooks-whole-file-review.json):38 blocks/114 lines
unchanged. Original CBT closure failure and per-thread storage controls remain
qualified diagnostic evidence. Exact prior final-source hash and line proof
match;no new source edit/build/runtime claim. Residual decisions stay pending.

[Twelfth-file declaration controls](ntpsapi-declaration-controls.json):12 actual
x86/x64 C/C++ compilations isolate NTSYSAPI import decoration as the conflict
for three omitted query/set declarations. Omitting only that line passes;
EXTERN_C does not. Historical investigation checkpoint;the whole-file result below supersedes it.

[Twelfth-file review](ntpsapi-whole-file-review.json):38->38 blocks,339->310
lines. Four over-broad omissions narrowed after [compiler controls](ntpsapi-controls.zip).
Original logical anchors restored;SDK type/PEB boundary decisions remain pending.
Whole-file candidate passed P5 cached build,publication and Full20.

[P5 checkpoint](batch-p5-checkpoint.json) and [verification archive](p5-verification-evidence.zip)
record 8 fewer blocks/42 fewer lines,source identity,11-image dual publication,
message/keyboard/focused/task-order/Full20 and boundary/governance passes.
The initial runner-environment failure is retained alongside the same-package
fresh-process pass;residual differences remain pending owner decisions.

[Thirteenth-file review](wmsg16-whole-file-review.json):37->37 non-error blocks,
150->146 lines. Restore original entry and harmless local declaration;real
PostThreadMessage rejection and exact-body cleanup controls passed. Original
#line mapping checked against compiler preprocessing;P6 delivery pending.

[P6 delivery receipt](batch-p6-checkpoint.json) and [verification evidence](p6-verification-evidence.zip): wmsg16.c 37->37 non-error blocks,150->146 changed lines;cached full,publish11x2,message,focused/task-order,Full20 and governance passed. Residual decisions remain pending owner.

[c_main.c review progress](c-main-review-progress.json): entry36 non-error blocks,+133/-30;single-use IRQ helper reduction candidate only. Whole-file review and delivery remain incomplete;not counted among completed files.

[c_main whole-file review](c-main-whole-file-review.json):36->36 points,163->158 lines;P7 local final candidate,verification/delivery pending. Prior progress record is historical candidate evidence.

[P7 receipt](batch-p7-checkpoint.json), [verification evidence](p7-verification-evidence.zip):c_main36->36 blocks,163->158 lines;all gates passed,residuals pending owner.

[Client ntstubs whole-file review](client-ntstubs-whole-file-review.json):35->35 blocks,1805->1803 lines;five include-equivalence contexts pass;P8 verification complete,delivery accompanies receipt.

[Queue whole-file review](queue-whole-file-review.json):35->35 points,4178->4173 lines;harmless original allocator declarations/constant restored. P8 verification complete,combined with client include restoration (-7 lines).

[P8 receipt](batch-p8-checkpoint.json) and [evidence](p8-verification-evidence.zip):client ntstubs -2 lines and queue -5 lines,0 block change;cached full,publish11x2,focused,Full20 and governance passed. Residuals remain pending owner.

[wcntl32 whole-file review](wcntl32-whole-file-review.json):34 points,+37/-32=69 lines unchanged;all remaining allocation/release and mapped-reference differences retain concrete failure evidence,pending owner. No source edit or repeated build.
