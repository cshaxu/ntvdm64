# M0 T276 S8 original provider-form review

## Result

The S6 list of 26 lexical external-boundary candidates is not a list of 26
missing functions.  Direct source review gives the following non-final forms:

- 13 rows have an original selected source body or a selected original body
  with historical dispatch indirection.
- 2 rows are selected callback/global variables, not functions.
- 1 row is the CRT/toolchain import `malloc`.
- 3 rows are optional historical feature calls with no provider body in either
  selected upstream source tree; their callers are macro-guarded.
- 8 rows have no provider body in the selected source universe and require a
  later package/ABI decision rather than an invented replacement.

The detail ledger is
[`m0-t276-s8-original-provider-form-ledger.tsv`](m0-t276-s8-original-provider-form-ledger.tsv).
It preserves the S6 identity and declaration evidence for every row.

## Important corrections

- The PIC rows are original SoftPC machine mechanisms.  Their names appear in
  `ica.h` in direct, macro and callback forms; they are not evidence that an
  adapter is already needed.
- `outsb`, `insb`, `outsw` and `insw` have selected original bodies in
  `base/support/ios.c`, despite declarations under `host/inc/nt_mon.h`.
- `ReadMem` and `WriteMem` are debugger extension callback globals.  Treating
  them as missing procedures would lose their `WINDBG_EXTENSION_APIS` binding
  contract.
- Hunter and generic-driver entries are build-profile candidates.  The selected
  sources and both upstream source trees expose only their guarded callers and
  declarations, not a provider body.

## Limit

This is definition-form evidence only.  It neither enables a source body nor
selects a modern implementation, adapter ABI, build target, profile, failure
contract or Bochs binding.  S9 must expand the actual package-level ABI and
failure contract for the rows that remain actionable after this correction.
