# M0 T280 S6 — whole DEM compile-surface closure plan

S5 closed the only reached private VDM declaration/control boundary.  S6
does not select a BOP or enable a provider.  It admits the entire original
`dos/dem/sources` selection to a reproducible syntax-only survey, so the next
adapter package is derived from all original DEM bodies rather than from a
trace-selected leaf service.

## Scope

- Compile each of the sixteen original C bodies with MSVC x64 and x86 `/Zs`,
  `/MT`, `/W4`, the original component include roots, and the existing
  `adapter-win32/include/nt.h` forced include.
- Keep each raw compiler log only in the disposable
  `build/M0-T280/s6/<architecture>/` run root.
- Publish one committed ledger with a first blocking source family, its
  unique future owner, and the mapping rule for every body.
- Identify package-wide source-form prerequisites without adding a source
  body, a BOP route, a new compatibility shim, or a guest execution claim.

## Explicit boundary

The survey separates host-local declarations from guest numeric address
operations.  A reached historical `GetVDMAddr`/`Sim32*` path is not a reason
to cast an address to `PVOID`: its future owner is `adapter-softpc`, which
must use the session `guest_memory` mapping-manager instance and a bounded
lease before calling `adapter-bochs`.  Host-local compiler objects such as
`VDMQUERYDIRINFO`, `DISK_GEOMETRY`, or a temporary Win32 structure are not
mapping-manager keys.

## Non-goals

- no original DEM body edit, link, provider enablement, BOP ingress change,
  Bochs change, guest trace, or private NT kernel call;
- no per-file workaround that duplicates a package-wide ABI family;
- no conclusion that a syntax-clean translation unit is runtime complete.

## Exit criteria

The ledger covers all sixteen source selections, has a unique owner and next
disposition for every syntax blocker, records both architecture outcomes, and
distinguishes public-header completion, original-package globals, historical
Win32/NT contracts, and SoftPC/CCPU mechanical contracts.  A verifier checks
the selection and ledger shape; formal compiler output is evidence, not an
acceptance substitute.
