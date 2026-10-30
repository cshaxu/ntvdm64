# Single-scope graph-transfer candidate: withdrawn

An isolated attempt changed the project-owned borrowed-window graph so the
pin-transfer search is skipped when `current_scope` is already the outermost
scope. Each successfully visited node first retains its exact sampled pin;
without nesting, the intended transfer is from that scope to itself. The
nested path was unchanged. This was an experiment, not a mirror or guest
change. It did not alter publication or omit graph traversal.

The current-source graph-snapshot and native-nested selected-object fixtures
both passed, including their normal, retirement and callback markers. The
separately sealed ten-image SOL candidate completed the first three Options
open/close/recovery rounds, but round four closed the dialog without a
responsive parent (`parent_error=1460`, worker still running at the probe's
check). Raw probe, observer, candidate manifest, fixture output and build
logs are retained here. This non-pass does not establish whether the
candidate caused it: earlier baseline runs have also shown intermittent
recovery failures. No paired comparison established a time improvement.

The candidate edit was therefore removed from runtime source and WOW32 was
rebuilt from the restored source; the candidate package was not published.
The test cleanup subsequently reported an access-denied removal of its owned
`O:\tmp\nc9e\system32\ntvdm.exe`; a later process-path inventory found no
live process from that stage. The stage remains partially present and is not
silently treated as cleaned: the standard cleanup checks the complete staged
binary manifest and will not remove an already partial stage without further
identity/ownership review. No full acceptance is claimed.

## Failure-site recheck

The fourth-round game thread's sampled EIP was candidate WOW32 RVA `681b3`.
Disassembly of the hash-identified candidate DLL shows this is immediately
after an indirect call through IAT RVA `8f450`; its import table identifies
that entry as `RaiseFailFastException`. The function builds a noncontinuable
exception with code `c0000420`, matching the implementation of
`wow_user_native_call_failed`. The restored build's map names that function
and its corresponding disassembly has the same body at a different RVA.
Candidate disassembly, import entry and binary hashes are retained here.
This is stronger evidence than an ordinary slow-window timeout: the game
thread had reached an explicit fail-fast call when sampled. It does not
establish that termination completed, which failure stage/error was supplied,
whether the candidate caused it, or whether it matches any earlier non-pass.

The probe printed `STILL_ACTIVE` and could not obtain a responsive parent;
that state does not rule out an in-progress fatal exception. Its failure-only
stack walker reports code bytes for frame 1, not for the sampled EIP; frame 1
lands in padding and cannot be trusted as a caller. The run used `-NoTrace`,
so the stage and error logged by the existing opt-in `retirement_trace` were
not recorded. The current-build map cannot resolve the candidate's caller
by identical RVA: several nearby candidate and restored bytes differ, as
shown in `pc-byte-comparison.json`. Do not attribute the fatal condition to
the single-scope change from this evidence alone.

For a future independently reproduced failure, the shared failure snapshot
now records the thread's integer registers as well as EIP, ESP and EBP. The
candidate fail-fast disassembly moves its error argument into ESI, but the
register must only be interpreted against a matching candidate image and
actual instruction position; this change cannot recover the missing value
from the past run. A standalone x86 owned-child check compiled with `/W4 /WX`,
recorded the registers, resumed the child and observed normal exit. Output and
input hashes are in `register-probe-*`. This is a diagnostic change only.
