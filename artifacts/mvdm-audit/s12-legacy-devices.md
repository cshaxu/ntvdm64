# S12: unselected legacy device and helper branches

The observed MVDM product commands do not select `REAL_VGA`, `NULLSTRUCT`,
`BACK_M`, `SYNCH_TIMERS`, `FIFO_ON`, `GORE` or `IRET_HOOKS`. No definitions
or undefinitions of those flags were found in project/generated C and header
sources or the recorded compiler predefined macros. The selected source and
header closure contained 180 original-source positive branches, replaced by
markers with their preprocessor shells retained. Existing divergence markers
and three project-modified BACK_M branches were not bulk removed; the latter
require separate review. The companion JSONL records paths and body hashes.
This evidence does not cover unobserved build profiles.

Independent reconstruction from `001a3f272` verified all 180 body hashes and
34 complete files. Cached x86/x64 product builds and documentation governance
passed. All ten product images matched the release exactly or differed only by
four PE linker timestamp bytes; runtime tests and publication were not
repeated for equivalent images.
