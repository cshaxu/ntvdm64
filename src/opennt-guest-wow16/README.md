# opennt-guest-wow16

Load-only WOW16/bin86 guest carry recovered byte-identically from the existing
repository-local guest inventory during M0 T274 S4. It is not a claim that the
external OpenNT WOW16 source universe is complete here. This component and the
retained output products never supply a host compile or link input.

## Provenance

- [WOW16 carry manifest](../../docs/etc/operations/m0-t274-s4-wow16-carry-manifest.tsv)
  records 44 directly recovered local files.
- [bin86 carry manifest](../../docs/etc/operations/m0-t274-s4-bin86-carry-manifest.tsv)
  records 10 directly recovered local files under `bin86/`.
- [WOW16 output manifest](../../docs/etc/operations/m0-t274-s4-wow16-output-manifest.tsv)
  records the 87 immutable products retained in `build/output/wow16`.
- [S4 closure evidence](../../docs/etc/operations/m0-t274-s4-wow16-guest-recovery-closure-001.md)
  records recovery and in-place output verification.

`build/output/wow16` remains in place; it is neither copied into this root nor
made a host input. App may later load only manifest-selected immutable bytes.

## Divergence register

None. All carried files beneath this component are byte-identical to the local
source path and SHA-256 recorded in the two carry manifests.
