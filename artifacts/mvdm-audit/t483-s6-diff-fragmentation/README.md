# T483 S6: reduce remaining mirror difference blocks

Owner admitted S6 after [S5 closure](../../../docs/etc/evidence/m0-t483-s5-closure.md).
Source baseline:8c1c54aeac9b97d020e86f5db50ca7d744aebd91.

[file-worklist.csv](file-worklist.csv) is the file order:descending remaining
blocks,then lexical repository path. Each file must be completed as a unit;
within it follow source order. Recount after each delivered file and refresh
remaining order when newly proven masks change a count. Do not restart completed
files or renumber the old R evidence to hide an unresolved difference.

## Counting contract

Initial inventory covered1052 host C/H plus9 other text mirrors:1061 total.
P10 recovers one byte-exact original header:current1053 host C/H plus9 other
text mirrors,1062 total. Deleted files,guest,binaries and other
resources are outside this source queue.510 files are byte-identical to origin.
Normalized/masked comparison has807 zero-difference files and255 nonzero files.
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

[Mouse whole-file review](mouse-io-whole-file-review.json):33 points/349 lines unchanged;prior351 count had one false original-arm pairing. Maintained inventory now refuses the three added host-arrow guard pairings. This -2 counting correction is separate from19 blocks/85 lines delivered source reduction. Historical source-inventory snapshot remains immutable.

[P9 receipt](batch-p9-checkpoint.json) and [evidence](p9-verification-evidence.zip):wumsg33->32 points,113->109 lines;single-use rollback cleanup inlined,634 shared original compiler lines unchanged,all required gates passed.

[clres whole-file candidate](clres-whole-file-review.json):31->30 non-error points,3352->2733 difference lines;full original units restored. P10 cached full running;not delivered. Byte-exact newres.h declaration recovery is separately recorded,not a mirror-diff reduction.

[P10 receipt](batch-p10-checkpoint.json) and [evidence](p10-verification-evidence.zip):clres31->30 blocks,3352->2733 lines. Cached full,publish11x2,bitmap/focused/Full20 passed;final#line-only rebuild differs solely in PE timestamps.28 retained function compiler anchors match original. Residual decisions remain pending owner.

[clmsg whole-file candidate](clmsg-whole-file-review.json):27->27 non-error blocks,1042->979 difference lines;two original functions and five macros restored. P11 cached full pending;not delivered.

[P11 receipt](batch-p11-checkpoint.json) and [evidence](p11-verification-evidence.zip):clmsg27->27 blocks,1042->979 lines;cached full,publish11x2,focused and Full20 passed. Residual owner decisions remain pending.

[userk whole-file review](userk-whole-file-review.json):27->27 non-error points,117->115 lines;two added comments removed,original line resets and production tokens unchanged. Accumulated for next substantive batch;residual ABI/native-owner and block-count tradeoffs remain pending.

[wdde whole-file review](wdde-whole-file-review.json):26 points,+128/-15=143 lines unchanged;original-body failure controls and current conversion/callback/retirement chain support residual decisions. P11 DDE service-double scope is stated precisely;no repeated build.

[handtabl investigation](handtabl-investigation.json): declaration restoration candidate only; whole-file review and compilation pending. Rejected conditional-reorder trial increases fragmentation and was not applied.

[handtabl whole-file review](handtabl-whole-file-review.json):26->24 blocks,1481->1381 lines;original declarations and KernelPtoH restored;delivery pending. Assignment helpers withdrawn after actual full link failures;see review evidence. Adds byte-exact original winerrp.h dependency.

[P12 receipt](batch-p12-checkpoint.json) and [evidence](p12-verification-evidence.zip):whole-file restorations,failed link controls,final cached build,publish11x2 and Full20. Net reduction2 blocks/102 lines;residual decisions pending.

[P13 receipt](batch-p13-checkpoint.json) and [evidence](p13-verification-evidence.zip):input.c26->25 non-error blocks,5527->5503 lines. Original declarations restored,Full20 and actual-provider input services/lifetime passed;scattered macro block-count choice remains pending.

[sevid001 review](sevid001-whole-file-review.json):25 non-error points/+25/-25=50 lines unchanged;prior per-point counterexamples and current chain reviewed. Known UNCHAINED baseline failure remains explicit;no repeated runtime claim.

- [wkman whole-file review](wkman-whole-file-review.json):25->25 blocks,46->45 lines;format-only local restoration,remaining boundaries pending owner.

