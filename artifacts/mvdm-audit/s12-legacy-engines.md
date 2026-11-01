# S12: unselected historical CPU and display branches

The observed MVDM product command graphs do not select `A3CPU`, `GISP_CPU`,
`GISP_SVGA` or `HERC`; no project/generated C or header definitions of those
flags were found. In the observed source/header closure, 116 outermost
original-source branches were replaced with registered markers. One nested
candidate was subsumed by its removed outer branch; an existing divergence
marker was retained. The JSONL records locations and removed-body hashes.
Enabled CCPU and software V86 code was preserved. Unobserved build profiles
are not covered.

Independent reconstruction from `4c462ad7f` verified 116 body hashes and all
35 resulting files. Cached x86/x64 product builds and documentation governance
passed. All ten product images matched the release exactly or differed only by
four PE linker timestamp bytes. Runtime tests and republication were not
repeated for equivalent images.
