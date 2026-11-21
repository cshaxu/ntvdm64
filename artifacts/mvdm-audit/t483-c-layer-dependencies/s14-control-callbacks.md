# S14 P163: remove artificial control handoffs

Owner approved replacing all simplifiable control handoffs with callbacks;
retain only demonstrated cross-thread cleanup. No mirror or original guest
source/binary changed. Non-error mirror delta is 0 points / 0 lines.

## Complete mechanism disposition

The former shared exit handoff contained eight operations. Seven now execute
synchronously through the B-owned mvdm_softpc_control contract, registered by F:

- MVDM_EXIT_CONSOLE_GROUP: submit broker Console exit request; retain original
  once-only result under the B console lock.
- MVDM_EXIT_PARTICIPANT: submit participant exit request. Actual service grant
  remains a separate wait on the original broker-owned signal.
- MVDM_EXIT_STATUS and MVDM_EXIT_STATUS_ON_LOSS: read existing failure state,
  preserving the lost-service exit code assignment.
- MVDM_EXIT_CLOSE_FRONTENDS: close on the original cleanup thread after rundown
  joined producers. vm_host_close waits for outstanding users, but this caller
  holds no payload reference. No guest/ICA work moves to the pool.
- MVDM_WOW_STARTED and MVDM_WOW_BIND: preserve start/retry/bind order and errors;
  invoke existing RPC from the caller without the extra local wait.

Only MVDM_EXIT_CONSOLE_CONTROL retains a handoff: the original cleanup caller
must park while another thread claims cleanup and invokes the Console handler.
The existing one-shot threadpool wait acknowledges first, then invokes the
existing shutdown helper. It is created only for character workers. WOW-only
workers no longer create two unused control events or this threadpool wait.

The generic shared request structure and operation/wow_request/wow_task/status
slots are removed. There is no new queue, polling, retry state or second state
machine. Callback bind occurs before guest execution, unbind after callers stop.
The borrowed context lives for that worker; concurrent bind/unbind is unsupported.
Required broker signals, input watcher and original event-thread join remain.

## Counts and final-code validation

Control-operation handoffs 8 -> 1: seven removed in this batch. Including P161
COMMAND, this mechanism went from nine to one: eight removed cumulatively.
One callback slot replaces the seven relays; there are now twelve reviewed outer
callback slots across the tracked chains (B-to-D 9, B-to-F 3), not twelve queues.
B/C unapproved direct outer dependencies remain 0/0. Whole-repository redundant
queues/state machines remain a separately queued audit, not completed here.

Cached full x86, x64 and reference builds passed. Package
83074208-c3f3-486f-9d50-3742dd984e77 was published to assets/release and
O:/winnt/system32 before integration tests; all 22 image hashes verified.

- Fifteen frontend/lifecycle cases pass, including same-thread WOW start/retry/
  bind, Console request deduplication, synchronous frontend close after rundown,
  withheld exit authorization, service loss and retained Console cleanup.
- Actual package COMMAND passes modern EDIT/Ctrl+Q, CMD echo, DOS MEM and return.
- Actual WOW dialog guest passes eight phases, six initializations, modal and
  keyboard paths, rollback and exactly-once task retirement.
- Actual Win3.1 two-VM test passes native 54:08 AH=0/AH=1, 320 identity/stack/
  register/flags checks and normal Windows exit. Guest input/54:0A not covered.
- Original installation bytes verified unchanged; isolated copies cleaned.

Evidence: build/t483-s14-control-callbacks. Targeted regression only; no Full claim.
