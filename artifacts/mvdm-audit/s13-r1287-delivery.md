# S13 original diagnostic continuation batch r1287

Ten mirror files received source-verified original diagnostic or continuation
line corrections. These include two Video7 exclusions, printer paths, event
fullscreen projection, and 17 matched condition continuations; adjacent
original code and conditional signatures were checked. Selected program
statements and conditions remain unchanged. [Batch evidence](s13-r1287-continuation-batch.json);
[initial source notes](s13-r1286-pending-lines.json).

Cached x86, x86-extra and x64 [builds passed](s13-r1287-full-build-results.json);
[original-line selected/rejected tests passed](s13-r1287-guard-evidence.json).
The [image comparison](s13-r1287-build-image-compare.json) showed six
non-timestamp bytes in NTVDM: three `nt_event.c` error-report line-number
immediates changed by the corrected original continuation. The already-started
[Full suite](s13-r1287-full-evidence.json) passed, including Console and Window
17/17. The user has since authorized reuse of test evidence when all binary
changes are verified diagnostic line-number constants or timestamps. All ten
images were published and verified at `assets/release` and `O:/winnt/system32`.
No guest source or binary was modified.

The [census](s13-r1287-counts.json) records 1,056 existing C/H files: 182
original-identical, 874 reviewed different and zero pending in the per-file
inventory. Rename-aware net 580 deleted paths against `c97d4ee2c` are separate.
This batch does not claim exhaustive proof of all possible macro contexts;
the registered USER multi-cohort projections remain.
