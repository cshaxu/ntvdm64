# T485 S13 closure

## Scope and evidence

Reconcile the final code and delivery of S6-S12 before resuming N004. All1039 file hashes and pinned XP inputs match. Every one of4550 frozen IDs has a delivered restoration, evidenced residual, or explicit final-file replacement map; no unreviewed/local-unverified entry remains. The [machine receipt](s13-closure.json) and [per-file census](s13-final-census.csv) are the final accounting.

## Diff accounting

| Owner | Files | Frozen byte points/lines | Current byte points/lines |
| --- | ---: | ---: | ---: |
| S10 | 30 | 201/102739 | 1834/2939 |
| S11 | 48 | 874/11865 | 722/2384 |
| S12 | 48 | 803/31802 | 556/1390 |
| S6 | 1 | 11/19 | 11/11 |
| S7 | 20 | 170/41330 | 1741/2852 |
| S8 | 851 | 3465/328879 | 1818/1818 |
| S9 | 41 | 674/32219 | 640/1457 |

Total6198/548853 ->7322/12851: +1124blocks,-536002lines. This is not a block-count reduction. Restoring original LF exposes dispersed necessary and diagnostic differences previously merged by CRLF. The owner approved those format restorations. Preserve newline/EOF/#line differences; exclude only proved#error-replaced bodies, not intact unused code. The fixed before commit is6e3e88b53bc57e7f047c9438cae741df5dd14694.

Frozen4550 IDs are1598restored,1215residual and1737superseded by explicit file maps. Pending5056 records comprise1327semantic/binding records and3729diagnostic-line records; these are not current raw blocks. Partial restoration is not complete restoration. No blanket owner approval of residuals is implied.

The4508 intermediate filtered count was stale; final reviewed-metrics and main IDs both have4550. Fresh S12 per-file total556/1390 corrects historical cumulative556/1394 without a source edit. Per-S historical starting revisions also differ from this single frozen baseline.

## Delivery and closure

S13 P5/P7/P8 restore LF and diagnostic lines. P8 commit4a849ef9e is pushed. Cached full build passes, runtime2 is dual-published with26matching hashes rechecked at closure, and61CPU/DPMI cases pass. [P7](s13-p7-line-repair.json) and [P8](s13-p8-line-repair.json) prove every image byte unchanged from the Full-tested runtime14 except package-ID and named PE timestamps. Retain Full23/23; do not repeat unchanged passing tests or claim a fresh runtime2 Full.

The retained final Full includes actual NESticle graphics, WOW, Win3.1 standard/enhanced and Win95 desktop gates. Both enhanced systems prove window input, fullscreen text, returned window input, second fullscreen and the same KVM window. Win95 retains the explicitly approved exit-one boundary. The receipt embeds actual matrix and roundtrip outcomes.

No original guest was changed or rebuilt. Final documentation reconciliation changes no product or test input. S13 is closed with this P9 commit/push; S14 N004-N012 is admitted under the existing continuous-execution approval. Migration remains39/159 until N004 delivers. Other package/capability obligations remain in the unchanged [plan](../../operations/m0-t485-h-repair-plan.md).
