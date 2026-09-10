# M0 T301 S1 P5 — Complete MVDM Source-Union Correction

The approved editions place the same historical package at different paths:

- `OpenNT/base/mvdm`
- `OpenNT-4.5/nt/private/mvdm`

The former generator accidentally searched only the first. T301 now indexes
both roots. If a project-mirrored file has the same relative path and bytes in
both editions, the ledger records it as an edition-equivalent identity and
uses the OpenNT copy only as the canonical reader. This does not merge
different bytes, different paths, linkage forms, or same-named definitions.

The complete-union regeneration yields:

- 12,110 physical zero-degree definition identities;
- 4,265 retained original-MVDM variant-family edges;
- 12,426 physical direct calls leaving the MVDM closure; and
- 1,446 first-degree spelling index rows.

These regenerated ledgers supersede the P4 intermediate counts. They remain
only an audit frontier: no external body is selected or read for final
disposition, no provider/import/build change is admitted, and `src.old` is
not an input.
