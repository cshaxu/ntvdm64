# M0 T310 S8 P4 — CCPU VGLOB native-pointer contract audit

## Scope

This package closes the six selected CCPU `vglob.c` accessor pairs for
`fwd_str_read_addr`, `bwd_str_read_addr`, `mark_byte`, `mark_word`,
`mark_string`, and `read_byte_addr`.  The S8 whole-tree scope remains only
`mvdm-host` and `mvdm-support`.

## Source reading

- The selected CCPU GDP declaration carrier,
  `softpc.new/base/ccpu386/evid_c.h`, declares all six stored fields as
  `IU32 *`.
- The original `vglob.c` bodies stored and returned those fields through
  `IUH *` signatures; NT4's 32-bit build made the unrelated base types
  interchangeably pointer-sized.
- `softpc.new/base/inc/egacpu.h` repeated the same six public declarations.
- The generated C-video `VideoVector` is a separate interface family.  Its
  source-era K&R rule-table mismatch stays under `MVDM-HOST-DIV-042` and its
  existing generated private carrier.  This change neither edits nor enables
  that table.

## Disposition

The six accessor declarations now use the GDP record's existing `IU32 *`
type, preserving their original names, fields, ordering and value semantics.
They carry private native pointers to generated rule entries; they are not
guest linear/physical addresses and must not become mapping-manager IDs.
This mirror divergence is registered as `MVDM-HOST-DIV-051`.

## Verification

The formal selected-original candidate was regenerated and rebuilt outside the
sandbox on both supported host architectures:

- `build/M0-T310/S8/p1-machine-source/x64/cvidc-vglob-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/cvidc-vglob-x86-build.log`

Both `original-softpc-candidate` archives completed.  Neither log reports a
`vglob.c` `C4057`, `C4311`, or `C4312` diagnostic.  Unrelated historical
warnings remain intentionally unsuppressed for later whole-tree source review.
