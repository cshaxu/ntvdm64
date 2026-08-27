# mvdm-guest

Complete load-only MVDM guest mirror.  The explicit guest roots are:

- `dos/v86/`: selected DOS/V86 guest;
- `bin86/`: selected Bin86 bootstrap carry;
- `wow16/`: selected WOW16 source, resources and products; and
- `font16/`: selected original 16-bit font products.

None is a host library, compile input or link input. App loads only
manifest-selected immutable guest bytes.

## Provenance

- [DOS carry manifest](../../docs/etc/operations/m0-t274-s3-dos-guest-mirror-manifest.tsv)
  records every selected DOS/V86 path and its source hash.
- [WOW16 carry manifest](../../docs/etc/operations/m0-t274-s4-wow16-carry-manifest.tsv)
  and [bin86 carry manifest](../../docs/etc/operations/m0-t274-s4-bin86-carry-manifest.tsv)
  record the selected Win16 paths and hashes.
- [T297 root move manifest](../../docs/etc/operations/m0-t297-s1-guest-root-move-manifest.tsv)
  records all 1,991 canonical old-to-new paths. Its
  [duplicate-elision manifest](../../docs/etc/operations/m0-t297-s1-guest-root-duplicate-elision-manifest.tsv)
  records the ten old supplementary Win16 paths removed only after an equal
  SHA-256 canonical WOW16 target was verified.
- `build/output/dos` and `build/output/wow16` remain in-place immutable
  product evidence. They are not copied here or linked into the host.

## Divergence register

None. Apart from this component README, every carried file below `dos/v86/`,
`bin86/`, `wow16/` and `font16/` remains byte-identical to its recorded local
source path.
