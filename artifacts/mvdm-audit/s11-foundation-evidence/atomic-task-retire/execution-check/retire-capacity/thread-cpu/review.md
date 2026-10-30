# SOL dialog thread CPU observation

The existing Options probe now records CPU time for the thread owning the
game window using `GetThreadTimes`, alongside the worker process CPU time and
wall-clock timings. The observation is read-only and does not change runtime
code. The coherent integrated package is identified by `manifest.json`; the
probe source and executable hashes are recorded in `hashes.txt`.

All eight open, close and parent-recovery rounds passed. Opening took
1.328–2.484 seconds and closing 1.531–2.344 seconds. The owning thread used
1.235–1.969 seconds of CPU during opening and 1.562–1.906 seconds during
closing. Its measured CPU time closely tracks the worker's total CPU time in
these samples, subject to timer granularity and separate sampling boundaries.
This supports investigating work on that thread, rather than attributing the
observed delay solely to another worker thread. It does not identify a hot
function, exclude waits or scheduling delays, explain the earlier 7.641-second
outlier, or establish a regression or fix. Full product acceptance remains open.
