# M0 T460 S3 — Clipboard deferred-lifetime progress record

## Question and authority

S3 must prove the selected original delayed-render, owner-replacement and
dead-task Clipboard paths with immutable Win16/native peers.  It may repair
only a source-proven existing host boundary; it may not add a Clipboard cache,
manager, listener, DDE endpoint, guest change or guest rebuild.

This is an **active progress record, not S3 closure or a release claim**.

## Source result

The selected original route is intact:

- `src/mvdm/wow32/wuclip.c` retains the original `WU32OpenClipboard`,
  `WU32SetClipboardData`, `WU32SetClipboardViewer` and viewer-chain owners.
  A null `HMEM16` remains the original delayed-render request.
- `src/mvdm/wow32/wmtbl32.c` selects `WM32NoThunking` for
  `WM_RENDERFORMAT` (0x305), `WM_RENDERALLFORMATS` (0x306),
  `WM_DESTROYCLIPBOARD` (0x307), and `WM_DRAWCLIPBOARD` (0x308).
  `W32Win16WndProcEx` still enters the original callback route.
- `src/mvdm/wow32/wkman.c::W32DestroyTask` calls original
  `pfnWOWCleanup` before project-only management retirement;
  `src/mvdm/wow32/wuwind.c` retains the original `DestroyWindow` contract
  that removes Clipboard ownership and viewer-chain membership.

The full-handle audit also covered every current consumer of
`pfnGetFullUserHandle`: DDE conversion, MDI/message conversion, CTLCOLOR,
dialog routing and the selected Clipboard calls.  The original
`GetFullUserHandle` body returns an NT4 USER index/uniqueness token and
explicitly retains that token after a dead DDE peer.  A modern public USER
operation cannot consume that encoding.  The repair therefore keeps the
original all-WOW conversion token intact, and resolves the existing
`windows[index]` live HWND only at the three selected public Clipboard
operands.  No callback, table, DDE rule or retired identity was reimplemented.

The source audit found one necessary modern-host boundary.  Original
`HWND32(h16)` is a signed short carrier and its `FULLHWND32` counterpart is an
NT4 USER index/uniqueness token.  The current host records the actual modern
HWND in the existing per-worker object table at `wow_user_window_publish`;
neither token can reproduce that HWND.  `MVDM-HOST-DIV-454` therefore changes
the three original Clipboard operands that pass a live window to public USER:

1. `WU32OpenClipboard`;
2. `WU32SetClipboardViewer`; and
3. both operands of `WU32ChangeClipboardChain`.

The registered full-handle callback and existing native-HWND helper retain
their original token projection.  The new, narrowly named public-USER helper
alone resolves the existing live table value; the original index/uniqueness
fallback therefore remains intact for message, DDE and dialog conversion.
There is no second window table or Clipboard state.  The runtime registration
trace for the task-owned custom class recorded `CreateWindowEx result=00CF01C0`;
the selected public Clipboard boundary subsequently resolved its short carrier
to a live modern HWND instead of supplying a short carrier or an NT4 token to
modern USER.

The closure-audit caller census confirms the patch changes only the three
Clipboard operands above plus the existing live-native helper.  It adds no
exported API and removes the uncalled `wow_user_window_refresh_return` helper.
All remaining `FULLHWND32` sites are pre-existing message, DDE, alias or dialog
conversion sites and remain outside S3.  `windows[index]` is still written only
by existing publish/retire routines and cleared on retirement; the patch adds
no writer, cache, reverse lookup or lifetime owner.

The dead-owner audit also establishes the required order from production
sources, independently of the still-unavailable public Clipboard run:
`W32DestroyTask` calls original `W32FreeTask`, reports the original `ExitVDM`
edge, invokes its registered original `pfnWOWCleanup` while the task identity
is live, and only then invokes the project management-record retirement.
`registered_cleanup` dispatches that `pfnWOWCleanup` to the existing task
lifecycle owner; it has no Clipboard data or notification policy.  Separately,
the original `wmtbl32` rows route `WM_RENDERFORMAT`, `WM_RENDERALLFORMATS`,
`WM_DESTROYCLIPBOARD`, and `WM_DRAWCLIPBOARD` through `WM32NoThunking` to the
existing Win16 window-procedure callback.  The newly added witness phase 13
therefore exits via the ordinary task path and phase 14 can only be recorded
from that original callback; it does not call a project-owned teardown helper.

The prior S2 `MVDM-HOST-DIV-452` sequence observation was also re-audited for
this S. It is not a replacement Clipboard cache: original `WU32ICBGetHandle`
and `W32EmptyClipboard` remain the sole data/cache and retirement owners.
Original NT4 USER used the registered private `pfnEmptyCB` to notify that
cache when a native writer replaced Clipboard contents; modern public USER has
no equivalent registration ABI. The three-field sequence observation merely
causes that original retirement owner to run before a cached handle is reused.
Removing it would retain stale original guest data after native replacement,
not restore an importable original call path.

## Inputs and executable observations

The only current source-level build evidence is the scoped adapter build at
`build/t460-s3/causal-r1/result-r2.json`, which completed successfully.  It is
not a release candidate: no whole-component rebuild, package publication or
Clipboard acceptance claim has been made from that build.  The current x86
object tree also passes `verify-wow-user-callback-abi.ps1`
(`WOW_USER_INPUT_CALLBACK_ABI_21_OK`).  This confirms the original callback
table's ABI and owner placement only; it does not prove Clipboard delivery.

The older S2 acceptance narrative names `transfer-r42` and `transfer-r43` as
passing evidence.  The retained files at those exact paths instead contain
`failure.txt` and an observer result of `run16` exit `0x0000042b`.  Until their
provenance is reconciled, neither directory is admissible as a successful
historical runner or Clipboard witness.  This S3 record therefore does not use
that assertion as evidence.

The task-owned immutable Clipboard probe now gets through guest startup,
custom Win16 class creation and the first selected public Clipboard call.  A
temporary trace, removed immediately after the run, recorded:

```text
OpenClipboard h16=0D66 hwnd=01170D66 result=0 error=5
```

The same isolated private-desktop case with the retained stock owner has the
same `ERROR_ACCESS_DENIED`.  Thus the short carrier has resolved to a live
published modern HWND, but modern USER refuses Clipboard access from that
isolated desktop.  This is a security/isolation boundary, not a handle mapping
or startup failure and not a successful delayed-render/replacement/task-loss
acceptance result.

