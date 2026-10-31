# V86guard release: separate WOW worker retirement

The existing `verify-wow-idle-workers.ps1` ran against the V86guard package
using two already-built guest witnesses with explicitly checked SHA-256
identities. Both startup-only launchers returned while their independent
WOW workers remained alive. Worker 30532 exited successfully; worker 13208
then continued to dispatch its own idle callback and subsequently exited
successfully. The observer's real process handles, task IDs and guest records
are retained in `native.txt`, `identities.json` and `observer.txt`; input and
trace hashes are retained separately. The ten package images were matched to
the V86guard published manifest in the companion export recheck.

This is a real-worker normal-exit witness, not a forced concurrent failure,
same-worker task-retirement proof, full guest lifecycle test, or attribution
of the earlier intermittent SOL error. The native observer was compiled for
the test; neither guest witness nor installed guest files were rebuilt or
modified.
