# S13 context-matched diagnostic lines r1280

A ten-file batch restores original diagnostic lines in 83 existing excluded
branches, matched by original group signature and retained-code context. It
also corrects two printer alternate/continuation mappings and the 16-bit and
32-bit DPMI IRET group/continuation mappings that had been assigned to the
wrong original locations. Selected code and conditional decisions remain
unchanged. [Per-file evidence](s13-r1280-context-line-batch.json),
[printer](s13-r1278-printer-diagnostic-lines.json), and
[DPMI IRET](s13-r1279-dpmiint-diagnostic-lines.json).

Cached x86, x86-extra and x64 [builds passed](s13-r1280-full-build-results.json).
[Selected/rejected tests](s13-r1280-guard-evidence.json) passed, including
the printer and IRET error lines. The [ten-image comparison](s13-r1280-build-image-compare.json)
shows eight identical and two timestamp-only images, with no other differences;
the [r1259 Full](s13-r1259-full-evidence.json) is reused. All ten images were
published and verified at `assets/release` and `O:/winnt/system32`. No guest
source or binary was modified.

The [census](s13-r1280-counts.json) remains 1,056 existing C/H: 182 identical
to original, 874 reviewed with differences, and zero pending in this inventory;
net 580 deleted paths from the baseline are separate. Historical error arms
without directly mapped original contexts are not claimed as exhaustively
verified, and the registered USER multi-cohort projections remain.