The host-object boundary has been independently checked rather than inferred
from that one error: a Clipboard belongs to a **window station**, while a
desktop only contains its windows and display surface.  A fresh window station
would therefore provide the required disposable Clipboard, but the retained
pre-S3 native controls already attempted `CreateWindowStation` with both broad
and reduced `WINSTA_ACCESSCLIPBOARD`/desktop rights and received
`ERROR_ACCESS_DENIED` before binding or creating a desktop.  A fresh desktop
inside the accessible interactive `WinSta0` cannot solve this: it still uses
that station's user Clipboard and its `OpenClipboard` call returns the same
access denial.  The project must not substitute a private data store, sample
the user's Clipboard, or claim that either path proves the selected original
callbacks.  These are host isolation facts, not a reason to add a WOW
Clipboard implementation.

There is also no evidence that merely authorizing a shared-Clipboard rerun
would make this executable context usable.  A prior owner-authorized native
control (`M0 T445 S4`, r256) used an explicit opt-in, hidden self-owned windows
on the current desktop, did not alter window-station ACLs or other windows,
and still received `ERROR_ACCESS_DENIED` from `OpenClipboard`; its native
baseline never reached an EDIT assertion.  That control is not WOW acceptance,
but it rules out describing this S3 boundary as a missing permission prompt or
as a guest/adapter-specific failure.  A real acceptance environment needs a
separate logon/session or an execution identity granted a disposable window
station, rather than a retry against the same denied station.

The retained `causal-r1/transfer-r10-full` run makes that boundary more
specific. Its coherent staged package completed the observer normally
(`exit=0x00000000`) and emitted the guest report, so the WOW worker and probe
had actually started. The seven records end with
`CLP1 phase=1 errors=1 guest=0 native=4096`: all task, USER, procedure,
class and hidden-owner-window checkpoints had succeeded, then the first
original `WU32OpenClipboard` failed. This run predates the later failed
attempts to substitute a complete logon environment; those unproven test-only
environment changes have been removed. It is positive evidence for startup
and the narrow live-HWND boundary, and negative evidence for Clipboard access
on the private Desktop--not a deferred-render acceptance result.

The historical runner reconstruction used the checked-in modern `pwsh.exe`
path directly, rather than launching a detached child from a short-lived outer
process.  Its parent is medium integrity.  The direct runner completed enough
of the probe to preserve the guest report and boundary trace; the earlier
detached-runner result is retained only as a runner-configuration failure.
The general reconstruction requirement is now recorded in
`docs/rules/EXECUTION.md` for every execution action, not only desktop or
Clipboard tests.

A later exact-script attempt (`transfer-r47`) used the same modern host path
and the current coherent package, but its nested private-desktop observer
wrote only `result=not-started` and returned exit 5; it did not create a guest
report.  It is therefore an unequal launch-context result, not a product or
Clipboard verdict, and cannot supersede the retained `transfer-r10-full`
startup/`ERROR_ACCESS_DENIED` observation.  The current probe and native bridge
did assemble/compile before that observer boundary, and the source package's
`WOW32.DLL` hash equals the current x86 Ninja output.

For a bounded control, `transfer-r48` used the exact observer binary retained
by the successful `transfer-r10-full` route (`5BCD3CF2...1C7C1556`) with the
current package-r11 `WOW32.DLL` (`59F7479E...8942B6B`).  The observer completed
normally and the current probe again emitted the seven records through hidden
owner creation, ending at the first `WU32OpenClipboard` failure
(`phase=1`, `errors=1`, `native=4096`).  This proves that r47 is a newer
observer-container incompatibility, not a product-startup regression.  The
old observer is a diagnostic control only, not a substitute for bringing the
current observer back to equivalent private-desktop behavior.

`verify-win16-message-probe.ps1` remains the non-Clipboard control for the
native-to-Win16 callback/lifetime route.  Its prior passing normal case
exercised two immutable Win16 tasks, nested callbacks, native peer directions
and normal original task retirement.  It does not establish that public USER
Clipboard sends any of the selected notifications.

An earlier rerun stopped at the isolated-scope ownership gate because a live
broker belonging to another package still owned the required scope. It neither
terminated that broker nor produced a product verdict. `transfer-r48` was run
after that process was absent and supersedes it as the current startup control.

## Follow-up and closure boundary

S3 remains active. The live retained runner has ruled out a current
worker/completion failure: it completes normally through class and hidden-owner
window creation, then public USER rejects the first Clipboard access on the
private Desktop. Before closure, a real public-Clipboard fixture that preserves
user state must be available under an explicitly approved isolation boundary;
only then may the checked-in immutable witness run through:

1. null-data delayed render into the original owner callback;
2. native owner replacement and its original destroy/viewer notifications;
3. owner task destruction while deferred data exists, with no stale callback
   or duplicate cleanup.

The witness now reserves the final task-loss transition rather than treating a
normal process exit as proof: after delayed render and native replacement it
re-establishes delayed `CF_TEXT`, checkpoints phase 13, and exits without
explicitly destroying its owner or unloading its peer. Its original
`WM_RENDERALLFORMATS` callback renders the data and checkpoints phase 14. The
acceptance parser requires both records only after the ordinary transfer
records; until the public-Clipboard fixture reaches them, this is a test
contract, not runtime evidence.

`transfer-r49` assembled that updated NE probe (`D5CAD3D3...0EE3C65A`) and
compiled its native bridge, then reached the same normal observer exit and
first-`OpenClipboard` boundary as r48. Its seven retained records never reach
the new phase 13/14 assertions, so r49 is build/source evidence for the added
task-loss contract and remains a runtime non-pass.

The current correction is source-proven and build/regression-checked, but it
is not published and does not reduce those three real acceptance obligations.

## 2026-10-25 task-loss execution update

The current coherent staged witness has now passed all thirteen pre-exit guest
checkpoints: its custom owner receives the original `WM_RENDERALLFORMATS`
entry while `W32DestroyTask` is disposing that owner.  This establishes that
the live public-Clipboard route is no longer blocked at the earlier private
desktop access boundary.  It is nevertheless **not an acceptance pass**.

The initial candidate exposed `publish-native-return` with
`ERROR_NOT_FOUND` after the callback had returned.  A deliberately temporary
control that suppressed the ordinary full-graph publication during resource
cleanup removed that fail-fast but instead left the same owner blocked inside
the original `DestroyWindow` / `WM_RENDERALLFORMATS` call.  The control and
its tracing were removed; neither behavior is part of the current candidate.

Consequently the remaining S3 defect is bounded: the adapter must preserve
the original cleanup callback's completion and the ordinary terminal window
retirement without either publishing a dead graph or suppressing a required
cleanup transition.  The witness currently proves callback entry and all
earlier Clipboard operations, but does not yet prove callback return, phase
14, launcher completion, or dead-task cleanup.  No package, deployed image,
or S3 closure claim may use this result.

### 2026-10-25 frozen-input control

