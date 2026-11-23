# T484 S3 P16: redirector producers

Four PowerShell producers now use CMD/native targets: VrMisc and VrInit
failure fixtures on x86/x64, COMMAND redirection on the actual x86 worker
profile, and local NetAPI assertions in the existing original-provider test.
The six local assertions are preserved, increasing provider coverage from18
to24 without a second provider graph or `/FORCE:UNRESOLVED`.

The disabled-ingress producer always threw an obsolete-Bochs diagnostic and
is retired. Two historical matrix wrappers reference absent producers; the
remaining-provider wrapper also references an absent disposition TSV. They
are replaced by the current runtime family entry, not claimed as passing
their historical eleven-graph or x64 product matrices. No original source,
guest image or product ABI changes.

The final runtime entry is `test/observation/verify-redirector-family.ps1`.
It delegates all binary production to `tools/build-fixtures.cmd`, runs five
lifecycle instances,14 capacity assertions and24 provider assertions. All
pass; the provider MAP verifies nine actual original entrypoints. COMMAND
redirection retains real thread/pipe completion under a15-second timeout.
[Runtime result](../../../artifacts/build-migration/t484/s3-redirector-lifecycle/result.json)
records executable identities and outcomes.

Initial candidates exposed a missing NTVDM compilation define and unresolved
nonselected NetAPI providers. The final lifecycle profile selects NTVDM;
NetAPI assertions reuse the existing fully resolved provider fixture rather
than suppressing linker errors. An initial aggregate run failed its evidence
path check; relative child paths fixed that runner error and the final full
family run passed. These failures are not product regressions or passes.

Cached full host build PID9536 passed. All11 raw inputs and22 published hashes
match the unchanged manifest/package d8983d2b-3fe1-4d18-8ea9-64613c84346c;
no changed product image requires republication. The adjacent build and
identity receipts record this bounded verification. This is not a Full runtime
rerun. The containing commit delivers P16; S3, S4 and S5 remain unfinished.
