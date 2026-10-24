# M0 T451 S3 — mutation, destruction and copied-state audit

## Scope and status

This is the source-first working audit for S3. It records the actual owner
chains for the finite R05, R09 and R10 rows from the S1 ledger before any new
mechanism is considered. It is not a closure record and does not claim that
the current environment-limited fixture attempt is a pass.

The audit found no missing second mutation cache, no local synthetic window
registry, and no guest change to add. The selected production paths already
form one copied-view transaction owned by the page domain and window-word
binding. Any S3 correction must therefore repair a demonstrated break in
those paths, rather than create another refresh or retirement engine.

## R05 — host-originated scalar and geometry updates

| Required fact | Current producer and consumer chain | Boundary conclusion |
| --- | --- | --- |
| A public native USER mutation is visible after it returns to original WOW code. | `wow_user_task_lifecycle.c::WOW_NATIVE_CALL` routes selected public operations through `native_call_end`; `publish_guest_return` calls `publish_call_windows`, `wow_user_window_refresh_payload`, `wow_user_borrow_client_graph`, and menu refresh after the native call completes. | The host is authoritative for geometry/style; the copied WND view updates only after a successful public sample. There is no independent dirty bit or guessed final `WINDOWPOS`. |
| A callback that directly reads original WND fields sees the pre-dispatch state. | `wow_user_registration_bridge.c` calls `wow_user_window_refresh_message`; `wow_user_object_bindings.c` derives only typed roots from original thunk inputs. Original `msutil.c::SendSizeMessage` reads `hwnd->rcClient`. | `WM_WINDOWPOSCHANGING`, `WM_WINDOWPOSCHANGED` and `WM_NCCALCSIZE` refresh before the original callback; ordinary post-call publication samples final host state. |
| Recovered internal control mutation does not bypass the sole view owner. | `wow_edit_call_bindings.c::wow_edit_alter_window_style` invokes the native service and immediately calls `wow_window_client_refresh`; that routine writes the same retained client WND backing. | This is a direct native operation with an explicit same-owner refresh, not a parallel control state cache. |

The existing `graph-snapshot` fixture asserts this group with
`WOW_GRAPH_NATIVE_MUTATION_CCPU` (child ID, reparenting and deferred geometry)
and `WOW_GRAPH_MUTATION_GROUP_CCPU` (class, scroll/frame/iconic/popup and
invalid inputs). Those assertions are the S3 executable target; a successful
compile alone is not evidence.

## R09 — destruction, rollback and low-word reuse

| Required fact | Current producer and withdrawal chain | Boundary conclusion |
| --- | --- | --- |
| Actual host destruction withdraws the exact enrolled generation. | `wow_window_terminal_observer` retains the binding before downstream hooks, accepts only actual `WM_NCDESTROY`, rechecks the property after reentry, then calls `wow_window_words_terminal`. | A synthetic `WM_NCDESTROY` is only a message. A downstream destroy/reuse cannot make the retained old binding clean a successor HWND generation. |
| Original control/dialog cleanup remains the behavior owner. | `wow_window_words_terminal` marks the original `WFDESTROYED` state, invokes selected original control/dialog finalizers, then retires copied identity afterwards. | The adapter does not replace original control cleanup or invent a terminal-message policy. |
| A retired low word cannot affect a replacement window. | `wow_user_window_retire` requires `TYPE_WINDOW`, full `wUniq`, and exact `phead`, asks the page domain to retire that backing, then clears the table row. | A stale generation rejects rather than retrying against a reused low word. Nested terminal cleanup observes `enrolled`/`terminal_pending` and releases each owner only once. |

The existing `graph-snapshot`, `window-reclaim`, and `native-creation` rows
cover native slot reuse, retained generation isolation, deferred reclamation,
creation veto/unwind and terminal table emptiness. Their required markers are
`WOW_NATIVE_SLOT_REUSE_CCPU`, `WOW_MANAGED_SLOT_REUSE_CCPU`,
`WOW_DEFERRED_RECLAIM_CCPU`, and `WOW_NATIVE_CREATION_BINDING`.

### Host terminal-observation boundary

The selected original `createw.c::xxxDestroyWindow` marks its private WND with
`WFDESTROYED`, then `xxxFreeWindow` destroys children and sends
`WM_NCDESTROY`.  The original kernel therefore knows whether that notification
belongs to its actual free path.  Modern USER exposes neither that WND flag nor
an equivalent per-window destruction transaction.

The current adapter's `WH_CALLWNDPROCRET` observer can preserve the original
control/dialog message ordering, but cannot itself distinguish an application
`SendMessage(WM_NCDESTROY)` from a native destroy path.  The existing public
`EVENT_OBJECT_DESTROY` research fixture was built and exercised on 2026-10-23:
for a root window it arrives before the `WM_NCDESTROY` body while `IsWindow`
is still true; it does not arrive for a hand-sent `WM_NCDESTROY`; and a parent
destroy did not produce a child event before that child's terminal body.
Thus it is useful corroboration, not a complete replacement for the original
kernel transaction.  The run ended
`DESTROY_EVENT_RESEARCH failures=0 semantic_equivalence=unproven`.

This is a real adaptation boundary, not evidence that a small public-API swap
can restore exact NT4 private semantics.  No production change is selected
until the S3 acceptance matrix can exercise the actual adapter chain on a
private desktop and establish whether the reachable WOW lifecycle exposes a
harmful discrepancy.  A future semantic change must retain original control
finalization while separately proving exact identity withdrawal for root and
child windows; it may not treat the incomplete WinEvent observation as a
universal substitute.

