# T484 S3 P190: remote enumeration witness producer

The retained focused probe exercises original RAP failure dispatch, repeated
unavailable-provider calls, enumeration status/count behavior and a separate
local workstation query. Its expected status is caller-supplied uint32.

`verify-redirector-remote.ps1` now invokes
`tools/build-fixtures.cmd x86 guest-remote <decimal-status>` and copies the
cached RAPTEST.COM. CMD rejects missing, nondecimal, noncanonical and overlong
arguments; CMake also validates the uint32 bound. The exact NASM define and
source remain unchanged. Other fixture invocations reset the cached option
to its default rather than inheriting an invalid earlier configuration.

The original probe compares AX, so the uint32 upper-bound build retains its
existing assembler truncation warning. Byte equivalence is not proof that such
a value is a meaningful real DOS return status. No probe logic is changed to
silence that warning. No original guest or production source changes.

## Verification

Owned `build/t484-s3-remote-probe/run.ps1` verifies exact old/new image bytes
for0,50,234,4294967295, no-op cache, refusal of missing/negative/leading-zero/
nonnumeric/out-of-range inputs, and restoration of a valid configuration after
the failed configure. images.json records actual compared hashes; result.json
records the final run status. Old assembler invocation is an oracle only.

Cached full and11 raw/22 published hash verification are recorded separately in
delivery-result.json. Unchanged host products retain their existing dual
publication. No new remote-service/guest runtime or Full pass is claimed.
Final documentation/parser checks are recorded by closure-result.json.

Fixed census316/355 disposed,39 pending is unchanged. Of P183's18 extra NASM
scripts,17 producer sites are handled and one remains. Eighteen supplemental
caller chains retain shared preparation/staging work. S3 is not closed.
