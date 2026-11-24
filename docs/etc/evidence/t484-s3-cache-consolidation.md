# T484 S3 P160 consolidate component build audit

B0020 Test-FullComponentBuild.ps1 had no current caller. It generated its own
configure/build CMD files, counted ten images and duplicated the current two-pass
CMD stability gate. Delete that132-line runner. Keep its useful PE architecture
and intermediate byte/timestamp assertions in verify-generated-input-stability.ps1,
which already covers all11 images, ROM/DEF inputs and eight actual production graphs.
It invokes only tools/build.cmd for production work; no second build recipe remains.

The old redirector assertion required session_thread_finish from NTVDM. That symbol
still exists at ordinal219, but current VDMREDIR neither references it in source
nor imports ordinal219 in the actual DLL. Read-only dumpbin PID9692 confirms the
current imports. Do not restore a stale binding merely to satisfy an obsolete test.
Existing nonworker closure audit owns redirector source/link boundaries.

Background PID19844 executes the consolidated two-pass gate and then verifies
11 raw and22 published hashes. Evidence lives in build/t484-s3-cache-consolidation:
result.json, cache.log and redir-imports.log. No product or original guest changed.
This is build/cache acceptance, not a new Full runtime pass.

B0019 Test-BuildHygiene remains open: its cleanup safety assertions are useful,
but its older configure/cache test path and S4 cleaner dependency need reconciliation.
No claim that all build self-tests or publication paths are now CMD-only.

PID19844 passed both builds and all identity assertions in167.5 seconds. Product
inputs and both published copies remained unchanged; no republication was needed.
Disposition totals:290/355 complete,65 remain.
