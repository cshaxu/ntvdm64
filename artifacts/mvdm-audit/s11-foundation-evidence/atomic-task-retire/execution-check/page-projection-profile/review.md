# Page-domain projection diagnostics and candidate-link correction

The first diagnostic build did not select its intended object: `lib /remove`
was given a basename whereas the original member is stored with the
`obj/adapter-softpc/` path. LNK4014 and LNK4006 warned that the member was not
removed and its replacement was ignored; its subsequent SOL pass with no
profile file is a non-measurement. This same mistake affected the isolated
index-reuse and empty-alias product-package comparisons; those records have
been corrected. Standalone direct-object fixtures and the later independently
rebuilt integrated source/package were not selected through that broken archive.

The corrected diagnostic used the exact archive member name, produced no
LNK4014/LNK4006 warnings and `lib /list` showed the diagnostic object as the
sole page-domain member. Its isolated ten-image package completed three SOL
Options interactions. This diagnostic maintains the integrated index reuse
and samples cumulative QPC at 64-call boundaries. Matching the first/last
flush inside each phase yielded, for later open phases, 192 projections in
2.0–2.05 seconds of observed wall interval: 260–267 ms total inclusive page
projection, including 26–28 ms validation, 64–68 ms preparation and 121–127 ms
replacement. Later close intervals covered 128 calls and 1.36 seconds, with
181–183 ms total, including 19–20 ms validation, 44–45 ms preparation and
83–85 ms replacement. First opening was 323 ms total for 256 covered calls.

Counter deltas omit the partial intervals before the first and after the last
flush. Times are inclusive and may double count nesting; QPC/logging perturbs
behavior, and this is not a process-wide or exclusive CPU profile. `total`
includes allocation and cleanup not in the three named subintervals. Thus the
replacement phase is the largest measured part of this function, but the
function alone does not explain multi-second modal latency. Raw samples,
source, build logs, map, staged manifest, analysis and hashes are retained.
No diagnostic code was merged or published.
