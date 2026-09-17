# BDM Terminal Task Manager

## Objective

Deliver `dtmgr.exe`: a native Windows Console character-mode task manager for
this product's brokered DOS and Win16/WOW16 work only.  It presents a compact,
continuously refreshable Terminal/conhost TUI with a graphical character
layout, lets the user select a listed task with Up/Down, and asks BaseSrv to
terminate the selected product task after an explicit confirmation.

`dtmgr.exe` is not a guest DOS program and does not emulate a DOS screen.  It is
a product-owned Console executable that may run in Windows Terminal, conhost,
or another supported Console host.  It observes only tasks registered with
this product's `basesrv.exe`; it never enumerates or controls unrelated
Windows processes.

## User-visible contract

- With a reachable BaseSrv, show one row for every broker-known active task:
  stable broker worker ID, program/image name, kind (`DOS`,
  `Win16` or `WOW16`), parent task when present, start time, elapsed duration,
  and broker-defined state.
- Up/Down changes the selection; refresh does not lose the selected stable
  worker ID when that worker remains present. `K`/Delete requests termination
  of the selected worker, and a visible confirmation is required before any
  state change. If it hosts COMMAND plus child DOS/WOW tasks, the confirmation
  names that scope and states that all of them will end. Escape/Q exits
  dtmgr only.
- If no task is registered, display an empty table and an explicit “no product
  tasks” state.  If BaseSrv is absent, unreachable, incompatible or exits while
  dtmgr is running, display an empty table with the precise broker-status reason,
  disable termination, and remain usable until the user exits or reconnection
  succeeds.
- A terminate request is scoped to one selected broker worker ID. BaseSrv owns
  authorization, ID-to-worker resolution, cancellation/rundown and result
  reporting; dtmgr must never call `TerminateProcess` or trust a client-supplied
  native handle/PID. BaseSrv uses only its pre-existing authenticated,
  process-local worker handle, and its existing worker-exit watch performs the
  original cleanup after the process exits.

## Architecture and ownership

`src/dtmgr-exe/` will own the executable, Console rendering, keyboard navigation and
presentation-only selection state. `src/basesrv-exe/` will own a small versioned,
authenticated management endpoint and the authoritative worker snapshot and
termination state transitions. `src/product-abi/` may carry only fixed-width
versioned request/response declarations where genuinely shared; the service
IDL and its semantics remain BaseSrv-owned. `run16`, `ntvdm` and the original
MVDM/OpenNT mirrors retain their existing lifecycle owners and receive no TUI
policy.

The snapshot uses copied fixed-width broker worker IDs and descriptive metadata.  It
contains no guest pointers, native pointers, inherited handles, or trusted
sender-local handle values.  It uses the existing application/protocol version
agreement and same-user access policy; mismatched versions are an unavailable
broker state, not a best-effort decode.

## Ordered S plan

1. **Management contract and BaseSrv lifecycle audit.** Freeze the current
   authoritative worker/guest-record lifecycle; design the smallest snapshot,
   query, worker-terminate request, response and disconnect/rundown ABI. Prove
   how DOS, Win16 and WOW16 task kinds are represented without inventing a
   second registry. Identify original BaseSrv owner calls to retain and every
   required product-owned binding.
2. **Read-only dtmgr TUI.** Implement and test `dtmgr.exe` Console rendering,
   refresh, absent/incompatible broker state, empty state, selection stability,
   resize behavior and keyboard navigation. It must make no terminating call.
3. **Broker-authorized termination and end-to-end closure.** Implement the
   confirmed single-worker request, server authorization, worker/task teardown,
   response/rundown and races (task exits, worker/broker dies, stale selection,
   duplicate request). Prove that only the selected registered worker and its
   explicitly disclosed hosted tasks are affected, then run the established
   direct and interactive COMMAND/MEM/EDIT regression matrix unchanged.
4. **Disconnected empty-state repair.** When BaseSrv is absent, incompatible
   or disconnects, retain the ordinary empty table and the `No product tasks.`
   row, add a distinct `BaseSrv not connected` status/retry line, and do not
   start a broker or exit dtmgr. Build, deploy and regress the four-program
   package; leave T419 open for owner Console acceptance.
5. **Real process-handle management binding.** Keep the authenticated server
   check unchanged, but make dtmgr pass the same owned, real self-process
   `system_handle` as its passing RPC fixture for both snapshot and terminate;
   prove a live registered COMMAND worker is rendered before closing T419.
   In the owner-approved shared delivery scope, restore the public run16
   compact-first-option convention for every resolvable bare image (for
   example `command/c`), before the original classifier; it is not a
   COMMAND-specific parser and does not reinterpret drive- or
   backslash-qualified paths.
6. **Selected-worker authority completion.** Preserve the query/synchronize
   worker self-handle used by the worker's authenticated connection, but let
   the BaseSrv watch retain only the same-access duplicate of the worker
   already prepared by the launcher reservation. This gives the server—not
   dtmgr—the existing selected-worker termination authority. Prove it with
   one controlled live COMMAND worker, then verify it disappears through the
   normal watch cleanup.
## Verification and exit criteria

Every S builds on the formal Win32/x86 row and preserves the established
product regressions: direct `run16.exe COMMAND.COM`, `MEM.EXE`, `EDIT.COM`,
then interactive COMMAND children `COMMAND`, `MEM` and `EDIT`, including
post-EDIT `MEM`.  The final S additionally proves:

- empty-list behavior with a live empty BaseSrv and unavailable behavior with
  no BaseSrv;
- version/access rejection and reconnect after a broker restart;
- stable worker identity across refresh and safe disappearance of an exited
  worker;
- correct display of one real DOS task and one real Win16/WOW16 task once the
  latter product path is available; until WOW16 is runnable, its fixture must
  assert the explicit unavailable state rather than fabricate a row; and
- selected-worker termination, cancellation/confirmation behavior, disclosed
  effect on hosted child tasks, and no effect on a sibling worker or unrelated
  Windows process.

Ship no visual source, build artifact or runtime log outside the normal build
and `O:\winnt\logs` rules. The task closes only after an independent diff
review, product artifacts, evidence of the lifecycle/negative matrix and a
clean committed/pushed worktree.

## Non-goals and boundaries

This does not create a generic Windows task manager, a process enumerator, a
second broker registry, remote administration, a GUI window, a guest BOP/UI,
or a new MVDM/OpenNT mirror overlay. It does not claim to make WOW16 runnable;
it consumes that task type once the existing lifecycle recovery genuinely
provides it.