The current `win16_clipboard_probe.asm` hash is identical to the retained
`clipboard-transfer-r7/CLIPPRB.EXE` build input.  The transfer runner now
accepts that sealed probe/native-bridge pair only after it verifies both source
hashes and both artifact hashes against its retained `inputs.csv`; this permits
an isolated rerun without rebuilding any Win16 guest test input.

The resulting `clipboard-transfer-r11-frozen-current` run is **not** a
task-loss verdict.  Its native peer received `ERROR_ACCESS_DENIED` from its
first public `OpenClipboard`, and the guest report therefore ended at phase 2
with the expected rejected transfer marker before phases 3--13 or
`WM_RENDERALLFORMATS` were reached.  The observer then timed out with the
worker still live.  This is the same private-desktop public-Clipboard access
boundary already documented above, not evidence that the terminal callback or
the current `DestroyWindow` return path regressed.  The no-guest-rebuild
control is retained for a later equivalent public-Clipboard execution context;
it must not be treated as a pass or used for publication.

The current interactive session is not globally denied Clipboard access: the
repository's host-only `console-startup-observer` access probe opened and
closed it successfully without reading or changing a format
(`clipboard-access-current-desktop.txt`, `open=1`, `close-error=0`).  The
remaining distinction is deliberate test containment.  Running the full
fixture on that shared station would replace real Clipboard contents, while a
private desktop cannot reach the public Clipboard.  No shared-Clipboard
functional run has been performed or implied by the access probe.

The S3 witness runner itself was corrected before it can be used as evidence:
the generated native bridge calls USER32 and therefore must link `user32.lib`;
also, the repository's x86 compiler setup is a batch-file driver and must be
called through `cmd /c call`, rather than directly replacing the PowerShell
runner.  A direct driver control compiles `wow_clipboard_native_bridge.c` with
that exact dependency set and returns zero.  Earlier partial records that stop
after assembler output are runner failures, not Clipboard verdicts.  The last
valid retained guest records provide two distinct negative observations: one
run reaches the selected guest `OpenClipboard` and fails in the native peer's
immediate read, while an earlier run fails at guest `OpenClipboard` itself.
Both occur before delayed render and neither may be misreported as a callback
or task-loss failure.  The next admissible runtime verdict must retain the
bridge trace, which records whether the rejected public call is the guest
owner operation or the native peer operation.

## Current coherent static gate

## 2026-10-25 callback-return source result

The exact `DestroyWindow(owner)` control has now isolated the remaining
non-pass without changing guest code or replacing any Clipboard behavior.  Its
private owner receives `WM_RENDERALLFORMATS`; the guest checkpoint sequence
confirms entry, `OpenClipboard`, owner inspection, delayed `SetClipboardData`
and `CloseClipboard`.  The missing checkpoint is the return of the *outer*
original `DestroyWindow` call after that procedure has completed.

The paired callback trace is decisive about the location of the loss: the
outer `RET_WNDPROC` reaches `host_simulate`, an inner `WM_ACTIVATEAPP` callback
returns normally, and the outer frame never records its post-`host_simulate`
return.  `wow16cal.asm` explicitly permits this result: after a callback it
compares the saved task with `curTDB`, and on a difference enters the original
`SwitchTask` path rather than returning directly to the native frame.

This is not a missing Clipboard callback and cannot safely be repaired by a
window-local TLS flag.  The examined TLS/nested-scope candidate was removed:
it still executed every guest Clipboard operation but left the same original
outer frame suspended.  The retained production route has two project scopes
around the original `W32Win16WndProcEx`/`CallBack16` transition, but its
modern-USER arrival adapter has no original SMS sender, reply identity or
resume ownership for the activation-induced task switch.  It therefore cannot
prove which task must be made runnable, nor may it invent a scheduler event or
force a return from `host_simulate`.

The source-shaped result is finite: S3 retains the live-HWND Clipboard operand
repair and the immutable owner-destroy witness, but this cross-task callback
return requires the already-declared message/task execution boundary.  That
boundary must establish one delivery owner and the original task-return
identity before this witness can be accepted.  It is not permissible to
special-case Clipboard, `WM_RENDERALLFORMATS`, `DestroyWindow`, or a task ID;
nor to add polling, a timeout, a synthetic reply, or a second scheduler.  No
candidate DLL, package or deployed image is produced from this non-pass.

## 2026-10-25 terminal-owner source audit

The final task-loss adaptation was re-audited against the original cleanup
owner before any further runtime claim.  The conclusion is narrow and does
not create a second Clipboard implementation:

- Original `ntuser/kernel/cleanup.c::_WOWCleanup` performs **pseudo**
  destruction for a departing WOW task.  It changes the original window
  procedure/class association and releases original USER objects, but it does
  not call public `DestroyWindow` for the modern host HWND.
- The project-owned lifecycle table is consequently the sole bridge that can
  identify the actual modern HWND.  A public `DestroyWindow` is the only host
  operation that could request delayed formats, but the rejected controls
  below establish that it cannot be inserted into the existing `SG_EXIT`
  teardown sequence without violating the original callback-stack contract.
  No window destruction reorder is currently retained.
- No WOW cache, delayed-format list, callback queue or lifetime state is
  duplicated.  The normal terminal loop remains responsible only for its
  established native-window retirement and original resource cleanup.

Accordingly, replacing original `pfnWOWCleanup` is not viable: it is required
for original resource and procedure cleanup but has no modern HWND operation.
The remaining runtime gap is therefore a real host-task-transition seam, not
an unavailable isolated-public-Clipboard witness alone; this source result is
not a runtime acceptance result.

The `WM_RENDERALLFORMATS` dispatch relation was also checked against the full
original exit order.  `SignalProc(SG_EXIT)` sets `TDF_IGNOREINPUT` before
`ModuleUnload`; `W32DestroyTask` likewise calls `W32FreeTask` before clearing
that bit.  A one-message exception can enter the original procedure, but the
rejected control shows that `TDF_INITCALLBACKSTACK` still prevents its root
callback from returning.  Clearing either flag around destruction would weaken
the original termination contract and is not retained.

After the complete scoped source/test batch, the repository's current x86
Ninja WOW32 tree passed both checked-in gates through the reconstructed modern
runner:

- `WOW32_S1_FORMAL_GRAPH_OK`: 77 selected original sources, the original
  `wow32.rc`, 75 original-owner objects and 29 target bindings are present in
  the real link graph; and
- `WOW_USER_INPUT_CALLBACK_ABI_21_OK`: all 21 original input-table slots,
  including `W32EmptyClipboard@0`, `WU32ICBStoreHandle@8` and
  `W32Win16WndProcEx@24`, resolve with their required x86 calling conventions.

These are composition/ABI checks only.  They deliberately do not upgrade the
unavailable public-Clipboard exercise to a runtime pass, and publication and
S3 closure remain withheld for the three recorded lifecycle obligations.

