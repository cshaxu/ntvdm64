# Real CBT hook and idle task in one WOW worker

The reusable `verify-wow-idle-cbt-shared.ps1` test ran the three existing,
hash-checked host and guest witnesses without rebuilding any guest. The stage's
ten input hashes match the published graph-owner package. It observed two
distinct task retirements in the same WOW worker: the CBT guest completed its
four phase records with nested button creation, veto, recovery, unhook and
thunk release; the peer remained running and received an idle dispatch after
the CBT task retired. Both `run16 --wait` requests completed successfully.
The manifest, native output, guest records and hashes are retained here.

This exercises a real registered guest hook in a shared worker, complementing
the selected-object forced-overlap test. It does not force the hook's graph
capture to coincide with another task's window retirement, and does not prove
the old intermittent SOL/Winmine 1400 was caused by the repaired lock gap.
