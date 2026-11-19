# S14 WOW resource and callback ownership review

Research-only checkpoint against `732f4a23fcfa53aca579dbb965d1392d6c26ecc4`.
Question: which actual implementations behind the C-to-F edges can move to B,
and which contain original private state or product lifecycle decisions?
This record does not authorize a mirror increase or close dependency IDs.

## Inputs and method

Read the complete implementation bodies below in the current working tree;
follow named callers using `rg` over `src` and `test`. Inspect the associated
headers, `common/guest-memory/access.h`, `compat/include/guest_memory_access.h`,
`non-mvdm/windows/core/ntuser/inc/user.h`, and the abnormal-return portion of
`mvdm-host/wow32/wcall16.c`. Paths in the table are relative to
`src/wow32-dll/source`. This is source evidence, not a fresh compile, link or
runtime test. Earlier selected-graph inventory remains separately recorded.

## Implementation dispositions

| File | Observed responsibility and state | Proposed ownership and necessary follow-up |
| --- | --- | --- |
| `wow_bitmap_bindings.c` | TLS conversion context, serialized scratch/DC lifetime, screen capabilities; `wow_bitmap_load` calls original `WOWLoadBitmapA`. | Move independent scope/DC/lock mechanics to B. Keep original-call orchestration separate so B does not call C. Split C `newres.h`, original declarations and alias macros from the resource carrier. Preserve recursive calls and failure cleanup. |
| `wow_button_resources.c` | Caller-owned GDI atlas; create into a temporary resource object, publish only after rendering succeeds; restore selected object and release on failure. | GDI mechanics are a B candidate, but current metadata uses C `OEMBITMAPINFO` and `OBI_COUNT`. Resolve this representation boundary before relocation; moving its current header wholesale would introduce B-to-C dependency. Do not duplicate a button state engine. |
| `wow_button_lifetime.c` | TLS invocation stack, private `PBUTNWND`/`PLOOKASIDE`, deferred release at the oldest same-control invocation, native-window dispatch wrapper. | Private control-state lifetime belongs with C algorithms; native entry composition stays outer. Only independent resource leaves belong B. Exact original carrier and mirror cost remain to be established before editing C. |
| `wow_callback_frame_lease.c` | CPU continuation capture/restore; protected-address conversion; bounded copied-memory leases; worker-active check and fatal-selector boundary. | CPU and selector operations belong C, generic lease mechanics B, worker cancellation/terminal policy outer. Do not move the whole file to B or conceal active/fatal checks behind new callbacks. The D access header is only a forwarding include to B, but removing that include detour alone does not solve implementation ownership. |
| `wow_class_client_bindings.c` | Scoped class context; original registration; native class publication and procedure normalization; completed guest-class projection and rollback; optional CPU-position diagnostic. | Separate native resource publication from original class lists, task/procedure representation and guest projection. Original structures and algorithms remain C; product entry composition remains outer. Do not classify the whole implementation as independent B merely because it calls `RegisterClass`-related APIs. Trace class-word and window-creation providers before choosing the exact split. |
| `wow_cleanup_bindings.c` | Native window/procedure verification plus temporary reconstruction of private USER child/sibling lists; invokes `_WOWCleanup`, then restores root and clears links in `finally`. | Private cleanup projection and original invocation belong C; independent native operations may be B. Preserve no-application-callout procedure replacement and retained object identity. Native handle checks alone cannot replace original cleanup. Full object-retirement chain remains to audit. |
| `wow_user_message_bridge.c` | Native queue initialization, owner-thread checks, sent-message-only peeks, private queue wake-bit update and native-call release/reacquire hooks. | C owns original queue/task ordering; B can own independent native queue operations only if their reentrant call boundary is retained outside B. Preserve PM_NOREMOVE/PM_QS_SENDMESSAGE and the wait-specific PM_NOYIELD contract; do not introduce a second message loop. Native-call providers remain to audit. |

## Specific ordering evidence

- `wcall16.c` invokes `wow_callback_control_restore` only during abnormal
  unwinding. Its return value gates restoration of `ptd->vpStack`,
  `vpCBStack` and callback flags. The current provider checks whether the
  worker is still active before touching CPU state, reloads CS/SS against
  current descriptors, and does not restore CR3 or cached descriptors.
  A relocation must preserve both machine restoration and terminal-abandonment
  semantics; blindly copying CPU setters into B is not a solution.
