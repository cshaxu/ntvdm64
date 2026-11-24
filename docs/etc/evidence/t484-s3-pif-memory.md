# T484 S3 P181 native PIF conventional memory

## Ownership and change

Six maintained Win31/Win95 test runners duplicated the same four conventional
memory field writes and standard checksum calculation. Use the existing
format-only PIF.EXE parser/editor instead of retaining test-specific binary
mutation. Add optional --min-memory/--max-memory KiB WORD fields to its existing
create/update command. Apply to STDPIF and the Windows386 record when present;
omitted options retain old behavior. Leave scheduling, application choice,
limits policy and guest installation decisions to each caller.

The six callers now create their PIF once with128/640KiB, retaining all other
arguments. Delete Set-Win95ProbePifMemory.ps1 and two inline patchers. No new
binary patch tool, original guest edit, mirror diff or runtime algorithm change.
The two multitask callers are repaired as actual consumers, not counted as new
entries in the frozen355-file census. All four editor/identity rows still need
configuration and shared staging migration; current tally remains315/355,40.

## Evidence

Extend existing pif_hash_tool_test.ps1. Five title variants compare complete
native create/update results to the previous field writes, including untouched
bytes and checksums. Show verifies all four128/640fields. Out-of-WORD, negative,
malformed and missing numeric values fail without changing the existing file.
Zero-checksum legacy updates retain that convention; create seals its new
record. Existing PIF/HASH and structured root-update cases remain enabled.

Background build/test PID19504 in build/t484-s3-pif-memory exits0 in7.9seconds.
Cached full/publication/published-test PID22624 uses the existing addon release
owner, publishes PIF.EXE plus its source/image hashes, preserves other addon
entries and verifies all14 addon images. The eleven host images remain subject
to the usual raw and dual-location22-hash verification. Addon utilities publish
to assets/release (not host system32), as established by S3 P1. No winnt.zip or
Win31/Win95 installation changes. The existing PowerShell publication owner is
still an explicit S4 migration item; this batch does not claim CMD-only staging.

This is format construction equivalence and utility regression, not a fresh
Windows3.1/95 guest-runtime pass. Final delivery/governance receipts and the
containing P181 commit identify the delivered code.

PID22624 completes in65.2seconds, exit0: cached host build,11 raw/22 published
host identities, PIF publication, both published utility suites and14 addon
hashes pass. PID26708 governance/parser/diff checks pass in25.3seconds.
