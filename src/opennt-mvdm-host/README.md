# opennt-mvdm-host

Canonical selected non-guest/non-tool OpenNT MVDM mirror. M0 T274 S5 recovered
its original package topology, headers and static resources byte-identically;
S6 then recovered every selected original provider body and remaining input
without changing its contents. This component has no locally authored provider
replacement.

## Provenance

- [S1 canonical union ledger](../../docs/etc/operations/m0-t274-s1-non-guest-mvdm-union-ledger.tsv)
  selects one source for every host path.
- [S5 topology manifest](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-manifest.tsv)
  records all 1,689 host rows, including imported/deferred disposition, source
  path and SHA-256.
- [S5 closure evidence](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-closure-001.md)
  records the topology/body boundary.
- [S6 host-input manifest](../../docs/etc/operations/m0-t274-s6-mvdm-host-input-manifest.tsv)
  records the 639 S5-deferred selections and their copied-file hashes.
- [S6 closure evidence](../../docs/etc/operations/m0-t274-s6-mvdm-host-package-supply-closure-001.md)
  records full selected-host coverage and verification.

The root contains all 1,689 selected host inputs across 23 original package
roots: 1,050 topology/header/resource inputs and 639 S6-recovered inputs
(612 C/C++/assembly provider bodies plus 27 remaining forms).

## Divergence register

None. Every current file is byte-identical to the selected source and hash
recorded by the S5 topology manifest and, for the deferred inputs, S6 manifest.