## 2026-10-25 lock attribution control

The current `clipboard-r11` isolated witness repeated the native-peer failure
after the guest's selected original close.  The bridge saw
`OpenClipboard` fail with `ERROR_ACCESS_DENIED`, while `GetOpenClipboardWindow`
returned `0x00070812`; that HWND did not resolve to a thread or process in the
owned WOW worker.  Before the product launch, the same checked-in observer's
private-desktop `clipboard_access_probe` opened and closed Clipboard
successfully without inspecting or changing its contents.  Therefore the
evidence rules out both a generally unusable private desktop and a WOW
`WU32CloseClipboard` lock retained by this worker.  It identifies a foreign
holder in the shared `WinSta0` Clipboard during the test interval.

This does not make the callback path pass.  A desktop is not a Clipboard
isolation boundary, and this execution identity is denied creation of a fresh
window station; the test must remain a runtime non-pass until a disposable
window-station or otherwise independently controlled session is available.

## 2026-10-25 authorized shared-Clipboard attempt

With explicit authorization to use the current user's Clipboard, the same
sealed fixture was run twice without `MVDM_OBSERVER_PRIVATE_DESKTOP`.  Both
runs began on the shared interactive desktop and used a newly staged current
package; neither rebuilt the guest probe or native bridge.  In both cases the
guest completed its original `OpenClipboard` / `EmptyClipboard` /
`SetClipboardData` / `CloseClipboard` sequence and recorded phase 2 with no
guest error.  Its immediate native peer then received `ERROR_ACCESS_DENIED`
from public `OpenClipboard`, before the delayed-render or task-loss phases.

The first run reported a foreign open-window handle and PID; the second
reported no open-window owner, which is the normal observable form of an
external `OpenClipboard(NULL)` holder.  Immediately after those runs, the
host-only observer again opened and closed the same Clipboard successfully.
Thus the result is an externally contested shared Clipboard, not a retained
WOW lock and not a private-desktop denial.  The runs are retained as
`build/t460-s3/clipboard-transfer-r12-shared` and `...-r13-shared`.

No product retry, sleep, polling or command-specific workaround was added:
such a change would alter the original Clipboard failure contract instead of
testing it.  Since this blocks phase 3 and later, it supplies no runtime
verdict for the ordered `WM_RENDERALLFORMATS` task-loss change.

The final traced run (`...-r14-shared-callback-trace`) also establishes the
next boundary precisely: after the phase-2 native peer refusal and the guest
exit request, public USER entered the original `WM_RENDERALLFORMATS` procedure
(`W32WndProc-enter message=0306`) but no matching original-procedure return
occurred before the observer's owned deadline.  Earlier `WM_DESTROYCLIPBOARD`
and viewer notifications did return.  This is evidence of an externally
contended delayed-render callback, not evidence that the adapter rejected the
message: it reached the original dispatcher.  It remains a runtime non-pass;
no source-local claim of a successful task-loss callback may be made.

## 2026-10-25 rejected terminal-callback variants

Two source-shaped controls were exercised and then removed before any
publication.  They establish a concrete boundary for the remaining work:

- Injecting `WM_RENDERALLFORMATS` during adapter terminal destruction reaches
  the original owner procedure, but the root `CallBack16(RET_WNDPROC)` frame
  does not return after nested Clipboard work.  The trace shows the root
  frame entered and its nested calls returned, but no matching root return.
- Destroying the actual modern owner HWND before the adapter marks its task
  cleanup lets public USER itself issue `WM_RENDERALLFORMATS`.  With the
  original `TDF_IGNOREINPUT` rule it is intentionally suppressed.  Allowing
  only that one message reaches the original procedure, but again leaves the
  root callback unreturned: `SG_EXIT` has already set
  `TDF_INITCALLBACKSTACK`, whose original teardown callback-stack convention
  is incompatible with this modern synchronous delivery point.

Neither control changed guest media, introduced a cache/listener, or entered a
release package.  The production source remains at the prior selected
Clipboard/lifecycle boundary.  Further work must find a pre-existing original
task-transition dispatch point that can complete the callback before
`SignalProc` enters its teardown-stack mode, rather than weakening input
suppression or manually simulating the callback.

An independently compiled host-only control then ran the same public contract
without WOW, guest code, NTSRV or a worker: delayed `CF_TEXT`, `DestroyWindow`,
and `WM_RENDERALLFORMATS` attempting `OpenClipboard` for its own owner window.
It entered the callback exactly once but received `ERROR_ACCESS_DENIED` and
could not render (`HOST_RENDERALL entered=1 rendered=0 error=5 result=1`).
The result is retained in `build/t460-s3/host-renderall-control-r1/result.txt`.
That isolates the current failure to the shared host Clipboard environment;
the one-off control source was removed after producing this evidence so it
does not become an unstable permanent test dependency.

## 2026-10-25 complete USER teardown audit

The complete, read-only OpenNT USER source resolves the remaining ordering
question; the partial project mirror was insufficient to do so.  This is a
source conclusion, not a new import:

- `windows/core/ntuser/kernel/createw.c::xxxDW_SendDestroyMessages` calls
  `DisownClipboard()` only when the **actual Clipboard-owner window** is being
  destroyed.  `DisownClipboard()` in `clipbrd.c` synchronously sends
  `WM_RENDERALLFORMATS` before it clears that owner.
- `windows/core/ntuser/kernel/queue.c::xxxDestroyThreadInfo` instead calls
  `PatchThreadWindows(pti)` during thread loss.  The original
  `createw.c::PatchThreadWindows` comment and body state that it replaces
  application procedures with server-only procedures specifically so cleanup
  does not call applications.  It does not call `xxxDestroyWindow`.
- The original `wow32/wuser.c::WU32SignalProc(SG_EXIT)` sets
  `TDF_INITCALLBACKSTACK | TDF_IGNOREINPUT` before `ModuleUnload`; original
  `wcall16.c::CallBack16` deliberately reinitializes a callback stack under
  that flag.  The rejected controls therefore exposed the expected contract,
  not a missing one-message exception.

Consequently, an application that normally destroys its Clipboard-owner
window receives the selected original render-all callback while its callback
stack is live.  A task that exits without destroying that owner is not owed a
new render-all callback from thread/task cleanup; cleanup must instead retire
the native owner without a callback into the departed task.  The former
`phase 14` task-exit assertion in the disposable probe is stronger than the
selected original contract and cannot be used as a production-failure oracle.
Making it pass would require a new task-exit callback scheduler or a Clipboard
payload cache, both explicitly outside this S3 scope.

