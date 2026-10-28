# S10 original CGA/EGA pixel contract checkpoint

This is supporting evidence for the still-pending `nt_cga.c`, `nt_ega.c`
and `nt_munge.c` source roles, not their completed classification or product
acceptance. The current input/NLS ledger gate and its counts remain unchanged.

`tests/observation/verify-planar-painter-object-contract.ps1` links the actual
cached production `obj/host/nt_cga.obj`, `nt_ega.obj` and `nt_munge.obj` into
`tests/mvdm-host/planar_painter_object_fixture.c`. It hashes all three objects,
their original sources, the graph, driver and fixture inputs before execution
and verifies unchanged inputs afterwards. Its map checks identify nine actual
paint providers and three actual munge providers. Original LUT initialization
also runs. No original source, product binary or guest is rewritten.

The controlled DIB is fixture-owned. The invalidation sink verifies that the
complete expected frame already exists before publication. Independent bit
extraction and pixel-center scaling produce the expected buffer, rather than
calling another copy of the original LUT/munge algorithm. Each case verifies
the invalidation rectangle, unchanged source and buffer guards, and released
real kernel mutex. Coverage includes all 256 sample bytes, all 16 EGA plane
masks, medium/high EGA paths, three scales, varying source pitch and geometry,
both CGA source parities, repeated initialization and null-output guards.

The final matrix requires exactly 26,114 cases/publications and 182,801
assertions. A separate `--wrong-pixel` run intentionally changes one expected
pixel only: it must fail with exit 1 after one publication. This rejects an
incorrect oracle; it is not a production failure being waived.

Three unrelated graph services referenced by other exported CGA modes have
explicit fail-fast fixture boundaries (exits 90--92), never success stubs.
Text-buffer state is fixture-owned. No text-mode teardown or monochrome stretch
capability is claimed. The tested paths must not enter those services.

Initial harness attempts exposed `/TC` treating object inputs as C, followed
by unresolved services belonging to untested CGA modes. Those are harness
construction failures, not original runtime defects. A source-recompiled
`/Gy` control subsequently passed; the final retained run uses actual cached
objects, not that recompiled control. Raw attempts remain in the owned
`build/t473-s10/planar-*` phase directories.

Remaining work is finite: reconcile both frozen source catalogs and original
recipes, bind these production functions/dispatch to the candidate image,
audit valid geometry/caller preconditions and mode-specific alternatives,
and verify real frame transport, mode changes, failures and retirement.
This controlled-memory test does not establish DOS/WOW/Win3.x rendering or
the candidate's release qualification. Earlier SOL Options failures remain
unresolved. No publication, push or S10 closure is claimed by this checkpoint.

Retained final receipts and their SHA-256 manifest are in
`planar-pixel-runs/`; large binaries stay in the disposable build directory.