- `wow_class_client_publish` calls `_RegisterClassEx` before
  `publish_completed_class`: the original algorithm supplies WOW words and
  task association after initial native publication. Projecting the initial
  allocation instead would publish incomplete state. Failure destroys the
  newly published native class through the existing class-word owner.
- `wow_user_message_bridge` wraps native peeks because they may invoke client
  code synchronously. Its ordinary return and scheduler-wait return use
  distinct existing end operations. These must be traced, not replaced by a
  plain public API call on the basis of matching function names.
- The button atlas header reaches `OEMBITMAPINFO` and OBI constants through
  `opennt_user_multipass.h`, which includes original C `user.h`. Defining
  `WOW_ORIGINAL_BUTTON_RESOURCES` in the wrapper is not proof of an independent
  B declaration closure.
- Source search finds `wow_bitmap_bindings_shutdown` calls in tests, but no
  production call in `src`; the production definition and declaration remain.
  Its current terminal-only INIT_ONCE/lock lifecycle must not be described as
  verified production teardown. Reconcile final link and unload ownership
  before deciding whether to wire, retain or remove it.

## Result and remaining evidence

Seven complete bodies classified; no implementation moved and no dependency
record closed. The historical direct ledger remains 150 eliminated / 418 open
of 618 total records. Mirror changes: zero points, zero lines.

This review gives concrete ownership constraints, not completed migration
designs for every chain. Next inspect class-word/window-creation, native-call
entry/return and USER retirement providers, then reconcile destination files,
source provenance, mirror cost and final-code validation. Full four-component
inventory and caller/lifetime verification remain unfinished. No guest source
or binary was changed; no tests or release package were rerun for this review.

## Runtime binding and retirement follow-up

Against P32 `be33fdff8`, read both complete NTVDM implementations
`src/ntvdm-exe/wow/wow_user_runtime.c` and `wow_user_session_binding.c`,
their headers and `src/common/wow/user_runtime.h`. Also read the selected
native-call, callback, publication, initialization and retirement functions
in `wow_user_task_lifecycle.c`; this is not a complete read of that large file.

The concrete C0618 path is original `taskman.c`/`queue.c` current-thread
access, expanded through the existing task-order bindings to
`wow_user_runtime_current`, resolved to the NTVDM provider. The ledger's
COFF/map evidence matters here: absence of the expanded symbol spelling in
the original C source is not absence of the dependency.

The runtime has independent SRW-lock/binding mechanics, but its unbind
operation invokes `retire_thread` before clearing TLS. Initialization in
WOW32 installs `retire_current_thread` and `dispose_process`. Retirement
performs original USER cleanup, refuses live owned windows, removes native
hooks and input associations, calls `DestroyTask`, and frees task resources.
These bodies cannot be treated as B-owned generic resource callbacks.

The migration design is to separate fallible retirement from the B binding
primitive. The composing outer boundary must validate the binding, perform
retirement while it is still current, and only then clear the B binding.
Failure retains the binding and provider state. Process disposal remains
after all thread retirement and before removal of session hooks. Production
unbind calls occur in `wow_user_session_binding.c`; direct test callers must
be divided into primitive tests and complete retirement tests, not weakened
to accept premature unbind. Reuse the existing binding; do not introduce a
second TLS current pointer. Final representation of C task/class references
and the destination of their algorithms remain part of the complete design.

The native-call return path reacquires the data lock, marks the suspended
scope ready, reacquires original execution ownership through
`xxxDirectedYield` if needed, pops the scope, then publishes native window/menu
results before guest continuation. A scheduler wait without execution
ownership defers publication until ownership is acquired. Callback entry
likewise uses original `xxxSleepTask`/`xxxDirectedYield`; these are C scheduler
semantics, not a generic B mutex operation.

Existing tests supply relevant contracts, not fresh acceptance:
`test/adapter-mvdm-host-out/wow/wow_user_runtime_fixture.c` checks lock
release/reacquire and rejects unbind while held. The lifecycle fixture in the
same directory checks that a live owned window makes unbind fail with
ERROR_BUSY while retaining the exact task and binding. Keep these negative
requirements after splitting the entrypoints. No production change, test run,
new exception, or dependency elimination follows from this source review.

