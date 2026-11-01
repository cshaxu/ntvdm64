# S12: non-NTVDM host alternatives

The current x86 worker and WOW32 Ninja command/dependency join observed 443
MVDM C source files, all compiled with an explicit `NTVDM` definition. Of 434
MVDM headers, all observed consumers explicitly define it for 305; the other
129 have mixed or not-explicitly-defined command contexts and were not used
for this cleanup. `host_def.h` also defines NTVDM, so an absent command-line
definition is not evidence of an absent preprocessor definition. No project,
generated C/header, Windows SDK or Visual Studio header undefinition was
found. The companion CSV records consumer counts by source/header.

Only in the explicitly defined contexts, 226 original-source non-NTVDM
branch bodies or alternatives were replaced by markers with their conditionals
retained. Original ranges and hashes are recorded in the JSONL. Existing
divergence markers and modified bodies were excluded; removed bodies were
also checked not to contain the held `i386`, `SPC486` or
`TAKE_REAL_MODE_LIMIT_FAULT` labels. Unobserved targets and profiles remain
outside this evidence.

Independent reconstruction from `addf2b75a` checked all 226 body hashes,
36 complete files and the relevant per-consumer flags. Cached x86/x64 product
builds, a separate VDMREDIR build and documentation governance passed. All ten
product images matched the release exactly or differed only by four PE linker
timestamp bytes; runtime tests and publication were not repeated for
equivalent images.
