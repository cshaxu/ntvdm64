# M0 T261 S10 — configuration and residual host-source-truth audit

## Question

After S8/S9, does any live `opennt-host` production source remain a generic
configuration shim or an unclassified source-truth row? Does the reached
configuration-complete notification preserve the original OpenNT meaning?

## Inputs and procedure

- Re-read the binding S1/S2 host ledgers and S8 final audit.
- Enumerated every live `src/opennt-host` production path and the formal
  `opennt-host` source list in `t260-s8-component-manifest.json`.
- Scanned for `*_shim.*`, old configuration-shim names and unregistered
  `DIVERGENCE(HOST-DIV-...)` edits.
- Compared the retained PIF triplet with its exact OpenNT paths and compared
  the reached `UMBNotify` fragment with
  `refs/opennt/base/mvdm/softpc.new/host/src/nt_umb.c`.
- Reviewed the `MS_bop_E` caller and executed the focused configuration
  fixture using the existing formal Ninja cache.

## Current disposition

- `softpc.new/host/inc/nt_pif.h`, `nt_uis.h`, and `src/nt_pif.c` are
  byte-identical, re-rooted original OpenNT files. Their SHA-256 values are,
  respectively, `A8069D676141812CDDC35ADB0B29EA58E58BBA0A7F29DCF287468108188B8D09`,
  `FC00F8C6FFFF9C57F5F0B203C102709DD8A456CD2644DF1DFEB5B5155B81C71E`, and
  `4AE2C1D2C4A66DC63C9B2DDD673FCAE067F2FBDAB953DC8D96AECFCF03C262BC`.
- `softpc.new/host/src/nt_error.c` is the re-rooted original host body with
  only `HOST-DIV-012..014`, all registered in `opennt-host/README.md`.
- `vdmredir/{vrinit,vrnmpipe,vrputil}.c` are the re-rooted reached original
  host bodies with only `HOST-DIV-015..023`, likewise one-to-one registered.
  Their compatibility headers are same-shaped declarations, not generic host
  providers.
- There are eleven live `opennt-host` files including `README.md`, no
  `*_shim.{c,h}` path, no `config/` source root and no `config_complete_shim`
  reference in a live production input. The formal archive compiles the four
  reached original host C bodies listed above.
- The configuration-complete route is correctly BOP-owned:
  `opennt-bop/softpc/nt_bop_config_done.c::MS_bop_E` obtains the original
  `AL` notification code, calls `UMBNotify(0)` only for code zero, then calls
  `demDasdInit()`. The reached original `nt_umb.c::UMBNotify(UCHAR)` has an
  empty result. Its extracted fragment retains that signature and result;
  `(void)code` is the sole `BOP-DIV-081` adjustment required by MSVC `/W4 /WX`.
  It is not an UMB allocator, BIOS action or device capability.

## Verification

- Refreshing the existing `build/t261/s9-r001` Ninja graph rebuilt only five
  affected edges: two `opennt-bop` C objects, the fixture object,
  `opennt-bop.lib`, and the fixture executable. No `bx-core` object was
  recompiled.
- `t245-s11-config-complete-fixture.exe`: reproduces `exit=2`. The route
  reaches the original `demDasdInit()` tail, whose `demFloppyInit()` invokes
  the explicit unavailable FDC/INT13 seam. The result is a known
  `opennt-bop`/machine dependency, not a configuration-shim or `opennt-host`
  layout failure. The rebuilt `UMBNotify` object itself compiles and links.

## Interpretation

S10's configuration and residual-source-truth condition is satisfied. The
only full historical `nt_umb.c` dependency not imported is its UMB allocator
and product configuration shell, which is outside the reached empty-notify
body and remains a later machine/XMS owner package, not a hidden `opennt-host`
shim. No later-discovered `opennt-host` row requires implementation. The
separate `demDasdInit` FDC/INT13 tail remains explicitly outside S10.
