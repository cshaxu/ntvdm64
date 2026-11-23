# T484 S3 P44: shared Win16 probe production

B0043 (DDEML) and B0046 (multimedia timer) now delegate assembly to the existing
CMD fixture entry and one native win16-probes target. Runtime runners select
and copy the corresponding cached output. They retain their source, assembler,
DLL and runtime identity records, private-desktop checks and original assertions.
These are independently authored probe binaries, not rebuilt original guest.

The graph contains all nine selected variants: DDEML initialization,
registration and its self-callback control, window-word registration with and
without self-callback, the timer DLL and default/load-only/no-timer executables.
All nine match the former direct NASM commands byte for byte. The second build
reports no work and preserves every output timestamp. Both edited runners parse.
Existing selection precedence is retained, including LoadOnly before NoTimer.

[Variant verification](../../../artifacts/build-migration/t484/s3-win16-probes/verification.json)
records exact hashes and the owned run. No new guest runtime pass is claimed;
identical probe bytes reuse existing runtime coverage under the owner's
no-repeat authorization. Cached full host build,11 raw identities,22 published
hashes and documentation governance are recorded separately. Product sources,
original guest media and published image bytes remain unchanged.

Both entries remain pending for their shared logon-helper/runtime staging
production chain. RPC package identity stamping belongs to the outstanding
publication work; other Win16 probes have not been migrated by this batch.
This is native probe production delivery, not S3 or whole-chain closure.
