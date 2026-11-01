# S12: unselected diagnostic and historical platform branches

The observed MVDM product commands do not select `SHORT_TRACE`, `EGA_DUMP`,
`SUN_VA`, `EMM_DEBUG`, `VR_DIAGNOSE` or `DUMB_TERMINAL`. No definitions or
undefinitions of those flags were found in project/generated C and header
sources. Sixty-two original-source branches or alternatives in the observed
source/header closure were replaced with markers, retaining conditionals. The
companion JSONL records paths, ranges and removed-body hashes. This evidence
does not cover unobserved profiles.

Independent reconstruction from `3b3b01297` verified all 62 body hashes and
17 complete files. Cached x86/x64 product builds and documentation governance
passed. All ten product images matched the prior release exactly or differed
only by four PE linker timestamp bytes. Runtime tests and publication were
not repeated for equivalent images.
