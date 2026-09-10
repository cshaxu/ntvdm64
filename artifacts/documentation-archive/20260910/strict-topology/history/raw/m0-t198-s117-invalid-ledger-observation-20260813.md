# M0 T198 S117 Record: Ledger-Equipped Run Without Ledger Export

S117 rebuilt, linked and ran the declared harness once. The ledger object was
present, but the fixture omitted its output loop, so `run.log` contains only
the legacy first accepted/fallback summaries. The run is valid for build and
path continuity, but invalid for the task's per-attempt attribution purpose.
It is retained as evidence and is not retried under S117.

S118 owns the test-only export correction and non-guest verification.
