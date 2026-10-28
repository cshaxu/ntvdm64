# Superseded untracked S10 scratch evidence

These files were generated during this S10 audit, were never tracked, and had no references from tracked `artifacts/` or `docs/` at the time of review. They were removed individually after review; no source, guest, build tree, or tracked evidence was deleted.

* `cross-369/ccpu-prod-debug-runtime-recheck.md`: superseded by the tracked `ccpu-debug-pig-provider-review.md`, which records the selected shim as partial with the same limits on folded symbols and absent runtime proof.
* `cross-369/remaining-guest-dedup-live-{observer.py,observations.json}` and `shard3-all-excluded-census{.py,.json,-review.md}`: frozen all-file/guest census outside the current host C/H per-file scope, superseded by the tracked host C/H inventory and exclusion reviews. They did not establish functionality.
* `nine-way/part-8/existing-classifications-uncovered-r1{.csv,-baseline.json}`, `sample101-300-baseline-observer.py`, and `uncovered-sample-observer.py`: unreferenced historic samples/one-off observers. The r1 sample hash was checked against its baseline, but the original exclusion inputs were not frozen and the current shard inputs have changed, so exact replay is not established. The separately referenced COMMAND-diff correction has already been retained.

This disposition does not resolve any remaining host C/H classification or functional test. The authoritative current inventory and recovery worklist remain unchanged by scratch cleanup.
