# S12: non-MVDM original-source placeholder branches

The observed x86 and x64 product commands do not select `LATER`, `NEVER`,
`NOT_NEEDED` or `BOGUS`; no definitions or undefinitions of these labels were
found in project/generated C or header sources. Eleven unselected bodies match
the original OpenNT sources. A twelfth LATER body in `btnctl.c` had been
adapted in commit `ead0aa64a0`; its existing active callout-safe release and
OPENNT-HOST-117 records are preserved. Their conditional shells remain. The
companion JSONL records source ranges, hashes and original/adapted attribution.
Unobserved profiles are not covered.

Independent reconstruction from `a88e3a8a9` verified twelve hashes and ten
complete files. Cached x86/x64 product builds and documentation governance
passed. All ten product images matched the prior release exactly or differed
only by four PE linker timestamp bytes; runtime tests and publication were
not repeated for equivalent images.
