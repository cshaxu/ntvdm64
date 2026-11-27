# S13 Final Repair Reconciliation

Scope: 1039 assigned files,4550 frozen points. Do not substitute current diff
blocks for frozen IDs,or whole-file review for original-point restoration.
Baseline: S12 runtime14,1930f0852 product and29014a4f5 closure/admission.

## First reconciliation batch

All1039 current source hashes match files.csv. No duplicate pending IDs.
S8 is851files/900frozen points;the earlier851/851 shorthand is file progress,
not the point denominator. Before reconciliation there were1316 pending rows,including112 newer
residual IDs outside the frozen4550 population;their exact count and mapping
remain under review and are not extra frozen points.

[Point synchronization](s13-ledger-sync.json) preserves all656 prior records
and their final source-backed replacements:394explicit delivered restorations
and262existing delivered residual decisions. Restored entries are not present in the pending list. No source or package changed.
The restored set includes S6 selector1,S7 points138,S8 declarations2 and
S9 points253. The residual set is S7 points78 and S9 points184.

The remaining S7 whole-file/replacement-ID mappings and S8 format/line-only
closure must be reconciled separately,not silently counted as restored.
S10-S12 final dispositions also require cross-register and receipt validation.
Full runtime14 passed23/23;unchanged-code Full is not rerun. This document
is reconciliation progress,not S13 closure or permission to start N004.

## Later-owner pending coverage

S10:754restored306residual;S11:18restored434residual;S12:432restored213residual.
S10/S11 pending coverage is complete. S12 had11missing pending rows from
P13 vrnmpipe.c/vrremote.c:the sync script updated existing rows but did not
append absent IDs. Append the eleven final main-table necessity/minimality
records unchanged,with their provider and final-package evidence. Pending
rows are now1327,including112replacement/new IDs outside frozen inventory.
All953 S10-S12 residual IDs now have pending records;no product change and
no new owner approval inferred. The source and runtime delivery stays valid;
the previous complete-registration claim is corrected by this reconciliation.

## S8 final-file mapping

[Final mapping](s13-s8-final-map.json) reconciles all851files/900frozen points.
606files are byte-exact XP;245files retain only1818#line points/1818lines
after admitted exclusion projection. No remaining code,format or EOF changes.
All current hashes match the final S8 census.898old classifications are now
explicitly superseded by this file-level accounting;the two earlier proven
whole restorations remain separate. Do not call all900points byte-restored.
Each of1818current diagnostic points has its physical line,original logical
line and S8-L identifier in pending decisions;no production code is changed.
The pending population is now3145including these diagnostic records.
S7 replacement-ID correspondence and final receipt coverage remain open.

## S7 delivered whole-file mapping

[Seven-file map](s13-s7-final-map.json) covers839remaining historical IDs,
with110already registered semantic residual IDs and398normalized lines.
All seven final sources match the delivered review hashes;their last source
commits are ancestors of current main. Frozen points are marked superseded,
not falsely credited as restored. Existing pending records remain intact.

The S7 measurement helper omitted#line and normalized CRLF. Consequently
those110/398numbers are not complete byte-sensitive mirror metrics. The first
current census pass resolves10of20files;10need their existing explicit
exclusion correspondence because selector expressions differ from upstream.
Do not treat generic matcher assertions as product failures or count excluded
bodies as active differences. Complete this census before S13 closure.
All4550frozen IDs now have either explicit final dispositions or linked
whole-file replacement accounting;final metric and receipt reconciliation
remain open. No source/package changes and no Full rerun.

## S7 byte census follow-up

[Byte census](s13-s7-byte-census.json) now computes all20files,1685points/
7188lines including CRLF,EOF and#line. This is the computed census pending
final exclusion-pair review,not a new closure claim. Condition canonicalization
is used only to align guards;the compared source bytes remain unchanged.
Four source-reviewed explicit ranges cover translated-cache flush,monitor TEB
initialization,VESA monitor switching and the moved X86GFX wrapper. Added
condition frames remain counted. LF alone separates compiler physical lines.

Three delivered files still use CRLF against LF originals. Pure LF candidates:

| File | Current points/lines | LF candidate points/lines |
| --- | --- | --- |
| dpmiint.c | 1/2600 | 46/238 |
| dpmiselr.c | 1/1862 | 9/62 |
| i386/dpmi386.c | 1/407 | 3/219 |

These candidates reduce4350diff lines,while exposing55previously joined
blocks. They have not yet been applied;the standing format-restoration
approval covers this kind of point increase. Whole-file residual necessity,
exclusion mapping and source-hash ledger updates remain before final accounting.
No product source or package changed;runtime14 Full23/23 remains valid and
was not rerun. This batch corrects measurement evidence only.

## P5 LF restoration and residual correction

