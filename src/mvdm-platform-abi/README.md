# mvdm-platform-abi

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

The component presently contains the 23 resolved direct platform
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

## Divergence register

None. Every current file below `source/` is byte-identical to the exact source
path and hash recorded in the import manifest.
