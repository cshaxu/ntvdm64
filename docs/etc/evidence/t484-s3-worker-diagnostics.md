# T484 S3 P24: native worker diagnostic producers

Build-T420S35EnvironmentTrace.ps1 parsed obsolete Ninja variable and rule
spellings, compiled a wrapper, rebuilt one archive and relinked a worker.
Its ten boundary choices plus lifecycle-only mode now use one native
worker-diagnostics group. The retired builder has no current executable caller;
historical research references retain their original meaning.

The graph reads each current owner's target compile flags, substitutes only
the named archive member in place, and reuses product link inputs, dependency
files and options. XMEM failure also substitutes its coupled dispatch wrapper.
Exact member/library presence is checked at configure time. No parallel copy
of the production source list or interpreter parsing of build.ninja remains.
Outputs and event logs are isolated under diagnostics/<boundary> in the cache.
They are not published as production images or complete diagnostic packages.

All 11 variants built and linked; final rerun reported no work. The first
attempt found an expired implicit include in command_environment_trace.c;
adding its explicitly used guest_location.h resolved the compile error.
No original guest or product source changed. The existing diagnostic code's
address assumptions remain diagnostic scope, not new product guarantees.

test/build/test-worker-diagnostics.py verifies every map contains the expected
wrapper object (two for XMEM), and records image hashes. It performs no build.
The [link evidence](../../../artifacts/build-migration/t484/s3-worker-diagnostics/link-selection.json)
covers 11 variants, explicitly runtime_tested=false. No guest diagnostic or
failure-injection acceptance is claimed from this build migration.

Final cached full host build and unchanged 11 raw/22 published identities are
separate receipts. Product code and test inputs for the preceding COMMAND and
WOW runtime cohorts did not change; their passing receipts remain applicable,
not new runtime runs. The containing commit delivers P24. S3 inline producers,
S4 publication and S5 end-to-end interpreter-free acceptance remain open.
