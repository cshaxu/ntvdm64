# Baseline SOL native-return failure

The retained pre-single-scope ten-image package was run with existing WOW
window and callback traces in a test-owned stage. Three SOL Options
open/close/recovery rounds passed. During the fourth open the worker exited
with `c0000420`. The last window trace records task thread 31320 at
`TaskRetirement stage=publish-native-return status=1400`; the same thread
had just created the dialog and its children. Guest callback trace records
the dialog's `WM_WINDOWPOSCHANGING` callback returning immediately before
the failure. Exact package manifest, output, callback traces, input hashes
and observer report are retained.

This establishes that the withdrawn single-scope optimization is not
necessary to reproduce a native-return fail-fast with invalid-window status.
The withdrawn candidate's earlier snapshot also showed a fail-fast call but
did not record its stage or error, so exact equivalence between the failures
is not proved. Existing `publish_guest_return` short-circuits window
publication, borrowed-client graph publication and menu refresh; the single
failure marker does not identify which suboperation returned 1400, the
offending HWND or a race. Callback ordering is evidence of proximity, not a
root-cause proof. Trace instrumentation and the isolated stage can alter
timing. This is a product non-pass, not Full acceptance.

## Distinct 1400 paths and existing controls

`publish_call_windows` passes the native call's returned payload, returned
window and target to the graph refresh. An operand observed live before the
call can be dead on return; the current graph treats a dead optional payload
differently from a dead mandatory graph root. The real-CCPU graph fixture's
`WOW_RETURNED_WINDOWS_CCPU` control checks dead and mixed payloads, while
`WOW_GRAPH_SNAPSHOT_CCPU` explicitly rejects a dead required root and checks
withdrawal and a retained reader. These establish the controlled behavior,
not which identity the failed SOL call supplied.

The independent menu-refresh path iterates enrolled local windows and fails
if `GetWindowInfo` fails. `WOW_MENU_DEAD_ENROLLMENT_CCPU` deliberately leaves
a destroyed window in a synthetic table and observes 1400. It establishes
that menu refresh can independently produce the same error, not that real
SOL left a stale enrollment. The native-nested fixture also covers a window
destroyed during a native send and a dead target, but its selected-object
test is not a real SOL failure trace. These controls do not authorize
ignoring either failure or inferring that one is the real cause. The next
useful witness must identify the failing substage and exact window generation
in an actual failure, including whether that identity is a required root,
optional returned operand or enrolled menu owner.

## Error provenance limit in the retirement sweep

The mandatory graph refresh first calls `wow_window_words_collect_retired`.
That collector can continue scanning after a terminal operation returns
FALSE and calls `wow_window_words_release` before returning its aggregate
FALSE. It does not explicitly save the first failure's last-error across
release or later iterations; release can invoke class/guest cleanup and heap
operations. Therefore the final status is not, from source inspection alone,
guaranteed to be the first terminal operation's error. Other failure routes,
such as a failed required-root capture or `GetWindowInfo` on an enrolled menu
owner, can independently set invalid-window status. This observation does
not establish that the retained SOL failure entered the retirement sweep, nor
that release actually overwrote its error in that run. It is a bounded error
provenance question; preserve the existing failure and ownership semantics
until a controlled multi-entry terminal-failure witness establishes what is
returned and which cleanup operations run.

The real-CCPU deferred-terminal fixture now explicitly seeds a different
last-error before a single pending-retirement sweep while the guest page
domain is unavailable. It passed and observed `ERROR_INVALID_STATE`, then
reentered the page domain and completed retirement and storage reclamation.
The selected-object output, fixture exit and source/binary hashes are in
`retirement-error-one/`. This checks one failure with its existing retained
caller reference; it does not exercise a second pending entry or prove that
cleanup cannot overwrite the first error in another lifetime configuration.

The two-entry controlled fixture now exercises distinct terminal failures in
both scan orders. Before ADAPTER-WOW-108 it returned the later error (5023
or 1168); after the change it returns the first (1168 or 5023). It restores
the deliberately corrupted identities, releases retained references, and
completes reclamation. The full selected-object `graph-snapshot` fixture also
passed after rebuilding WOW32, including the two-entry case, graph mutation,
handle reuse, nested callbacks and terminal-failure controls. Its output is
in `build/t473-s11/retirement-error-final-graph-elevated`; before/after
outputs are retained under `retirement-error-two/`. This synthetic failure
proves error preservation, not the cause of the intermittent SOL 1400.
