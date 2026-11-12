# T483 current mirror-difference classification

This is the dynamic register view after S5 P2, source commit
`3ac24c6a730d69a164311e18c4db97d7b0e83021`. The CSV is the sole per-item
authority; [current-baseline.json](current-baseline.json) records its hash,
ledger reconciliation and fresh source census. Stable IDs include completed
work and must not be called remaining live differences.

| Current decision | Registered | Completed | Outstanding |
| --- | ---: | ---: | ---: |
| retain | 1766 | 1 | 0 |
| restore-original | 79 | 79 | 0 |
| delete | 120 | 120 | 0 |
| minimize | 117 | 117 | 0 |
| correct | 205 | 4 | 201 |
| pending-evidence | 755 | 0 | 755 |
| Total | 3042 | 321 | 956 |

Retain has no scheduled repair. Its one completed row is D2233, corrected from
restore-original to retain in S2. Non-retain completed entries total320.
Restore-original now includes D0090/D0095/D0124/D0127 restored in S5 P2.
S5's frozen207-entry assignment remains6 complete/201 outstanding; its two
restored entries no longer belong to the current correct category.
The thirteen delivered cmdmisc-related entries remain outstanding because
specific fault verification is pending. S6 remains held.

## Fresh source census

The fixed1060 host C/H census contains491 byte-identical files,561 differing
live files and8 deleted files. All1052 live original pins match. Fresh lexical
blocks total9724 live +8 deletions =9732. Added/removed lines relative to pinned
originals total31476/159713, including deleted files. These counts include
#error, #line, comments and exclusions; they do not count semantic defects.
The S1 snapshot was9596 blocks and35233/163083 added/removed lines: current
blocks are136 higher, while added-plus-removed lines are7127 lower. Block count
and diff-line volume are distinct metrics; neither alone proves necessity.

The previous259 exclusion-only /302 other-difference file split has not been
reclassified after remediation and is not presented as a fresh current count.
S1 historical evidence remains in final-census-reconciliation.json and
final-lexical-recount.json. It must not substitute for this current view.

## Update contract

Every P delivery must reconcile per-item implementation status and changed
decisions with its execution ledger, refresh this category summary and source
census, and retain evidence links. Completed IDs remain historical traceability,
not outstanding work. Missing evidence stays outstanding. Repairs that preserve
necessary differences must not be labelled fully restored or diff-free.
