# Published separate WOW worker retirement witness

The existing `tests/observation/verify-wow-idle-workers.ps1` was run against
`build/t473-s11/trace-scalar-package/package`, with the existing immutable
prebuilt IDLPROBE and IDLPEER witnesses. Its result was PASS. All ten image
hashes in the retained package manifest matched `assets/release` at the time
of the run. The guest witness and diagnostic trace hashes are retained in
`evidence-hashes.csv`; no guest was modified or rebuilt.

The observer saw two separate worker processes. Both startup-only launchers
returned while their workers remained alive. Worker 6392 exited with code zero;
the probe then observed idle activity in worker 35956, which also subsequently
exited with code zero. See `native.txt`, `identities.json` and `result.txt`.
The test explicitly checks process-handle exits and idle events after the first
exit, rather than relying on a shared-worker task marker.

This is a bounded normal-path witness. It does not directly instrument the
`WK32ExitKernel` entry, prove all process-exit cleanup or resolve the historic
intermittent SOL return error. The source-level distinction between process
termination and explicit WOW32 unload remains in the adjacent unload review.
