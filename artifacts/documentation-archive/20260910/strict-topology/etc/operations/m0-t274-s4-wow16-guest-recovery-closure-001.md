# M0 T274 S4 WOW16/bin86 guest recovery closure

## Inputs and scope

S4 uses only `src.old/opennt-guest/wow16`,
`src.old/opennt-guest/bin86`, and the existing in-place
`build/output/wow16` tree. It does not compile, reimport external source,
move output products or create a host link input.

## Procedure

`Recover-T274GuestMirror.ps1` copied the two local carry roots with
source/destination SHA-256 conflict refusal. `Export-GuestOutputManifest.ps1`
hashed the existing output tree in place.

## Observations

- 44 WOW16 carry files totaling 282,617 bytes were recovered directly into
  `opennt-guest-wow16`.
- 10 bin86 carry files totaling 340,995 bytes were recovered directly into
  `opennt-guest-wow16/bin86`.
- `build/output/wow16` remains at its original location and its 87 files,
  totaling 2,299,306 bytes, are recorded by SHA-256 in the output manifest.
- The external-only 1,261 ledger paths remain out of scope. Their absence does
  not block the defined load-only carry/output recovery and no completeness
  claim is made for them.
- The scoped live host-input scan found no product build/link consumer. Its
  only four textual hits are the S1 governance ledger generator's owner-name
  classification, not a host input.

## Interpretation

The recovered local files and in-place output manifest define the current
WOW16 guest inventory. Later app composition may select products by this
manifest, but no source/object/library from either guest component may satisfy
a host symbol.
