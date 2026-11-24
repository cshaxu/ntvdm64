# T484 S3 P131 lifecycle overlap producers

The last three B0195 producer modes now use the native CMD graph:
TerminalSchedule, CallbackGraphOverlap and HookGraphOverlap. They exercise
window retirement serialization, callback graph ownership and the real hook
path with forced interleavings. These remain valuable standalone native/CCPU
contracts, not Full gates or real guest acceptance.

The native targets reuse production compiler profiles and cumulative provider
archives. Exact-count observation insertion fails configuration if its source
anchor changes; no production hooks or source changes were introduced. Remove
the old manual compiler/environment/Ninja-parser/archive builder as one unit,
and remove its obsolete compiler/build-root parameters and caller arguments.
All runtime assertions remain intact. Normal entries and failure modes now
share the same native producer, with no interpreted build fallback.

PID24036 passed terminal and callback modes but the hook fixture crashed.
The existing diagnostic observer PID11968 identified a null read at fixture
RVA4264a, within WU32StdHookProc (RVA425d0). The standalone fixture bypasses
DLL attach and had omitted its now-required hook provider binding. Bind the
same four original functions as the production DLL entry before invoking the
hook. Do not add a product workaround or fake provider.

PID29748 passes all three modes, cached full host build, and unchanged11 raw
host/22 published-copy hashes. This verifies existing publication; no images
were republished. The final parameter-cleanup rerun is recorded in the receipt.
Raw runs, failed attempt and diagnostic evidence: build/t484-s3-lifecycle-overlap.
No mirror or original guest modifications. Existing compiler/link warnings
remain visible. S3 remains open; S4/S5 are not complete.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-overlap/result.json).