The remaining verification split is therefore finite: prove callback/data
materialization through the normal owner-destroy path when the public
Clipboard station is available, and prove that task-loss cleanup retires the
owner and original object state without a stale callback, duplicate cleanup or
leak.  No production source change, guest change, or candidate package follows
from this audit.

## 2026-10-25 public Clipboard HWND return audit

The audit then covered all seven Clipboard HWND crossings: six in original
`wuclip.c` plus `WU32GetOpenClipboardWindow` in `wuser31.c`, instead of only
the three inbound operands. In the paired NT4 USER implementation,
`USER16` was a cast of the paired USER handle representation.  On the modern
public USER boundary, the native return from `GetClipboardOwner`,
`GetClipboardViewer`, `SetClipboardViewer`, or `GetOpenClipboardWindow` can
instead be an arbitrary native HWND. The old direct `GETHWND16` conversion
therefore silently made
the native low word look like a guest identity.  It could collide with a live
worker-local handle and is not a valid cross-process Win16 handle.

`MVDM-HOST-DIV-454` uses only two existing authorities. Guest input resolves
through the worker's exact owned-object table. A native return first uses that
same table; when it is not owned, it may use the already committed page-domain
shared window view, but only after the normal graph refresh and only when its
full native identity still equals the stored full HWND. Unknown, retired, and
low-word-colliding native windows become zero. This adds no alias, proxy,
listener, cache or second authority. It preserves representable observed
relationships without reviving unsafe raw truncation; an arbitrary external
viewer chain remains outside this bounded public-USER adaptation.

The inverse public operand must use the same committed view as well. Otherwise
an observed predecessor returned by `SetClipboardViewer` could be represented
to Win16 but could not be supplied to the next original
`ChangeClipboardChain`. `wow_user_window_public_handle` therefore checks the
owned table first and then the existing page-domain full-identity snapshot.
The production fixture proves this round trip and proves that a graph refresh
after native destruction withdraws both directions. Generic original
`HWND32`/`FULLHWND32` conversion remains untouched.

The input adapter distinguishes an explicit zero handle from a failed nonzero
lookup. This is required because `OpenClipboard(NULL)` is a valid original
operation, whereas passing a retired nonzero Win16 handle must retain an
invalid-handle failure rather than silently becoming the current-task
operation. The same guard applies before `ChangeClipboardChain` and
`SetClipboardViewer` enter public USER.

The production-object page-domain fixture exercises the production runtime
wrapper as well as its table primitive: zero-extended and sign-extended
short guest input resolve to the exact live native window; a different full
native value with the same low word is rejected; and both directions reject a
retired entry. This is a handle/lifetime verification only; it does not claim
the externally contested Clipboard station can complete the deferred-render
runtime phases.

The 2026-10-25 `window-borrow` fixture compiled the production sources and
executed those three new projection assertions. The fixture's existing
`wow_user_window_refresh_return` test helper had already been removed as
unused production API; its two fixture callers now use the generic typed
payload refresh directly, preserving the same test operation without
reintroducing an exported wrapper.

The failure has now been measured rather than inferred: in its normal
callback mode the test samples `6,107,136` free bytes before the case and
`6,139,904` after it.  Thus the disputed 32-KiB block was released during the
case, not leaked by it.  The existing whole-fixture total is not a valid
per-callback allocation oracle because it includes deferred retirement from
an earlier row.  The fixture now asserts the actual isolation contract:
the case must not reduce free guest memory (`after >= before`), while a
larger value is an allowed deferred release.  The production runtime wrapper
positive, collision and retired identity checks all pass in that same run;
no product behavior or Clipboard conclusion is changed by this fixture
correction.

The static callback-ABI verifier also had a test-only invocation defect: its
parameter default evaluated before `PSScriptRoot` was reliable and could
resolve the repository as its drive root. It now resolves its own script directory
after parameter binding. A normal repository-root invocation passed with
`WOW_USER_INPUT_CALLBACK_ABI_21_OK`; this is ABI evidence, not public
Clipboard delivery evidence.

The current execution identity was rechecked with the current built
`console-startup-observer` in its no-content `clipboard_access_probe` mode.
It returned `open=0 open-error=5 close-error=0`; the probe does not enumerate,
read, empty or set Clipboard formats. This is a current host-access boundary,
not a reason to retry the mutating delayed-render witness or to add retry/
polling behavior to the original path.

### 2026-10-25 current source-build and fixture boundary

After the seven crossing audit, the affected x86 `wow32.dll` component was
rebuilt from the current source and the dependent `ntvdm.exe` target was found
up to date.  The original callback-table verifier passed with
`WOW_USER_INPUT_CALLBACK_ABI_21_OK`, including the original `WU32ICBStoreHandle`
and `W32EmptyClipboard` owners in `wuclip.obj`.  This establishes that the
strict public-HWND adaptation did not alter the selected original callback ABI.

The production-object `window-borrow` fixture was then launched through the
same checked modern PowerShell runner.  Its current parent token was denied
while creating the fixture's isolated page-domain desktop:
`WOW_PAGE_DOMAIN_DESKTOP_UNAVAILABLE error=5`.  This is a runner-context
failure before the HWND projection or Clipboard paths execute, not a passing
fixture and not a product defect result.  It must be retried only through the
documented desktop-capable runner; it does not justify a reduced assertion,
private-desktop bypass, or alternate Clipboard mechanism.

The independent exact-body control for the existing lifecycle publication
helpers passed from the current source with
`NATIVE_WAIT_PUBLICATION_BODY errors=0 no_debt=1 native_continuation=1
discharged_once=1 ordinary_return=1 failure_retained=1 exception_retained=1
last_error=1`.  It proves the selected native-return publication/unwind body
still discharges exactly once and retains failure/exception state.  As declared
by that checked-in fixture, it is not a scheduler, CCPU or guest Clipboard
acceptance substitute.

## 2026-10-25 terminal-window ownership audit

The project-owned terminal window loop is not a second Clipboard lifecycle.
It is the minimal modern-host realization of the original thread-exit pair:
OpenNT `queue.c::xxxDestroyThreadInfo` calls `PatchThreadWindows` before
`handtabl.c::DestroyThreadsObjects`; the latter destroys every non-process
object owned by the departing thread.  On this host, the Win16 task does not
terminate its underlying modern GUI thread, so merely retaining the original
WOW task cleanup would leave its real native HWNDs alive.  The adapter first
uses `wow_window_words_prepare_exit` to replace each selected procedure with
the matching no-guest-callback terminal procedure, then calls public
`DestroyWindow` to let modern USER perform normal child/order teardown.  It
therefore preserves the original safety property—no callback into a departing
application—while releasing the resource the original kernel would destroy
when its thread actually exits.  It neither requests `WM_RENDERALLFORMATS`
nor stores Clipboard data; normal owner-driven `DestroyWindow` remains the
only route that may render deferred formats.

