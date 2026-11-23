# T484 S3 P62 SOL and WRITE probe producers

B0068-B0070 now delegate their SOL gameplay, SOL options and WRITE lifecycle
witness compilation to tools/build-fixtures.cmd. Three targets in the existing
native fixture graph retain their original sources, /O2 /MT /W4 /WX profile
and distinct USER/GDI/DWM or PSAPI/DBGHELP link inputs. A shared runtime helper
records the owned producer PID, command, timeout and exit, then copies the
selected executable into the caller's evidence directory. CompilerDriver is
retained as an unused optional compatibility parameter; it cannot select an
alternate compiler implementation.

All guest/runtime assertions and cleanup remain unchanged. No product or
original guest source changes. The pending entries still include physical
runtime/package staging; local producer migration is not whole-chain closure.

Verification and reference-command comparisons are recorded under
artifacts/build-migration/t484/s3-apps, with raw commands/logs in
build/t484-s3-apps. The comparison distinguishes byte identity from PE
timestamp-only differences. Argument/architecture rejection checks and native
cache verification are not guest gameplay or WRITE runtime acceptance.
Final delivery requires the cached host build, unchanged dual-package hashes,
documentation gate and commit/push receipts.

All three actual helper builds and x64 rejection controls passed. The original
CL commands produced equal-size images, each differing from its native-graph
counterpart in only two PE timestamp bytes. No other byte difference exists.
No-argument controls returned 2 for both SOL probes and 87 for WRITE. The
cached full host build passed; these results do not claim a new guest run.

Final verification passed: native no-op builds preserve hashes and timestamps;
all eleven raw host inputs and twenty-two published hashes are unchanged.
Both publication locations therefore already hold the exact current product;
no recopy or fresh guest runtime pass is claimed. Documentation governance and
runner parsing pass. Fixed inventory:134 completed dispositions,221 remaining;
53 pending producer-chain migrations and168 awaiting disposition reconciliation.
