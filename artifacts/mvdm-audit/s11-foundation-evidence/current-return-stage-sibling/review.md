# Current-source shared-worker return-stage diagnostic

The [failure-only publication-stage diagnostic](../current-return-stage-sol/review.md)
was run in an isolated package with Winmine and the prebuilt CDLPROBE guest.
The test uses the existing diagnostic runner with a 30-second sibling wait and
60-second observer bound; it neither rebuilds nor changes the guest. The
prebuilt guest identity is in `sibling-inputs.json`; actual runtime image
hashes, including the resealed diagnostic WOW32, are in `inputs.json`.

The sibling completed 16 explicit dialog closes, left the final dialog to task
retirement and exited; its final checkpoint records 17 initializations, zero
guest errors and 16 closes. The same worker kept Winmine alive, and flag,
reset, open and close actions passed. See `sibling-result.txt`, `probe.txt`
and `sibling-report.json`. No publication-stage failure or 1400 was recorded
in the diagnostic trace under `build/t473-s11/s11-current-return-stage-sibling/`.

This is one timed shared-worker witness with diagnostic instrumentation, not a
forced interleaving or proof that the earlier SOL failure is fixed. It is not
an acceptance test of every lifetime path or a performance benchmark. No
installed product or guest image was modified.