[Restoration proof](s13-p5-lf-restoration.json) applies all three LF candidates:
3points/4869lines ->58points/519lines,delta+55points/-4350lines. The increase
only exposes existing separated differences;no new code was added. Every
non-line-ending byte,physical line count,#error and#line is unchanged.
Main file hashes now identify the LF files. Prior source hashes in historical
receipts remain intact. Runtime14 remains the published package;no build,
publication or runtime test is claimed for this format-only update.

R00144 had stale whitespace-only necessity text,although S7 P1 explicitly
retained the final DOSX-ownership boundary at this location. Main and pending
records now identify the actual single early-return check in DpmiFaultHandler,
its shared predicate and guest-ownership evidence. It stays pending;no whole
point restoration is claimed. This resolves the contradictory explanation,
not a newly discovered runtime defect. Pending row count is unchanged.

The computed S7 census becomes1740points/2838lines after these three changes;
final exclusion-pair review and all-owner receipt reconciliation remain open.
The original1039file/4550frozen-point population is unchanged. Full23/23 is
retained for runtime14;this format-only update does not repeat Full.

## P6 exclusion and package reconciliation

[Reconciliation](s13-p6-reconciliation.json) preserves every current S7
exclusion pair and the26actual published-file hashes. Both publication
locations still match runtime14;the existing Full receipt has23passing gates.
No runtime test was repeated. The known Win95 exit-one boundary is unchanged.

The prior computed census over-excluded nt_fulsc.c original2111-2122 because
its X86GFX guard was split around the retained getNtScreenState function.
Use the already reviewed1752-2110 and2123-2174 ranges separately. Corrected
S7 count:1741points/2826lines;this is a measurement correction,not a code
reduction. Previous computed counts remain historical evidence.

Source review also proves a remaining diagnostic-line defect in dpmi32.c:
#line102 precedes original100,#line106 precedes original105,and the shortened
DpmiInitDosxRM lacks an anchor before original229-303. The final source
therefore does not yet satisfy the original logical-line contract. Correct
these anchors and verify real compiler line output before S13 closure. This
is not a new runtime failure or authority to rerun all previously passing Full
gates. No source change is made in P6;the correction is the next concrete step.

## P7 diagnostic-line repair delivered

[Receipt](s13-p7-line-repair.json):dpmi32.c removes the misplaced pre-dispatch
anchor,corrects the excluded DBG and return anchors,and restores original225
after the shortened initializer. Nonerror8points/18lines ->7points/18lines.
Only#line directives changed. Real product preprocessing matches80original
statements with zero line mismatches;preprocessed code ignoring line metadata
is byte-identical. The six remaining diagnostic anchors have S13-L0001-L0006
pending records,separate from frozen point IDs and semantic residual counts.

Cached full build PID1128 passed;runtime1 is dual-published with26matching
hashes;61CCPU/DPMI cases pass. All13images differ from runtime14 only by the
coherent16-byte package-ID resource;NTVDM also changes COFF/debug timestamps.
Every other binary byte is identical. Preserve prior Full23/23 through this
explicit input-equivalence analysis;no new Full execution is claimed.

S7 current census is1740points/2826lines. P5+P7 actual source changes reduce
4350diff lines and expose54additional blocks;the separate P6 projection
correction is not source reduction. Static unique-statement review finds
remaining line candidates only in dpmi32.h,dpmiint.c and modesw.c. Confirm
these with product preprocessing and repair their complete files next. Other
17files have no such static mismatch;this is not complete branch coverage.
S13 remains open;N004 stays gated.

## P8 remaining S7 line repair and diagnostic register

[Receipt](s13-p8-line-repair.json) completes dpmi32.h,dpmiint.c and modesw.c
line anchors:76points/316lines ->77points/342lines (+1point/+26lines).
Only#line changes;this fits the owner's standing1point/below100line approval.
Two actual compiler contexts match703original-statement records with zero
unexplained mismatches. The new prefix decoder switch is explicitly not an
original-statement witness. Initial strict output comparison found blank-line
differences;nonempty preprocessed code lines and literals remain identical.
S7's20file static review now has zero unique-statement line mismatches.

Cached full build passed;runtime2 dual publication matches26hashes;61CCPU
cases pass. Images differ from runtime1 only by package GUID and PE timestamps;
all other bytes are identical. Full23/23 is retained with that explicit
identity analysis,not re-executed or claimed as a fresh runtime2 Full.

S7 current nonerror byte census:1741points/2852lines. S6 current census is
11points/11lines,all diagnostic anchors;its historical0/0was semantic only.
Register the previously omitted S6/S7 diagnostic anchors separately from frozen
IDs,excluding the six already registered S13-L rows. Directive counts are not
added to diff-block totals. The exact per-file inventory,hashes and pending-row
count are in the receipt. All1039file hashes reconcile. S13 final cross-owner
accounting and closure remain;no migration progress is claimed for repair.

## P9 closure

[Final reconciliation](s13-closure.md) resolves all1039files4550frozen points,
registers qualified residuals and preserves runtime2 delivery/Full equivalence.
Current byte census7322points12851lines;no new product edits or repeated Full.
S13 closes with P9;S14 resumes N004-N012.
