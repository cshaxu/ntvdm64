# M0 T310 S8 P4 C-video runtime-error call-contract closure

## Question

Can every selected generated C-video unit call the existing original
`CrulesRuntimeError` provider on x86 and x64 with its actual ABI, rather than
relying on the NT4 compiler's implicit-`int` extension?

## Original contract

- `softpc.new/base/cvidc/ev_glue.c` defines
  `GLOBAL void CrulesRuntimeError(char *message)`.
- Every selected `SEVID000.c` through `SEVID030.c` includes the original
  generated support header `j_c_lang.h` and calls that entry for generated
  invalid-bit/bitfield assertions.
- The original header carried declarations for other J-code helpers, but not
  this existing provider.  Consequently modern C emitted C4013 at the reached
  generated call sites.

## Recovery

`MVDM-HOST-DIV-075` adds only the exact existing `void(char *)` declaration to
the common generated support header.  It does not add a provider, change a
generated C-video body, alter a vector/table slot, route data through the
mapping manager, or replace the original diagnostic behavior.

## Verification

From cleaned formal Ninja output trees, the complete original C-video archive
was rebuilt:

- x64: `original-softpc-cvidc.lib`, `[41/41]`, zero errors, zero
  `CrulesRuntimeError` C4013 records.
- x86: `original-softpc-cvidc.lib`, `[26/26]`, zero errors, zero
  `CrulesRuntimeError` C4013 records.

The x64 rebuild still reports two unrelated original C4013 records for
`printf` in `ev_glue.c` and `j_c_lang.c`; they are intentionally left visible
for their separate CRT/source-contract cluster.  No warning was suppressed.

## Disposition

This closes one complete generated C-video diagnostic-provider ABI contract.
It does not close the broader C-video vector/table and base-video callback ABI
clusters, which remain subject to the whole-tree P4 source review.
