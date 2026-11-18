# Project Status

## Current Work

**Active: M0 T483 S8**

The owner closed S7 and admitted the final sequential audit in S8.
The first three sequential file reviews carry forward from S7 P47 and P48.
S8 P165 completed review through `profile.c`. Next is position 209, `queue.c`.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S8, Ordinary Mode with initial and counter-review passes. |
| Admission And Approval | Owner directed S7 closure, the final sequential audit in S8, and a cumulative report on entry to every file. |
| Candidate Proposal | [Mirror diff cleanup](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Objective | Requalify remaining original-relative differences in fixed file order. Restore original code where supported, minimize necessary adaptations, and register evidenced residual decisions. |
| Non-goals | No guest edits or builds. Preserve prior ownership decisions, exclusion policy, and unrelated work. |
| Reference Baseline | S7 P48 commit `3700937a9646b6102329153f9a8bcf168f5e9594`, package `a24c96b7-15df-40a8-8e77-3ca5d05a2c44`. OpenNT revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`. |
| Files And ABI Surface | The ordered live mirror list, associated callers and providers, diff register, and pending decisions. |
| Applicable Rules | Original-first, whole-file review, keep excluded #error arms and original #line coordinates. Preserve harmless original code. Group coupled points within a file. |
| Verification | Pin source and original hashes; validate line anchors. After each substantive whole-file batch, run cached x86/x64 builds, publish and hash-check all 11 images at both destinations, run applicable targeted tests, commit and push. |
| Expected Markers | Each file has an evidenced disposition for every point and stable before/after non-error metrics. Pending residuals and restored points are reconciled between both ledgers. |
| Asset Needs | Pinned source mirrors, existing cached build and targeted fixtures, and isolated runtime supervisors. |
| Reporting Requirements | On entry give queue position, path, IDs, non-error points, added and deleted lines, problem and evidence. Give completed/remaining files, cumulative point/line reductions, global non-error mirror and point/line totals, and pending rows. On exit give the same counts plus dispositions, line/exclusion checks and verification/delivery status. Keep file counts, diff points and pending rows distinct. |
| Stop Conditions | Proposed increases in non-error points or changed lines require prior owner approval. Record missing evidence as investigation. Preserve parallel changes and verify actual runtime package identity. |
| Exit Criteria | All 231 positions have a disposition, including the three carried forward. Deliver verified restorations, reconcile actual source hashes and counts, and register unresolved owner choices with evidence. |
| Original Owner Request | Close S7 and admit S8 for a final sequential audit. Report the current file, cumulative progress and global totals when entering every file. |
| Similar-Issue Sweep | Check remaining changes in each complete file and its actual caller/provider chain. Evaluate independent local simplifications within that ownership boundary. |

## Current Technical Baseline

S7 ends with 1,974 non-error points and 27,210 changed lines across 231
mirrors. It removed 73 points and 1,764 lines. The pending backlog has 1,566
rows. Those rows are decision records and can group multiple original review
IDs. After S8 P165, 208 of 231 positions are covered and 23 remain queued.
P107 Win16 shared clipboard transfer remains deferred by owner direction. Both P106 and P107 failed at the same stage. The candidate passed 56 open-state checks and 24 ownership cases. Rerun the real transfer when native OpenClipboard succeeds.

S8 has removed 62 points and 821 diff lines, leaving 1,912 points and 26,389 lines. The live pending backlog has 1,532 rows.
[P1 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p1-checkpoint.json).
[P2 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p2-checkpoint.json).
[P3 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p3-checkpoint.json).
[P4 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p4-checkpoint.json).
[P5 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p5-checkpoint.json).
[P6 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p6-checkpoint.json).
[P7 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p7-checkpoint.json).
[P8 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p8-checkpoint.json).
[P9 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p9-checkpoint.json).
[P10 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p10-checkpoint.json).
[P11 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p11-checkpoint.json).
[P12 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p12-checkpoint.json).
[P13 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p13-checkpoint.json).
[P14 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p14-checkpoint.json).
[P15 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p15-checkpoint.json).
[P16 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p16-checkpoint.json).
[P17 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p17-checkpoint.json).
[P18 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p18-checkpoint.json).
[P19 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p19-checkpoint.json).
[P20 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p20-checkpoint.json).
[P21 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p21-checkpoint.json).
[P22 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p22-checkpoint.json).
[P23 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p23-checkpoint.json).
[P24 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p24-checkpoint.json).
[P25 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p25-checkpoint.json).
[P26 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p26-checkpoint.json).
[P27 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p27-checkpoint.json).
[P28 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p28-checkpoint.json).
[P29 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p29-checkpoint.json).
[P30 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p30-checkpoint.json).
[P31 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p31-checkpoint.json).
[P32 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p32-checkpoint.json).
[P33 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p33-checkpoint.json).
[P34 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p34-checkpoint.json).
[P35 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p35-checkpoint.json).
[P36 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p36-checkpoint.json).
[P37 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p37-checkpoint.json).
[P38 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p38-checkpoint.json).
[P39 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p39-checkpoint.json).
[P40 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p40-checkpoint.json).
[P41 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p41-checkpoint.json).
[P42 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p42-checkpoint.json).
[P43 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p43-checkpoint.json).
[P44 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p44-checkpoint.json).
[P45 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p45-checkpoint.json).
[P46 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p46-checkpoint.json).
[P47 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p47-checkpoint.json).
[P48 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p48-checkpoint.json).
[P49 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p49-checkpoint.json).
[P50 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p50-checkpoint.json).
[P51 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p51-checkpoint.json).
[P52 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p52-checkpoint.json).
[P53 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p53-checkpoint.json).
[P54 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p54-checkpoint.json).
[P55 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p55-checkpoint.json).
[P56 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p56-checkpoint.json).
[P57 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p57-checkpoint.json).
[P58 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p58-checkpoint.json).
[P59 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p59-checkpoint.json).
[P60 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p60-checkpoint.json).
[P61 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p61-checkpoint.json).
[P62 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p62-checkpoint.json).
[P63 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p63-checkpoint.json).
[P64 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p64-checkpoint.json).
[P65 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p65-checkpoint.json).
[P66 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p66-checkpoint.json).
[P67 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p67-checkpoint.json).
[P68 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p68-checkpoint.json).
[P69 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p69-checkpoint.json).
[P70 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p70-checkpoint.json).
[P71 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p71-checkpoint.json).
[P72 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p72-checkpoint.json).
[P73 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p73-checkpoint.json).
[P74 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p74-checkpoint.json).
[P75 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p75-checkpoint.json).
[P76 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p76-checkpoint.json).
[P77 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p77-checkpoint.json).
[P78 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p78-checkpoint.json).
[P79 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p79-checkpoint.json).
[P80 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p80-checkpoint.json).
[P81 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p81-checkpoint.json).
[P82 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p82-checkpoint.json).
[P83 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p83-checkpoint.json).
[P84 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p84-checkpoint.json).
[P85 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p85-checkpoint.json).
[P86 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p86-checkpoint.json).
[P87 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p87-checkpoint.json).
[P88 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p88-checkpoint.json).
[P89 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p89-checkpoint.json).
[P90 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p90-checkpoint.json).
[P91 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p91-checkpoint.json).
[P92 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p92-checkpoint.json).
[P93 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p93-checkpoint.json).
[P94 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p94-checkpoint.json).
[P95 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p95-checkpoint.json).
[P96 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p96-checkpoint.json).
[P97 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p97-checkpoint.json).
[P98 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p98-checkpoint.json).
[P99 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p99-checkpoint.json).
[P100 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p100-checkpoint.json).
[P101 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p101-checkpoint.json).
[P102 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p102-checkpoint.json).
[P103 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p103-checkpoint.json).
[P104 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p104-checkpoint.json).
[P105 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p105-checkpoint.json).
[P106 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p106-checkpoint.json).
[P107 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p107-checkpoint.json).
[P108 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p108-checkpoint.json).
[P109 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p109-checkpoint.json).
[P110 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p110-checkpoint.json).
[P111 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p111-checkpoint.json).
[P112 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p112-checkpoint.json).
[P113 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p113-checkpoint.json).
[P114 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p114-checkpoint.json).
[P115 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p115-checkpoint.json).
[P116 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p116-checkpoint.json).
[P117 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p117-checkpoint.json).
[P118 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p118-checkpoint.json).
[P119 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p119-checkpoint.json).
[P120 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p120-checkpoint.json).
[P121 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p121-checkpoint.json).
[P122 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p122-checkpoint.json).
[P123 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p123-checkpoint.json).
[P124 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p124-checkpoint.json).
[P125 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p125-checkpoint.json).
[P126 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p126-checkpoint.json).
[P127 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p127-checkpoint.json).
[P128 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p128-checkpoint.json).
[P129 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p129-checkpoint.json).
[P130 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p130-checkpoint.json).
[P131 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p131-checkpoint.json).
[P132 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p132-checkpoint.json).
[P133 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p133-checkpoint.json).
[P134 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p134-checkpoint.json).
[P135 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p135-checkpoint.json).
[P136 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p136-checkpoint.json).
[P137 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p137-checkpoint.json).
[P138 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p138-checkpoint.json).
[P139 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p139-checkpoint.json).
[P140 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p140-checkpoint.json).
[P141 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p141-checkpoint.json).
[P142 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p142-checkpoint.json).
[P143 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p143-checkpoint.json).
[P144 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p144-checkpoint.json).
[P145 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p145-checkpoint.json).
[P146 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p146-checkpoint.json).
[P147 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p147-checkpoint.json).
[P148 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p148-checkpoint.json).
[P149 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p149-checkpoint.json).
[P150 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p150-checkpoint.json).
[P151 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p151-checkpoint.json).
[P152 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p152-checkpoint.json).
[P153 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p153-checkpoint.json).
[P154 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p154-checkpoint.json).
[P155 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p155-checkpoint.json).
[P156 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p156-checkpoint.json).
[P157 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p157-checkpoint.json).
[P158 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p158-checkpoint.json).
[P159 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p159-checkpoint.json).
[P160 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p160-checkpoint.json).
[P161 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p161-checkpoint.json).
[P162 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p162-checkpoint.json).
[P163 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p163-checkpoint.json).
[P164 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p164-checkpoint.json).
[P165 receipt](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-p165-checkpoint.json).

Follow the [frozen sequential order](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s8-sequential-order.csv).
The list follows first occurrence in the pending register, then the live
register order for the five other files. Paths are normalized and grouped
rows expanded for coverage. Within each file, compare in original line order.

[Main mirror worklist](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/file-worklist.csv).
[Pending decisions](../../artifacts/mvdm-audit/t483-remaining-reaudit/pending-decisions.csv).
[S7 batch receipts](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).

## S1 Closure Record

[T483 S1 closure](../etc/evidence/m0-t483-s1-closure.md).

## S2 Closure Record

[T483 S2 closure](../etc/evidence/m0-t483-s2-closure.md).

## S3 Closure Record

[T483 S3 closure](../etc/evidence/m0-t483-s3-closure.md).

## S4 Closure Record

[T483 S4 closure](../etc/evidence/m0-t483-s4-closure.md).

## S5 Closure Record

[T483 S5 closure](../etc/evidence/m0-t483-s5-closure.md).

## S6 Closure Record

[T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md).

## S7 Closure Record

[T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md).

## Recent M0 Closures

- [T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md)
- [T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md)

## Recent Governance

Each sequential-file entry and exit reports both file and whole-audit totals.
The unrelated project [TODO](TODO.md) remains separate.
