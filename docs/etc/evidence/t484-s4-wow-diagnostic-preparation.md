# T484 S4 WOW diagnostic preparation

## Scope and implementation

The queued WOW performance investigation still consumes callback, graph, page
and observer diagnostics. They are focused diagnostics, not every-Full gates.
Their CMD/native producers remain; mandatory runtime preparation moves to one
prepare-wow-diagnostic.cmd owner. It reads immutable assets/winnt.zip using
native CMake archive extraction, validates the release manifest and every
image hash, and matches worker/WOW raw inputs before copying their MAPs.

Each baseline or diagnostic gets a fresh build-owned directory. Diagnostic
selection is restricted to ntvdm.exe or WOW32.DLL plus its adjacent MAP. Only
the staged diagnostic copy receives the selected release package identity;
raw builds, release images and original guest files remain unchanged. A failed
preparation retains its incomplete directory without a success marker. Existing
output is rejected. The authored CMD template uses LF, generated CALL-label
script CRLF, matching the existing native staging owners.

The four callers retain their source/object substitution, timing, API-error,
normal-exit and immutable-input assertions. Runtime code comes from the CMD
prepared package; retained source/object/image snapshots are evidence only.
Remove redundant sealed-image and unit/native executable copies. Observer's
native reference uses the same canonical mask2 latency probe as its guest
measurement, replacing its obsolete temporary latency.exe reference.

## Retained failures

Initial PID20996 completes cached full build, graph native unit and all three
real baseline cases. Diagnostic preparation then fails because its mixed-slash
input was passed to COPY without canonicalization. Canonicalize the optional
image argument before COPY; retain the original failure in build evidence.

PID32752 passes preparation contracts and baseline but its diagnostic launcher
returns0x51a (ERROR_REVISION_MISMATCH), before the worker exists. The raw
instrumented image had not been bound to the published package identity.
Stamp only the owned copy through the existing native package tool. Do not
weaken the launcher identity checks or publish the diagnostic.

## Verification and delivery state

build/t484-s4-wow-diagnostics contains the owned commands, process identities,
stdout/stderr, stage contracts and final per-case records. The final serial
run is PID22420, started2026-11-24T17:24:46Z. It finishes17:33:26Z with exit0. All four real diagnostic chains pass;
graph and page include baseline and instrumented ordinary/sampled/private
runs, callback records45 pairs, observer records60 operations including both
native reference desktops. Interpretation of measured performance remains
outside this migration. Final11 raw host inputs and22 dual-published hashes
are unchanged. Cached full build passed in the initial run; subsequent changes
only repair test preparation and were exercised by the final configured CMD
tool. No new production image or Full regression is claimed.

P13 closes four frozen entries:341/355 disposed,14 pending. The24 overlapping
supplemental chains remain pending. S4 and S5 are not closed.

The stage contract checks exact released image/MAP copies, refusal to reuse
an output directory, unsupported diagnostic rejection, malformed manifest
rejection and hash mismatch before guest extraction. Page additionally checks
the original EMS guest. Real latency runs distinguish ordinary, sampled and
private desktops and do not claim physical mouse latency.
