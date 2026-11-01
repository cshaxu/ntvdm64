# S12: compound conditions in observed MVDM C sources

The observed worker and WOW32 command contexts were evaluated for boolean
combinations of CCPU, CPU_40_STYLE, PROD, PIG, MONITOR, X86GFX, NTVDM,
V7VGA, SPC386 and C_VID. EGG, VGG, LIM and BIT_ORDER2 were considered only
where the source unconditionally includes their defining `host_def.h` before
first use; no relevant project/generated or SDK undefinitions were found.
Only fixed outcomes in every observed context and exact original-source bodies
were selected. The companion CSV records each expression and its per-graph
values; the JSONL records original ranges and removed-body hashes.

Seventy unselected branch bodies were replaced with markers, retaining their
conditionals. Existing divergence markers and two modified `nt_graph.c`
bodies were excluded. A `trace.c` body containing the held SPC486 label was
also retained, as were unobserved targets, headers and unsupported expressions.
Selected MONITOR paths are not treated as globally disabled.

An independent evaluator checked each expression against its recorded values;
independent reconstruction from `0b2f188c3` verified all 70 body hashes and
21 complete files, including checks for held CPU labels. Cached x86/x64
product builds and documentation governance passed. All ten product images
matched the release exactly or differed only by four PE linker timestamp
bytes; runtime tests and publication were not repeated for equivalent images.
