# M0 T263 S4 — OpenNT `suballoc` utility admission

Date: 2026-08-24

## Decision

The complete eight-file original `base/mvdm/suballoc/` package is admitted
source-identically at `src/opennt-utils/suballoc/`. It is the historical
suballocation implementation used by the original XMS and DPMI32 bodies.
Its placement preserves that reusable original utility ownership without
turning `opennt-utils` into a general-purpose project helper root.

## Provenance and verification

- Source: `O:\repos.external\opennt\base\mvdm\suballoc`, Git `5c5b979e`.
- Target: `src/opennt-utils/suballoc`.
- Count: eight original files: `debug.c`, `foo.bar`, `makefile`, `sources`,
  `suballcp.h`, `suballoc.c`, `suballoc.txt`, `tsa.c`.
- Verification: every source/target pair was checked by upstream-relative
  path and SHA-256 after copy. No local body changes or divergence rows exist.

## Dependency disposition

The original direct consumers observed in the source tree are `xms.486` and
`dpmi32`. Their owner packages remain `opennt-bop` plus declared
`adapter-softpc` prerequisites. This packet deliberately adds neither a
modern static library nor an allocation substitute: compilation waits until a
reached owner package can preserve the original header/callback contract.

`vdmutils` was separately audited and remains split by product: guest utility
programs (`forcedos`, `graftabl`) are candidates for `opennt-guest`, while
host/UI products (`pifedit`, `win`) require their own owner admission. None
is silently placed in `opennt-utils`.
