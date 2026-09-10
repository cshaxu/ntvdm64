# T221 S3 FCB wildcard Overlay closure

The Overlay wildcard family is now closed for its admitted scope.

The source-built MSVC x64 `/MT` provider fixture at
`build/M0-T221-S3/092` covers:

- `50:07` Overlay wildcard delete succeeds while the matching host file stays
  present;
- `50:20` successfully renames an Overlay-only FCB file to a distinct target
  template, leaves a source tombstone, and retains target COW contents;
- same-name rename retains the carry failure; and
- Direct, Readonly and Virtual retain their existing respectively direct,
  access-denied, and unavailable dispositions.

Both services share the same visible enumeration and private mutation backend.
No host path, handle, BOP decode, or DOS kernel state enters that backend.
This does not close the remaining T221 search or Virtual packages.
