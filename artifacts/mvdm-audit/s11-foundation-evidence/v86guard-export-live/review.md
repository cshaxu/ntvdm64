# V86guard release: live VDD memory-export recheck

The ten inputs in `build/t473-s11/v86guard-package/package/system32` were
compared by SHA-256 with the published V86guard release manifest; all ten
matched. The existing isolated-stage VDD test was then run using that package,
the cached selected x86 VDD and `console-startup-observer`. A fresh package
identity reseals the temporary images, so their output hashes in `manifest.json`
are not the release hashes. The original immutable MEM guest exited zero.
The VDD resolved the real worker's query and reallocation exports by name and
ordinal, and verified allocation, reallocation, release, and preservation of
the original allocation and free-space observation after an oversized
reallocation failed. The test stage and its registry overlay were removed by
the test's existing cleanup.

The first invocation selected `terminal-observer.exe` instead of the required
`console-startup-observer.exe` and exited 64 without creating a product
observer report. It is a harness invocation failure, not a product run. The
second invocation produced the retained report and both VDD pass markers.
This verifies one real-worker path on this release, not all memory providers,
concurrent callers, OS commit failures or guest task-retirement paths.
