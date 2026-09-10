# M0 T225 S51 P1 — Source-built post-handoff integration observation

Fresh S51 staging revalidated the same immutable original-toolchain NTIO,
NTDOS, COMMAND and SHARE/TARGET identities through the dedicated S51 stager.
Using the existing formal lifecycle-r3 native CLI and only its existing
default-off ledger option, one 1,000,000-tick Direct and Readonly run each
returned exit 4. Their complete logs are byte-identical:
`4f0153fc7fc69555dc02450ad3e7c07e9f094fe7465ea3d15c6815d70674f407`.

Each log records exactly one accepted/resumed lifecycle entry:
`50:36`, `CS:EIP=0041:5a70`, `DS:ESI=0c41:0100`, resume `5a74`; count is one
and overflow zero. Neither `50:3C` nor `54:0B` occurs before the budget
terminal. The run also retains the existing repeated `read beyond limit`
diagnostics.

S50 separately proves the generic source-shaped `mov SS`/`mov SP`/push/`retf`
mechanics. Therefore S51 does not attribute this unchanged native boundary to
those mechanics and does not select a BOP, provider, device or Bochs patch.
The result is a whole-owner limitation requiring a source/current explanation
of the retained read-limit path before any repair is admitted.
