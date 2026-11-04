# S13 diagnostic line follow-up r1276

Three original diagnostic lines in already-excluded `video.c` arms were
corrected after source-context comparison: 315, 1887 and 2045. The selected
conditions and executable body were not changed. [Source proof](s13-r1276-video-diagnostic-lines.json);
[negative and selected tests](s13-r1276-guard-evidence.json).

Cached x86, x86-extra and x64 [builds passed](s13-r1276-full-build-results.json).
[Ten-image comparison](s13-r1276-build-image-compare.json) shows eight
identical and two timestamp-only images; no non-timestamp bytes changed.
The already-passed [r1259 Full](s13-r1259-full-evidence.json) is reused.
All ten images were published and verified at `assets/release` and
`O:/winnt/system32`. No guest source or binary was modified.

[Reverse diagnostic audit](s13-r1276-error-line-reverse-audit.json) uniquely
matched 3,759 `#line`/`#error` arms to original physical lines; 26 local
selectors or projections have no exact signature match and retain linked
context evidence. It does not claim verification of unanchored cases.
The [census](s13-r1276-counts.json) remains 1,056 existing C/H: 182 original
identical, 874 reviewed different, zero pending in the inventory. Net 580
deleted paths from baseline are separate. All possible future macro states
are not claimed; the two registered USER multi-cohort projections remain.
