# T484 S3 P74 current message transaction producer

The current mode of B0101 now uses the shared native one-object diagnostic
provider. It copies wmdisp32.c byte-for-byte into the diagnostic include root,
compiles the unchanged transaction wrapper with the real original TU profile,
and substitutes that object in the current WOW32 link order. The runner keeps
normal DLL identity, manifest and disposable-root restrictions.

The explicit P21Control mode still requires the exact historical DLL hash and
fixed original revision. It is retained unchanged as a separate historical
branch, not silently mixed with current objects or replaced by a current-build
counterexample. Its interpreted production steps are NOT migrated in this
batch. B0101 therefore remains pending-producer-migration, not complete.
The next work must migrate the sealed control recipe without widening its
identity contract; current native success is not historical control success.

No original guest or production source is modified. Current-mode producer,
object/profile and byte-copy checks are separate from actual BeforeCallback/
AfterCallback guest fault acceptance, which is not rerun in this batch.
Raw evidence: build/t484-s3-message-transaction; concise delivery receipts:
artifacts/build-migration/t484/s3-message-transaction.

## Verification

Cached full host build PID2148 exited0. Final verification PID32732 exited0:
229 objects retain their order with exactly one owner replacement, original
source copy is byte-identical, cached output hash and mtime are unchanged,
x64 is rejected, and the callback-frame fixture passes. The sealed-control
mode rejects the current provider before creating its output directory.
All11 production inputs and22 published hashes remain unchanged; existing
dual-location publication is verified, not replaced by diagnostic images.
No fresh guest transaction fault run is claimed. Fixed355 inventory remains
150 complete and205 pending:51 producer migrations and154 reconciliations.
