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