- [cmdexec whole-file review](cmdexec-whole-file-review.json):24->17 blocks,123->82 lines;P14 build/publication/Full passed; governance/Git follow receipt.

[P14 receipt](batch-p14-checkpoint.json) and [evidence](p14-verification-evidence.zip):combined7 blocks/42 lines restored;Full20 passed.

- [keyba whole-file review](keyba-whole-file-review.json) and [preprocessor proof](keyba-preprocessor-proof.zip):24->24 points,72->56 lines;P15 build/publication/keyboard/Full passed;Git delivery follows receipt.

[P15 receipt](batch-p15-checkpoint.json) and [evidence](p15-verification-evidence.zip):keyba24->24 points,72->56 lines;keyboard/PIC90 and Full20 passed.

[P16 client VDM review](client-vdm-whole-file-review.json):four original functions restored;23->20 blocks,796->632 lines;cached build,publish11x2,Full20 passed;Git accompanies receipt.

[P16 receipt](batch-p16-checkpoint.json) and [evidence](p16-verification-evidence.zip).

[P17 dlgmgr review](client-dlgmgr-whole-file-review.json):22->17 blocks,295->242 lines;cached build,publish11x2,Full20 passed;Git accompanies receipt.

[P17 receipt](batch-p17-checkpoint.json) and [evidence](p17-verification-evidence.zip).

[client/wow whole-file review](client-wow-whole-file-review.json):21->21 non-error blocks,+0/-832 unchanged;94 original logical rows exact. Current native ownership checked against29 historical function controls;no repeated build/runtime claim. Residual decisions remain pending.

[cleanup whole-file candidate](cleanup-whole-file-review.json):21->20 non-error blocks,66->44 lines;original ClientDied restored through existing native RTL exception boundary. P18 cached full,publish11x2,focused37,lifecycle and Full20 passed;Git accompanies receipt.

[P18 receipt](batch-p18-checkpoint.json) and [evidence](p18-verification-evidence.zip).

## Owner manual review order after P18

[Current descending file list](owner-review-order.csv) and [counting summary](owner-review-order.json) cover every remaining nonzero mirror,including already reviewed files. Sort by non-error blocks descending,then path ascending. Prior worklist ranks remain as history. Ambiguous exclusion matches remain counted and explicitly flagged. The owner selects the next file;automatic progression stops after P18 delivery.

[P19 owner-approved FNID extraction](ntstubs-compat-extraction.json): retire kernel/ntstubs.c from the mirror census and active registers; preserve its sole selected function in src/compat. Removed from mirror accounting:130 non-error blocks/9237 changed lines. This is source reclassification, not original restoration. Cached full,publish11x2,focused38,lifecycle and Full20 passed;Git accompanies receipt.

[P19 receipt](batch-p19-checkpoint.json) and [evidence](p19-verification-evidence.zip).

## P20 owner-requested usage-ratio scan

[Full 254-file report](usage-ratio/REPORT.md), [ascending CSV](usage-ratio/all-diff-files-by-usage.csv), [summary](usage-ratio/summary.json), [receipt](usage-ratio/receipt.json), and [evidence](usage-ratio/evidence.zip). 806 current product contexts passed read-only preprocessing; all objects remain unchanged. C function scopes and header declaration lower bounds are distinguished; macro definitions are not silently classified unused. This scan changes no mirror, publication or per-file semantic-audit status and admits no extraction.

[P21 owner-approved dialog lookup extraction](client-wow-compat-extraction.json)
retires client/wow.c:21 non-error blocks/832 lines. Cached full,publish11x2,focused38,lifecycle and Full20 passed;Git accompanies receipt.

[P21 receipt](batch-p21-checkpoint.json) and [verification evidence](p21-verification-evidence.zip).
Current census:1061 files,253 nonzero,2142 blocks/43260 lines.
P20 usage-ratio reports remain the immutable pre-extraction snapshot;client/wow.c is now retired.

[P22 owner-approved input membership extraction](input-compat-extraction.json)
retires kernel/input.c:25 non-error blocks/5503 lines. Cached build,targeted input/lifecycle verification and publish11x2 passed;Git accompanies receipt.
Full is explicitly not required for this batch.

[P22 receipt](batch-p22-checkpoint.json) and [verification evidence](p22-verification-evidence.zip).
Current census:1060 files,252 nonzero,2117 blocks/37757 lines.
The P20 ratio snapshot also predates the now-retired kernel/input.c.

