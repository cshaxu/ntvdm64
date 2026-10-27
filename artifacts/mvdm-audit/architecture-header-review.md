# Nonselected architecture-header checkpoint

S10 reconciled 125 additional file variants beneath
`softpc.new/host/inc/{mips,ppc}/{prod,pig}`. Each matches its frozen hash.
The dispositions exclude only these nonselected platform declaration paths;
they do not discard common algorithms, C-VID equivalents, x86 ABI carriers or
guest capability. No mirror-different path is reclassified by this batch.

The per-file records and methods are:

- [Shard 0: 33 paths](shard-0/architecture-header-role-review.csv),
  [method](shard-0/architecture-header-role-review.md).
- [Shard 1: 36 paths](shard-1/architecture-variant-header-review.csv),
  [method](shard-1/architecture-variant-header-review.md).
- [Shard 2: 27 paths](shard-2/architecture-generated-headers.csv),
  [method](shard-2/architecture-generated-headers.md).
- [Shard 3: 29 paths](shard-3/arch-generated-headers-review.csv),
  [method](shard-3/arch-generated-headers-review.md).

The boundary combines original `obj.vdm/CDEFINE.INC` architecture selection,
current generator include roots and the observed compiler dependencies. The
fresh worker dependency snapshot has 632 VALID blocks, zero STALE blocks and
zero exact-path hits for this cohort. Include/cache absence is not the sole
reason for exclusion. The original architecture selector and current selected
CPU40/CCPU x86 profile determine which variants are applicable. No direct copy
of these paths into the x86 carriers was found in the examined generators;
that bounded finding does not prove every generated declaration's provenance.

Some declarations remain actively consumed through byte-identical files at
other paths. Their presence prevents treating file-variant exclusion as
capability exclusion or restoration proof. Selected original source behavior
and the adapter declaration ABI still require their independent reviews.

The merged ledger now has 224 partial, 626 excluded and 3,834 pending paths.
The prior build-carrier checkpoint remains historical evidence for its own
recorded counts. These are classification changes only: no product or guest
bytes changed and no runtime acceptance is claimed. S10 and the separate
42,963-row external-provider relevance review remain open.

The retained [checker](architecture-header-checkpoint.py) compares this
checkpoint with `c6628af46`; its [result](architecture-header-checkpoint-result.json)
records the 4,684-path inventory, frozen hash, disjoint partitions, 125 exact
pending-to-excluded changes, unchanged partial-build manifest set and live
source identities. No path excluded here has a live hash drift. The two
previously recorded concurrent README/`nt_bop.c` drifts remain explicit.
The [background run record](architecture-header-runner-result.json) also
records successful documentation governance and whitespace verification.
