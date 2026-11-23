# T484 S3 P78 measurement producers and isolation disposition

B0104 contains no product compiler/linker invocation, but its fresh test-package
identity path still calls Set-PackageIdentity.ps1 and writes the copied manifest.
It therefore remains pending for S4 staging reconciliation, not exempted merely
because its process/desktop/identity-reading helpers are standalone test code.
No isolation ownership, temporary-directory policy or runtime cleanup changes
are made in this batch.

B0105/B0106 now delegate three measurement targets to CMD/native fixtures:
window render, production publisher/mouse primitives, and the existing test-only
high-resolution publisher variant. Both architectures are explicit; the old
arbitrary CompilerWrapper argument is replaced by Architecture=x86/x64. There
are no checked-in callers of the old wrapper argument. Source lists, /MT /O2,
/W4 /WX, render C11 and variant selection remain. Common fixture linking adds
its normal map and /OPT:REF policy; no byte-equivalence claim is made for these
test executables. Production code and immutable guest files are unchanged.

The shared measurement runner owns separate bounded build/measurement children,
stdout/stderr, start/PID/exit receipts and a fresh disposable root. The two
public scripts retain their result text and publisher switch. Build commands
are no longer emitted in PowerShell. Measurement assertions are unchanged.

[Measurement receipt](../../../artifacts/build-migration/t484/s3-measurements/measurement-result.json)
records PID5364 exit0 for all six architecture/target cases. Each render test
samples240 frames; each worker test measures64 publisher deliveries and200000
mouse queue operations. These values characterize this run, not a universal
latency promise or evidence of product improvement. Full host build, cache,
wrapper and unchanged dual-publication checks are recorded at delivery.
Raw runs: build/t484-s3-measurements. No new guest acceptance is claimed.

Cached full build PID26336 and final verification PID20924 exit0. All six
measurement images retain hash/mtime on no-op builds; the three public runner
cases also pass. All11 raw host inputs and22 published hashes are unchanged:
existing dual publication is verified, no measurement binary is published.
B0105/B0106 complete; B0104 remains pending. Fixed355 inventory is154 complete
and201 remaining:52 pending producer/staging items and149 reconciliations.
