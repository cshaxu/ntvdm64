# S10 parallel C-VID declaration and vector ownership

Six pending physical roles are reconciled as partial at base `de8fc1a69`.
Both pinned original editions and current files are byte-identical. The
original compiler's selected declaration excerpts for `cpu_c.h`, `pigger_c.h`,
`cpuint_c.h`, `fm_c.h` and `fpu_c.h` are retained and independently checked.
These excerpts establish selected declarations, not ABI equivalence, actual
CPU/FPU behavior or concurrent lifetimes. The PIG-named header's selection
does not establish PIG debug capability.

For `vglfunc.c`, 78 original object relocation slots resolve to the released
image's original `SINIT011/012/013` functions and three remaining slots are
zero. This establishes the initial `C_Video` vector, not dynamic republishing,
mode selection or execution. Per-file source text retention is 100%; a
behavior-restoration percentage cannot be determined from these observations.
Other C-VID headers remain pending; unobserved preprocessing is not exclusion.

The [shard 1 correction](shard-1/parallel-review-correction.md) supersedes
the earlier path-parser result. The [shard 3 review](shard-3/cvidc-header-role-review.md)
records the actual build configurations and their limits. Their retained
excerpts survive cleanup of disposable build output. No guest or product
image changed in this checkpoint.
