# T473 S7 WOW32 session and callback-owner audit

## Question

Can the selected original WOW32 callback and per-task termination paths execute
through the current worker without treating NT4 USER private state as a modern
host object, duplicating task identity, or adding a second intercomponent
control transport?

## Inputs and procedure

- Active T473 S7 packet and the T473 S2 [WOW/session reconciliation]
  (m0-t473-s2-wow32-session-reconciliation.md).
- Selected original `src/mvdm/wow32/wcall16.c` and `wkman.c` bodies.
- Current worker/session carriers, WOW32 bindings and BaseSrv lifecycle
  sources named below.
- T449's retained real two-task desktop witness in
  [its S3 record](m0-t449-s3-targeted-termination-progress.md), plus the
  linked service reservation and worker-input fixtures.

This is a source and retained-evidence audit.  It does not recast a historical
fixture or a prior desktop witness as a fresh current-package application run.
The stale formal Ninja root was deliberately not used: it lacks an object
required by the current generated selection and is a cache-maintenance matter,
not a WOW behavior result.

## Owner and authority chain

| Concern | Behavior owner | Boundary owner | Authority and failure rule |
| --- | --- | --- | --- |
| 16-bit callback frame and result | Original `wcall16.c::wow_original_CallBack16` | `wow_callback_frame_lease.c` | Original code constructs, switches, simulates and resumes the frame.  The binding owns only a call-scoped CPU40 guest-memory lease and capture/restore of the finite execution-control fields.  A failed lease or selector restore fails the existing call; no pointer survives the call. |
| Session/TLS ownership | `session` thread lifecycle | `wow_user_runtime.c`, `wow_user_session_binding.c` | A worker has one TLS binding per executing session thread.  Attach refuses a second binding; detach refuses live peer threads and keeps registration if provider disposal fails. |
| Original task cleanup | Original `wkman.c` task destruction and selected USER cleanup | `wow_user_task_lifecycle.c` only for unavailable host-resource release | The lifecycle carrier may release its own public-USER registrations and copied views only after original task retirement.  It does not publish private USER pointers or choose a task to terminate. |
| Individual WOW task close | Original `wkman.c::W32HungAppNotifyThread` | `wow_task_termination.c` | The original handler remains the terminating routine.  The binding maps its live original `HTASK` to one opaque service ticket and invokes that handler locally because its historical calling convention exits the caller thread. |
| Management identity | NTSRV lifecycle registry | `lifecycle.c`, worker-base RPC wrappers | NTSRV issues the opaque ticket and accepts it only for the current authenticated worker generation.  WOW owns `HTASK`; the service never interprets a private task structure or uses an untrusted sender-local handle. |

The selected build generator compiles all selected original `mvdm/wow32`
sources into `WOW32.DLL`.  The parent worker exports only the finite session,
guest-memory and CPU40 ABI (`wow_user_runtime_*`, guest lease and CCPU control
primitives).  It does not link a second WOW implementation or an NT4 USER
server.

## Lifecycle and rejection trace

1. Original `wkman.c::W32Thread` has the source-owned `iTask` and `HTASK`
   only after original `InitTask` succeeds.  It binds them through
   `wow_task_termination_bind`.
2. `OpenNtBaseClientBindWowTask` sends them by the existing BaseSrv RPC.  The
   service verifies PID/generation and WOW-worker class, then allocates an
   opaque label identity.  Duplicate original task binding is rejected.
3. A management close becomes a service-owned instruction.  Worker-base only
   fetches its opaque identity.  `nt_bop.c` calls the loaded provider export
   `W32TerminateTaskTicket`; the binding resolves it only against a still-live
   local `HTASK` entry.
4. The binding serializes the historical one-global-handler input, starts the
   original handler on a local thread and drops only the serial gate when that
   handler exits.  Natural original destruction calls
   `wow_task_termination_retire`; an original explicit cancellation calls
   `wow_task_termination_fail`, which reopens the same service row rather than
   reporting a termination success.
5. Worker death removes the service watch/labels.  A later worker generation
   cannot receive the former ticket.  Separate WOW deliberately has no
   durable task label and remains whole-worker-close-only.

This preserves the required distinction: a task-close request is neither a
worker close nor a scheduler implemented by the management service.

## Callback nesting and public USER boundary

`wow_user_runtime_enter/leave` owns only an SRW lock around public-host calls.
`leave_for_callout` and `reenter_after_callout` reuse that same lock; no second
queue, private SMS list, or alternate execution engine is introduced at the
worker boundary.  `wow_callback_frame_lease` reacquires a bounded guest lease
at each original frame access and clears it at release.  On an abnormal
original callback unwind, `wcall16.c` restores only the saved frame fields
through the binding and retains the original `TDF_INITCALLBACKSTACK` ordering.

The provider does contain project-owned public-USER view and message bindings
where NT4 server structures cannot be composed.  They are not private USER
state: native HWND/menu/dialog resources remain modern USER objects, while
project records are bounded metadata with retirement/refusal rules.  This S7
audit found no new direct conversion of a modern handle, pointer or token into
an NT4 private structure.  Completing every message/object/control behavior
is not claimed here; those existing provider families retain their explicit
later acceptance obligations.

## Evidence and result

T449 S3's real shared-worker witness established two independently closeable
live tasks, sibling survival, a later task incarnation, idempotent duplicate
submission, and distinct whole-worker close.  Its service fixture covered
ticket authentication, stale/reuse, failure-reopen and retirement; its
worker-input fixture covered close priority and 50 handle-reuse cycles.

The current source trace confirms that those claims still follow the same
original owner and RPC route.  No S7-specific production source defect has
been found in this cohort, and therefore no runtime image was changed or
published.  The attempted stale formal-cache validation is intentionally not
counted as pass/fail evidence, per the active owner direction.

## Remaining finite work

- Execute the current selected-provider/formal route once its stale generated
  cache is reconciled through the ordinary generator; this is verification
  maintenance, not a source repair.
- Keep real guest coverage for public window/menu/dialog and mixed callback
  behavior attached to their existing provider-family packets.  This audit
  does not claim them merely because the session and termination transport is
  sound.
- If a future trace proves that a project-owned USER binding creates behavior
  independent of a selected original caller, open a bounded recovery packet;
  do not extend this session carrier into a general USER replacement.