[P23 owner-approved object ownership extraction](handtabl-compat-extraction.json)
retires kernel/handtabl.c:24 non-error blocks/1381 lines. Cached build,targeted tests and publish11x2 passed;Git accompanies receipt.
Full waived by owner.

[P23 receipt](batch-p23-checkpoint.json) and [verification evidence](p23-verification-evidence.zip).
Current census:1059 files,251 nonzero,2093 blocks/36376 lines.
The P20 ratio snapshot also predates the now-retired kernel/handtabl.c.

[P24 USER client extraction](client-compat-extraction.json):8 non-error blocks/3564
lines retired by owner approval. Cached build,targeted tests and publish11x2 passed.

[P24 receipt](batch-p24-checkpoint.json) and [verification evidence](p24-verification-evidence.zip).
Current census:1058 files,250 nonzero,2085 blocks/32812 lines.
The P20 ratio snapshot predates the now-retired client/client.c.

[P25 RTL time retirement](rtl-time-retirement.json):6 points/1216 lines.
Unused member removal;cached build,archive verification,publish11x2 and targeted tests passed.

[P25 receipt](batch-p25-checkpoint.json) and [verification evidence](p25-verification-evidence.zip).
Current census:1057 files,249 nonzero,2079 blocks/31596 lines.
P20 usage ratios predate the now-removed RTL time.c.

[P26 xt storage extraction](xt-storage-extraction.json):4 recorded points/339
lines;includes two historical ambiguous-arm masks. Cached build,archive/symbol
proof,publish11x2 and targeted CCPU/WOW verification passed.

[P26 receipt](batch-p26-checkpoint.json) and [verification evidence](p26-verification-evidence.zip).
Current census:1056 files,248 nonzero,2075 blocks/31257 lines.
P20 usage ratios predate the now-retired support/xt.c.

[P27 rectangle helper extraction](rect-compat-extraction.json), [receipt](batch-p27-checkpoint.json) and [verification evidence](p27-verification-evidence.zip). The live census is 1055 files, 247 nonzero, 2072 blocks and 30889 lines.

[P28 dialog pointer extraction](miscutil-compat-extraction.json), [receipt](batch-p28-checkpoint.json) and [verification evidence](p28-verification-evidence.zip). The live census is 1054 files, 246 nonzero, 2069 blocks and 30691 lines.

[Original function paths, signatures and line ranges](extracted-function-provenance.md).

[P29 button style table extraction](inctlpan-compat-extraction.json), [receipt](batch-p29-checkpoint.json) and [verification evidence](p29-verification-evidence.zip). The live census is 1053 files, 245 nonzero, 2065 blocks and 29810 lines.

[P30 full USER handle extraction](handles-compat-extraction.json), [receipt](batch-p30-checkpoint.json) and [verification evidence](p30-verification-evidence.zip). The live census is 1052 files, 244 nonzero, 2060 blocks and 29531 lines.

[P31 control brush extraction](random-compat-extraction.json), [receipt](batch-p31-checkpoint.json) and [verification evidence](p31-verification-evidence.zip). The live census is 1051 files, 243 nonzero, 2054 blocks and 29170 lines.

[P32 unused NetAPI archive retirement](apibuff-retirement.json), [receipt](batch-p32-checkpoint.json) and [verification evidence](p32-verification-evidence.zip). The live census is 1050 files, 242 nonzero, 2050 blocks and 28996 lines.

[S7 P1 receipt](s7-p1-checkpoint.json) and [verification evidence](s7-p1-verification-evidence.zip) record the packstr and illegalp restorations and file-by-file audit dispositions.

[S7 P2 receipt](s7-p2-checkpoint.json) records eight additional dispositions,
the `userrtl.h` comment restoration, and the reviewed `nt_fulsc.c` mask
override for the census. The current non-error census is 2,042 points and
28,754 changed lines. The worklist keeps its historical rank for traceability.

[S7 P3 receipt](s7-p3-checkpoint.json) records eight reviewed files and
the original comment and blank-line restoration in `nt_cga.c`. The current
census is 2,041 non-error points and 28,751 changed lines.

[S7 P4 receipt](s7-p4-checkpoint.json) and
[verification evidence](s7-p4-verification.zip) record eight more reviews,
the original executive declaration groups restored in `ntexapi.h`, and
the cached build, dual publication and targeted tests. The current
census is 2,041 non-error points and 28,487 changed lines.
