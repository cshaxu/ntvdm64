# opennt-abi

This mirror component holds only byte-identical, declaration-only OpenNT
headers required outside the canonical MVDM source union. It owns no behavior,
provider body, compatibility implementation, or machine access.

## Layout and provenance

- `source/public/...` and `source/private/...` preserve only logical upstream
  topology; neither source-tree nor edition names appear in product paths.
- [T283 S1 selection ledger](../../docs/etc/operations/m0-t283-s1-platform-abi-selection-ledger-001.tsv)
  records the exact selected source/hash, rejected corresponding candidate and
  basis for every header. The companion [canonical import manifest](../../docs/etc/operations/m0-t283-s2-platform-abi-canonical-import-manifest-001.tsv)
  records each selected logical product path.
- [T274 S2 import manifest](../../docs/etc/operations/m0-t274-s2-platform-abi-import-manifest.tsv)
  is retained as the historical initial-import record; T283's canonical
  manifest is the current selection authority.
- [T274 S2 direct-include ledger](../../docs/etc/operations/m0-t274-s2-platform-abi-direct-include-ledger.tsv)
  records every scanned direct declaration need and its disposition.

The component presently contains the 24 resolved direct platform
headers plus three package-led DEM declaration carriers (`ntdddisk.h`,
`ntddstor.h`, and `ntioapi.h`) recorded by
[T280 S8](../../docs/etc/operations/m0-t280-s8-platform-declaration-import-manifest.tsv).
CRT and public SDK headers remain toolchain inputs; ambiguous, generated, and
unresolved include forms remain explicit ledger rows for their owning MVDM
package rather than speculative copies.

T281 S1 selected the byte-exact public internal `vdm.h` from the OpenNT
baseline after checking the reached `VdmServiceClass` layout used by original
`softpc.new/host/src/nt_bop.c`; its hash and target path are recorded in the
same manifest. This supplies declarations only and does not admit monitor,
selector, or provider behavior.

T282 S2 selected byte-exact public internal `vdmapi.h` and PIF `pif.h` for
the original COMMAND package. `vdmapi.h` supplies the `VDMINFO` and
`GetNextVDMCommand` declarations; `pif.h` supplies only the historical PIF
layouts. Neither import supplies a callable VDM API, PIF provider, selector or
runtime route. Their hashes and original callers are recorded in the same
manifest.

T282 S3 selected byte-exact public internal `winconp.h` for original COMMAND
keyboard-layout declarations. It does not enable keyboard policy, registry
access, console initialization, a `54:0E` provider or guest-memory writes.

T309 S2 P3 selected byte-exact public internal `conapi.h` for the original
SoftPC-host PIF/error/console source cohort. It supplies declarations only;
private Console/CSRSS operations remain named adapter or product-boundary
decisions and this import enables neither a console product shell nor a VDM
route.

T291 S5 P3 selected byte-exact public SDK `ntcsrsrv.h` for the original
OpenNT BaseSrv VDM package now mirrored in `opennt-host`. It is only the
declaration carrier for the original CSR server shapes; it neither recreates
CSR nor enables a BaseSrv runtime route. Its source/hash/final-disposition row
is `OPENNT-PLATFORM-FILE-0001` in the shared host tracker.

T290 S3 selected byte-exact public internal `align.h` for the reached original
Redirector named-pipe provider.  The newer OpenNT cohort preserves its
historical macro names while using `ULONG_PTR` alignment arithmetic on both
supported host widths.  The differing OpenNT-4.5 private candidate is recorded
in the canonical import manifest; this is a source-cohort selection, not an
adapter or source-body divergence.

T290 S5 selected the complete byte-exact OpenNT
`public/internal/ds/inc` declaration package under
`source/public/internal/ds/inc`. This preserves the historical declaration
cohort required by the original Redirector NetAPI source without importing a
Lanman implementation, server, RPC service, or Redirector route. The
package-selection record and per-file source/hash manifest are
[here](../../docs/etc/operations/m0-t290-s5-ds-declaration-package-selection-001.md)
and
[here](../../docs/etc/operations/m0-t290-s5-ds-declaration-package-manifest-001.tsv).

The same S5 compilation closure selected exact OpenNT
`public/internal/base/inc` and `public/internal/net/inc` declaration cohorts.
They supply the original `smbgtpt.h` and `tstring.h` dependencies of the DS
headers only; neither package activates its historical host services. Their
selection and per-file hashes are recorded
[here](../../docs/etc/operations/m0-t290-s5-public-internal-declaration-cohort-selection-001.md)
and
[here](../../docs/etc/operations/m0-t290-s5-public-internal-declaration-cohort-manifest-001.tsv).

T290 S5 selected the complete byte-exact OpenNT
`public/internal/ds/inc` declaration package under
`source/public/internal/ds/inc`. This preserves the historical declaration
cohort required by the original Redirector NetAPI source without importing a
Lanman implementation, server, RPC service, or Redirector route. The
package-selection record and per-file source/hash manifest are
[here](../../docs/etc/operations/m0-t290-s5-ds-declaration-package-selection-001.md)
and
[here](../../docs/etc/operations/m0-t290-s5-ds-declaration-package-manifest-001.tsv).

## Divergence register

None. Every current file below `source/` is byte-identical to the exact source
path and hash recorded in the import manifest.
