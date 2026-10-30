# Real Win16 CBT hook with graph-owner release

The existing prebuilt `CBTPROB.EXE` was run in an isolated stage from the
graph-owner release using `tests/observation/verify-win16-prebuilt-cbt.ps1`.
The prebuilt executable and its independent source match the previously
recorded hashes. The fresh stage's ten input hashes match the published
candidate manifest; resealing changes its package identity and image hashes.
No guest was modified or rebuilt.

The guest's four checkpoints show zero errors: real current-task CBT hook
registration, window-class queries, nested creation, veto of one creation,
successful later creation, destruction, unhook and callback thunk release.
The launcher exited zero. The raw binary report, decoded records, observer,
stage manifest and input hashes are retained here. This is a real guest hook
interaction, not a forced cross-thread retirement race; the independent
controlled hook-overlap fixture supplies that narrower scheduling witness.
Neither establishes the cause of the old intermittent 1400.
