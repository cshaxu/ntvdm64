# opennt-mvdm-host

Canonical selected non-guest/non-tool OpenNT MVDM mirror. M0 T274 S5 recovered
its original package topology, headers and static resources byte-identically;
provider bodies remain deliberately deferred to S6. This component has no
locally authored provider replacement.

## Provenance

- [S1 canonical union ledger](../../docs/etc/operations/m0-t274-s1-non-guest-mvdm-union-ledger.tsv)
  selects one source for every host path.
- [S5 topology manifest](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-manifest.tsv)
  records all 1,689 host rows, including imported/deferred disposition, source
  path and SHA-256.
- [S5 closure evidence](../../docs/etc/operations/m0-t274-s5-mvdm-host-topology-closure-001.md)
  records hash verification and the S6 body boundary.

The root currently contains 1,050 exact topology/header/resource inputs. The
612 C/C++/assembly provider-body rows and 27 non-topology inputs are explicit
S6 ledger rows, not missing or substituted code.

## Divergence register

None. Every current imported file is byte-identical to the selected source and
hash recorded in the S5 topology manifest.
