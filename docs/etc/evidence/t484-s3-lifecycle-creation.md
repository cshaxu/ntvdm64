# T484 S3 P126 creation and reclaim producers

B0195 adds standard-class, class-procedure, native-creation and window-reclaim
to the shared CMD/native lifecycle graph. All use actual CCPU40 page storage;
reclaim alone replaces the existing free leaf with its bounded failure fixture.
Class-procedure retains the original typed-caller ABI. Runtime assertions are
unchanged. No production, mirror or original guest source changes.

Initial PID22540 failed compilation because the class-procedure fixture lacked
the explicit WW layout header. Adding its current owner header resolves this;
no test expectation or production behavior was weakened. PID18976 passes all
four, cached full host build and11 raw/22 published image identity checks.
Publication was verified unchanged, not repeated.

Retrospective review removes nine unreachable old producer branches (31 lines
before whitespace cleanup). Argument validation proves these three migrated
cases cannot select the old fault variants. Native-creation's CreationUnwind
producer remains necessary and is not deleted. Final PID20528 repeats all four
after branch deletion, verifies cache hash/mtime and x64 rejection2. Subsequent
whitespace-only cleanup does not change the verified instructions.

Twenty-two earlier native fixture binaries remain raw hash-identical; their
final graph dry run reports no work. Standard/class binding, creation nesting,
rejection/veto, terminal cleanup and deferred reclaim/retry assertions pass.
These are bounded actual-CCPU/native contracts, not real guest acceptance.
Existing compiler/import warnings remain visible.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-creation/result.json)
records the failed attempt, successful runs and cache/publication identities.
Raw evidence is in build/t484-s3-lifecycle-creation. B0195 remains partial:
27/30 normal cases native; menu-resource, page-domain, dialog-lifecycle and
fault variants remain. File dispositions remain225/355 complete,130 open.
Other migrated tools still require retrospective review; S3/S4/S5 are not closed.