## 2026-10-25 probe-contract correction

The disposable Win16 probe previously ended its task while its delayed owner
was still live and treated a resulting `WM_RENDERALLFORMATS` as mandatory.
That assertion contradicted the completed OpenNT cleanup audit above. The
probe now establishes delayed ownership, calls the original `DestroyWindow`
entry while its procedure and callback stack are live, requires exactly one
`WM_RENDERALLFORMATS` result, and only then ends the task. Phase 14 remains a
callback-return record, but now means `owner_destroy_renderall`, never
`task_loss_renderall`. This corrects test scope without adding a host callback
scheduler, listener, cache, guest change, or production lifecycle behavior.

## 2026-10-25 callback-continuation ownership audit

The retained owner-destroy witness does reach the selected original
`W32Win16WndProcEx -> CallBack16` route, but a nested callback may cause the
original Win16 scheduler to select another task before the outer native window
procedure resumes.  This is an intended original control flow, not a
Clipboard-specific exception.  `kernel31/wow16cal.asm::WOW16ApiCall` saves the
current `curTDB`; `WOW16CallContinue` compares it after the callback and jumps
to `tasking.asm::SwitchTask` on a change.  `SwitchTask` is explicitly documented
as a non-subroutine: it saves the old task's guest state and changes to the
next task's stack.

The current project adaptation has two independent C-stack scopes around this
route.  `wow_user_registration_bridge.c::call_previous_window_proc` enters a
`wow_task_callback_scope` before invoking the registered original
`W32Win16WndProcEx`; `wow_user_native_extension.c::CallBack16` enters another
before invoking the renamed original `wcall16.c::CallBack16`.  Those scopes are
valid only for a call which returns to the same native stack frame.  Each is
stored as the current `wow_task_order_thread::execution` pointer, and leave
requires that exact stack address to remain the current scope.  That condition
is not a durable continuation identity after the original task switch.

The task binding has the same boundary.  `wow_user_runtime.c` owns one native
TLS binding, populated by `registered_init_task`; its context is changed for
task creation, selected original USER critical-section client-view handoff,
and retirement, but not by `wow16cal.asm::WOW16CallContinue`/`SwitchTask`.
Therefore a nested host-to-guest entry after an original switch can still see
the preceding task's adapter carrier even though `curTDB` identifies a
different guest task.  The existing `client_context` callback transfers the
projected client page when the recovered original USER scheduler changes its
`CSOwningThread`; it is not a mapping from the original 16-bit `curTDB`, nor a
reply/continuation record for a native window callback.

This identifies a concrete common defect, but does not license a local
Clipboard change.  Removing these scopes outright would discard the existing
same-task native-call ownership contract; retaining them around all
`CallBack16` paths assumes an ordinary C return that original `SwitchTask`
explicitly does not promise.  The required successor is the already-declared
message/execution-continuation boundary: retain original `CallBack16`,
`WOW16CallContinue`, and `SwitchTask`; give only callback paths that can switch
tasks a durable adapter continuation keyed by the existing original task
identity; rebind the active adapter carrier only at a verified original task
handoff; and retire that continuation exactly once on original return,
task-loss, or worker loss.  It must carry no invented SMS, synthetic reply,
polling loop, timeout, or Clipboard-specific policy.  Before implementation,
the successor must classify every current `wow_task_callback_scope` caller as
either provably same-task or continuation-capable, and prove the original
task-identity source and return/loss hooks.  The public Clipboard station is
currently contested before the revised disposable probe reaches this boundary,
so this source audit is a design result, not a runtime pass or a candidate
release.

### 2026-10-25 correction: task-switch scope hypothesis is not a repair basis

The preceding continuation paragraph identified a real distinction between
the Win16 scheduler and the adapter's native scopes, but it overreached by
describing the TLS carrier as necessarily stale after `SwitchTask`. Further
source tracing resolves that point: original `wkman.c::W32Thread` creates a
distinct native thread for each Win16 task, sets that thread's
`CURRENTPTD()->htask16`, and calls `pfnInitTask(..., td.htask16, ...)` on the
same thread. The existing session thread hook binds one runtime TLS record on
every such thread, and `registered_init_task` installs the carrier created
from that exact `task_id`. The original guest scheduler can park task A and
run task B, but B runs with B's native-thread/TLS binding; A's outer C scope
remains on A's suspended native stack until A is scheduled again.

Accordingly there is no source proof that the two callback scopes are invalid
merely because `WOW16CallContinue` selects another task. They remain a
candidate only if a traced callback crosses a different native-thread entry
without the session hook, or violates their exact push/pop ordering. Neither
condition has been observed in the current contested Clipboard run. No
production change may be based on the earlier hypothesis; the Clipboard
investigation returns to the proven owner-destroy callback route and must
obtain a reproducible failure before selecting a callback/lifecycle repair.

### 2026-10-25 definitive probe ABI correction

Source tracing identified a test-owned stack defect. Original
`wow16/user/user.api` declares `CloseClipboard(VOID)`;
`user.asm` emits `DUserThunk CLOSECLIPBOARD,0`, and
`wow32/wutbl2.h` specifies zero argument bytes for both CloseClipboard and
EmptyClipboard. The independent probe incorrectly pushed a zero WORD before
each call. Pascal callee cleanup therefore retained those words. Inside
WM_RENDERALLFORMATS this displaced the saved registers and the far return
address. The phase-18 record proved only that CloseClipboard returned; it
did not prove a valid callback stack. The DPMI 017F stack in the failed trace
is the original locked fault stack selected by BeginUseLockedPMStack, not
evidence of an adapter task-identity failure.

All twelve stray argument pushes were removed. The probe now checks its
saved-word stack depth before restoring registers on every window callback.
No product callback/scheduler repair follows from this test defect.

Before adding the final stack-depth assertion, the unchanged candidate
package passed the complete private-desktop owner-destruction callback path
in `build/t460-s3/clipboard-zeroarg-r1`: fourteen phases, zero errors,
WM_RENDERALLFORMATS returned 447A0000 and DestroyWindow returned. Full
guest/native transfer in the private desktop was denied at native
OpenClipboard with ERROR_ACCESS_DENIED and is explicitly a non-pass.
The owner-authorized shared Clipboard run in
`build/t460-s3/clipboard-zeroarg-shared-r1` passed all eighteen phases,
including both transfer directions, rejected handoff, delayed rendering,
replacement and synchronous owner destruction. These results disprove the
earlier callback-continuation failure hypothesis for this witness. Final
guarded-probe and coherent-package acceptance remain necessary before S3
closure. The subsequent `clipboard-zeroarg-guarded-r1` shared run passes all
eighteen phases with the callback SP invariant enabled. The six obsolete
diagnostic modes and callback-scope bypass macro were then removed; the final
runner reports transfer coverage only for the full transfer mode. A clean
rerun of that simplified form first encountered another live owned Win95
menu test broker and correctly refused concurrent shared-service execution;
this is a test admission refusal, not a product failure.

