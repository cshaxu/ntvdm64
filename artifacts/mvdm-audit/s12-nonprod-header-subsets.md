# S12 — non-PROD selected header subsets

DIV-504 retains original conditions and active alternatives while replacing
10 non-PROD original branch bodies in eight MVDM host headers. For each
header, current valid Ninja dependencies were joined with the compiler flags
of the actual ten product target command closures; every observed product
consumer of the eight selected headers defines PROD. Six selected objects
without dependency rows were resource or assembler objects and are not used
to assert C preprocessor coverage. The [ledger](s12-nonprod-header-subsets.jsonl)
records each original body hash, selected consumer count and graph. The
headers and bodies match OpenNT; arbitrary non-PROD or non-product profiles
are not claimed equivalent.

Independent byte reconstruction verified all eight files and ten replacements
against `3bd343b30`. Cached x86/x64 ten-component build passed, including
VDMREDIR and both hooks. All ten published images matched exactly or only in
PE link timestamps; no repeat runtime regression or republish was necessary.
No guest changed. Other headers and conditional selectors remain under audit.
