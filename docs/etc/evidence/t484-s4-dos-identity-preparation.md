# T484 S4 P43 DOS identity preparation

The DOS identity test now uses the existing CMD worker build and system32
staging owner, with explicit baseline and console observer inputs. Each
case stages a fresh diagnostic package and preserves its image manifest
and link map. The CMD guest preparation uses released PIF.EXE, the existing
authored witness build, and the shared guest-ini owner. The INI operation
matches exact driver filenames within the selected section and preserves
unrelated bytes and configuration.

The diagnostic worker writes to the log path assigned for this invocation.
Runtime snapshots are scoped to the current physical package. The runner
retains the runtime test order and copies the evidence before cleanup. The
standalone analyzer supports a run label and leaves the original report
untouched when a label is supplied.

PID21068 exercised DOSONLY=1 and 0 against owned O:/tmp/i311 and i310
installations. Both collected the A/B start and end markers and the host
observer records. Both later timed out waiting for Windows retirement,
matching the previous known limit of this investigation. Both owned guest
copies were cleaned. PID12608 ran the standalone analysis for label p43
with exit zero. It confirmed the existing same-PSP observer mismatch in
both modes and incorrect Run16 waiter completion in DOSONLY=0. Its
findings-p43.json resides under build/side-dos-identity. This is diagnosis
of lifecycle bookkeeping, with no product runtime change or shutdown pass.

PID15264 completed the cached full build, exact filename and section
removal tests for guest-ini, and 22 host hashes across the two published
locations. The existing release artifacts remain valid for this test-only
change. Detailed receipts are under build/t484-s4-dos-identity-preparation.

The preparation caller is migrated. Census: 354/355 disposed, 1 pending,
plus 4 overlapping supplemental chains. The final regression gate
remains pending.
