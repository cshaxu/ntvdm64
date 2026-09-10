# M0 T299 S3 — missing-definition normalization closure

## Final first-degree identity rule

An original first-degree implementation exists only when an individual raw call
maps to a concrete original source-root/path/hash/definition-line/signature
identity. Same spellings never merge implementations. A non-public raw call
without such a definition is an individual `missing-original-definition` row.

## Verified result

- All 2,819 raw external calls occur in the call-to-implementation ledger.
- 358 mappings point to concrete original implementation identities: 200
  include-constrained matches and 158 discovery-only candidates.
- 1,443 raw calls are individually marked `missing-original-definition` with
  their caller source path/hash/line and a missing basis.
- 1,157 raw calls are public Win32/CRT system providers and correctly have no
  OpenNT body; they are not classified as missing original implementations.
- The 76 physical implementation identities are unique by source root, path,
  hash, definition line and signature. All 528 second-degree candidates name
  one of those concrete implementation identities as caller.

This S3 ledger is the complete first-degree closure and the sole input for a
later second-degree audit.
