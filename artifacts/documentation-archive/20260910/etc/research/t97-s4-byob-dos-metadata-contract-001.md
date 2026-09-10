# T97 S4 BYOB DOS Metadata Contract 001

## Result

The pathname-search profile input is now a closed, identity-bound declaration.
`ntdos64-byob-profile-v4` / `nt4-en-us-command-smoke-v4` requires exactly four
`guest_search_metadata` entries: `command`, `target`, `config`, and `autoexec`.
Each supplies a DOS attribute byte and the DOS packed time/date values.

The declaration is copied into `byob_profile_selection`; it contains no host
path, handle, file-information query, or current-time source.  It therefore
does not create an ambient host-directory capability.  Versions v1--v3 remain
accepted for their pre-existing startup contracts, but do not carry the v4
pathname-search capability.

## Source-shaped use boundary

OpenNT `demsrch.c` pathname search returns projected attributes, write time,
write date, size, and name in the DTA.  The v4 values are the profile-author's
immutable replacement for the historical host file-information source.  Byte
size remains bound to the separately identity-checked BYOB components or the
fixed boot-file materialization; it is not declared twice.

This work does not enable `50:09` or `50:0B`.  A later S4 integration slice
must consume only this finite snapshot through the common DEM plane, retain
the source DTA no-more result, and leave `50:0A/0C` (FCB) deferred.

## Verification

The following compiled and ran successfully on the local Windows/Clang tool
chain:

```text
clang -std=c11 -Wall -Wextra -Werror \
  -Wno-error=unneeded-internal-declaration -D_CRT_SECURE_NO_WARNINGS \
  -Isrc/cli tests/runner/byob_profile_test.c src/cli/byob_profile.c \
  src/cli/byob_identity.c src/cli/byob_image.c -lbcrypt \
  -o artifacts/build/byob_profile_test_t97s4.exe
artifacts/build/byob_profile_test_t97s4.exe
# byob-profile-test: owned BYOB profile acceptance and rejection families verified
```

The single suppressed error promotion is an existing Clang diagnostic for
`json_skip_value`, unrelated to this contract.  All other warnings remain
errors.  The test accepts a complete v4 record, verifies copied values, and
rejects a record with the required metadata member renamed.

## Non-goals retained

No host namespace fixture is linked, no host directory is enumerated, no FCB
form is mapped, no file operation is restored, and neither Bochs nor the
cross-island adapter ABI changes.