## R10 — copied task and invocation-local fixed fields

| Required fact | Current authority and timing | Boundary conclusion |
| --- | --- | --- |
| Task version, compatibility flags and 16-bit identity are established before original direct consumers. | Original `queue.c::xxxInitTask` writes `pti->dwExpWinVer`, `pti->pClientInfo->dwExpWinVer`, compatibility fields and task flags. `wow_user_task_lifecycle_init` establishes the carrier then invokes that original body before publishing the first guest view. | `wow_task_client_info` is an explicit project carrier, not a modern TEB/CLIENTINFO overlay; original `xxxInitTask` remains the behavior producer. |
| Switching task execution updates only the selected guest projection. | `client_context` in `wow_user_task_lifecycle.c` calls the page-domain expected-version/context exchange as part of the original scheduling owner. | An inactive callback projection is not an authority and does not own a guest borrow. |
| Original control/default-window routines receive only their required values. | `wow_edit_client_info` reads the bound original task carrier; `wow_default_capture_context` samples only the specific shell-message, border, colour and display inputs consumed by its original routine. | No modern private `SERVERINFO` or `CLIENTINFO` pointer is interpreted or cached as an NT4 structure. |

The S3 evidence group must retain `WOW_CLIENT_CONTEXT_CCPU` and the
version-bearing native-creation assertions, in addition to the S2
initial-publication proof. These prove timing and swap/retirement behavior;
they do not claim a wholesale NT4 server shared-memory model.

## Current execution observation

The low-integrity command token first made two independent current x86
provider/worker invocations of the tracked
`verify-wow-native-slot-group.ps1` group incomplete. Eleven rows passed:
`native-dispatch-gate`, `window-procedure`,
`class-client`, `class-query`, `class-lifetime`, `task-lifecycle`,
`native-send`, `native-nested`, `native-foreign`, `native-indirect`, and
`init-rollback`. The task row emitted
`WOW_USER_TASK_LIFECYCLE errors=0 native_direct=5`; all eleven retained their
required runner markers and exited zero.

The eight non-passing rows were `window-borrow`, `class-procedure`,
`native-creation`, `standard-class`, `process-retirement`, `dialog-lifecycle`,
`creation-unwind`, and `graph-negative`. Their common prerequisite is the
fixture private desktop: the direct rows reported
`WOW_PAGE_DOMAIN_DESKTOP_UNAVAILABLE error=5`; process-retirement's first
assertion was its `CreateDesktopW` call; and the dialog runner consequently
observed its fixture's unavailable exit instead of the normal context marker.
This is an unavailable test-environment result, not a passing or failing
product assertion; it leaves the R05/R09 CCPU acceptance open.

The historical r13 evidence used this same checked-in runner and this same
fixture-owned `CreateDesktopW(..., GENERIC_ALL, ...)` path; it was not a
different test or a permissive fallback. On 2026-10-23 the runner was rerun
from a desktop-capable, medium-integrity token. A one-shot uniquely named
private-desktop probe first created and closed successfully, isolating the
previous `ERROR_ACCESS_DENIED` to the low-integrity command executor rather
than to the source, interactive session, or fixture.

The fresh, source/artifact-hashed group at
`build/t451-s3/desktop-current/matrix/summary.json` completed all 19 cases
with `failed: 0`. In addition to the eleven rows above, the previously blocked
`window-borrow`, `class-procedure`, `native-creation`, `standard-class`,
`process-retirement`, `dialog-lifecycle`, `creation-unwind`, and
`graph-negative` rows all exited zero. This is actual private-desktop
execution of the normal, negative, reuse, destruction, rollback and lifecycle
fixtures, not a skipped prerequisite. No production source, guest image, or
release asset changed for this evidence recovery.

## Incidental Run16 cold-start correction

While preserving the S3 source audit, a release-blocking native CUI regression
was traced to `run16-exe/main.c::connect_broker`.  A missing NTSRV endpoint on
this host first returned `ERROR_ACCESS_DENIED` (5), not only the conventional
`RPC_S_SERVER_UNAVAILABLE`.  The existing launcher consequently returned
before its already-approved five-second NTSRV bootstrap grace was entered.

The correction changes no RPC protocol, task request or worker lifecycle.  It
treats exactly those two pre-submission startup results as eligible for the
single existing NTSRV creation, retains the created process handle only to
observe an early service exit, and reuses the same five-second deadline and
local RPC retry.  A later non-startup error still returns directly; a
duplicate-endpoint child exit preserves the original observed RPC status.

`build/t451-s3/coldstart-candidate-cold-result.json` records a genuine hidden
Console cold start of the staged package:
`run16.exe cmd.exe /d /c ver` completed with exit zero.  The staged candidate,
`assets/release`, and `O:\winnt\system32` were then compared for all ten host
images; every SHA-256 agreed.  The installed bit-identical command completed
with exit zero in `installed-run16-console-result.json`.  Guest media and
configuration were neither modified nor rebuilt. This evidence delivers the
Run16 correction. The later desktop-capable 19-row matrix closes the prior
private-desktop R05/R09 acceptance prerequisite; the separately documented
public-host terminal-observation boundary remains a design conclusion, not a
test-environment failure.