The final simplified runner also records every actual product image hash and
checks its package manifest before staging. With the complete newly built
`final-reviewed-package-r1` it passes both `final-package-callback-r1`
(fourteen phases, private desktop) and `final-package-transfer-r1`
(eighteen phases, authorized shared Clipboard). All callback stack guards
remain enabled. The same package passes `final-package-messages-r1`: actual
USER16 two-task send/nesting/reply/post/filter/quit/string-copyback, native
peer in both directions and normal task retirement. The current selected
objects also pass the task-lifecycle fixture, 21-slot callback ABI and
77-original-source formal graph checks. PIC/keyboard selected-object
regressions pass with fourteen and forty-six assertions respectively.

Shared Clipboard `clipboard-final-contract-r2/r3` are retained as failed
external-access cases, not silently converted to passes: r2 observed Radeon
PID 31284 holding the Clipboard (ERROR_ACCESS_DENIED), and r3 also received
that error without an owner HWND. Neither exhibited the old invalid callback
return. The later final-package pass does not claim those failures passed.
Broad product/publication gates and the remaining PIC runtime matrix are
still required; these focused results do not close S3 by themselves.

The retained DOS nonzero-continuation runner was repaired to resolve the
actual package root and its system32 binary directory rather than the
retired flat layout. Its normal branch uses no injected user input. Both
direct and established short-stage runs currently return 0x57 before any
guest output; the short-stage report is retained under
`final-dos-nonzero-r2/logs`, and the product matrix's compact PATH control
has the same result in `final-dos-nonzero-r3/logs`. These are non-passes.
There is no evidence yet attributing that startup refusal to PIC, keyboard,
Clipboard or the immutable guest. Do not publish the newly built package
as a broadly accepted replacement until this finite startup boundary and
the required package gates are resolved. The previously owner-directed
PIC/keyboard publication remains separate from this candidate.

That DOS startup boundary is subsequently resolved as a harness-input issue:
`Generate-ObservationNinja.mjs` rebuilt the actual current
`console_startup_observer.c` in the stable x64 observation tree. Using this
observer with the same candidate and short-stage route passes the full
noninteractive batch in `final-dos-nonzero-r4/logs`: real guest exits 7/99,
both errorlevels retained, subsequent MEM output, and COMMAND exit zero.
The old copied observer failures remain non-passes and no product fix was
made for them. Future package acceptance must use the newly compiled observer
and retain its identity rather than inheriting a stale package test EXE.

Final source review removed the temporary per-callback scheduling and CCPU
simulation-depth diagnostics after identifying the probe's argument-cleanup
defect. No callback/simulation behavior change is retained from that theory;
the original `ntthread.c` returns to the baseline bytes. The retained task
lifecycle change queries the selected thread's actual registered idle hook,
not an unrelated zero-thread registration. The clean-source coherent build
and acceptance are rerun rather than reusing pre-cleanup binary results.

## Reviewed-source package rerun (progress, not closure)

`review-build-result.json` retains four zero exits: x86 component build,
second x86 no-change increment, x64 components and x86 hook/redirection. The
post-diagnostic-cleanup package is `final-reviewed-package-r2`; the previous
owner-published PIC/keyboard package remains installed, not this candidate.
With the freshly source-built observer, `review-package-results.json` records
passing fourteen-phase owner-destruction callback, actual DOS IRQ0 control
and reset, fourteen PIC assertions, forty-six keyboard assertions, original
task lifecycle, callback ABI, formal graph and DOS nonzero continuation.
The first message invocation incorrectly selected SharedWorkerControl without
its cross-worker prerequisites and was refused before runtime. The corrected
historical plain invocation `review-messages-r3` passes its original two-task
message/native-peer/normal-retirement contract.

`review-clipboard-r2` and the separately retained `review-clipboard-r3` do not
pass complete transfer: each has a partial guest report and public
OpenClipboard denial. The first identifies external owner PID 31284 in the
native bridge trace, the same Radeon process observed earlier. The old
invalid callback stack does not recur. Earlier complete transfer successes
remain evidence for their exact packages, not automatic passes for r2.
No retry/polling workaround or unrelated-process termination was added.

`review-frontiers-r2` observes actual Minesweeper, Solitaire and Write windows
on the private desktop with the source-built snapshot tool. These observations
use five-second post-launch retention; the initial zero-retention r1 run
proves only launcher completion and is not window-creation evidence. Neither
observation is interactive gameplay or normal application exit acceptance.

The independently published add-ons/tools are submitted separately as a
bounded progress delivery under the linked add-on audit. The core runtime
candidate and the remaining S3 acceptance/publication stay open; this record
does not declare the entire worktree clean or S3 closed.

## Task loss versus live owner destruction

The S3 source audit distinguished actual task exit from DestroyWindow followed
by exit. `verify-wow-clipboard-transfer.ps1 -OwnerTaskExit` now assembles the
independent probe with one delayed CF_TEXT owner, records its original HWND,
and exits without calling DestroyWindow. `wow_user_task_prepare_exit` retains
original queue/cleanup ordering: mark cleanup, withdraw the departing window
procedure, destroy native windows while their original resource owner is
still live, then retire the task through original cleanup/taskman.

`task-exit-r3` and `final-source-task-exit` pass seven phases, zero guest
errors, original TaskRetirement complete, no guest WM_RENDERALLFORMATS
dispatch, and a separate headless native observer's `IsWindow`/Clipboard-owner
retirement checks. This supplements, rather than replaces, the live-owner
DestroyWindow/WM_RENDERALLFORMATS witness. The normal transfer mode still
requires its entire eighteen-phase original contract.

The first draft correctly exercised normal rundown but its host assertion
mistook WOWExec's own windows for probe windows (`task-exit-r1`). The corrected
check binds the complete native HWND to the exact reported guest low word.
`task-exit-r2` failed earlier in its unrelated eager/second-open preamble.
The final task-loss mode therefore has one original open/empty/deferred-set/
close and direct exit, with no eager-transfer preamble or retry. All earlier
failed reports are retained as non-passes. Callback-only mode likewise now
isolates delayed setup and live-owner destruction, reporting the actual
rendered handle; complete bidirectional transfer remains a separate mode.

