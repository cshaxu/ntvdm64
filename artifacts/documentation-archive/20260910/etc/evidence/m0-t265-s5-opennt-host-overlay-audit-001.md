# M0 T265 S5 OpenNT-host mirror/overlay audit

## Result

The 11-file production host mirror contains seven byte-identical re-rooted
OpenNT files and four source-shaped subsets/minimal edits. `nt_error.c` retains
101 of 1,190 baseline lines; `vrinit.c` retains 78 of 748; `vrnmpipe.c` retains
1,046 of 3,412; `vrputil.c` retains 190 of 194. Their removed regions are
complete, unadmitted NT4 product-shell blocks, not substitute algorithms.

Three oversized product-binding declaration blocks now live in the private
`opennt-host-overlay`: `nt_error_bindings.h` (HOST-DIV-012/014),
`vrinit_bindings.h` (017), and `vrnmpipe_product_seams.h` (024/025). The
original OpenNT functions retain their ordering, layout and reached failures.
Existing statement-local x64/failure edits remain registered in
`src/opennt-host/README.md`.

Formal Ninja compiled changed `nt_error.c`, `vrinit.c`, and `vrnmpipe.c`; then
linked and ran `t263-s8-opennt-host-mirror-fixture.exe` with exit zero on
2026-08-24. No BOP, capability, machine or host provider was enabled.
