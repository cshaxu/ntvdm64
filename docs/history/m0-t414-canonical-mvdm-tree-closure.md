# M0 T414 — Canonical MVDM source-tree closure

The owner requested that the original MVDM material no longer be scattered
among host, guest, tools, and firmware roots, but instead be physically laid
out as OpenNT presents `base/mvdm` for direct visual comparison.

T414 completed that request without importing, rewriting, deleting, or
reclassifying original source. The full selected union is now rooted at
`src/mvdm/`; its 4,615 baseline blobs pass exact identity verification. The
private overlay is intentionally adjacent at `src/mvdm-overlay/`, not hidden
inside the mirror. External kernel-VDM and `win16/fonts` carriers remain
outside because they are not selected MVDM source.

The formal Win32/x86 three-program build and focused lifecycle/CCPU/DPMI/
memory/C-VID fixtures passed from the new tree. Current source layout and
policy authorities were updated, while historical records and retired
generators retain their contemporaneous paths. The current task version is
`0.0.414` with protocol 3; no `O:\winnt` executable was published because
this layout-only closure introduced no runtime behavior change.

Evidence: [S1 map](../etc/evidence/m0-t414-s1-canonical-mvdm-tree-map.md),
[S2 recomposition](../etc/evidence/m0-t414-s2-mechanical-recomposition.md),
and [S3 closure](../etc/evidence/m0-t414-s3-canonical-mvdm-tree-closure.md).

No active numeric M/T/S packet remains. The queue retains the next
owner-approved candidate; this closure does not admit it.