## Complete lifecycle body review

Against P33 `f449bde29`, finish reading all of
`src/wow32-dll/source/wow_user_task_lifecycle.c`, including its generated
native wrapper definitions, initialization rollback, message loop, launch
notification and terminal-window destruction. Together with the preceding
reviews this covers ten complete implementation bodies (the original seven,
two NTVDM runtime/binding files, and this lifecycle file). It does not mean
all transitively called providers have been audited.

| Responsibility | Concrete implementation/state | Required disposition |
| --- | --- | --- |
| Product start notification | `notify_start`, `launch_request`, `start_pending`, `start_attempt` | Keep product request identity and RPC transaction outside B/C. Preserve the current call-frame attempt identity across reentrant notification, without retaining a task pointer across RPC. The exact carrier change must not create duplicate task identity/state. |
| Original task execution | `xxxInitTask`, `xxxSleepTask`, `xxxUserYield`, `xxxDirectedYield`, `DestroyTask`; private TDB/process/queue fields | C owns these algorithms and their original state. Host readiness and callback adapters must be traced to finite original owner seams, not moved as an arbitrary 1,800-line C insertion. |
| Data lock and thread binding | `wow_user_runtime_enter/leave/current/set_context` | Independent mechanics are B candidates; separate retirement as described above, and preserve one authoritative binding. |
| Native USER call wrappers | HWND conversion, result roots, native call begin/end, publication, destruction without publication | Mixed C representation/execution contract and native USER operation. They are not ready for whole-file B migration. Keep the single shared handoff implementation; adding one callback bridge per wrapper would preserve the outward dependency. |
| Task/module resource cleanup | `wow_user_task_lifecycle_exit`, original `_WOWCleanup` | Resource cleanup does not retire THREADINFO. Task-zero module cleanup must not destroy the task's native windows. |
| Native-thread retirement | `retire_task`, terminal window destruction, hook/input withdrawal, `DestroyTask` | Preserve destruction while guest resource callbacks are still usable, followed by original cleanup and final thread-carrier release. Retirement refusal retains ownership; product thread completion remains outer. |

### Start-chain evidence

`mvdm-host/wow32/wkman.c::W32Thread` retries `pfnInitTask` up to its existing
bounded loop before destroying a failed task. The selected
`wow_user_registration_bridge.c::registered_init_task` resumes an already
initialized task's pending view publication and notification instead of
calling `xxxInitTask` again. Its notification callback is
`common/base_srv/base_rpc_client_launch.c::OpenNtBaseClientWowStarted`, which
uses the authenticated execution connection and `Client_WowStarted` RPC.
This is concrete product-service knowledge, not an independent compatibility
operation or a guessed meaning of the callback name.

### Wait, callback and teardown evidence

`wait_for_task_or_message` uses native queue/event waits, then updates original
`ptdb->nEvents` and shared `nEvents` under the existing runtime lock. It
distinguishes native sent-message dispatch from guest execution ownership;
an input-plan transaction defers receiving until original InitTask releases
that plan. `client_context` exchanges debugger task identity and active WOW
page-domain client state. Moving only the native wait into B cannot transfer
these C responsibilities.

The native wrappers deliberately distinguish ordinary return publication
from DestroyWindow's no-publication return. Terminal cleanup first marks and
prepares owned windows under the lock, retains each window's storage, drops
the lock for native destruction/reentrant calls, and then reconciles surviving
relations and retired classes. Replacing this with generic resource freeing
would lose the required callback lifetime.

### Verification requirements for the eventual split

Preserve existing runtime/lifecycle fixtures and add assertions at changed
boundaries only: failed start notification does not rerun InitTask; module
cleanup leaves the task bound; retirement failure retains exact owner state;
native callouts do not hold the data lock; callback return reacquires the
original execution owner; destroyed HWNDs are not republished. Reuse existing
Win16 lifecycle/dialog acceptance for final code. No test was run in this
research checkpoint, and no mirror-increase candidate is approved by it.
