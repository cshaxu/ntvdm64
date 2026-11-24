# T484 S3 P178 DLL BOOLEAN ABI witness cleanup

B0169 compiled the complete current DLL entry bridge against a historical header
with only8 of33 registration providers. Extending this ABI test with25 unrelated
registration stubs would duplicate evolving product composition. Remove the
unused historical provider header instead. Preserve the meaningful independent
one-byte return and explicit-unload cleanup assertions in a narrow current-source
projection. No product source, bridge template, original mirror or guest changes.

The native fixture graph reads the current bridge's unsigned-char declaration
and selects its exact suffix beginning at result = W32DllInitialize(...),
including cleanup and return. Missing/ambiguous anchors fail configuration.
The harness supplies only the DllMain signature/local declaration and the two
cleanup declarations; it does not copy the return conversion algorithm. The
actual source declaration remains the ABI authority. CMD builds the executable,
controlled DLL and Windows-loader witness in the stable x86 cache.

Removed coverage: the two old dialog_bindings counters and the partial mock
registration table. This batch does not prove all registrations or actual WOW
DLL lifecycle; its result labels explicitly exclude them. Actual production
initialization remains in real guest/host-attach tests. These are distinct
contracts and cannot substitute for each other.

Run build/t484-s3-wow-entry-native, PID3112, 2026-11-24T10:29:51Z to10:30:08Z,
exit0. Poisoned EAX upper bytes produce attach=1, false_attach=0,
thread_false=0, detach_false=0, cleanups=2. Process-termination and thread-detach
calls do not repeat unload cleanup. The actual Windows loader rejects false
attach, accepts true attach and signals both cleanup events over8 unload rounds.
C4100 warnings arise from existing naked assembly parameters; no new failure.

Cached full/11 raw/22 publication checks run as PID23820. Only B0169 becomes
migrated-delivered after successful delivery. Shared staging remains pending
elsewhere; expected ledger315/355 disposed,40 remain. The standalone runner
remains PowerShell assertions, but compilation and generation are CMD/native.

PID23820 completed in62.1seconds, exit0.11 raw/22 published identities match;
no redundant publication. Governance is recorded by closure-result.json in
the same batch. Source review confirms no remaining provider-header consumers.
