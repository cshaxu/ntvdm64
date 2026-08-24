# M0 T261 S11 — final OpenNT-host manifest and exception audit

## Question

Do the final `opennt-host` production tree, its formal build inputs and its
local source-divergence register agree on one original-source-first closure?

## Inputs and procedure

- Enumerated all `src/opennt-host` paths and compared the PIF originals with
  their exact OpenNT paths.
- Compared every `DIVERGENCE(HOST-DIV-...)` occurrence with the component
  README register.
- Searched current production components for all S8-deleted generic shim
  names and configuration paths.
- Inspected the formal component manifest's `opennt-host` sources and every
  consumer's public include form.
- Refreshed and executed the existing formal Ninja graph; exercised the PIF,
  direct-access-error and VDMREDIR fixture paths, classifying an observed
  non-host tail by owner.

## Owner and manifest result

The production root contains eleven files including `README.md`:

- Exact re-rooted OpenNT source: `softpc.new/host/inc/nt_pif.h`,
  `softpc.new/host/inc/nt_uis.h`, and `softpc.new/host/src/nt_pif.c`.
- Reached original bodies with local exceptions: `softpc.new/host/src/nt_error.c`,
  plus `vdmredir/vrinit.c`, `vrnmpipe.c` and `vrputil.c`.
- Same-shaped include closure only: `nt_error_compat.h`,
  `vrnmpipe_compat.h` and `vrputil_compat.h`; none is a provider body.

The formal manifest compiles exactly the four reached host C bodies:
`nt_error.c`, `vrinit.c`, `vrnmpipe.c`, and `vrputil.c`. The PIF triplet is
retained as byte-identical production source but is not falsely represented as
a reached static-library object.

Every source `HOST-DIV-012` through `HOST-DIV-023` appears once in a live
source body and once in `src/opennt-host/README.md`; no other host divergence
identifier appears. The deleted `host_namespace`, `redir_session_shim`,
`xms_shim`, `opennt_dem_drive_policy_facade`, `top_level_nosupport_shim` and
`config_complete_shim` paths have no live production source reference.

## Build and focused result

The existing graph refreshed and executed its pending 101 test-object/link
edges, then the 77 dependent executable relinks after the final
`opennt-bop.lib` update. A final `ninja -n all` reported `no work to do`.
This was a test closure rebuild: it compiled no `bx-core` source.

- `t234-s2-command-pif-direct-fixture.exe`: exit `0`.
- `t237-s5-bop-nosupport-fixture.exe` (`nt_error.c` path): exit `0`.
- `t251-s3-redir-ingress-fixture.exe`: exit `8`, after completing its
  VDMREDIR initialization and named-pipe helper/round-trip portion. The
  failing tail is the fixture's subsequent `mailslot_regression()`, which is
  an `opennt-bop/vdmredir` BOP-body/guest-memory path, not an
  `opennt-host/vdmredir` named-pipe provider failure. This pre-existing
  cross-owner tail is not accepted as a host fixture pass.
- `t245-s11-config-complete-fixture.exe`: exit `2` at its later original
  `demDasdInit` FDC/INT13 path, as recorded by S10. `UMBNotify` itself
  compiles/links with its original empty-result semantics.

## Interpretation

The source/manifest/exception requirement is closed. The two nonzero
cross-owner fixture tails are explicitly attributed to `opennt-bop`/machine
semantics and do not expose a remaining `opennt-host` shim or source-layout
row. They remain evidence for their own owner packages; S11 does not relabel
them as passing host behavior.