After the probe ABI cause was established, the temporary DPMI register-trace
extension was also removed. `dpmiint.c`, its termination helper/header and
`ntthread.c` now match their pre-investigation baseline; no abandoned tracing
or scheduler change is retained. The seven actual original code files still
changed are Clipboard identity, PIC/keyboard and separately approved Video7
sources; all their exceptions are recorded in `src/mvdm/README.md`.

## Frozen-source acceptance and remaining delivery gate

`final-source-build-results.json` has four successful dependency-selected
build steps, including the second no-change x86 increment. The package is
`final-source-package`; it retains original system media, root applications
and configuration but does not recursively copy historical build/log/test
output. No original guest content changes. The affected images are:

| Image | SHA-256 |
| --- | --- |
| ntvdm.exe | F45A1908A1F1C8C5D6E5A53542EF46D35686486DBA2047E12503402D7F3763AE |
| WOW32.DLL | 710E8184AA05F637801C12067940EF46B68CE39B5248C19DD46DBAC1FC52786A |
| VDMREDIR.DLL | CAFDC81425DD1045DFF249D0DF0EBA4BEC87B3F13EB34F5FFB9BB89BD61B678B |

Its full ten-image manifest retains unchanged images too. The final explicit
three-mode run `final-source-clipboard-results.json` passes all modes:
`final-clipboard-callback` has thirteen delayed-setup/live-destruction phases;
`final-clipboard-task-exit` has seven direct-rundown phases;
`final-clipboard-transfer` retains all eighteen bidirectional, rejected,
deferred, replacement and owner-destruction phases. Every mode checks zero
guest errors and unchanged actual package/fixture inputs. This is not a
retry loop or removal of the full transfer assertions. Earlier failures
remain non-passes: the first broad callback attempt failed in its unnecessary
eager/second-open preamble, and the broad transfer attempt was refused before
runtime by another live owner test's BaseSrv. That broker was confirmed exited
before this final three-mode group started.

The same frozen package passes actual DOS IRQ0 control/reset plus the
twenty-four PIC/RTC/mouse assertions, forty-six keyboard assertions, the
production-object task-lifecycle fixture, twenty-one-slot callback ABI,
seventy-seven-source formal graph, actual two-task/native-peer messages,
DOS errorlevels 7/99 with subsequent MEM and normal COMMAND return, three
read-only application-window frontiers, and the actual packed-painter
boundary/guard fixture. `final-source-acceptance-results.json` retains the
individual results, including the two superseded non-passes rather than
overwriting them with this final Clipboard group.

The original code delta is seven files, 161 added/23 removed lines (net 138),
excluding the exception register. No new Clipboard behavior/cache/scheduler
exists; no temporary CCPU/DPMI investigation change remains. PIC and keyboard
have one original owner each; the additional two keyboard facts describe
separate real port consumption and normal IRQ acknowledgement, not duplicate
PIC state. RTC/mouse fixtures call their actual original providers.

This is **not** a full production-P/S3-closure verdict. The owner subsequently
clarified that probe-driven interaction is allowed. The inherited interactive
DOS17/Window17 and gameplay/input gates are therefore being run using the
existing isolated-desktop probes, without Computer Use. Read-only window
creation does not substitute for those gates. The candidate-only submission
question is superseded, not approved; the ordinary coherent publication,
verification, review and submission requirements still apply. The earlier
PIC/keyboard package remains published until the new package passes them.

S4 still owns repeated combined transfer, the inherited selected structured
format (CF_DIB) and complete selected-format/lifetime acceptance before DDE
handoff. S2's text-only delivery does not silently discharge that requirement.
These are inherited S4 obligations, not a transfer of a proved S3 foundation
defect. No finite test result here completes the overall WOW32 program.

## R4 candidate gate — 2026-10-25

The exact `r3-package` built after the shared-view correction was rebuilt with
the otherwise stale x64 `monitor-rpc-test.exe`; all five R3 build stages and
the no-change x86 increment succeeded. R4 then passed independent callback,
task-exit, page-domain lifecycle (including committed external-HWND return and
post-retirement rejection), callback ABI, formal graph, PIC-reset (24
PIC/RTC/mouse assertions plus DOS IRQ0 control/reset), keyboard (46
assertions), Win16 message, packed-painter and the complete product gate.
The complete gate includes Console17, Window17, all six RPC rows, GUI routes,
identity negatives, modern EDIT, rapid relaunch, interactive relaunch and
Console close/return routes. Previously run private-desktop WINMINE and SOL
gameplay probes also pass against the same package.

The independent full shared-Clipboard mode is the sole R4 non-pass. Its first
fresh root reached 132 of 216 expected bytes; a single strict fresh retry
reached 96. In the retry native trace, public `OpenClipboard` returned
`ERROR_ACCESS_DENIED` with owner PID `0xA7C` (2684), confirmed as unrelated
`RadeonSoftware.exe`. The former root also recorded access denial while
Windows exposed no owner PID. The probe's callback and task-loss modes pass;
there is no observed assertion failure in the public-HWND mapping itself.
No unrelated process was stopped and no retry loop was used. Accordingly this
is an explicit external global-Clipboard release gate, not a passed full
transfer or a reason to publish the new coherent candidate.

## R5 shared-view round-trip correction — 2026-10-25

Source review found and repaired one closure defect in the R4 adapter: it
could return an already observed external `HWND` to Win16, but the inverse
public operand path searched only the owned table. R5 reuses the existing
page-domain exact snapshot after that table lookup, so the returned viewer can
be presented to the next original `ChangeClipboardChain`; it does not alter
generic original handle conversion. The production page-domain fixture proves
external return → inverse public operand → native full HWND, and proves both
directions reject it after destruction and graph withdrawal.

`r5-package` was rebuilt in five cached stages, including a no-change x86
increment. It passes callback, task-exit, page-domain, ABI and graph fixtures;
the whole product gate; and both isolated WINMINE and SOL gameplay probes.

The remaining full-transfer row was then run once with a freshly identified
global Clipboard owner, `logioptionsplus_agent.exe` (PID 12216), stopped under
owner authorization. It passes all 18 checkpoints with no guest errors:
`guest_to_native=1`, `native_to_guest=1`, `rejected=1`, `deferred=1`, and
`replacement=1`. The bridge trace has three successful public calls and no
`OpenClipboard` failure. This resolves the prior environment-only
`ERROR_ACCESS_DENIED` measurements (Radeon had already auto-restarted after
its first termination, proving its PID was not a durable attribution).

R5 was published after that result: all ten manifest images were copied to
`assets/release` and `O:\winnt\system32`, each matched its package SHA-256, and
the pre-publication files were saved recoverably under
`build/t460-s3/publish-r5/recovery`. The exact published manifest is
`build/t460-s3/publish-r5/published-manifest.json`. This evidence records a
passed candidate; source review, governance and coherent commit remain the
separate closure steps.
