# opennt-platform-abi

This mirror component holds only byte-identical, declaration-only OpenNT
headers required outside the canonical MVDM source union. It owns no behavior,
provider body, compatibility implementation, or machine access.

## Layout and provenance

- `source/opennt/...` preserves paths relative to
  `O:\repos.external\OpenNT`.
- `source/opennt-4.5-private/...` preserves paths relative to
  `O:\repos.external\OpenNT-4.5\nt\private`.
- [T274 S2 import manifest](../../docs/etc/operations/m0-t274-s2-platform-abi-import-manifest.tsv)
  records every imported path, SHA-256, direct MVDM consumer and import action.
- [T274 S2 direct-include ledger](../../docs/etc/operations/m0-t274-s2-platform-abi-direct-include-ledger.tsv)
  records every scanned direct declaration need and its disposition.

The component presently contains the 22 initially resolved direct platform
headers plus three package-led DEM declaration carriers (`ntdddisk.h`,
`ntddstor.h`, and `ntioapi.h`) recorded by
[T280 S8](../../docs/etc/operations/m0-t280-s8-platform-declaration-import-manifest.tsv).
CRT and public SDK headers remain toolchain inputs; ambiguous, generated, and
unresolved include forms remain explicit ledger rows for their owning MVDM
package rather than speculative copies.

## Divergence register

None. Every current file below `source/` is byte-identical to the exact source
path and hash recorded in the import manifest.
