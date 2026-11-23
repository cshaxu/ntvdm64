# T484 S3 P45: fixed Win16 probe producers

B0048, B0049, B0050 and B0052 now use one thin runtime delegation helper for
PrintDlg cancellation, default printing, raw spool and ToolHelp probes. CMD and
the native Win16 graph own assembly, dependencies and cached outputs. Both EXE
and listing files are declared outputs, so a missing listing is rebuilt rather
than silently omitted. The helper records its owned process, command and result;
each caller retains its previous output/listing names and runtime assertions.

The actual helper was invoked for all four probes. Each executable and assembly
listing matches its prior direct NASM command byte for byte. The final no-op
preserves all cached timestamps. All four callers and the helper parse. The
nine DDEML/timer variants delivered previously retain their recorded hashes.

[Verification](../../../artifacts/build-migration/t484/s3-win16-fixed/verification.json)
and adjacent receipts record these observations. No original guest is modified
or rebuilt, and no new Win16 runtime pass is claimed. Identical witness binaries
reuse existing runtime coverage under the owner's no-repeat authorization.
Cached full host build,11 raw images,22 published hashes and documentation
governance are recorded separately. Product images remain unchanged.

The four original entries remain pending for the shared package-staging chain.
The new helper is runtime orchestration, not a compiler-command generator;
it is included in this delivery's review although absent from the fixed original
355-file inventory. Final S5 sweeping must also cover newly introduced files.
Other Win16 producers, publication migration and clean acceptance remain open.
